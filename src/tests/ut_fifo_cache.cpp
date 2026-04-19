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

#include <vector>

#include <gtest/gtest.h>

#include "cache/fifo_cache.h"
#include "cache/thread_safe_fifo_cache.h"

namespace {

////////////////////////////////////////////////////////////////////////////////
// class cache_fixture

template<typename TParam>
class cache_fixture : public ::testing::Test
{};

struct fifo_cache
{
    using cache = ::wstux::cache::fifo::fifo_cache<size_t, std::string>;

    static cache create(size_t cap = 10, size_t shards = 0) { (void)shards; return cache(cap); }
};

struct thread_safe_fifo_cache
{
    using cache = ::wstux::cache::fifo::thread_safe_fifo_cache<size_t, std::string>;

    static cache create(size_t cap = 10, size_t shards = 2) { return cache(cap, shards); }
};

using fifo_types = testing::Types<fifo_cache, thread_safe_fifo_cache>;
TYPED_TEST_SUITE(cache_fixture, fifo_types);

} // <anonymous> namespace

TYPED_TEST(cache_fixture, eviction)
{
    using param_type = TypeParam;
    using cache_type = typename param_type::cache;

    cache_type cache = param_type::create(4, 4);

    EXPECT_FALSE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(0, 4, 'b'));
    EXPECT_TRUE(cache.emplace(1, 4, 'b'));
    EXPECT_TRUE(cache.emplace(2, 4, 'b'));
    EXPECT_TRUE(cache.emplace(3, 4, 'b'));
    EXPECT_TRUE(cache.contains(0));

    EXPECT_TRUE(cache.emplace(4, 4, 'b'));
    EXPECT_TRUE(cache.size() == 4);

    EXPECT_FALSE(cache.contains(0));
    EXPECT_TRUE(cache.contains(1));
    EXPECT_TRUE(cache.contains(2));
    EXPECT_TRUE(cache.contains(3));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
