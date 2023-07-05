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

#ifndef _LRU_CACHE_STD_BASE_LRU_CACHE_H
#define _LRU_CACHE_STD_BASE_LRU_CACHE_H

#include <list>
#include <unordered_map>

namespace wstux {
namespace lru {
namespace details {

template<typename TKey, typename TValue, class THash, class TKeyEqual>
class base_lru_cache
{
protected:
    typedef TKey                    key_type;
    typedef TValue                  value_type;
    typedef size_t                  size_type;
    typedef THash                   hasher;
    typedef TKeyEqual               key_equal;
    typedef value_type&             reference;
    typedef const value_type&       const_reference;
    typedef value_type*             pointer;
    typedef const value_type*       const_pointer;

    typedef std::list<key_type>     _lru_list_t;

    struct _hash_table_value
    {
        template<typename... TArgs>
        explicit _hash_table_value(TArgs&&... args)
            : value(std::forward<TArgs>(args)...)
        {}

        value_type value;
        typename _lru_list_t::iterator lru_it;
    };
    typedef _hash_table_value       _table_value_t;
    typedef std::unordered_map<key_type, _table_value_t, hasher, key_equal> _hash_table_t;

    explicit base_lru_cache(size_type capacity)
        : m_capacity(capacity)
        , m_size(0)
    {
        /*
         * In the update(...) function, the hash table is first inserted into
         * the hash table, after which the size is checked, so a situation may
         * arise when the table stores (capacity + 1) elements. Because of this
         * behavior, recalculation and reallocation of the table can occur, so
         * the table is reserved for (capacity + 1) elements to prevent
         * rehashing of the table hash.
         */
        m_hash_tbl.reserve(capacity + 1);
    }

    virtual ~base_lru_cache() {}

    size_type capacity() const { return m_capacity; }

    void clear()
    {
        m_size = 0;
        m_hash_tbl.clear();
        m_lru_list.clear();
    }

    void erase(const key_type& key)
    {
        typename _hash_table_t::iterator it = m_hash_tbl.find(key);
        if (it != m_hash_tbl.end()) {
            m_lru_list.erase(it->second.lru_it);
            m_hash_tbl.erase(it);
            --m_size;
        }
    }

    typename _hash_table_t::iterator find_in_tbl(const key_type& key)
    {
        return m_hash_tbl.find(key);
    }

    template<typename... TArgs>
    void insert(const key_type& key, TArgs&&... args)
    {
        if (m_size >= m_capacity) {
            m_hash_tbl.erase(m_lru_list.front());
            m_lru_list.front() = key;
            std::pair<typename _hash_table_t::iterator, bool> rc =
                m_hash_tbl.emplace(key, _table_value_t(std::forward<TArgs>(args)...));
            rc.first->second.lru_it = m_lru_list.begin();
            move_to_top(rc.first);
        } else {
            typename _lru_list_t::iterator it = m_lru_list.emplace(m_lru_list.end(), key);
            std::pair<typename _hash_table_t::iterator, bool> rc =
                m_hash_tbl.emplace(key, _table_value_t(std::forward<TArgs>(args)...));
            rc.first->second.lru_it = it;
            ++m_size;
        }
    }

    bool is_find(typename _hash_table_t::iterator& it) const { return (it != m_hash_tbl.end()); } 

    void move_to_top(typename _hash_table_t::iterator& it)
    {
        m_lru_list.splice(m_lru_list.end(), m_lru_list, it->second.lru_it);
    }

    void reserve(size_type new_capacity)
    {
        m_capacity = new_capacity;
        m_hash_tbl.reserve(m_capacity);
    }

    size_type size() const { return m_size; }

    static void load(const typename _hash_table_t::iterator& it, value_type& res)
    {
        res = it->second.value;
    }

    static void store(const typename _hash_table_t::iterator& it, value_type&& val)
    {
        it->second.value = val;
    }

private:
    size_t m_capacity;

    size_t m_size;
    _hash_table_t m_hash_tbl;
    _lru_list_t m_lru_list;
};

} // namespace details
} // namespace lru
} // namespace wstux

#endif /* _LRU_CACHE_STD_BASE_LRU_CACHE_H */

