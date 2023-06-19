# thread-safe-lru-cache

LRU cache and thread safe LRU cache implementation in C++.

## Description

A *least recently used* (LRU) cache is a fixed size cache that behaves just like
a regular lookup table. LRU cache remembers the order in which elements are
accessed and once its capacity is reached, it uses this information to replace
the least recently used element with a newly inserted one.

This repository provides two implementations of an LRU cache: one has only the
basic functionality described above, and another containing a thread-safe
implementation.

## Basic Usage

Two main classes are provided `::wstux::lru::lru_cache` and `::wstux::lru::thread_safe_lru_cache`.
A basic usage example of these may look like so:

__`::wstux::lru::lru_cache`__
```C++
#include <iostream>
#include "lru/lru_cache.h"

using lru_cache = ::wstux::lru::lru_cache<int, int>;

int fibonacci(int n, lru_cache& cache)
{
    if (n < 2) return 1;

    int value;
    if (cache.find(n, value)) {
        return val;
    }

    value = fibonacci(n - 1, cache) + fibonacci(n - 2, cache);
    cache.emplace(n, value);

    return value;
}

int main()
{
    lru_cache cache(10);
    std::cout << "fibonacci(15)" << fibonacci(15, cache) << std::endl;
    return 0;
}
```

__`::wstux::lru::thread_safe_lru_cache`__
```C++
#include <iostream>
#include "lru/thread_safe_lru_cache.h"

using lru_cache = ::wstux::lru::thread_safe_lru_cache<int, int>;

int fibonacci(int n, lru_cache& cache)
{
    if (n < 2) return 1;

    int value;
    if (cache.find(n, value)) {
        return val;
    }

    value = fibonacci(n - 1, cache) + fibonacci(n - 2, cache);
    cache.emplace(n, value);

    return value;
}

int main()
{
    lru_cache cache(10);
    std::cout << "fibonacci(15)" << fibonacci(15, cache) << std::endl;
    return 0;
}
```

## License

&copy; 2023 Chistyakov Alexander.

Open sourced under MIT license, the terms of which can be read here — [MIT License](http://opensource.org/licenses/MIT).

