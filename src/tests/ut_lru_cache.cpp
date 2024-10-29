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

#include <testing/testdefs.h>

#include "lru/lru_cache.h"
#include "lru/thread_safe_lru_cache.h"

namespace {

////////////////////////////////////////////////////////////////////////////////
// class thread_safe_cache_fixture

class thread_safe_cache_fixture : public ::testing::Test
{
    using base = ::testing::Test;
    using test_data_vector = std::vector<size_t>;

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

    template<typename TCache>
    void init_threads(TCache& c)
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
    template<typename TCache>
    void thread_main(TCache& cache)
    {
        const test_data_vector& td = m_test_data;
        const std::string val = "";

        std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
        std::uniform_int_distribution<size_t> rand(0, m_test_data.size() - 1);

        --m_run_threads;
        while (! m_is_start) {}

        for (size_t i = 0; i < 10 * td.size(); ++i) {
            const typename TCache::key_type& key = td[rand(gen)];
            typename TCache::value_type val;
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

////////////////////////////////////////////////////////////////////////////////
// class cache_fixture

template<typename TLruCache>
class cache_fixture : public ::testing::Test
{};

struct lru_cache
{
    using cache = ::wstux::lru::lru_cache<size_t, std::string>;

    static cache create(size_t cap = 10, size_t shards = 0) { (void)shards; return cache(cap); }
};

struct thread_safe_lru_cache
{
    using cache = ::wstux::lru::thread_safe_lru_cache<size_t, std::string>;

    static cache create(size_t cap = 10, size_t shards = 2) { return cache(cap, shards); }
};

using lru_types = testing::Types<lru_cache, thread_safe_lru_cache>;
TYPED_TEST_SUITE(cache_fixture, lru_types);

} // <anonymous> namespace

TYPED_TEST(cache_fixture, contains)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create();

    typename lru_cache::value_type val;
    EXPECT_FALSE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_TRUE(cache.contains(0));
}

TYPED_TEST(cache_fixture, contains_touch)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create(4);

    typename lru_cache::value_type val;
    EXPECT_FALSE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_TRUE(cache.emplace(1, 4, 'b'));
    EXPECT_TRUE(cache.emplace(2, 4, 'b'));
    EXPECT_TRUE(cache.emplace(3, 4, 'b'));
    EXPECT_TRUE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(5, 4, 'b'));
    EXPECT_TRUE(cache.contains(0));
    EXPECT_FALSE(cache.contains(1));
}

TYPED_TEST(cache_fixture, emplace)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create();

    typename lru_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_TRUE(cache.find(0, val));
    EXPECT_TRUE(val == "bbbb") << val;
}

TYPED_TEST(cache_fixture, DISABLED_zero_size)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create(0, 0);

    typename lru_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_FALSE(cache.find(0, val)) << cache.size();;
}

TYPED_TEST(cache_fixture, insert)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create();

    typename lru_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    EXPECT_TRUE(cache.find(0, val));
    EXPECT_TRUE(val == "bbbb") << val;
}

TYPED_TEST(cache_fixture, multi_insert)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create(1);

    typename lru_cache::value_type val;

    for (size_t i = 0; i < std::numeric_limits<int16_t>::max(); ++i) {
        EXPECT_TRUE(cache.insert(i, std::string(4, 'b')));
    }

    EXPECT_TRUE(cache.insert(std::numeric_limits<int32_t>::max(), std::string(4, 'b')));
    for (size_t i = 0; i < std::numeric_limits<int16_t>::max(); ++i) {
        EXPECT_FALSE(cache.find(i, val));
    }
    EXPECT_TRUE(cache.find(std::numeric_limits<int32_t>::max(), val));
    EXPECT_TRUE(val == "bbbb") << val;
}

TYPED_TEST(cache_fixture, empty)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create();

    EXPECT_TRUE(cache.empty());
    cache.emplace(0, 4, 'b');
    EXPECT_FALSE(cache.empty());
}

TYPED_TEST(cache_fixture, erase)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create();

    EXPECT_TRUE(cache.empty());
    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    EXPECT_FALSE(cache.empty());

    typename lru_cache::value_type val;
    EXPECT_TRUE(cache.find(0, val));

    cache.erase(0);
    EXPECT_TRUE(cache.empty());
    EXPECT_FALSE(cache.find(0, val));
}

