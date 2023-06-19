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

#include <atomic>
#include <random>
#include <thread>

#include <testing/perfdefs.h>

#include "lru/thread_safe_lru_cache.h"

namespace {

using lru_cache = ::wstux::lru::thread_safe_lru_cache<std::string, std::string>;
using test_data_vector = std::vector<lru_cache::key_type>;

////////////////////////////////////////////////////////////////////////////////
// class cache_env

class cache_env : public ::testing::Environment
{
    using base = ::testing::Environment;

public:
    virtual void SetUp() override
    {
        base::SetUp();

        m_test_data.reserve(count());
        for (size_t i = 0; i < count(); ++i) {
            m_test_data.emplace_back(std::string(100, 'x') + std::to_string(i));
        }
    }

    static size_t count() { return 100000; }

    static const test_data_vector& test_data() { return m_test_data; }

    static std::atomic_size_t hit_count;
    static std::atomic_size_t total_count;

private:
    static test_data_vector m_test_data;
};

std::atomic_size_t cache_env::hit_count = {0};
std::atomic_size_t cache_env::total_count = {0};
test_data_vector cache_env::m_test_data = {};

////////////////////////////////////////////////////////////////////////////////
// class cache_fixture

class cache_fixture : public ::testing::Test
{
    using base = ::testing::Test;

public:
    virtual void SetUp() override
    {
        base::SetUp();
 
        m_threads_count = std::thread::hardware_concurrency();
        m_run_threads = m_threads_count;

        cache_env::hit_count = {0};
        cache_env::total_count = {0};
    }

    void join_threads()
    {
        for (std::thread& t : m_threads) {
            t.join();
        }
    }

    void init_state()
    {
        m_is_start = {false};
        m_is_stop = {false};
        m_run_threads = m_threads_count;
    }

    template<typename TCache>
    void init_threads(TCache& cache)
    {
        m_threads.clear();
        m_threads.reserve(threads_count());
        for (size_t i = 0; i < threads_count(); ++i) {
            m_threads.emplace_back([this, &cache]() -> void { thread_main(cache); });
        }
    }

    bool is_threads_started() const { return (m_run_threads == 0); }
    bool is_threads_stopped() const { return (m_run_threads == threads_count()); }

    void start() { m_is_start = true; }
    void stop() { m_is_stop = true; }

    size_t threads_count() const { return m_threads_count; }

protected:
    template<typename TCache>
    void thread_main(TCache& cache)
    {
        const test_data_vector& td = cache_env::test_data();

        std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
        std::uniform_int_distribution<size_t> rand(0, cache_env::count() - 1);

        size_t hit_count = 0;
        size_t total_count = 0;

        --m_run_threads;
        while (! m_is_start) {}

        typename TCache::value_type val;
        while (! m_is_stop) {
            for (size_t i = 0; i < 1000; ++i) {
                size_t r = rand(gen);
                if (cache.find(td[r], val)) {
                    ++hit_count;
                } else {
                    cache.insert(td[r], td[r]);
                }
                total_count++;
            }
        }

        ++m_run_threads;

        cache_env::hit_count += hit_count;
        cache_env::total_count += total_count;
    }

private:
    std::atomic_bool m_is_start = {false};
    std::atomic_bool m_is_stop = {false};
    std::atomic_size_t m_run_threads = {0};
    size_t m_threads_count;
    std::vector<std::thread> m_threads;
};

} // <anonymous> namespace

PERF_TEST_F(cache_fixture, cache_request)
{
    PERF_INIT_TIMER(request);

    lru_cache cache(cache_env::count(), threads_count());
    lru_cache::value_type val;
    const test_data_vector& td = cache_env::test_data();
    for (size_t i = 0; i < std::min(cache_env::count(), td.size()); ++i) {
        if (! cache.find(td[i], val)) {
            cache.insert(td[i], td[i]);
        }
    }

    init_threads(cache);

    // Waiting until all threads have started.
    while (! is_threads_started()) {}

    PERF_START_TIMER(request);

    // Start work;
    start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    stop();
    while (! is_threads_stopped()) {}

    PERF_PAUSE_TIMER(request);

    // Finish threads.
    join_threads();

    PERF_MESSAGE() << "hit_count   = " << cache_env::hit_count;
    PERF_MESSAGE() << "total_count = " << cache_env::total_count;
    PERF_MESSAGE() << "speed = "
        << (cache_env::total_count / PERF_TIMER_MSECS(request)) << " requests/ms";
}

