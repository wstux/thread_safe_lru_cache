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

#include <random>
#include <thread>
#include <type_traits>

#include <benchmark/benchmark.h>

#include "cache/thread_safe_lru_cache.h"
#include "cache/thread_safe_rr_cache.h"
#include "cache/thread_safe_ttl_cache.h"

namespace {

constexpr size_t g_k_count = 100000;
const size_t g_k_threads_count = []() -> size_t {
        size_t threads_count = std::thread::hardware_concurrency();
        return (threads_count == 0) ? 16 : threads_count;
    }();
const std::vector<size_t> g_k_test_data = []() -> std::vector<size_t> {
        std::vector<size_t> test_data(g_k_count);
        for (size_t i = 0; i < g_k_count; ++i) {
            test_data[i] = i;
        }
        return test_data;
    }();

////////////////////////////////////////////////////////////////////////////////
// class cache_fixture

template<typename T>
class cache_fixture : public benchmark::Fixture
{
public:
    using param_type = T;
    using cache_type = typename param_type::cache;
    using key_type   = typename cache_type::key_type;
    using value_type = typename cache_type::value_type;

    static_assert(std::is_same<key_type, value_type>::value, "Invalid types");

public:
    void SetUp(const ::benchmark::State&) {}
    void TearDown(const ::benchmark::State&) {}

    const std::vector<value_type>& test_data() const { return g_k_test_data; }
};

struct lru_cache
{
    using cache = ::wstux::cache::lru::thread_safe_lru_cache<size_t, size_t>;

    static cache create(size_t cap, size_t shards = g_k_threads_count) { return cache(cap, shards); }
};

struct rr_cache
{
    using cache = ::wstux::cache::rr::thread_safe_rr_cache<size_t, size_t>;

    static cache create(size_t cap, size_t shards = g_k_threads_count) { return cache(cap, shards); }
};

struct ttl_cache
{
    using cache = ::wstux::cache::ttl::thread_safe_ttl_cache<size_t, size_t>;

