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

#ifndef _TESTING_TIMER_H
#define _TESTING_TIMER_H

#include <chrono>

namespace testing {
namespace details {

class timer final
{
    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

public:
    timer(bool run = false)
    {
        if (run) {
            start();
        }
    }

    void pause()
    {
        m_time_ms = value_ms();
        is_start = false;
    }
    
    void start()
    {
        is_start = true;
        m_start = std::chrono::high_resolution_clock::now();
    }

    void stop()
    {
        is_start = false;
        m_time_ms = 0.0;
    }

    double value_ms()
    {
        if (is_start) {
            time_point cur = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> ms_double = cur - m_start;
            m_time_ms += ms_double.count();
        }
        return m_time_ms;
    }
    
private:
    bool is_start = false;
    time_point m_start;
    double m_time_ms = 0.0;
};

} // namespace details
} // namespace testing

#endif /* _TESTING_TIMER_H */

