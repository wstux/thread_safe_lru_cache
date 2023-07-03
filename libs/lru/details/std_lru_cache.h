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

#ifndef _LRU_CACHE_STD_LRU_CACHE_H
#define _LRU_CACHE_STD_LRU_CACHE_H

#include <functional>
#include <list>
#include <unordered_map>

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
        m_cache.reserve(m_capacity + 1);
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
            move_to_top(m_lru_list, it->second.lru_it);
            return true;
        }
        return false;
    }

    template<typename... TArgs>
    bool emplace(const key_type& key, TArgs&&... args)
    {
        typename _lru_map_t::iterator it = m_cache.find(key);
        if (it != m_cache.end()) {
            move_to_top(m_lru_list, it->second.lru_it);
            return false;
        }

        insert_to_cache(key, std::forward<TArgs>(args)...);
        return true;
    }

    bool empty() const { return (size() == 0); }

    void erase(const key_type& key)
    {
        typename _lru_map_t::iterator it = m_cache.find(key);
        if (it != m_cache.end()) {
            m_lru_list.erase(it->second.lru_it);
            m_cache.erase(it);
            --m_size;
        }
    }

    bool find(const key_type& key, value_type& result)
    {
        typename _lru_map_t::iterator it = m_cache.find(key);
        if (it == m_cache.end()) {
            return false;
        }

        move_to_top(m_lru_list, it->second.lru_it);
        result = it->second.value;
        return true;
    }

    bool insert(const key_type& key, const value_type& val)
    {
        typename _lru_map_t::iterator it = m_cache.find(key);
        if (it != m_cache.end()) {
            move_to_top(m_lru_list, it->second.lru_it);
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
        typename _lru_map_t::iterator it = m_cache.find(key);
        if (it != m_cache.end()) {
            it->second.value = val;
            move_to_top(m_lru_list, it->second.lru_it);
            return;
        }

        insert_to_cache(key, val);
    }

    void update(const key_type& key, value_type&& val)
    {
        typename _lru_map_t::iterator it = m_cache.find(key);
        if (it != m_cache.end()) {
            it->second.value = std::move(val);
            move_to_top(m_lru_list, it->second.lru_it);
            return;
        }

        insert_to_cache(key, std::move(val));
    }

private:
    struct _map_value_t
    {
        template<typename... TArgs>
        explicit _map_value_t(TArgs&&... args)
            : value(std::forward<TArgs>(args)...)
        {}

        value_type value;
        typename std::list<key_type>::iterator lru_it;
    };

    /// \todo   Remove key duplicate.
    using _lru_list_t = std::list<key_type>;
    using _lru_map_t = std::unordered_map<key_type, _map_value_t, hasher, key_equal>;

private:
    template<typename... TArgs>
    inline void insert_to_cache(const key_type& key, TArgs&&... args)
    {
        if (m_size >= m_capacity) {
            m_cache.erase(m_lru_list.front());
            m_lru_list.front() = key;
            std::pair<typename _lru_map_t::iterator, bool> rc =
                m_cache.emplace(key, _map_value_t(std::forward<TArgs>(args)...));
            rc.first->second.lru_it = m_lru_list.begin();
            move_to_top(m_lru_list, rc.first->second.lru_it);
        } else {
            typename _lru_list_t::iterator it = m_lru_list.emplace(m_lru_list.end(), key);
            std::pair<typename _lru_map_t::iterator, bool> rc =
                m_cache.emplace(key, _map_value_t(std::forward<TArgs>(args)...));
            rc.first->second.lru_it = it;
            ++m_size;
        }
    }

    template<typename TIterator>
    inline static void move_to_top(_lru_list_t& list, TIterator& it)
    {
        list.splice(list.end(), list, it);
    }

private:
    size_t m_capacity;

    size_t m_size;
    _lru_map_t m_cache;
    _lru_list_t m_lru_list;
};

} // namespace lru
} // namespace wstux

#endif /* _LRU_CACHE_STD_LRU_CACHE_H */

