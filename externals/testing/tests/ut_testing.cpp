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

#include "testing/testdefs.h"
#include "testing/utils.h"

class test_env : public ::testing::Environment
{
public:
    virtual void SetUp() override
    {
        EXPECT_TRUE(13 == 1) << "expected fail";
        EXPECT_TRUE(::testing::utils::cpu_time_msecs_self() < 0)
            << "expected fail; " << ::testing::utils::cpu_time_msecs_self();
    }
};

class test_fixture_1 : public ::testing::Test
{
public:
    virtual void SetUp() override { EXPECT_TRUE(1 == 1); }
};

class test_fixture_2 : public ::testing::Test
{
public:
    virtual void SetUp() override { EXPECT_TRUE(1 == 2) << "expected fail"; }
};

template<typename TType>
class typed_fixture_1 : public ::testing::Test
{
public:
    virtual void SetUp() override { EXPECT_TRUE(1 == 1); }
};

template<typename TType>
class typed_fixture_2 : public ::testing::Test
{
public:
    virtual void SetUp() override { EXPECT_TRUE(1 == 1); }
};

using types_1 = testing::Types<uint8_t, uint16_t, uint32_t>;
TYPED_TEST_SUITE(typed_fixture_1, types_1);

using types_2 = testing::Types<uint8_t, uint16_t, uint32_t>;
TYPED_TEST_SUITE(typed_fixture_2, types_2);

TEST(case_name_1, assert_true)
{
    ASSERT_TRUE(2 == 1) << "expected fail";
    EXPECT_TRUE(2 == 1) << "expected fail";
}

TEST(case_name_1, assert_false)
{
    ASSERT_FALSE(1 == 1) << "expected fail";
    EXPECT_TRUE(2 == 1) << "expected fail";
}

TEST(case_name_1, expect_false)
{
    EXPECT_FALSE(1 == 2);
}

TEST(case_name_1, expect_true)
{
    EXPECT_TRUE(1 == 1);
}

TEST(case_name_1, assert_throw)
{
    std::function<void()> throw_fn = []() { throw std::runtime_error(""); };

    ASSERT_THROW(throw_fn, std::runtime_error);
    ASSERT_THROW(1 == 1, std::runtime_error) << "expected fail";
    EXPECT_TRUE(2 == 1) << "expected fail";
}

TEST(case_name_1, assert_no_throw)
{
    std::function<void()> throw_fn = []() { throw std::runtime_error(""); };

    ASSERT_NO_THROW(1 == 1);
    ASSERT_NO_THROW(throw_fn) << "expected fail";
    EXPECT_TRUE(2 == 1) << "expected fail";
}

TEST(case_name_1, expect_throw)
{
    std::function<void()> throw_fn = []() { throw std::runtime_error(""); };

    EXPECT_THROW(throw_fn, std::runtime_error);
    EXPECT_THROW(1 == 1, std::runtime_error) << "expected fail";
}

TEST(case_name_1, expect_no_throw)
{
    std::function<void()> throw_fn = []() { throw std::runtime_error(""); };

    EXPECT_NO_THROW(1 == 1);
    EXPECT_NO_THROW(throw_fn) << "expected fail";
}

TEST(case_name_2, expect_false)
{
    EXPECT_FALSE(1 == 2);
}

TEST(case_name_2, expect_eq)
{
    EXPECT_EQ(1, 1);
}

TEST_F(test_fixture_1, expect_true)
{
    EXPECT_TRUE(1 == 1);
}

TEST_F(test_fixture_1, expect_false)
{
    EXPECT_FALSE(1 == 2);
}

TEST_F(test_fixture_2, expect_true)
{
    EXPECT_TRUE(1 == 1);
}

TYPED_TEST(typed_fixture_1, expect_true)
{
    EXPECT_TRUE(1 == 1);
}

TYPED_TEST(typed_fixture_1, expect_false)
{
    EXPECT_FALSE(1 == 2);
}

TYPED_TEST(typed_fixture_2, test_name_1)
{
    EXPECT_TRUE(1 == 1);
}

int main(int /*argc*/, char** /*argv*/)
{
    ::testing::AddGlobalTestEnvironment(new test_env());
    return RUN_ALL_TESTS();
}

