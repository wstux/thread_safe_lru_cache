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

#ifndef _TESTING_TESTDEFS_H
#define _TESTING_TESTDEFS_H

#undef __PERFORMANCE_TESTS__

#include "testing/details/testdefs_impl.h"
#include "testing/details/tester.h"
#include "testing/testing_interface.h"

/*
 */

#define ASSERT_FALSE(cond)                      \
    if (! (cond)) ;                             \
    else return __FATAL_FAILURE_MESSAGE(cond)

#define ASSERT_TRUE(cond)                       \
    if ((cond)) ;                               \
    else return __FATAL_FAILURE_MESSAGE(cond)

#define EXPECT_FALSE(cond)                      \
    if (! (cond)) ;                             \
    else __FAILURE_MESSAGE(cond)

#define EXPECT_TRUE(cond)                       \
    if ((cond)) ;                               \
    else __FAILURE_MESSAGE(cond)

/*
 */

#define EXPECT_EQ(et, td)                       \
    if (et == td) ;                             \
    else __FAILURE_MESSAGE(cond)

/*
 */

#define ASSERT_THROW(cond, expected_exception)                          \
    if (true) {                                                         \
        bool is_caught = false;                                         \
        try {                                                           \
            (void)(cond);                                               \
        } catch (const expected_exception&) {                           \
            is_caught = true;                                           \
        } catch (...) {                                                 \
            ::testing::details::fatal()                                 \
                << "Expected: " #cond " throws an exception of type "   \
                << #expected_exception "." << std::endl                 \
                << "Actual: it throws a different type." << std::endl;  \
            goto __GOTO_LABEL(__label_test_expect_throw_, __LINE__);    \
        }                                                               \
        if (! is_caught) {                                              \
            ::testing::details::fatal()                                 \
                << "Expected: " #cond " throws an exception of type "   \
                << #expected_exception "." << std::endl                 \
                << "Actual: it throws nothing." << std::endl;           \
            goto __GOTO_LABEL(__label_test_expect_throw_, __LINE__);    \
        }                                                               \
    } else                                                              \
        __GOTO_LABEL(__label_test_expect_throw_, __LINE__):             \
            return __FATAL_FAILURE_MESSAGE(cond)

#define ASSERT_NO_THROW(cond)                                           \
    if (true) {                                                         \
        try {                                                           \
            (void)(cond);                                               \
        } catch (...) {                                                 \
            goto __GOTO_LABEL(__label_test_no_throw_, __LINE__);        \
        }                                                               \
    } else                                                              \
        __GOTO_LABEL(__label_test_no_throw_, __LINE__):                 \
            return __FATAL_FAILURE_MESSAGE(cond)                        \
                << "Expected: " #cond " doesn't throw an "              \
                << "exception." << std::endl << "Actual: it throws."    \
                << std::endl

#define EXPECT_THROW(cond, expected_exception)                          \
    if (true) {                                                         \
        bool is_caught = false;                                         \
        try {                                                           \
            (void)(cond);                                               \
        } catch (const expected_exception&) {                           \
            is_caught = true;                                           \
        } catch (...) {                                                 \
            ::testing::details::fail()                                  \
                << "Expected: " #cond " throws an exception of type "   \
                << #expected_exception "." << std::endl                 \
                << "Actual: it throws a different type." << std::endl;  \
            goto __GOTO_LABEL(__label_test_expect_throw_, __LINE__);    \
        }                                                               \
        if (! is_caught) {                                              \
            ::testing::details::fatal()                                 \
                << "Expected: " #cond " throws an exception of type "   \
                << #expected_exception "." << std::endl                 \
                << "Actual: it throws nothing." << std::endl;           \
            goto __GOTO_LABEL(__label_test_expect_throw_, __LINE__);    \
        }                                                               \
    } else                                                              \
        __GOTO_LABEL(__label_test_expect_throw_, __LINE__):             \
            return __FAILURE_MESSAGE(cond)

#define EXPECT_NO_THROW(cond)                                           \
    if (true) {                                                         \
        try {                                                           \
            (void)(cond);                                               \
        } catch (...) {                                                 \
            goto __GOTO_LABEL(__label_test_no_throw_, __LINE__);        \
        }                                                               \
    } else                                                              \
        __GOTO_LABEL(__label_test_no_throw_, __LINE__):                 \
            __FAILURE_MESSAGE(cond)                                     \
                << "Expected: " #cond " doesn't throw an "              \
                << "exception." << std::endl << "Actual: it throws."    \
                << std::endl

/*
 */

#define TEST(case_name, test_name)              \
    __TEST_IMPL(case_name, test_name)

#define TEST_F(test_fixture, test_name)         \
    __TEST_F_IMPL(test_fixture, test_name)

#define TYPED_TEST_SUITE(case_name, types)      \
    __INIT_TYPED_TEST_SUITE(case_name, types)

#define TYPED_TEST(case_name, types)            \
    __TYPED_TEST_IMPL(case_name, types)

#define RUN_ALL_TESTS() ::testing::details::tester::run_all_tests()

#endif /* _TESTING_TESTDEFS_H */

