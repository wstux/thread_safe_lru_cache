/*
 * The MIT License
 *
 * Copyright 2024 Chistyakov Alexander.
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

#ifndef _THREAD_SAFE_CACHE_LIBS_CACHE_BASE_RR_CACHE_H_
#define _THREAD_SAFE_CACHE_LIBS_CACHE_BASE_RR_CACHE_H_

#include <memory>
#include <vector>

#include "cache/details/traits.h"

namespace wstux {
namespace cache {
namespace rr {
namespace details {

////////////////////////////////////////////////////////////////////////////////
// Naive RR cache implementation

template<typename TKey, typename TValue, class THash, class TKeyEqual, class TAllocator>
class base_rr_cache
{
private:
    typedef cache::details::common::type_traits<TKey, TValue, THash, TKeyEqual, TAllocator> _traits_t;

protected:
    typedef typename _traits_t::allocator_type      allocator_type;
    typedef typename _traits_t::key_type            key_type;
    typedef typename _traits_t::value_type          value_type;
    typedef typename _traits_t::size_type           size_type;
    typedef typename _traits_t::hasher              hasher;
    typedef typename _traits_t::key_equal           key_equal;
    typedef typename _traits_t::reference           reference;
    typedef typename _traits_t::const_reference     const_reference;
    typedef typename _traits_t::pointer             pointer;
    typedef typename _traits_t::const_pointer       const_pointer;

    typedef std::pair<TKey, TValue>                 _table_value_t;
    typedef std::unique_ptr<_table_value_t>         _node_t;
    typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<_table_value_t>   _value_allocator_t;
    typedef std::vector<_node_t, allocator_type>    _hash_table_t;
    typedef std::vector<_node_t, allocator_type>    hash_table_type;

    base_rr_cache(size_type capacity, const allocator_type& alloc)
        : m_capacity(capacity)
        , m_size(0)
        , m_allocator(alloc)
        , m_hash_tbl(m_capacity, alloc)
    {}

    virtual ~base_rr_cache() { clear(); };

    inline size_type capacity() const { return m_capacity; }

    void clear()
    {
        for (_node_t& node : m_hash_tbl) {
            if (node.get() != nullptr) {
                _table_value_t* p = node.release();
                deallocate<_table_value_t>(m_allocator, p);
            }
        }
        m_hash_tbl.clear();
        m_size = 0;
    }

    void erase(const key_type& key)
    {
        typename _hash_table_t::iterator it = find_in_tbl(key);
        if (it != m_hash_tbl.cend()) {
            erase(it);
        }
    }

    void erase(typename _hash_table_t::iterator& it)
    {
        _table_value_t* p = it->release();
        deallocate<_table_value_t>(m_allocator, p);
        --m_size;
    }

    inline typename _hash_table_t::iterator find_in_tbl(const key_type& key)
    {
        const size_type idx = hasher{}(key) % m_capacity;
        //const _node_t& node = m_hash_tbl[idx];
        typename _hash_table_t::iterator it = m_hash_tbl.begin() + idx;
        //if (node && node->first == key) {
        if (it->get() != nullptr && (*it)->first == key) {
            return it;
        }
        return m_hash_tbl.end();
    }

    template<typename... TArgs>
    void insert(const key_type& key, TArgs&&... args)
    {
        const size_type idx = hasher{}(key) % m_capacity;
        _node_t& node = m_hash_tbl[idx];
        if (node.get() != nullptr) {
            node->first = key;
            node->second = std::move(value_type(std::forward<TArgs>(args)...));
        } else {
            _table_value_t* p_raw = allocate<_table_value_t>(m_allocator, key, std::move(value_type(std::forward<TArgs>(args)...)));
            node.reset(p_raw);
            ++m_size;
        }
    }

    inline bool is_find(typename _hash_table_t::iterator& it) const { return (it != m_hash_tbl.end()); }

    template<typename... TArgs>
    void reset(size_type new_capacity, TArgs&&... args)
    {
        clear();

        m_capacity = new_capacity;
        m_hash_tbl.resize(m_capacity);
    }

    inline size_type size() const { return m_size; }

    inline static const value_type& load(const typename _hash_table_t::iterator& it) { return (*it)->second; }

    inline static void load(const typename _hash_table_t::iterator& it, value_type& res) { res = (*it)->second; }

    inline static void store(const typename _hash_table_t::iterator& it, value_type&& val) { (*it)->second = val; }

private:
    template<typename T, typename... TArgs>
    static T* allocate(_value_allocator_t& allocator, TArgs&&... args)
    {
        using allocator_traits_t = std::allocator_traits<_value_allocator_t>;

        T* p_raw = allocator_traits_t::allocate(allocator, 1);
        allocator_traits_t::construct(allocator, p_raw, std::forward<TArgs>(args)...);
        return p_raw;
    }

    template<typename T>
    static void deallocate(_value_allocator_t& allocator, T* p_raw)
    {
        using allocator_traits_t = std::allocator_traits<_value_allocator_t>;

        allocator_traits_t::destroy(allocator, p_raw);
        allocator_traits_t::deallocate(allocator, p_raw, 1);
    }

private:
    size_type m_capacity;
    size_type m_size;
    _value_allocator_t m_allocator;
    hash_table_type m_hash_tbl;
};

} // namespace details
} // namespace rr
} // namespace cache
} // namespace wstux

#endif /* _THREAD_SAFE_CACHE_LIBS_CACHE_BASE_RR_CACHE_H_ */
