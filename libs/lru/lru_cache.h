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

#ifndef _LRU_CACHE_LRU_CACHE_H
#define _LRU_CACHE_LRU_CACHE_H

#include <functional>

#include "lru/details/features.h"
#if defined(LRU_CACHE_ENABLE_STD_OPTIONAL)
    #include <optional>
#endif

#if defined(LRU_CACHE_USE_BOOST_INTRUSIVE)
    #include "lru/details/intrusive_base_lru_cache.h"
#else
    #include "lru/details/std_base_lru_cache.h"
#endif

namespace wstux {
namespace lru {

/// \todo   Add allocator as template parameter.
template<typename TKey, typename TValue,
         class THash = std::hash<TKey>, class TKeyEqual = std::equal_to<TKey>>
class lru_cache : protected details::base_lru_cache<TKey, TValue, THash, TKeyEqual>
{
private:
    typedef details::base_lru_cache<TKey, TValue, THash, TKeyEqual>     base;

public:
    typedef typename base::key_type          key_type;
    typedef typename base::mapped_type       mapped_type;
    typedef typename base::value_type        value_type;
    typedef typename base::size_type         size_type;
    typedef typename base::hasher            hasher;
    typedef typename base::key_equal         key_equal;
    typedef typename base::reference         reference;
    typedef typename base::const_reference   const_reference;
    typedef typename base::pointer           pointer;
    typedef typename base::const_pointer     const_pointer;

    explicit lru_cache(size_type capacity)
        : base(capacity)
    {}

    lru_cache(const lru_cache&) = delete;
    lru_cache& operator=(const lru_cache&) = delete;

    size_type capacity() const { return base::capacity(); }

    void clear() { base::clear(); }

    bool contains(const key_type& key)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            base::move_to_top(it);
            return true;
        }
        return false;
    }

    template<typename... TArgs>
    bool emplace(const key_type& key, TArgs&&... args)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            base::move_to_top(it);
            return false;
        }

        base::insert(key, std::forward<TArgs>(args)...);
        return true;
    }

    bool empty() const { return (base::size() == 0); }

    void erase(const key_type& key) { base::erase(key); }

    bool find(const key_type& key, mapped_type& result)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            base::move_to_top(it);
            base::load(it, result);
            return true;
        }

        return false;
    }

    bool insert(const key_type& key, const mapped_type& val)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            base::move_to_top(it);
            return false;
        }

        base::insert(key, val);
        return true;
    }

#if defined(LRU_CACHE_ENABLE_STD_OPTIONAL)
    std::optional<mapped_type> get(const key_type& key)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            base::move_to_top(it);
            return base::load(it);
        }

        return std::nullopt;
    }
#endif

    void reserve(size_type new_capacity) { base::reserve(new_capacity); }

    size_type size() const { return base::size(); }

    void update(const key_type& key, const mapped_type& val)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            base::store(it, mapped_type(val));
            base::move_to_top(it);
            return;
        }

        base::insert(key, val);
    }

    void update(const key_type& key, mapped_type&& val)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            base::store(it, std::move(val));
            base::move_to_top(it);
            return;
        }

        base::insert(key, std::move(val));
    }

private:
    typedef typename base::_lru_list_t          _lru_list_t;
    typedef typename base::_hash_table_t        _hash_table_t;
};

} // namespace lru
} // namespace wstux

#endif /* _LRU_CACHE_LRU_CACHE_H */

