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

#include <iostream>
#include <cache/lru_cache.h>
#include <cache/thread_safe_lru_cache.h>

using lru_cache = ::wstux::cache::lru::lru_cache<int, int>;

using thread_safe_lru_cache = wstux::cache::lru::thread_safe_lru_cache<int, int>;

template<typename TCache>
int fibonacci(int n, TCache& cache)
{
    if (n < 2) return 1;

    int value = 0;
    if (cache.find(n, value)) {
        return value;
    }

    value = fibonacci(n - 1, cache) + fibonacci(n - 2, cache);
    cache.emplace(n, value);

    return value;
}

int main()
{
    lru_cache cache(10);
    std::cout << "lru_cache: fibonacci(15) = " << fibonacci(15, cache) << std::endl;

    thread_safe_lru_cache ts_cache(10, 4);
    std::cout << "thread_safe_lru_cache: fibonacci(15) = " << fibonacci(15, ts_cache) << std::endl;

    return 0;
}
