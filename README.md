# Thread-safe cache library

A high-performance, header-only C++ library implementing fixed-size caches with
multiple eviction policies and sharded thread-safe wrappers.

## Description

The library provides both single-threaded and thread-safe implementations for the
following caching policies:

* **Direct-Mapped Cache** - maps each key to a specific cache slot based on its hash.
* **FIFO (First-In, First-Out)** - evicts the oldest inserted element regardless
  of access frequency.
* **LFU (Least Frequently Used)** - evicts the element with the lowest access count.
* **LRU (Least Recently Used)** - evicts the element that has not been accessed
  for the longest time.
* **Random Replacement (RR)** - randomly selects a candidate element to evict
  when capacity is reached.
* **TTL (Time-To-Live)** - automatically invalidates and evicts entries after a
  specified expiration duration.

### Storage backends

Each cache implementation supports two interchangeable storage backends controlled
via the `THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE` macro:
* **Standard Library (`std`)** — uses `std::unordered_map` and `std::list` (zero
  external dependencies).
* **Boost.Intrusive (`boost::intrusive`)** — uses intrusive hash tables and lists
  to eliminate extra heap allocations and improve cache locality.

### Thread safety & Sharding architecture

Thread-safe caches are implemented via a sharded wrapper, requiring no external
locks or synchronization in the calling code.

Internally, a thread-safe cache partitions its storage into multiple independent
shards, each protected by its own mutex (`std::mutex` by default, customizable
via template parameter):

```
                ┌─────────────────────────────────────┐
                │          thread_safe_cache          │
                ├─────────────────────────────────────┤
                ||========|========|========|========||
                || Shrd_1 | Shrd_2 | Shrd_3 | Shrd_4 ||
                ||========|========|========|========||
                || data_1 | data_2 | data_3 | data_4 ||
                ||--------|--------|--------|--------||
                ||        |        |        |        ||
                ||--------|--------|--------|--------||
                └─────────────────────────────────────┘
                    /                  |         \
                Thread 1 accesses  Thread 2    Thread 3 accesses
                Shrd_1 (locked     accesses    Shrd_4 (locked
                by its mutex)      Shrd_3      by its mutex)
```

Total cache capacity is divided evenly across all shards. If the capacity is not
evenly divisible by the number of shards, the remainder is added to the first
shard:
* **Example 1:** Capacity `10` with `2` shards → `5` elements per shard.
* **Example 2:** Capacity `11` with `4` shards → Shard 1 gets `3` elements;
  Shards 2–4 get `2` elements each.

When inserting or looking up a key, the target shard is selected using the key's
hash. Eviction decisions are made locally within the selected shard. Consequently,
if a specific shard reaches its capacity, it will evict its entry even if other
shards still have free slots:

```text
                            +--------+
                            | data_4 |
                            +--------+
                            /
                           / Adding a new element
                          /
            |========|========|                  |========|========|
            | Shrd_1 | Shrd_2 |                  | Shrd_1 | Shrd_2 |
            |========|========|                  |========|========|
            | data_1 | data_2 |    =========>    | data_1 | data_4 |
            |--------|--------|                  |--------|--------|
            |        | data_3 |                  |        | data_2 |
            |--------|--------|                  |--------|--------|
           /             |
          /              | The oldest element is
  Shard 1 still has      |   evicted from Shrd_2
     free space      +--------+
                     | data_3 |
                     +--------+
```

## Basic Usage

Two main classes are provided `::wstux::lru::lru_cache` and `::wstux::lru::thread_safe_lru_cache`.
A basic usage example of these may look like so:

__`::wstux::cache::lru::lru_cache`__
```cpp
#include <iostream>
#include "cache/lru_cache.h"

using lru_cache = wstux::cache::lru::lru_cache<int, int>;

int fibonacci(int n, lru_cache& cache)
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
    std::cout << "fibonacci(15) = " << fibonacci(15, cache) << std::endl;
    return 0;
}
```

__`::wstux::cache::lru::thread_safe_lru_cache`__
```cpp
#include <iostream>
#include "cache/thread_safe_lru_cache.h"

using ts_lru_cache = wstux::cache::lru::thread_safe_lru_cache<int, int>;

int fibonacci(int n, ts_lru_cache& cache)
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
    // Capacity: 10 elements, Shards: 4
    ts_lru_cache cache(10, 4);
    std::cout << "fibonacci(15) = " << fibonacci(15, cache) << std::endl;
    return 0;
}
```

## License

&copy; 2023 Chistyakov Alexander.

Open sourced under MIT license, the terms of which can be read here — [MIT License](http://opensource.org/licenses/MIT).

