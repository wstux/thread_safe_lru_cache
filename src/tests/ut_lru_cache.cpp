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

#include "cache/lru_cache.h"

TEST(lru_cache, hit)
{
    using lru_cache = ::wstux::cache::lru::lru_cache<size_t, size_t>;
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

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
