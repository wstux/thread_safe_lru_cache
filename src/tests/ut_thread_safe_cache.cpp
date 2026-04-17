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
#include <limits>
#include <random>
#include <thread>

#include <gtest/gtest.h>

#include "cache/thread_safe_lru_cache.h"
#include "cache/thread_safe_rr_cache.h"
#include "cache/thread_safe_ttl_cache.h"

namespace {

////////////////////////////////////////////////////////////////////////////////
// class thread_safe_cache_fixture

template<typename TParam>
class thread_safe_cache_fixture : public ::testing::Test
{
    using base = ::testing::Test;
    using test_data_vector = std::vector<size_t>;
    using cache_type = typename TParam::cache;

public:
    virtual void SetUp() override
    {
        base::SetUp();

        m_run_threads = threads_count();

        m_test_data.reserve(count());
        for (size_t i = 0; i < count(); ++i) {
            m_test_data.emplace_back(i);
        }

        m_hit_count = {0};
        m_total_count = {0};
    }

    void join_threads() { for (std::thread& t : m_threads) { t.join(); } }

    void init_threads(cache_type& c)
    {
        m_threads.clear();
        m_threads.reserve(threads_count());
        for (size_t i = 0; i < threads_count(); ++i) {
            m_threads.emplace_back([this, &c]() -> void { thread_main(c); });
        }
    }

    size_t count() const { return 100; }

    bool is_threads_started() const { return (m_run_threads == 0); }

    void start() { m_is_start = true; }

    size_t threads_count() const { return 5; }

    size_t hit_count() const { return m_hit_count; }
    size_t total_count() const { return m_total_count; }

protected:
    void thread_main(cache_type& cache)
    {
        const test_data_vector& td = m_test_data;
        const std::string val = "";

        std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
        std::uniform_int_distribution<size_t> rand(0, m_test_data.size() - 1);

        --m_run_threads;
        while (! m_is_start) {}

        for (size_t i = 0; i < 10 * td.size(); ++i) {
            const typename cache_type::key_type& key = td[rand(gen)];
            typename cache_type::value_type val;
            if (! cache.find(key, val)) {
                cache.insert(key, key);
            } else {
                EXPECT_TRUE(key == val) << "key('" << key << "') != value('" << val << "'";
                ++m_hit_count;
            }
            ++m_total_count;
        }
    }

private:
    std::atomic_bool m_is_start = {false};
    std::atomic_size_t m_run_threads = {0};

    std::vector<std::thread> m_threads;
    test_data_vector m_test_data;

    std::atomic_size_t m_hit_count;
    std::atomic_size_t m_total_count;
};

struct thread_safe_lru_cache
{
    using cache = ::wstux::cache::lru::thread_safe_lru_cache<size_t, size_t>;

    static cache create(size_t cap = 10, size_t shards = 2) { return cache(cap, shards); }
};

struct thread_safe_rr_cache
{
    using cache = ::wstux::cache::rr::thread_safe_rr_cache<size_t, size_t>;

    static cache create(size_t cap = 10, size_t shards = 2) { return cache(cap, shards); }
};

struct thread_safe_ttl_cache
{
    using cache = ::wstux::cache::ttl::thread_safe_ttl_cache<size_t, size_t>;

    static cache create(size_t cap = 10, size_t shards = 2) { return cache(900, cap, shards); }
};

using cache_types = testing::Types<thread_safe_lru_cache, thread_safe_rr_cache, thread_safe_ttl_cache>;
TYPED_TEST_SUITE(thread_safe_cache_fixture, cache_types);

} // <anonymous> namespace

TYPED_TEST(thread_safe_cache_fixture, shards_leak)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create(1, 2);
    EXPECT_TRUE(cache.shards_size() == 1);

    EXPECT_TRUE(cache.insert(0, 3));
    EXPECT_TRUE(cache.insert(1, 4));

    EXPECT_FALSE(cache.contains(0));
    EXPECT_TRUE(cache.contains(1));
}

TYPED_TEST(thread_safe_cache_fixture, hit)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create(30, this->threads_count());

    this->init_threads(cache);

    // Waiting until all threads have started.
    while (! this->is_threads_started()) {}
    // Start work.
    this->start();
    // Finish threads.
    this->join_threads();

    const size_t count = this->count();
    const size_t hit_count = this->hit_count();
    const size_t threads_count = this->threads_count();
    const size_t total_count = this->total_count();
    const size_t actual_total_count = 10 * threads_count * count;
    EXPECT_TRUE(actual_total_count == total_count) << "total_count = " << total_count;
    EXPECT_TRUE(hit_count > 0) << "hit_count = " << hit_count;
    EXPECT_TRUE(hit_count < total_count) << "hit_count = " << hit_count;
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
