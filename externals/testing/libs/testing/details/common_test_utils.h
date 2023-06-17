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

#ifndef _TESTING_COMMON_TEST_UTILS_H
#define _TESTING_COMMON_TEST_UTILS_H

#ifdef __GNUG__
    #include <cstdlib>
    #include <cxxabi.h>
#endif

#include <cstring>
#include <string>

// Check RTTI enabling for typeid
#if defined(__clang__)
    #if __has_feature(cxx_rtti)
        #define __RTTI_ENABLED
    #endif
#elif defined(__GNUG__)
    #if defined(__GXX_RTTI)
        #define __RTTI_ENABLED
    #endif
#elif defined(_MSC_VER)
    #if defined(_CPPRTTI)
        #define __RTTI_ENABLED
    #endif
#endif

namespace testing {
namespace details {

inline std::string canonize(std::string s)
{
    static const std::string prefix = "std::__";
    if (s.compare(0, prefix.size(), prefix) == 0) {
        size_t end = s.find("::", prefix.size());
        if (end != s.npos) {
            s.erase(std::strlen("std"), end - std::strlen("std"));
        }
    }
    return s;
}

inline std::string demangle(const char* name)
{
#if defined(__GNUG__)
    int status = -4; // some arbitrary value to eliminate the compiler warning

    std::unique_ptr<char, void(*)(void*)> res(abi::__cxa_demangle(name, NULL, NULL, &status), std::free);
    return (status == 0) ? res.get() : name ;
#else
    // does nothing if not g++
    return name;
#endif
}

template <class TType>
std::string type_name() { return demangle(typeid(TType).name()); }

template <class TType>
std::string type_name(const TType& t) { return demangle(typeid(t).name()); }

template <typename T>
std::string canon_type_name()
{
#if defined(__RTTI_ENABLED)
    #if defined(__GNUG__)
        std::string name = type_name<T>();
        return canonize(name);
    #else
        return name;
    #endif
#else
  return "<type>";
#endif
}

} // namespace details
} // namespace testing

#if defined(__RTTI_ENABLED)
    #undef __RTTI_ENABLED
#endif

#endif /* _TESTING_COMMON_TEST_UTILS_H */

