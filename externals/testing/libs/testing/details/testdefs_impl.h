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

#ifndef _TESTING_TESTDEFS_IMPL_H
#define _TESTING_TESTDEFS_IMPL_H

/*
 */

#define __FATAL_MESSAGE_IMPL(cond)                              \
    ::testing::details::fatal()                                 \
        << __FILE__ << ":" << __LINE__ << ":" << std::endl      \
        << "    " << __PRETTY_FUNCTION__ << ":" << std::endl    \
        << "Failure condition '" << #cond << "'" << std::endl

#define __MESSAGE_IMPL(cond)                                    \
    ::testing::details::fail()                                  \
        << __FILE__ << ":" << __LINE__ << ":" << std::endl      \
        << "    " << __PRETTY_FUNCTION__ << ":" << std::endl    \
        << "Failure condition '" << #cond << "'" << std::endl

#define __FAILURE_MESSAGE(cond)                                 \
    ::testing::details::report_helper() = __MESSAGE_IMPL(cond)

#define __FATAL_FAILURE_MESSAGE(cond)                           \
    ::testing::details::report_helper() = __FATAL_MESSAGE_IMPL(cond)

/*
 */

#define __CVT_TO_STRING(x)  []() -> std::string { return #x; }()

#define __GOTO_LABEL_IMPL(label, line)  label ## line
#define __GOTO_LABEL(label, line)       __GOTO_LABEL_IMPL(label, line)

#define __TEST_CASE_NAME(suite_name)                \
    __g_private_##suite_name##_test

#define __TEST_CLASS_NAME(suite_name, test_name)    \
    suite_name##_##test_name##_test

#define __TEST_INSERT_RES(case_name, test_name)     \
    __##case_name##_##test_name##_res

#define __TEST_TYPE_PARAMS(suite_name)              \
    __test_type_##suite_name##_param

/*
 *  \brief  Implementation for TEST macro.
 */

#define __TEST_IMPL(case_name, test_name)                                   \
    class __TEST_CLASS_NAME(case_name, test_name)                           \
        : public ::testing::details::itest_suite                            \
    {                                                                       \
    public:                                                                 \
        virtual void test_body() override final;                            \
    };                                                                      \
    [[maybe_unused]] static bool __TEST_INSERT_RES(case_name, test_name) =  \
        ::testing::details::tester::insert(                                 \
            __CVT_TO_STRING(case_name), __CVT_TO_STRING(test_name),         \
            std::make_shared<__TEST_CLASS_NAME(case_name, test_name)>());   \
    void __TEST_CLASS_NAME(case_name, test_name)::test_body()

/*
 *  \brief  Implementation for TEST_F macro.
 */

#define __TEST_F_IMPL(suite_name, test_name)                                \
    class __TEST_CLASS_NAME(suite_name, test_name) : public suite_name      \
    {                                                                       \
    public:                                                                 \
        using decorator = ::testing::details::suite_decorator<              \
                    __TEST_CLASS_NAME(suite_name, test_name)>;              \
        using suite_ptr = ::testing::details::itest_suite::ptr;             \
        __TEST_CLASS_NAME(suite_name, test_name)() {}                       \
        static suite_ptr make_suite_ptr()                                   \
        {                                                                   \
            std::shared_ptr<__TEST_CLASS_NAME(suite_name, test_name)> p_ =  \
                std::make_shared<__TEST_CLASS_NAME(suite_name,              \
                                                   test_name)>();           \
            return std::make_shared<decorator>(p_);                         \
        }                                                                   \
    private:                                                                \
        virtual void test_body();                                           \
    };                                                                      \
    [[maybe_unused]] static bool __TEST_INSERT_RES(suite_name, test_name) = \
        ::testing::details::tester::insert(                                 \
            __CVT_TO_STRING(suite_name), __CVT_TO_STRING(test_name),        \
            __TEST_CLASS_NAME(suite_name, test_name)::make_suite_ptr());    \
    void __TEST_CLASS_NAME(suite_name, test_name)::test_body()

/*
 *  \brief  Implementation for TYPED_TEST macro.
 */

#define __INIT_TYPED_TEST_SUITE(case_name, types)                           \
    using __TEST_TYPE_PARAMS(case_name) = types

# define __TYPED_TEST_IMPL(case_name, test_name)                            \
    template<typename TTypeParam>                                           \
    class __TEST_CLASS_NAME(case_name, test_name)                           \
        : public case_name<TTypeParam>                                      \
    {                                                                       \
    public:                                                                 \
        using __decorator = ::testing::details::suite_decorator<            \
                    __TEST_CLASS_NAME(case_name, test_name)>;               \
    private:                                                                \
        using TestFixture = case_name<TTypeParam>;                          \
        using TypeParam = TTypeParam;                                       \
        virtual void test_body();                                           \
    };                                                                      \
    [[maybe_unused]] static bool __TEST_INSERT_RES(case_name, test_name) =  \
        ::testing::details::tester::insert_typed_case<                      \
                    __TEST_CLASS_NAME(case_name, test_name),                \
                    typename __TEST_TYPE_PARAMS(case_name)::type>(          \
            __CVT_TO_STRING(case_name), __CVT_TO_STRING(test_name));        \
    template<typename TTypeParam>                                           \
    void __TEST_CLASS_NAME(case_name, test_name)<TTypeParam>::test_body()

#endif /* _TESTING_TESTDEFS_IMPL_H */

