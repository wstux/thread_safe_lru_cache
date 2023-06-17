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

#ifndef _TESTING_TEST_UTILS_H
#define _TESTING_TEST_UTILS_H

#include <iostream>
#include <memory>
#include <type_traits>

#include "testing/details/common_test_utils.h"
#include "testing/details/timer.h"
#include "testing/details/typed_test_utils.h"

namespace testing {
namespace details {

class ienv
{
public:
    using ptr = std::shared_ptr<ienv>;

    virtual ~ienv() {}
    virtual bool set_up() = 0;
    virtual bool tear_down() = 0;
};

class itest_suite
{
public:
    using ptr = std::shared_ptr<itest_suite>;

    virtual ~itest_suite() {}
    virtual void test_body() = 0;
};

template<typename TType>
class perf_decorator final : public itest_suite
{
public:
    using ptr = std::shared_ptr<perf_decorator>;

    explicit perf_decorator(const std::shared_ptr<TType>& p_test)
        : m_p_test(p_test)
    {}

    virtual void test_body() override { m_p_test->__run_perf(); }

private:
    std::shared_ptr<TType> m_p_test;
};

template<typename TType>
class suite_decorator final : public itest_suite
{
public:
    using ptr = std::shared_ptr<suite_decorator>;

    explicit suite_decorator(const std::shared_ptr<TType>& p_test)
        : m_p_test(p_test)
    {}

    virtual void test_body() override { m_p_test->__run(); }

private:
    std::shared_ptr<TType> m_p_test;
};

class test_failer final
{
public:
    using ptr = std::shared_ptr<test_failer>;

    void init_case()
    {
        m_is_ut_failed = false;
        m_is_ut_fatal_failed = false;
    }

    bool is_case_failed() { return m_is_ut_failed; }

    void fail() { m_is_ut_failed = true; }

    void fatal() { m_is_ut_fatal_failed = m_is_ut_failed = true; }

    bool is_fatal() { return m_is_ut_fatal_failed; }

    static test_failer& get_instance()
    {
        static bool is_once = true;
        if (is_once) {
            is_once = false;
            m_p_instance.reset(new test_failer());
        }
        return *m_p_instance.get();
    }

private:
    bool m_is_ut_failed = false;
    bool m_is_ut_fatal_failed = false;

    static std::unique_ptr<test_failer> m_p_instance;
};

std::unique_ptr<test_failer> test_failer::m_p_instance = nullptr;

inline void init_case()      { test_failer::get_instance().init_case(); }
inline bool is_case_failed() { return test_failer::get_instance().is_case_failed(); }
inline bool is_fatal()       { return test_failer::get_instance().is_fatal(); }

template<typename TEnv>
class env_decorator final : public ienv
{
public:
    using ptr = std::shared_ptr<env_decorator>;

    explicit env_decorator(TEnv* p_env)
        : m_p_env(p_env)
    {}

    virtual ~env_decorator() {}

    virtual bool set_up() override
    {
        init_case();
        m_p_env->SetUp();
        return ! testing::details::is_fatal();
    }

    virtual bool tear_down() override
    {
        init_case();
        m_p_env->TearDown();
        return ! testing::details::is_fatal();
    }

private:
    std::shared_ptr<TEnv> m_p_env;
};

template<typename TTester, template<typename> class TCase, typename TTypes>
class typed_test_inserter
{
public:
    static bool insert(TTester& tester, const std::string& case_name,
                       const std::string& test_name, size_t level)
    {
        using head_t = typename TTypes::head;
        using tail_t = typename TTypes::tail;
        using case_t = TCase<head_t>;
        using decorator = typename case_t::__decorator;

        if (std::is_same<head_t, none_t>::value) {
            return true;
        }

        std::shared_ptr<case_t> p_ptr = std::make_shared<case_t>();
        typename decorator::ptr p_decorator = std::make_shared<decorator>(p_ptr);

        const std::string test_case_name = "[" + std::to_string(level) + "] " +
            case_name + "<" + canon_type_name<head_t>() + ">";
        bool result = tester.insert(test_case_name, test_name, p_decorator);
        if (! result) {
            return false;
        }

        return typed_test_inserter<TTester, TCase, tail_t>::insert(tester, case_name, test_name, ++level);
    }
};

template<typename TTester, template<typename> class TCase>
class typed_test_inserter<TTester, TCase, type_0>
{
public:
    static bool insert(TTester&, const std::string&, const std::string&, size_t)
    {
        return true;
    }
};

} // namespace details
} // namespace testing

#endif /* _TESTING_TEST_UTILS_H */

