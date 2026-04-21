/*
 * The MIT License
 *
 * Copyright 2026 Chistyakov Alexander.
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

#if defined(THREAD_SAFE_CACHE_DISABLE_BOOST)
    #undef THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE
#endif

#include <type_traits>

#include <benchmark/benchmark.h>

#include "cache/fifo_cache.h"
#include "cache/lfu_cache.h"
#include "cache/lru_cache.h"
#include "cache/rr_cache.h"
#include "cache/ttl_cache.h"

namespace {

constexpr size_t g_k_count = 100000;

struct fifo_cache
{
    using cache = ::wstux::cache::fifo::fifo_cache<size_t, size_t>;
    static cache create(size_t cap = 10) { return cache(cap); }
};

struct lfu_cache
{
    using cache = ::wstux::cache::lfu::lfu_cache<size_t, size_t>;
    static cache create(size_t cap = 10) { return cache(cap); }
};

struct lru_cache
{
    using cache = ::wstux::cache::lru::lru_cache<size_t, size_t>;
    static cache create(size_t cap = 10) { return cache(cap); }
};

struct rr_cache
{
    using cache = ::wstux::cache::rr::rr_cache<size_t, size_t>;
    static cache create(size_t cap = 10) { return cache(cap); }
};

struct ttl_cache
{
    using cache = ::wstux::cache::ttl::ttl_cache<size_t, size_t>;
    static cache create(size_t cap = 10) { return cache(900, cap); }
};

template<typename TParam>
static void insert(::benchmark::State& state)
{
    using param_type = TParam;
    using cache_type = typename TParam::cache;

    size_t k = 0;
    cache_type cache = param_type::create(2 * g_k_count);
    for (auto _ : state) {
        cache.insert(k % g_k_count, k % g_k_count);
        ++k;
    }
    state.SetItemsProcessed(k);
}

template<typename TParam>
static void emplace(::benchmark::State& state)
{
    using param_type = TParam;
    using cache_type = typename TParam::cache;

    size_t k = 0;
    cache_type cache = param_type::create(2 * g_k_count);
    for (auto _ : state) {
        cache.emplace(k % g_k_count, k % g_k_count);
        ++k;
    }
    state.SetItemsProcessed(k);
}

template<typename TParam>
static void update_insert(::benchmark::State& state)
{
    using param_type = TParam;
    using cache_type = typename TParam::cache;

    size_t k = 0;
    cache_type cache = param_type::create(2 * g_k_count);
    for (auto _ : state) {
        cache.update(k % g_k_count, k % g_k_count);
        ++k;
    }
    state.SetItemsProcessed(k);
}

template<typename TParam>
static void update(::benchmark::State& state)
{
    using param_type = TParam;
    using cache_type = typename TParam::cache;

    cache_type cache = param_type::create(2 * g_k_count);
    for (size_t i = 0; i < g_k_count; ++i) {
        cache.insert(i, i + 1);
    }

    size_t k = 0;
    for (auto _ : state) {
        cache.update(k % g_k_count, k % g_k_count);
        ++k;
    }
    state.SetItemsProcessed(k);
}

template<typename TParam>
static void find(::benchmark::State& state)
{
    using param_type = TParam;
    using cache_type = typename TParam::cache;

    cache_type cache = param_type::create(2 * g_k_count);
    for (size_t i = 0; i < g_k_count; ++i) {
        cache.insert(i, i + 1);
    }

    size_t k = 0;
    typename cache_type::value_type val;
    for (auto _ : state) {
        cache.find(k, val);
        ++k;
    }
    state.SetItemsProcessed(k);
}

template<typename TParam>
static void insert_overflow(::benchmark::State& state)
{
    using param_type = TParam;
    using cache_type = typename TParam::cache;

    cache_type cache = param_type::create(g_k_count);
    for (size_t i = 0; i < g_k_count; ++i) {
        cache.insert(i + g_k_count, i + g_k_count);
    }

    size_t k = 0;
    for (auto _ : state) {
        cache.insert(k, k);
        ++k;
    }
    state.SetItemsProcessed(k);
}

} // <anonymous> namespace

#define DECLARE_TYPED_BENCHMARKS(cache_type)        \
    BENCHMARK_TEMPLATE(insert, cache_type);         \
    BENCHMARK_TEMPLATE(emplace, cache_type);        \
    BENCHMARK_TEMPLATE(update_insert, cache_type);  \
    BENCHMARK_TEMPLATE(update, cache_type);         \
    BENCHMARK_TEMPLATE(find, cache_type);           \
    BENCHMARK_TEMPLATE(insert_overflow, cache_type)

DECLARE_TYPED_BENCHMARKS(fifo_cache);
DECLARE_TYPED_BENCHMARKS(lfu_cache);
DECLARE_TYPED_BENCHMARKS(lru_cache);
DECLARE_TYPED_BENCHMARKS(rr_cache);
DECLARE_TYPED_BENCHMARKS(ttl_cache);

BENCHMARK_MAIN();
