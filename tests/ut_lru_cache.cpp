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

#include <testing/testdefs.h>

#include "lru_cache/lru_cache.h"

namespace {

using lru_cache = ::wstux::cnt::lru_cache<size_t, size_t>;
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

    static size_t count() { return 10; }
    static const test_data_vector& test_data() { return m_test_data; }

private:
    static test_data_vector m_test_data;
};

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

        hit_count = {0};
        total_count = {0};
    }

protected:
    size_t hit_count;
    size_t total_count;
};

} // <anonymous> namespace

TEST_F(cache_fixture, insert)
{
    const test_data_vector& td = cache_env::test_data();
    size_t hit_count = 0;
    size_t total_count = 0;

    lru_cache cache(cache_env::count());
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
    EXPECT_TRUE((hit_count + cache_env::count()) == total_count)
        << "hit_count = " << hit_count << "; total_count = " << total_count;
}

TEST_F(cache_fixture, update)
{
    const test_data_vector& td = cache_env::test_data();
    size_t hit_count = 0;
    size_t total_count = 0;

    lru_cache cache(cache_env::count());
    for (size_t i = 0; i < 10; ++i) {
        for (const lru_cache::key_type& key : td) {
            lru_cache::value_type val;
            if (! cache.find(key, val)) {
                cache.update(key, key);
            } else {
                EXPECT_TRUE(key == val) << "key('" << key << "') != value('" << val << "'";
                ++hit_count;
            }
            ++total_count;
        }
    }
    EXPECT_TRUE((hit_count + cache_env::count()) == total_count)
        << "hit_count = " << hit_count << "; total_count = " << total_count;
}

int main(int /*argc*/, char** /*argv*/)
{
    ::testing::AddGlobalTestEnvironment(new cache_env());
    return RUN_ALL_TESTS();
}
