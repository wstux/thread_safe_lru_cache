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

#include <testing/testdefs.h>

#include "lru_cache/thread_safe_lru_cache.h"

namespace {

using lru_cache = ::wstux::cnt::thread_safe_lru_cache<size_t, size_t>;
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

    static size_t count() { return 100; }

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
    using insert_fn_t = std::function<void(lru_cache&,
                                           const lru_cache::key_type&,
                                           const lru_cache::value_type&)>;

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

    template<typename TCache>
    void init_threads(TCache& cache, const insert_fn_t& insert_fn)
    {
        m_threads.clear();
        m_threads.reserve(threads_count());
        for (size_t i = 0; i < threads_count(); ++i) {
            m_threads.emplace_back([this, &cache, &insert_fn]() -> void {
                                       thread_main(cache, insert_fn);
                                   });
        }
    }

    bool is_threads_started() const { return (m_run_threads == 0); }

    void start() { m_is_start = true; }

    size_t threads_count() const { return m_threads_count; }

protected:
    template<typename TCache>
    void thread_main(TCache& cache, const insert_fn_t& insert_fn)
    {
        const test_data_vector& td = cache_env::test_data();
        const std::string val = "";

        std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
        std::uniform_int_distribution<size_t> rand(0, cache_env::count() - 1);

        size_t hit_count = 0;
        size_t total_count = 0;

        --m_run_threads;
        while (! m_is_start) {}

        for (size_t i = 0; i < 10 * td.size(); ++i) {
            size_t r = rand(gen);
            const typename TCache::key_type& key = td[r];
            typename TCache::value_type val;
            if (! cache.find(key, val)) {
                insert_fn(cache, key, key);
            } else {
                EXPECT_TRUE(key == val) << "key('" << key << "') != value('" << val << "'";
                ++hit_count;
            }
            total_count++;
        }

        cache_env::hit_count += hit_count;
        cache_env::total_count += total_count;
    }

private:
    std::atomic_bool m_is_start = {false};
    std::atomic_size_t m_run_threads = {0};
    size_t m_threads_count;
    std::vector<std::thread> m_threads;
};

} // <anonymous> namespace

TEST_F(cache_fixture, insert)
{
    std::atomic_bool is_start = {false};

    lru_cache cache(10 * threads_count(), threads_count());

    const insert_fn_t insert_fn =
        [](lru_cache& c, const lru_cache::key_type& k, const lru_cache::value_type& v) -> void {
            c.insert(k, v);
        };
    init_threads(cache, insert_fn);

    // Waiting until all threads have started.
    while (! is_threads_started()) {}
    // Start work.
    start();
    // Finish threads.
    join_threads();

    const size_t actual_total_count = 10 * threads_count() * cache_env::count();
    EXPECT_TRUE(actual_total_count == cache_env::total_count)
        << "total_count = " << cache_env::total_count;
    EXPECT_TRUE(cache_env::hit_count > 0) << "hit_count = " << cache_env::hit_count;
    EXPECT_TRUE(cache_env::hit_count < cache_env::total_count)
        << "hit_count = " << cache_env::hit_count;
}

TEST_F(cache_fixture, update)
{
    std::atomic_bool is_start = {false};

    lru_cache cache(10 * threads_count(), threads_count());

    const insert_fn_t insert_fn =
        [](lru_cache& c, const lru_cache::key_type& k, const lru_cache::value_type& v) -> void {
            c.update(k, v);
        };
    init_threads(cache, insert_fn);

    // Waiting until all threads have started.
    while (! is_threads_started()) {}
    // Start work.
    start();
    // Finish threads.
    join_threads();

    const size_t actual_total_count = 10 * threads_count() * cache_env::count();
    EXPECT_TRUE(actual_total_count == cache_env::total_count)
        << "total_count = " << cache_env::total_count;
    EXPECT_TRUE(cache_env::hit_count > 0) << "hit_count = " << cache_env::hit_count;
    EXPECT_TRUE(cache_env::hit_count < cache_env::total_count)
        << "hit_count = " << cache_env::hit_count;
}

int main(int /*argc*/, char** /*argv*/)
{
    ::testing::AddGlobalTestEnvironment(new cache_env());
    return RUN_ALL_TESTS();
}
