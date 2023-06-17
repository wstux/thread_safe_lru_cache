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

#ifndef _TESTING_TESTER_H
#define _TESTING_TESTER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <vector>

#include "testing/details/test_utils.h"
#include "testing/details/timer.h"
#include "testing/details/typed_test_utils.h"

namespace testing {
namespace details {

class test_suite final
{
public:
    using ptr = std::shared_ptr<test_suite>;
    using case_ptr = itest_suite::ptr;
    using test_descr_t = std::pair<std::string, case_ptr>;
    using test_list_t = std::vector<test_descr_t>;

    explicit test_suite(const std::string& suite_name)
        : m_suite_name(suite_name)
    {}

    bool insert_case(const std::string& test_name, const case_ptr& p_case)
    {
        m_tests.emplace_back(test_name, p_case);
        return true;
    }

    int run_all_cases() const
    {
        const std::string prefix = std::to_string(m_tests.size()) + " tests from " + m_suite_name;

        std::cout << "[----------] " << prefix << std::endl;
        timer suite_sw(true);
        const int failed_count = run_tests();
        const double suite_ms = suite_sw.value_ms();
        std::cout << "[----------] " << prefix << " (" << suite_ms << " ms)" << std::endl << std::endl;

        return failed_count;
    }

    size_t tests_count() const { return m_tests.size(); }

private:
    static bool is_disabled(const std::string& test_name)
    {
        return test_name.rfind("DISABLED", 0) == 0;
    }

    int run_tests() const
    {
        int failed_count = 0;
        for (const test_descr_t& descr : m_tests) {
            const std::string& test_name = descr.first;
            const case_ptr& p_suite = descr.second;

            if (is_disabled(test_name)) {
                std::cout << "[DISABLED  ] " << m_suite_name << "." << test_name << std::endl;
                continue;
            }

            init_case();
            std::cout << "[RUN       ] " << m_suite_name << "." << test_name << std::endl;

            timer test_sw(true);
            p_suite->test_body();
            const double test_ms = test_sw.value_ms();

            failed_count += (is_case_failed() != 0) ? 1 : 0;
            const std::string res_str = (is_case_failed() == 0) ? "[       OK ] " : "[   FAILED ] ";
            std::cout << res_str << m_suite_name << "." << test_name << " (" << test_ms << " ms)" << std::endl;
        }

        return failed_count;
    }

private:
    const std::string m_suite_name;
    test_list_t m_tests;
};

class tester final
{
    using case_ptr = itest_suite::ptr;
    using suite_ptr = test_suite::ptr;

public:
    virtual ~tester() {}

    void add_env(ienv::ptr p_env) { m_envs.emplace_back(p_env); }

    bool insert_test(const std::string& case_name, const std::string& test_name,
                     const case_ptr& p_suite)
    {
        const size_t idx = gen_test_id(case_name);
        return m_tests[idx]->insert_case(test_name, p_suite);
    }

    int run_tests() const
    {
        const size_t tests_cnt = tests_count();
        size_t failed_count = 0;

        std::cout << "[==========] Setup environments." << std::endl;
        for (const ienv::ptr& p_env : m_envs) {
            if (! p_env->set_up()) {
                return 1;
            }
        }

        std::cout << "[==========] Running " << tests_cnt << " tests from "
                  << m_tests.size() << " test suits." << std::endl;
        timer total_sw(true);
        for (const suite_ptr& p_test : m_tests) {
            failed_count += p_test->run_all_cases();
        }

        const double total_ms = total_sw.value_ms();
        std::cout << "[==========] " << tests_cnt << " tests from " << m_tests.size()
                  << " test suits ran (" << total_ms << " ms)." << std::endl;
        if (failed_count != 0) {
            std::cout << "[  FAILED  ] " << failed_count << " tests." << std::endl;
        }
        std::cout << "[  PASSED  ] " << tests_cnt << " tests." << std::endl;

        std::cout << "[==========] Teardown environments." << std::endl;
        for (const ienv::ptr& p_env : m_envs) {
            if (! p_env->tear_down()) {
                return 1;
            }
        }

        return (failed_count == 0) ? 0 : 1;
    }

    static bool insert(const std::string& case_name, const std::string& test_name,
                       const itest_suite::ptr& p_suite)
    {
        return get_instance().insert_test(case_name, test_name, p_suite);
    }

    template<template<typename> class TCase, typename TTypes>
    static bool insert_typed_case(const std::string& case_name,
                                  const std::string& test_name)
    {
        return typed_test_inserter<tester, TCase, TTypes>::insert(get_instance(),
                                case_name, test_name, 0);
    }

    static tester& get_instance()
    {
        static bool is_once = true;
        if (is_once) {
            is_once = false;
            m_p_instance.reset(new tester());
        }
        return *m_p_instance.get();
    }

    static int run_all_tests() { return get_instance().run_tests(); }

private:
    tester() = default;

    size_t tests_count() const
    {
        return std::accumulate(m_tests.cbegin(), m_tests.cend(), 0,
            [](size_t a, const suite_ptr& b) -> size_t { return a + b->tests_count(); });
    }

    size_t gen_test_id(const std::string& case_name)
    {
        std::map<std::string, size_t>::iterator it = m_case_names.find(case_name);
        if (it == m_case_names.cend()) {
            it = m_case_names.emplace(case_name, m_tests.size()).first;
            m_tests.emplace_back(std::make_shared<test_suite>(case_name));
        }
        return it->second;
    }

private:
    std::vector<ienv::ptr> m_envs;

    std::map<std::string, size_t> m_case_names;
    std::vector<suite_ptr> m_tests;

    static std::unique_ptr<tester> m_p_instance;
};

std::unique_ptr<tester> tester::m_p_instance = nullptr;

std::ostream& fail()
{
    test_failer::get_instance().fail();
    return std::cerr;
}

std::ostream& fatal()
{
    test_failer::get_instance().fatal();
    return std::cerr;
}

std::ostream& msg() { return std::cout; }

class report_helper final
{
public:
    report_helper() {}
    void operator=(std::ostream& msg) const { msg << std::endl; }
};

} // namespace details
} // namespace testing

#endif /* _TESTING_TESTER_H */

