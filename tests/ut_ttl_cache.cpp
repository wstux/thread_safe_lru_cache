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

#include "ttl/ttl_cache.h"

namespace {

////////////////////////////////////////////////////////////////////////////////
// class cache_fixture

template<typename TTtlCache>
class cache_fixture : public ::testing::Test
{};

struct ttl_cache
{
    using cache = ::wstux::ttl::ttl_cache<size_t, std::string>;

    static cache create(size_t cap = 10, size_t shards = 0) { (void)shards; return cache(900, cap); }
};

using ttl_types = testing::Types<ttl_cache>;
TYPED_TEST_SUITE(cache_fixture, ttl_types);

} // <anonymous> namespace

TYPED_TEST(cache_fixture, contains)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    typename ttl_cache::value_type val;
    EXPECT_FALSE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_TRUE(cache.contains(0));
}

TYPED_TEST(cache_fixture, contains_expired)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    typename ttl_cache::value_type val;
    EXPECT_FALSE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(cache.contains(0));
}

TYPED_TEST(cache_fixture, contains_touch)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create(4);

    typename ttl_cache::value_type val;
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

TYPED_TEST(cache_fixture, contains_touch_expired)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create(4);

    typename ttl_cache::value_type val;
    EXPECT_FALSE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_TRUE(cache.emplace(1, 4, 'b'));
    EXPECT_TRUE(cache.emplace(2, 4, 'b'));
    EXPECT_TRUE(cache.emplace(3, 4, 'b'));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(5, 4, 'b'));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(cache.contains(0));
    EXPECT_FALSE(cache.contains(1));
}

TYPED_TEST(cache_fixture, emplace)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    typename ttl_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_TRUE(cache.find(0, val));
    EXPECT_TRUE(val == "bbbb") << val;
}

TYPED_TEST(cache_fixture, emplace_expired)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    typename ttl_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(cache.find(0, val));
}

TYPED_TEST(cache_fixture, insert)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    typename ttl_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    EXPECT_TRUE(cache.find(0, val));
    EXPECT_TRUE(val == "bbbb") << val;
}

TYPED_TEST(cache_fixture, insert_expired)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    typename ttl_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(cache.find(0, val));
}

TYPED_TEST(cache_fixture, multi_insert)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create(1);

    typename ttl_cache::value_type val;

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

TYPED_TEST(cache_fixture, multi_insert_expired)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create(1);

    typename ttl_cache::value_type val;

    for (size_t i = 0; i < std::numeric_limits<int16_t>::max(); ++i) {
        EXPECT_TRUE(cache.insert(i, std::string(4, 'b')));
    }

    EXPECT_TRUE(cache.insert(std::numeric_limits<int32_t>::max(), std::string(4, 'b')));
    for (size_t i = 0; i < std::numeric_limits<int16_t>::max(); ++i) {
        EXPECT_FALSE(cache.find(i, val));
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(cache.find(std::numeric_limits<int32_t>::max(), val));
}

TYPED_TEST(cache_fixture, empty)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    EXPECT_TRUE(cache.empty());
    cache.emplace(0, 4, 'b');
    EXPECT_FALSE(cache.empty());
}

TYPED_TEST(cache_fixture, erase)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    EXPECT_TRUE(cache.empty());
    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    EXPECT_FALSE(cache.empty());

    typename ttl_cache::value_type val;
    EXPECT_TRUE(cache.find(0, val));

    cache.erase(0);
    EXPECT_TRUE(cache.empty());
    EXPECT_FALSE(cache.find(0, val));
}

TYPED_TEST(cache_fixture, erase_expired)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    EXPECT_TRUE(cache.empty());
    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(cache.empty());

    typename ttl_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    cache.erase(0);
    EXPECT_TRUE(cache.empty());
    EXPECT_FALSE(cache.find(0, val));
}

#if defined(_THREAD_SAFE_TTL_CACHE_ENABLE_OPTIONAL)
TYPED_TEST(cache_fixture, get)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    std::optional<typename ttl_cache::value_type> val;
    val = cache.get(0);
    EXPECT_FALSE(val.has_value());

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    val = cache.get(0);
    EXPECT_TRUE(val.has_value());
    EXPECT_TRUE(*val == "bbbb") << *val;
}

TYPED_TEST(cache_fixture, get_expired)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    std::optional<typename ttl_cache::value_type> val;
    val = cache.get(0);
    EXPECT_FALSE(val.has_value());

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    val = cache.get(0);
    EXPECT_FALSE(val.has_value());
}
#endif

TYPED_TEST(cache_fixture, reset)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create(2);

    EXPECT_TRUE(cache.size() == 0);
    EXPECT_TRUE(cache.capacity() == 2);
    cache.emplace(0, 4, 'a');
    cache.emplace(1, 4, 'b');
    cache.emplace(2, 4, 'c');
    EXPECT_TRUE(cache.size() == 2) << cache.size();
    EXPECT_FALSE(cache.contains(0));
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));

    cache.reset(900, 4);
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
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    EXPECT_TRUE(cache.size() == 0);
    cache.emplace(0, 4, 'b');
    EXPECT_TRUE(cache.size() == 1);
    cache.erase(0);
    EXPECT_TRUE(cache.size() == 0);
}

TYPED_TEST(cache_fixture, update)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    typename ttl_cache::value_type val;
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

TYPED_TEST(cache_fixture, update_expired)
{
    using cache_type = TypeParam;
    using ttl_cache = typename cache_type::cache;

    ttl_cache cache = cache_type::create();

    typename ttl_cache::value_type val;
    EXPECT_FALSE(cache.find(0, val));
    EXPECT_FALSE(cache.find(1, val));

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(cache.find(0, val));

    cache.update(0, std::string(3, 'a'));
    EXPECT_TRUE(cache.find(0, val));

    cache.update(0, std::string(3, 'd'));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_FALSE(cache.find(0, val));

    cache.update(1, std::string(3, 'c'));
    EXPECT_TRUE(cache.find(1, val));
    EXPECT_TRUE(val == "ccc") << val;
}

TEST(ttl_cache, hit)
{
    using ttl_cache = ::wstux::ttl::ttl_cache<size_t, size_t>;
    using test_data_vector = std::vector<ttl_cache::key_type>;

    test_data_vector td(10);
    for (size_t i = 0; i < 10; ++i) {
        td.emplace_back(i);
    }

    size_t hit_count = 0;
    size_t total_count = 0;

    ttl_cache cache(10, 10);
    for (size_t i = 0; i < 10; ++i) {
        for (const ttl_cache::key_type& key : td) {
            ttl_cache::value_type val;
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

int main(int /*argc*/, char** /*argv*/)
{
    return RUN_ALL_TESTS();
}

