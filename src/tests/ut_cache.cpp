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

#include <gtest/gtest.h>

#include "cache/fifo_cache.h"
#include "cache/lru_cache.h"
#include "cache/rr_cache.h"
#include "cache/ttl_cache.h"
#include "cache/thread_safe_fifo_cache.h"
#include "cache/thread_safe_lru_cache.h"
#include "cache/thread_safe_rr_cache.h"
#include "cache/thread_safe_ttl_cache.h"

namespace {

////////////////////////////////////////////////////////////////////////////////
// class cache_fixture

template<typename TParam>
class cache_fixture : public ::testing::Test
{};

struct fifo_cache
{
    using cache = ::wstux::cache::fifo::fifo_cache<size_t, std::string>;

    static constexpr bool is_lru_base = true;

    static cache create(size_t cap = 10) { return cache(cap); }
    static void reset(cache& c, size_t cap) { c.reset(cap); }
};

struct lru_cache
{
    using cache = ::wstux::cache::lru::lru_cache<size_t, std::string>;

    static constexpr bool is_lru_base = true;

    static cache create(size_t cap = 10) { return cache(cap); }
    static void reset(cache& c, size_t cap) { c.reset(cap); }
};

struct rr_cache
{
    using cache = ::wstux::cache::rr::rr_cache<size_t, std::string>;

    static constexpr bool is_lru_base = false;

    static cache create(size_t cap = 10) { return cache(cap); }
    static void reset(cache& c, size_t cap) { c.reset(cap); }
};

struct ttl_cache
{
    using cache = ::wstux::cache::ttl::ttl_cache<size_t, std::string>;

    static constexpr bool is_lru_base = true;

    static cache create(size_t cap = 10) { return cache(900, cap); }
    static void reset(cache& c, size_t cap) { c.reset(900, cap); }
};

struct thread_safe_fifo_cache
{
    using cache = ::wstux::cache::fifo::thread_safe_fifo_cache<size_t, std::string>;

    static constexpr bool is_lru_base = true;

    static cache create(size_t cap = 10) { return cache(cap, 2); }
    static void reset(cache& c, size_t cap) { c.reset(cap); }
};

struct thread_safe_lru_cache
{
    using cache = ::wstux::cache::lru::thread_safe_lru_cache<size_t, std::string>;

    static constexpr bool is_lru_base = true;

    static cache create(size_t cap = 10) { return cache(cap, 2); }
    static void reset(cache& c, size_t cap) { c.reset(cap); }
};

struct thread_safe_rr_cache
{
    using cache = ::wstux::cache::rr::thread_safe_rr_cache<size_t, std::string>;

    static constexpr bool is_lru_base = false;

    static cache create(size_t cap = 10) { return cache(cap, 2); }
    static void reset(cache& c, size_t cap) { c.reset(cap); }
};

struct thread_safe_ttl_cache
{
    using cache = ::wstux::cache::ttl::thread_safe_ttl_cache<size_t, std::string>;

    static constexpr bool is_lru_base = true;

    static cache create(size_t cap = 10) { return cache(900, cap, 2); }
    static void reset(cache& c, size_t cap) { c.reset(900, cap); }
};

using cache_types = testing::Types<fifo_cache,
                                   lru_cache,
                                   rr_cache,
                                   ttl_cache,
                                   thread_safe_fifo_cache,
                                   thread_safe_lru_cache,
                                   thread_safe_rr_cache,
                                   thread_safe_ttl_cache>;
TYPED_TEST_SUITE(cache_fixture, cache_types);

} // <anonymous> namespace

TYPED_TEST(cache_fixture, contains)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create();

    EXPECT_FALSE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_TRUE(cache.contains(0));
}

TYPED_TEST(cache_fixture, emplace)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create();

    typename cache_type::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_TRUE(cache.find(0, val));
    EXPECT_TRUE(val == "bbbb") << val;
}

TYPED_TEST(cache_fixture, DISABLED_zero_size)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create();

    typename cache_type::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_FALSE(cache.find(0, val)) << cache.size();;
}

TYPED_TEST(cache_fixture, insert)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create();

    typename cache_type::value_type val;
    EXPECT_FALSE(cache.find(0, val));

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    EXPECT_TRUE(cache.find(0, val));
    EXPECT_TRUE(val == "bbbb") << val;
}

TYPED_TEST(cache_fixture, multi_insert)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create(1);

    typename cache_type::value_type val;

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
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create();

    EXPECT_TRUE(cache.empty());
    cache.emplace(0, 4, 'b');
    EXPECT_FALSE(cache.empty());
}

TYPED_TEST(cache_fixture, erase)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create();

    EXPECT_TRUE(cache.empty());
    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    EXPECT_FALSE(cache.empty());

    typename cache_type::value_type val;
    EXPECT_TRUE(cache.find(0, val));

    cache.erase(0);
    EXPECT_TRUE(cache.empty());
    EXPECT_FALSE(cache.find(0, val));
}

TYPED_TEST(cache_fixture, get)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create();

    std::optional<typename cache_type::value_type> val;
    val = cache.get(0);
    EXPECT_FALSE(val.has_value());

    EXPECT_TRUE(cache.insert(0, std::string(4, 'b')));
    val = cache.get(0);
    EXPECT_TRUE(val.has_value());
    EXPECT_TRUE(*val == "bbbb") << *val;
}

TYPED_TEST(cache_fixture, reset)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create(2);

    EXPECT_TRUE(cache.size() == 0);
    EXPECT_TRUE(cache.capacity() == 2);
    cache.emplace(0, 4, 'a');
    cache.emplace(1, 4, 'b');
    cache.emplace(2, 4, 'c');
    EXPECT_TRUE(cache.size() == 2) << cache.size();
    if (param_type::is_lru_base) {
        EXPECT_FALSE(cache.contains(0));
        EXPECT_TRUE(cache.contains(1));
    }
    EXPECT_TRUE(cache.contains(2));

    param_type::reset(cache, 4);
    EXPECT_TRUE(cache.size() == 0) << cache.size();
    EXPECT_TRUE(cache.capacity() == 4) << cache.capacity();

    cache.emplace(0, 4, 'a');
    EXPECT_TRUE(cache.contains(0));
    EXPECT_FALSE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
}

TYPED_TEST(cache_fixture, size)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create();

    EXPECT_TRUE(cache.size() == 0);
    cache.emplace(0, 4, 'b');
    EXPECT_TRUE(cache.size() == 1);
    cache.erase(0);
    EXPECT_TRUE(cache.size() == 0);
}

TYPED_TEST(cache_fixture, update)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create();

    typename cache_type::value_type val;
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

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
