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

#include <deque>
#include <list>
#include <vector>

#include "testing/perfdefs.h"

class test_env : public ::testing::Environment
{
public:
    virtual void SetUp() override
    {
        PERF_MESSAGE() << "test_env::SetUp()";
    }
};

class test_fixture : public ::testing::Test
{
public:
    virtual void SetUp() override {}
};

template<typename TType>
class typed_fixture : public ::testing::Test
{
public:
    virtual void SetUp() override
    {
        PERF_MESSAGE() << "typed_fixture::SetUp()";
    }
};

using types = testing::Types<std::vector<size_t>, std::list<size_t>,
                             std::deque<size_t>>;
TYPED_PERF_TEST_SUITE(typed_fixture, types);

PERF_TEST_F(test_fixture, perf)
{
    PERF_INIT_TIMER(test_perf);

    std::vector<size_t> v;
    for (size_t i = 0; i < 10000; ++i) {
        v.emplace_back(i);
    }
    size_t dummy = 0;
    for (size_t i = 0; i < 10000; ++i) {
        PERF_START_TIMER(test_perf);
        dummy += v[i];
        PERF_PAUSE_TIMER(test_perf);
    }
    PERF_MESSAGE() << "test_perf = " << PERF_TIMER_MSECS(test_perf) << " ms";
}

TYPED_PERF_TEST(typed_fixture, perf)
{
    PERF_INIT_TIMER(test);

    TypeParam v;
    for (size_t i = 0; i < 10000; ++i) {
        v.emplace_back(i);
    }
    size_t dummy = 0;
    PERF_START_TIMER(test);
    for (typename TypeParam::const_iterator it = v.cbegin(); it != v.cend(); ++it) {
        dummy += *it;
    }
    PERF_PAUSE_TIMER(test);
}

int main(int /*argc*/, char** /*argv*/)
{
    ::testing::AddGlobalTestEnvironment(new test_env());
    return RUN_ALL_PERF_TESTS();
}