    static cache create(size_t cap, size_t shards = g_k_threads_count) { return cache(900, cap, shards); }
};

BENCHMARK_TEMPLATE_METHOD_F(cache_fixture, insert)(benchmark::State& state)
{
    using param_type = typename Base::param_type;
    using cache_type = typename param_type::cache;
    using value_type = typename cache_type::value_type;

    const std::vector<value_type>& td = this->test_data();
    std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<size_t> rand(0, g_k_count - 1);

    cache_type cache = param_type::create(2 * g_k_count);
    size_t i = 0;
    for (auto _ : state) {
        const size_t k = td[rand(gen)];
        cache.insert(k, k);
        ++i;
    }
    state.SetItemsProcessed(i);
}

BENCHMARK_TEMPLATE_METHOD_F(cache_fixture, emplace)(benchmark::State& state)
{
    using param_type = typename Base::param_type;
    using cache_type = typename param_type::cache;
    using value_type = typename cache_type::value_type;

    const std::vector<value_type>& td = this->test_data();
    std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<size_t> rand(0, g_k_count - 1);

    cache_type cache = param_type::create(2 * g_k_count);
    size_t i = 0;
    for (auto _ : state) {
        const size_t k = td[rand(gen)];
        cache.insert(k, k);
        ++i;
    }
    state.SetItemsProcessed(i);
}

BENCHMARK_TEMPLATE_METHOD_F(cache_fixture, update_insert)(benchmark::State& state)
{
    using param_type = typename Base::param_type;
    using cache_type = typename param_type::cache;
    using value_type = typename cache_type::value_type;

    const std::vector<value_type>& td = this->test_data();
    std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<size_t> rand(0, g_k_count - 1);

    cache_type cache = param_type::create(2 * g_k_count);
    size_t i = 0;
    for (auto _ : state) {
        const size_t k = td[rand(gen)];
        cache.update(k, k);
        ++i;
    }
    state.SetItemsProcessed(i);
}

BENCHMARK_TEMPLATE_METHOD_F(cache_fixture, update)(benchmark::State& state)
{
    using param_type = typename Base::param_type;
    using cache_type = typename param_type::cache;
    using value_type = typename cache_type::value_type;

    const std::vector<value_type>& td = this->test_data();
    std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<size_t> rand(0, g_k_count - 1);

    cache_type cache = param_type::create(2 * g_k_count);
    if (state.thread_index() == 0) {
        for (size_t k : td) {
            cache.insert(k, k + 1);
        }
    }

    size_t  i = 0;
    for (auto _ : state) {
        const size_t k = td[rand(gen)];
        cache.update(k, k);
        ++i;
    }
    state.SetItemsProcessed(i);
}

BENCHMARK_TEMPLATE_METHOD_F(cache_fixture, find)(benchmark::State& state)
{
    using param_type = typename Base::param_type;
    using cache_type = typename param_type::cache;
    using value_type = typename cache_type::value_type;

    const std::vector<value_type>& td = this->test_data();
    std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<size_t> rand(0, g_k_count - 1);

    cache_type cache = param_type::create(2 * g_k_count);
    if (state.thread_index() == 0) {
        for (size_t k : td) {
            cache.insert(k, k + 1);
        }
    }

    typename cache_type::value_type val = 0;
    size_t i = 0;
    for (auto _ : state) {
        const size_t k = td[rand(gen)];
        cache.find(k, val);
        ++i;
    }
    state.SetItemsProcessed(i);
}

BENCHMARK_TEMPLATE_METHOD_F(cache_fixture, insert_overflow)(benchmark::State& state)
{
    using param_type = typename Base::param_type;
    using cache_type = typename param_type::cache;
    using value_type = typename cache_type::value_type;

    const std::vector<value_type>& td = this->test_data();
    std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<size_t> rand(0, g_k_count - 1);

    cache_type cache = param_type::create(g_k_count);
    if (state.thread_index() == 0) {
        for (size_t i = 0; i < g_k_count; ++i) {
            cache.insert(i + g_k_count, i + g_k_count);
        }
    }

    size_t i = 0;
    for (auto _ : state) {
        const size_t k = td[rand(gen)];
        cache.insert(k, k);
        ++i;
    }
    state.SetItemsProcessed(i);
}

BENCHMARK_TEMPLATE_METHOD_F(cache_fixture, request)(benchmark::State& state)
{
    using param_type = typename Base::param_type;
    using cache_type = typename param_type::cache;
    using value_type = typename cache_type::value_type;

    const std::vector<value_type>& td = this->test_data();
    std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<size_t> rand(0, g_k_count - 1);

    cache_type cache = param_type::create(g_k_count);

    size_t i = 0;
    typename cache_type::value_type val = 0;
    for (auto _ : state) {
        const size_t k = td[rand(gen)];
        if (! cache.find(k, val)) {
            cache.insert(k, k);
        }
        ++i;
    }
    state.SetItemsProcessed(i);
}

BENCHMARK_TEMPLATE_METHOD_F(cache_fixture, request_hot)(benchmark::State& state)
{
    using param_type = typename Base::param_type;
    using cache_type = typename param_type::cache;
    using value_type = typename cache_type::value_type;

    const std::vector<value_type>& td = this->test_data();
    std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<size_t> rand(0, g_k_count - 1);

    cache_type cache = param_type::create(g_k_count);
    if (state.thread_index() == 0) {
        for (size_t k : td) {
            cache.insert(k, k);
        }
    }

    size_t i = 0;
    typename cache_type::value_type val = 0;
    for (auto _ : state) {
        const size_t k = td[rand(gen)];
        if (! cache.find(k, val)) {
            cache.insert(k, k);
        }
        ++i;
    }
    state.SetItemsProcessed(i);
}

BENCHMARK_TEMPLATE_METHOD_F(cache_fixture, many_shards)(benchmark::State& state)
{
    using param_type = typename Base::param_type;
    using cache_type = typename param_type::cache;
    using value_type = typename cache_type::value_type;

    const std::vector<value_type>& td = this->test_data();
    std::mt19937 gen(std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_int_distribution<size_t> rand(0, g_k_count - 1);

    cache_type cache = param_type::create(g_k_count, 3 * g_k_threads_count);

    size_t  i = 0;
    typename cache_type::value_type val = 0;
    for (auto _ : state) {
        const size_t k = td[rand(gen)];
        if (! cache.find(k, val)) {
            cache.insert(k, k);
        }
        ++i;
    }
    state.SetItemsProcessed(i);
}

} // <anonymous> namespace

#define DECLARE_TYPED_BENCHMARKS(fixture, cache_type)                                                   \
    BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, insert,          cache_type)->Threads(g_k_threads_count); \
    BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, emplace,         cache_type)->Threads(g_k_threads_count); \
    BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, update_insert,   cache_type)->Threads(g_k_threads_count); \
    BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, update,          cache_type)->Threads(g_k_threads_count); \
    BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, find,            cache_type)->Threads(g_k_threads_count); \
    BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, insert_overflow, cache_type)->Threads(g_k_threads_count); \
    BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, request,         cache_type)->Threads(g_k_threads_count); \
    BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, request_hot,     cache_type)->Threads(g_k_threads_count); \
    BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, many_shards,     cache_type)->Threads(g_k_threads_count)

DECLARE_TYPED_BENCHMARKS(cache_fixture, lru_cache);
DECLARE_TYPED_BENCHMARKS(cache_fixture, rr_cache);
DECLARE_TYPED_BENCHMARKS(cache_fixture, ttl_cache);

BENCHMARK_MAIN();
