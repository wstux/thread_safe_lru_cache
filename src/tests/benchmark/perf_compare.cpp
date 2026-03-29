/*
 * The MIT License
 *
 * Copyright 2023 Chistyakov Alexander.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if defined(THREAD_SAFE_CACHE_DISABLE_BOOST)
    #undef THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE
#endif

#include <atomic>
#include <shared_mutex>
#include <random>
#include <thread>

#include <testing/perfdefs.h>

#include <thread-safe-lru/scalable-cache.h>

#include "lru/thread_safe_lru_cache.h"

namespace {

using test_data_vector = std::vector<std::string>;

////////////////////////////////////////////////////////////////////////////////
// class cache_env

class cache_env : public ::testing::Environment
{
    using base = ::testing::Environment;

public:
    virtual void SetUp() override
    {
        base::SetUp();

        m_threads_count = 32;
        m_test_data.reserve(count());
        for (size_t i = 0; i < count(); ++i) {
            m_test_data.emplace_back(std::string(100, 'x') + std::to_string(i));
        }
    }

    static size_t count() { return 100000; }

    static const test_data_vector& test_data() { return m_test_data; }

    static size_t threads_count() { return m_threads_count; }

private:
    static size_t m_threads_count;
    static test_data_vector m_test_data;
};

size_t cache_env::m_threads_count = {};
test_data_vector cache_env::m_test_data = {};

////////////////////////////////////////////////////////////////////////////////
// class cache_fixture

template<typename TType>
class cache_fixture : public ::testing::Test
{
    using base = ::testing::Test;
    using key_type = typename TType::key_type;

public:
    virtual void TearDown() override
    {
        base::TearDown();

        for (std::thread& t : m_threads) {
            t.join();
        }
    }

    void run_threads(std::function<void(const key_type&)> run_fn)
    {
        using namespace std::chrono_literals;

        std::unique_lock<std::shared_mutex> lock(m_mutex);

        m_threads.clear();
        m_threads.reserve(cache_env::threads_count());
        for (size_t i = 0; i < cache_env::threads_count(); ++i) {
            m_threads.emplace_back([this, &run_fn]() -> void { thread_main(run_fn); });
        }
        // Waiting until all threads have started.
        std::this_thread::sleep_for(1000ms);
    }

    void wait_finish()
    {
        m_is_start = true;
        std::unique_lock<std::shared_mutex> lock(m_mutex);
    }

    static double to_ns(double ms) { return (ms * 1000000.0); }

private:
    void thread_main(std::function<void(const key_type&)> run_fn)
    {
        const test_data_vector& td = cache_env::test_data();
        std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
        std::uniform_int_distribution<size_t> rand(0, cache_env::count() - 1);

        std::shared_lock<std::shared_mutex> lock(m_mutex);

        while (! m_is_start) {}

        for (size_t i = 0; i < td.size() * 10; ++i) {
            run_fn(td[rand(gen)]);
        }
    }

private:
    test_data_vector m_test_data;

    std::atomic_bool m_is_start = {false};
    std::shared_mutex m_mutex;
    std::vector<std::thread> m_threads;
};

struct lru_cache
{
    using key_type = std::string;
    using value_type = size_t;
    using cache = ::wstux::lru::thread_safe_lru_cache<key_type, value_type>;

    static bool find(cache& c, const key_type& k)
    {
        value_type val;
        return c.find(k, val);
    }
    static void insert(cache& c, const key_type& k, value_type val) { c.insert(k, val); }
};

struct ts_lru_cache
{
    using key_type = std::string;
    using value_type = size_t;
    using cache = ::tstarling::ThreadSafeScalableCache<key_type, value_type>;

    static bool find(cache& c, const key_type& k)
    {
        cache::ConstAccessor ac;
        return c.find(ac, k);
    }
    static void insert(cache& c, const key_type& k, value_type val)
    {
        c.insert(k, val);
    }
};

using cache_types = testing::Types<lru_cache, ts_lru_cache>;
TYPED_PERF_TEST_SUITE(cache_fixture, cache_types);

} // <anonymous> namespace

TYPED_PERF_TEST(cache_fixture, test)
{
    using cache_type = TypeParam;
    using key_type = typename cache_type::key_type;
    using value_type = typename cache_type::value_type;
    using lru_cache = typename cache_type::cache;

    PERF_INIT_TIMER(find);
    PERF_INIT_TIMER(insert);

    std::atomic_size_t hit_count = 0;
    std::atomic_size_t total_count = 0;

    lru_cache cache(cache_env::count() / 10, std::thread::hardware_concurrency());
    std::function<void(key_type)> run_fn = [&](const key_type& k) -> void {
        PERF_START_TIMER(find);
        const bool is_found = cache_type::find(cache, k);
        PERF_PAUSE_TIMER(find);

        if (is_found) {
            ++hit_count;
        } else {
            PERF_START_TIMER(insert);
            cache_type::insert(cache, k, (value_type)1);
            PERF_PAUSE_TIMER(insert);
        }
        ++total_count;
    };

    this->run_threads(run_fn);
    this->wait_finish();

    const double search_ms = PERF_TIMER_MSECS(find);
    const double insert_ms = PERF_TIMER_MSECS(insert);
    const double ms = search_ms + insert_ms;
    const size_t td_size = total_count;
    PERF_MESSAGE() << "total requests = " << td_size << " times; hit count = " << hit_count;
    PERF_MESSAGE() << "search time: " << search_ms << " ms; one element: " << this->to_ns(search_ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "insert time: " << insert_ms << " ms; one element: " << this->to_ns(insert_ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "total time: " << ms << " ms; one element: " << this->to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed: " << ((double)td_size / ms) << " requests/ms";
}

int main(int /*argc*/, char** /*argv*/)
{
    ::testing::AddGlobalTestEnvironment(new cache_env());
    return RUN_ALL_PERF_TESTS();
}

