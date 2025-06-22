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
#include <random>
#include <thread>

#include <testing/perfdefs.h>

#include "lru/thread_safe_lru_cache.h"

namespace {

using lru_cache = ::wstux::lru::thread_safe_lru_cache<size_t, size_t>;
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

        m_threads_count = std::thread::hardware_concurrency();
        m_test_data.reserve(count());
        for (size_t i = 0; i < count(); ++i) {
            m_test_data.emplace_back(i);
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

class cache_fixture : public ::testing::Test
{
    using base = ::testing::Test;

public:
    cache_fixture()
        : base()
        , m_run_threads(std::thread::hardware_concurrency())
        , m_total_count(0)
    {}

    virtual void TearDown() override
    {
        base::TearDown();
        
        for (std::thread& t : m_threads) {
            t.join();
        }
    }

    size_t request_count() const { return m_total_count; }

    void run_threads(std::function<void(size_t)> run_fn)
    {
        m_threads.clear();
        m_threads.reserve(cache_env::threads_count());
        for (size_t i = 0; i < cache_env::threads_count(); ++i) {
            m_threads.emplace_back([this, &run_fn]() -> void { thread_main(run_fn); });
        }

        // Waiting until all threads have started.
        while (! is_threads_started()) {}
    }

    void wait_finish()
    {
        m_is_start = true;

        while (! is_threads_stopped()) {}
    }

    static double to_ns(double ms) { return (ms * 1000000.0); }

private:
    bool is_threads_started() const { return (m_run_threads == 0); }
    bool is_threads_stopped() const { return (m_run_threads == cache_env::threads_count()); }

    void thread_main(std::function<void(size_t)> run_fn)
    {
        const test_data_vector& td = cache_env::test_data();
        std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
        std::uniform_int_distribution<size_t> rand(0, cache_env::count() - 1);

        --m_run_threads;
        while (! m_is_start) {}

        for (size_t i = 0; i < td.size(); ++i) {
            run_fn(td[rand(gen)]);
            ++m_total_count;
        }

        ++m_run_threads;
    }

private:
    test_data_vector m_test_data;

    std::atomic_bool m_is_start = {false};
    std::atomic_size_t m_run_threads = {0};
    std::atomic_size_t m_total_count = {0};
    std::vector<std::thread> m_threads;
};

////////////////////////////////////////////////////////////////////////////////
// class cache_wait_fixture

class cache_wait_fixture : public ::testing::Test
{
    using base = ::testing::Test;

public:
    cache_wait_fixture()
        : base()
        , m_run_threads(std::thread::hardware_concurrency())
        , m_total_count(0)
    {}

    virtual void TearDown() override
    {
        base::TearDown();
        
        for (std::thread& t : m_threads) {
            t.join();
        }
    }

    size_t request_count() const { return m_total_count; }

    void run_threads(std::function<void(size_t)> run_fn)
    {
        m_threads.clear();
        m_threads.reserve(cache_env::threads_count());
        for (size_t i = 0; i < cache_env::threads_count(); ++i) {
            m_threads.emplace_back([this, &run_fn]() -> void { thread_main(run_fn); });
        }

        // Waiting until all threads have started.
        while (! is_threads_started()) {}
    }

    void wait_finish(size_t secs = 1)
    {
        m_is_start = true;
        std::this_thread::sleep_for(std::chrono::seconds(secs));
        m_is_start = false;

        while (! is_threads_stopped()) {}
    }

    static double to_ns(double ms) { return (ms * 1000000.0); }

private:
    bool is_threads_started() const { return (m_run_threads == 0); }
    bool is_threads_stopped() const { return (m_run_threads == cache_env::threads_count()); }

    void thread_main(std::function<void(size_t)> run_fn)
    {
        const test_data_vector& td = cache_env::test_data();
        std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
        std::uniform_int_distribution<size_t> rand(0, cache_env::count() - 1);

        --m_run_threads;
        while (! m_is_start) {}

        while (m_is_start) {
            for (size_t i = 0; i < 1000; ++i) {
                run_fn(td[rand(gen)]);
                ++m_total_count;
            }
        }

        ++m_run_threads;
    }

private:
    test_data_vector m_test_data;

    std::atomic_bool m_is_start = {false};
    std::atomic_size_t m_run_threads = {0};
    std::atomic_size_t m_total_count = {0};
    std::vector<std::thread> m_threads;
};

} // <anonymous> namespace

PERF_TEST_F(cache_fixture, insert)
{
    PERF_INIT_TIMER(insert);

    lru_cache cache(2 * cache_env::count(), cache_env::threads_count());
    std::function<void(size_t)> run_fn = [&cache](size_t k) -> void {
        cache.insert(k, k);
    };

    run_threads(run_fn);

    PERF_START_TIMER(insert);
    wait_finish();
    PERF_PAUSE_TIMER(insert);

    const double ms = PERF_TIMER_MSECS(insert);
    const size_t td_size = request_count();
    PERF_MESSAGE() << "insert time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td_size / ms) << " insert/ms";
}

PERF_TEST_F(cache_fixture, emplace)
{
    PERF_INIT_TIMER(emplace);

    lru_cache cache(2 * cache_env::count(), cache_env::threads_count());
    std::function<void(size_t)> run_fn = [&cache](size_t k) -> void {
        cache.emplace(k, k);
    };

    run_threads(run_fn);

    PERF_START_TIMER(emplace);
    wait_finish();
    PERF_PAUSE_TIMER(emplace);

    const double ms = PERF_TIMER_MSECS(emplace);
    const size_t td_size = request_count();
    PERF_MESSAGE() << "emplace time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td_size / ms) << " emplace/ms";
}

PERF_TEST_F(cache_fixture, update_insert)
{
    PERF_INIT_TIMER(update_insert);

    lru_cache cache(2 * cache_env::count(), cache_env::threads_count());
    std::function<void(size_t)> run_fn = [&cache](size_t k) -> void {
        cache.update(k, k);
    };

    run_threads(run_fn);

    PERF_START_TIMER(update_insert);
    wait_finish();
    PERF_PAUSE_TIMER(update_insert);

    const double ms = PERF_TIMER_MSECS(update_insert);
    const size_t td_size = request_count();
    PERF_MESSAGE() << "update-insert time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td_size / ms) << " update/ms";
}

PERF_TEST_F(cache_fixture, update)
{
    PERF_INIT_TIMER(update);

    lru_cache cache(2 * cache_env::count(), cache_env::threads_count());
    std::function<void(size_t)> run_fn = [&cache](size_t k) -> void {
        cache.update(k, k);
    };

    for (size_t k : cache_env::test_data()) {
        cache.insert(k, k + 1);
    }

    run_threads(run_fn);

    PERF_START_TIMER(update);
    wait_finish();
    PERF_PAUSE_TIMER(update);

    const double ms = PERF_TIMER_MSECS(update);
    const size_t td_size = request_count();
    PERF_MESSAGE() << "update time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td_size / ms) << " update/ms";
}

PERF_TEST_F(cache_fixture, find)
{
    PERF_INIT_TIMER(find);

    lru_cache cache(2 * cache_env::count(), cache_env::threads_count());
    std::function<void(size_t)> run_fn = [&cache](size_t k) -> void {
        lru_cache::value_type val = 0;
        cache.find(k, val);
    };

    for (size_t k : cache_env::test_data()) {
        cache.insert(k, k + 1);
    }

    run_threads(run_fn);

    PERF_START_TIMER(find);
    wait_finish();
    PERF_PAUSE_TIMER(find);

    const double ms = PERF_TIMER_MSECS(find);
    const size_t td_size = request_count();
    PERF_MESSAGE() << "find time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td_size / ms) << " find/ms";
}

PERF_TEST_F(cache_fixture, insert_overflow)
{
    PERF_INIT_TIMER(insert_overflow);

    lru_cache cache(cache_env::count(), cache_env::threads_count());
    std::function<void(size_t)> run_fn = [&cache](size_t k) -> void {
        cache.insert(k, k);
    };

    for (size_t i = 0; i < cache_env::count(); ++i) {
        cache.insert(i + cache_env::count(), i + cache_env::count());
    }

    run_threads(run_fn);

    PERF_START_TIMER(insert_overflow);
    wait_finish();
    PERF_PAUSE_TIMER(insert_overflow);

    const double ms = PERF_TIMER_MSECS(insert_overflow);
    const size_t td_size = request_count();
    PERF_MESSAGE() << "insert overflow time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td_size / ms) << " insert_overflow/ms";
}

PERF_TEST_F(cache_wait_fixture, request)
{
    PERF_INIT_TIMER(request);

    lru_cache cache(cache_env::count(), cache_env::threads_count());
    std::function<void(size_t)> run_fn = [&cache](size_t k) -> void {
        lru_cache::value_type val = 0;
        if (! cache.find(k, val)) {
            cache.insert(k, k);
        }
    };

    run_threads(run_fn);

    PERF_START_TIMER(request);
    wait_finish();
    PERF_PAUSE_TIMER(request);

    const double ms = PERF_TIMER_MSECS(request);
    const size_t td_size = request_count();
    PERF_MESSAGE() << "insert time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td_size / ms) << " insert/ms";
}

PERF_TEST_F(cache_wait_fixture, request_hot)
{
    PERF_INIT_TIMER(request_hot);

    lru_cache cache(cache_env::count(), cache_env::threads_count());
    std::function<void(size_t)> run_fn = [&cache](size_t k) -> void {
        lru_cache::value_type val = 0;
        if (! cache.find(k, val)) {
            cache.insert(k, k);
        }
    };

    for (size_t k : cache_env::test_data()) {
        cache.insert(k, k);
    }

    run_threads(run_fn);

    PERF_START_TIMER(request_hot);
    wait_finish();
    PERF_PAUSE_TIMER(request_hot);

    const double ms = PERF_TIMER_MSECS(request_hot);
    const size_t td_size = request_count();
    PERF_MESSAGE() << "insert time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td_size / ms) << " insert/ms";
}

PERF_TEST_F(cache_wait_fixture, many_shards)
{
    PERF_INIT_TIMER(request);

    lru_cache cache(cache_env::count(), 3 * cache_env::threads_count());
    std::function<void(size_t)> run_fn = [&cache](size_t k) -> void {
        lru_cache::value_type val = 0;
        if (! cache.find(k, val)) {
            cache.insert(k, k);
        }
    };

    run_threads(run_fn);

    PERF_START_TIMER(request);
    wait_finish();
    PERF_PAUSE_TIMER(request);

    const double ms = PERF_TIMER_MSECS(request);
    const size_t td_size = request_count();
    PERF_MESSAGE() << "insert time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td_size) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td_size / ms) << " insert/ms";
}

int main(int /*argc*/, char** /*argv*/)
{
    ::testing::AddGlobalTestEnvironment(new cache_env());
    return RUN_ALL_PERF_TESTS();
}

