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

#include "lru/lru_cache.h"

namespace {

using lru_cache = ::wstux::lru::lru_cache<size_t, size_t>;
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
            m_test_data.emplace_back(i);
        }
    }

    static size_t count() { return 100000; }

    static const test_data_vector& test_data() { return m_test_data; }

private:
    static test_data_vector m_test_data;
};

test_data_vector cache_env::m_test_data = {};

////////////////////////////////////////////////////////////////////////////////
// class cache_fixture

class cache_fixture : public ::testing::Test
{
public:
    static double to_ns(double ms) { return (ms * 1000000.0); }
};

} // <anonymous> namespace

PERF_TEST_F(cache_fixture, insert)
{
    PERF_INIT_TIMER(insert);

    lru_cache cache(2 * cache_env::count());
    const test_data_vector& td = cache_env::test_data();
    for (size_t k : td) {
        PERF_START_TIMER(insert);
        cache.insert(k, k);
        PERF_PAUSE_TIMER(insert);
    }

    const double ms = PERF_TIMER_MSECS(insert);
    PERF_MESSAGE() << "insert time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td.size()) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td.size() / ms) << " insert/ms";
}

PERF_TEST_F(cache_fixture, emplace)
{
    PERF_INIT_TIMER(emplace);

    lru_cache cache(2 * cache_env::count());
    const test_data_vector& td = cache_env::test_data();
    for (size_t k : td) {
        PERF_START_TIMER(emplace);
        cache.emplace(k, k);
        PERF_PAUSE_TIMER(emplace);
    }

    const double ms = PERF_TIMER_MSECS(emplace);
    PERF_MESSAGE() << "emplace time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td.size()) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td.size() / ms) << " emplace/ms";
}

PERF_TEST_F(cache_fixture, update_insert)
{
    PERF_INIT_TIMER(update_insert);

    lru_cache cache(2 * cache_env::count());
    const test_data_vector& td = cache_env::test_data();
    for (size_t k : td) {
        PERF_START_TIMER(update_insert);
        cache.update(k, k);
        PERF_PAUSE_TIMER(update_insert);
    }

    const double ms = PERF_TIMER_MSECS(update_insert);
    PERF_MESSAGE() << "update-insert time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td.size()) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td.size() / ms) << " update/ms";
}

PERF_TEST_F(cache_fixture, update)
{
    PERF_INIT_TIMER(update);

    lru_cache cache(2 * cache_env::count());
    const test_data_vector& td = cache_env::test_data();
    for (size_t k : td) {
        cache.insert(k, k + 1);
    }

    for (size_t k : td) {
        PERF_START_TIMER(update);
        cache.update(k, k);
        PERF_PAUSE_TIMER(update);
    }

    const double ms = PERF_TIMER_MSECS(update);
    PERF_MESSAGE() << "update time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td.size()) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td.size() / ms) << " update/ms";
}

PERF_TEST_F(cache_fixture, find)
{
    PERF_INIT_TIMER(find);

    lru_cache cache(2 * cache_env::count());
    const test_data_vector& td = cache_env::test_data();
    for (size_t k : td) {
        cache.insert(k, k);
    }

    lru_cache::mapped_type val;
    for (size_t k : td) {
        PERF_START_TIMER(find);
        cache.find(k, val);
        PERF_PAUSE_TIMER(find);
    }

    const double ms = PERF_TIMER_MSECS(find);
    PERF_MESSAGE() << "find time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td.size()) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td.size() / ms) << " find/ms";
}

PERF_TEST_F(cache_fixture, insert_overflow)
{
    PERF_INIT_TIMER(insert_overflow);

    lru_cache cache(cache_env::count());
    for (size_t i = 0; i < cache_env::count(); ++i) {
        cache.insert(i + cache_env::count(), i + cache_env::count());
    }

    const test_data_vector& td = cache_env::test_data();
    for (size_t k : td) {
        PERF_START_TIMER(insert_overflow);
        cache.insert(k, k);
        PERF_PAUSE_TIMER(insert_overflow);
    }

    const double ms = PERF_TIMER_MSECS(insert_overflow);
    PERF_MESSAGE() << "insert overflow time: total = " << ms << " ms; "
                   << "one element = " << to_ns(ms / (double)td.size()) << " ns";
    PERF_MESSAGE() << "speed = " << ((double)td.size() / ms) << " insert_overflow/ms";
}

int main(int /*argc*/, char** /*argv*/)
{
    ::testing::AddGlobalTestEnvironment(new cache_env());
    return RUN_ALL_PERF_TESTS();
}