PERF_TEST_F(cache_fixture, cache_request_mutex)
{
    using lru_cache_mutex
        = ::wstux::lru::thread_safe_lru_cache<std::string,
                                              std::string,
                                              std::hash,
                                              std::unordered_map,
                                              std::list,
                                              std::mutex>;

    PERF_INIT_TIMER(request);

    lru_cache_mutex cache(cache_env::count(), threads_count());
    lru_cache_mutex::value_type val;
    const test_data_vector& td = cache_env::test_data();
    for (size_t i = 0; i < std::min(cache_env::count(), td.size()); ++i) {
        if (! cache.find(td[i], val)) {
            cache.insert(td[i], td[i]);
        }
    }

    init_threads(cache);

    // Waiting until all threads have started.
    while (! is_threads_started()) {}

    PERF_START_TIMER(request);

    // Start work;
    start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    stop();
    while (! is_threads_stopped()) {}

    PERF_PAUSE_TIMER(request);

    // Finish threads.
    join_threads();

    PERF_MESSAGE() << "hit_count   = " << cache_env::hit_count;
    PERF_MESSAGE() << "total_count = " << cache_env::total_count;
    PERF_MESSAGE() << "speed = "
        << (cache_env::total_count / PERF_TIMER_MSECS(request)) << " requests/ms";
}

PERF_TEST_F(cache_fixture, cache_request_hot)
{
    PERF_INIT_TIMER(request);

    lru_cache cache(cache_env::count(), threads_count());

    init_threads(cache);

    // Waiting until all threads have started.
    while (! is_threads_started()) {}

    PERF_START_TIMER(request);

    // Start work;
    start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    stop();
    while (! is_threads_stopped()) {}

    PERF_PAUSE_TIMER(request);

    // Finish threads.
    join_threads();

    PERF_MESSAGE() << "hit_count   = " << cache_env::hit_count;
    PERF_MESSAGE() << "total_count = " << cache_env::total_count;
    PERF_MESSAGE() << "speed = "
        << (cache_env::total_count / PERF_TIMER_MSECS(request)) << " requests/ms";
}

PERF_TEST_F(cache_fixture, cache_request_many_shards)
{
    PERF_INIT_TIMER(request);

    lru_cache cache(cache_env::count(), threads_count());

    constexpr size_t kRepeatCount = 11;
    for (size_t i = 1; i < kRepeatCount; i += 3) {
        cache_env::hit_count = {0};
        cache_env::total_count = {0};

        init_state();
        lru_cache cache(cache_env::count(), i * threads_count());

        init_threads(cache);

        // Waiting until all threads have started.
        while (! is_threads_started()) {}

        PERF_RESTART_TIMER(request);

        // Start work;
        start();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        stop();
        while (! is_threads_stopped()) {}

        PERF_PAUSE_TIMER(request);

        // Finish threads.
        join_threads();

        PERF_MESSAGE() << "BEGIN SHARDS COUNT " << i << " x CPU COUNT *******************";
        PERF_MESSAGE() << "hit_count   = " << cache_env::hit_count;
        PERF_MESSAGE() << "total_count = " << cache_env::total_count;
        PERF_MESSAGE() << "medium hit_count   = " << cache_env::hit_count / kRepeatCount;
        PERF_MESSAGE() << "medium total_count = " << cache_env::total_count / kRepeatCount;
        PERF_MESSAGE() << "medium speed = "
            << (cache_env::total_count / PERF_TIMER_MSECS(request)) << " requests/ms";
        PERF_MESSAGE() << "END SHARDS COUNT " << i << " x CPU COUNT *********************";
    }
}

PERF_TEST_F(cache_fixture, cache_request_medium)
{
    PERF_INIT_TIMER(request);

    lru_cache cache(cache_env::count(), threads_count());

    constexpr size_t kRepeatCount = 10;
    for (size_t i = 0; i < kRepeatCount; ++i) {
        init_state();
        lru_cache cache(cache_env::count(), threads_count());

        init_threads(cache);

        // Waiting until all threads have started.
        while (! is_threads_started()) {}

        PERF_START_TIMER(request);

        // Start work;
        start();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        stop();
        while (! is_threads_stopped()) {}

        PERF_PAUSE_TIMER(request);

        // Finish threads.
        join_threads();
    }

    PERF_MESSAGE() << "hit_count   = " << cache_env::hit_count;
    PERF_MESSAGE() << "total_count = " << cache_env::total_count;
    PERF_MESSAGE() << "medium hit_count   = " << cache_env::hit_count / kRepeatCount;
    PERF_MESSAGE() << "medium total_count = " << cache_env::total_count / kRepeatCount;
    PERF_MESSAGE() << "medium speed = "
        << (cache_env::total_count / PERF_TIMER_MSECS(request)) << " requests/ms";
}

int main(int /*argc*/, char** /*argv*/)
{
    ::testing::AddGlobalTestEnvironment(new cache_env());
    return RUN_ALL_PERF_TESTS();
}
