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

#ifndef _TESTING_TYPED_TEST_UTILS_H
#define _TESTING_TYPED_TEST_UTILS_H

namespace testing {
namespace details {

struct none_t {};

struct type_0 {};

template<typename T1>
struct type_1
{
    typedef T1 head;
    typedef type_0 tail;
};
template<typename T1, typename T2>
struct type_2
{
    typedef T1 head;
    typedef type_1<T2> tail;
};
template<typename T1, typename T2, typename T3>
struct type_3
{
    typedef T1 head;
    typedef type_2<T2, T3> tail;
};
template<typename T1, typename T2, typename T3, typename T4>
struct type_4
{
    typedef T1 head;
    typedef type_3<T2, T3, T4> tail;
};
template<typename T1, typename T2, typename T3, typename T4, typename T5>
struct type_5
{
    typedef T1 head;
    typedef type_4<T2, T3, T4, T5> tail;
};
template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6>
struct type_6
{
    typedef T1 head;
    typedef type_5<T2, T3, T4, T5, T6> tail;
};
template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7>
struct type_7
{
    typedef T1 head;
    typedef type_6<T2, T3, T4, T5, T6, T7> tail;
};
template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8>
struct type_8
{
    typedef T1 head;
    typedef type_7<T2, T3, T4, T5, T6, T7, T8> tail;
};
template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8, typename T9>
struct type_9
{
    typedef T1 head;
    typedef type_8<T2, T3, T4, T5, T6, T7, T8, T9> tail;
};
template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8, typename T9, typename T10>
struct type_10
{
    typedef T1 head;
    typedef type_9<T2, T3, T4, T5, T6, T7, T8, T9, T10> tail;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8, typename T9, typename T10,
         typename T11>
struct type_11
{
    typedef T1 head;
    typedef type_10<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> tail;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8, typename T9, typename T10,
         typename T11, typename T12>
struct type_12
{
    typedef T1 head;
    typedef type_11<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12> tail;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8, typename T9, typename T10,
         typename T11, typename T12, typename T13>
struct type_13
{
    typedef T1 head;
    typedef type_12<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> tail;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8, typename T9, typename T10,
         typename T11, typename T12, typename T13, typename T14>
struct type_14
{
    typedef T1 head;
    typedef type_13<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13,
                    T14> tail;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8, typename T9, typename T10,
         typename T11, typename T12, typename T13, typename T14, typename T15>
struct type_15
{
    typedef T1 head;
    typedef type_14<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13,
                    T14, T15> tail;
};

} // namespace details
} // namespace testing

#endif /* _TESTING_TYPED_TEST_UTILS_H */

