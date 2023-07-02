/*
 * The MIT License
 *
 * Copyright 2019 Chistyakov Alexander.
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

#include <cassert>
#include <functional>
#include <list>
#include <unordered_map>

#include "lru/details/intrusive_node.h"

namespace wstux {
namespace lru {

/// \todo   Add allocator as template parameter.
template<typename TKey, typename TValue,
         class THash = std::hash<TKey>, class TKeyEqual = std::equal_to<TKey>>
class lru_cache
{
public:
    typedef TKey                key_type;
    typedef TValue              value_type;
    typedef size_t              size_type;
    typedef THash               hasher;
    typedef TKeyEqual           key_equal;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;

    explicit lru_cache(size_t capacity)
        : m_capacity(capacity)
        , m_buckets(capacity)
        , m_size(0)
        , m_cache(_bucket_traits(m_buckets.data(), m_buckets.capacity()))
    {
        /*
         * In the update(...) function, the hash table is first inserted into
         * the hash table, after which the size is checked, so a situation may
         * arise when the table stores (capacity + 1) elements. Because of this
         * behavior, recalculation and reallocation of the table can occur, so
         * the table is reserved for (capacity + 1) elements to prevent
         * rehashing of the table hash.
         */
        //m_cache.reserve(m_capacity + 1);
    }

    lru_cache(const lru_cache&) = delete;
    lru_cache& operator=(const lru_cache&) = delete;

    size_type capacity() const { return m_capacity; }

    void clear()
    {
        m_size = 0;
        m_cache.clear();
        m_lru_list.clear();
    }

    bool contains(const key_type& key)
    {
        typename _lru_map_t::iterator it = m_cache.find(key);
        if (it != m_cache.end()) {
            move_to_top(m_lru_list, it);
            return true;
        }
        return false;
    }

    template<typename... TArgs>
    bool emplace(const key_type& key, TArgs&&... args)
    {
        typename _lru_map_t::iterator it = m_cache.find(key, m_cache.hash_function(), m_cache.key_eq());
        if (it != m_cache.end()) {
            move_to_top(m_lru_list, it);
            return false;
        }

        insert_to_cache(key, std::forward<TArgs>(args)...);
        return true;
    }

    bool empty() const { return (size() == 0); }
/*
    void erase(const key_type& key)
    {
        typename _lru_map_t::iterator it = m_cache.find(key, m_cache.hash_function(), m_cache.key_eq());
        if (it != m_cache.end()) {
            m_lru_list.erase(it->second.lru_it);
            m_cache.erase(it);
            --m_size;
        }
    }
*/
    bool find(const key_type& key, value_type& result)
    {
        typename _lru_map_t::iterator it = m_cache.find(key, m_cache.hash_function(), m_cache.key_eq());
        if (it == m_cache.end()) {
            return false;
        }

        move_to_top(m_lru_list, it);
        result = it->value;
        return true;
    }

    bool insert(const key_type& key, const value_type& val)
    {
        typename _lru_map_t::iterator it = m_cache.find(key, m_cache.hash_function(), m_cache.key_eq());
        if (it != m_cache.end()) {
            move_to_top(m_lru_list, it);
            return false;
        }

        insert_to_cache(key, val);
        return true;
    }

    void reserve(size_type new_capacity)
    {
        m_capacity = new_capacity;
        m_cache.reserve(m_capacity);
    }

    size_type size() const { return m_size; }

    void update(const key_type& key, const value_type& val)
    {
        typename _lru_map_t::iterator it = m_cache.find(key, m_cache.hash_function(), m_cache.key_eq());
        if (it != m_cache.end()) {
            it->value = val;
            move_to_top(m_lru_list, it);
            return;
        }

        insert_to_cache(key, val);
    }

    void update(const key_type& key, value_type&& val)
    {
        typename _lru_map_t::iterator it = m_cache.find(key, m_cache.hash_function(), m_cache.key_eq());
        if (it != m_cache.end()) {
            it->value = std::move(val);
            move_to_top(m_lru_list, it);
            return;
        }

        insert_to_cache(key, std::move(val));
    }

private:
    using _lru_node_t = details::lru_node<key_type, value_type>;
    using _lru_node_ptr_t = details::lru_node_ptr<key_type, value_type>;
    using _lru_list_t = details::_lru_list<key_type, value_type>;
    using _lru_map_t = details::_lru_map<key_type, value_type>;
    using _bucket_type = details::_lru_bucket_type<key_type, value_type>;
    using _bucket_traits = details::_lru_bucket_traits<key_type, value_type>;

private:
    template<typename... TArgs>
    inline void insert_to_cache(const key_type& key, TArgs&&... args)
    {
        if (m_size >= m_capacity) {
            _lru_node_ptr_t p_node = details::extract_node(m_cache, m_lru_list, m_lru_list.begin());
            p_node->key = key;
            p_node->value = value_type(std::forward<TArgs>(args)...);
            details::insert_node(m_cache, m_lru_list, std::move(p_node));
        } else {
            _lru_node_ptr_t p_node = std::make_unique<_lru_node_t>(key_type(key), value_type(std::forward<TArgs>(args)...));
            details::insert_node(m_cache, m_lru_list, std::move(p_node));
        }
    }

    template<typename TIterator>
    inline static void move_to_top(_lru_list_t& list, TIterator& it)
    {
        //list.splice(list.end(), list, it);
        list.splice(list.end(), list, list.iterator_to(*it));
    }

private:
    size_t m_capacity;

    std::vector<_bucket_type> m_buckets;
    size_t m_size;
    _lru_map_t m_cache;
    _lru_list_t m_lru_list;
};

} // namespace lru
} // namespace wstux

#endif /* _LRU_CACHE_LRU_CACHE_H */