#if defined(_THREAD_SAFE_LRU_CACHE_ENABLE_OPTIONAL)
TYPED_TEST(cache_fixture, get)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create();

    std::optional<typename lru_cache::value_type> val;
    val = cache.get(0);
    EXPECT_FALSE(val.has_value());

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    val = cache.get(0);
    EXPECT_TRUE(val.has_value());
    EXPECT_TRUE(*val == "bbbb") << *val;
}
#endif

TYPED_TEST(cache_fixture, reset)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create(2);

    EXPECT_TRUE(cache.size() == 0);
    EXPECT_TRUE(cache.capacity() == 2);
    cache.emplace(0, 4, 'a');
    cache.emplace(1, 4, 'b');
    cache.emplace(2, 4, 'c');
    EXPECT_TRUE(cache.size() == 2) << cache.size();
    EXPECT_FALSE(cache.contains(0));
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));

    cache.reset(4);
    EXPECT_TRUE(cache.size() == 0) << cache.size();
    EXPECT_TRUE(cache.capacity() == 4) << cache.capacity();

    cache.emplace(0, 4, 'a');
    EXPECT_TRUE(cache.contains(0));
    EXPECT_FALSE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
}

TYPED_TEST(cache_fixture, size)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create();

    EXPECT_TRUE(cache.size() == 0);
    cache.emplace(0, 4, 'b');
    EXPECT_TRUE(cache.size() == 1);
    cache.erase(0);
    EXPECT_TRUE(cache.size() == 0);
}

TYPED_TEST(cache_fixture, update)
{
    using cache_type = TypeParam;
    using lru_cache = typename cache_type::cache;

    lru_cache cache = cache_type::create();

    typename lru_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));
    EXPECT_FALSE(cache.find(1, val));

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    EXPECT_TRUE(cache.find(0, val));
    EXPECT_TRUE(val == "bbbb") << val;

    cache.update(0, std::string(3, 'a'));
    EXPECT_TRUE(cache.find(0, val));
    EXPECT_TRUE(val == "aaa") << val;

    cache.update(1, std::string(3, 'c'));
    EXPECT_TRUE(cache.find(1, val));
    EXPECT_TRUE(val == "ccc") << val;
}

TEST(lru_cache, hit)
{
    using lru_cache = ::wstux::lru::lru_cache<size_t, size_t>;
    using test_data_vector = std::vector<lru_cache::key_type>;

    test_data_vector td(10);
    for (size_t i = 0; i < 10; ++i) {
        td.emplace_back(i);
    }

    size_t hit_count = 0;
    size_t total_count = 0;

    lru_cache cache(10);
    for (size_t i = 0; i < 10; ++i) {
        for (const lru_cache::key_type& key : td) {
            lru_cache::value_type val;
            if (! cache.find(key, val)) {
                EXPECT_TRUE(cache.insert(key, key)) << "failed to insert key '" << key << "'";
            } else {
                EXPECT_TRUE(key == val) << "key('" << key << "') != value('" << val << "'";
                ++hit_count;
            }
            ++total_count;
        }
    }
    EXPECT_TRUE((hit_count + 10) == total_count)
        << "hit_count = " << hit_count << "; total_count = " << total_count;
}

TEST_F(thread_safe_cache_fixture, shards_leak)
{
    using lru_cache = ::wstux::lru::thread_safe_lru_cache<size_t, size_t>;

    lru_cache cache(1, 2);
    EXPECT_TRUE(cache.shards_size() == 1);

    EXPECT_TRUE(cache.insert(0, 3));
    EXPECT_TRUE(cache.insert(1, 4));

    EXPECT_FALSE(cache.contains(0));
    EXPECT_TRUE(cache.contains(1));
}

TEST_F(thread_safe_cache_fixture, hit)
{
    using lru_cache = ::wstux::lru::thread_safe_lru_cache<size_t, size_t>;

    lru_cache cache(30, threads_count());
    init_threads(cache);

    // Waiting until all threads have started.
    while (! is_threads_started()) {}
    // Start work.
    start();
    // Finish threads.
    join_threads();

    const size_t actual_total_count = 10 * threads_count() * count();
    EXPECT_TRUE(actual_total_count == total_count()) << "total_count = " << total_count();
    EXPECT_TRUE(hit_count() > 0) << "hit_count = " << hit_count();
    EXPECT_TRUE(hit_count() < total_count()) << "hit_count = " << hit_count();
}

int main(int /*argc*/, char** /*argv*/)
{
    return RUN_ALL_TESTS();
}

