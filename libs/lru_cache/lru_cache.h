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

#include <functional>
#include <list>
#include <unordered_map>

namespace wstux {
namespace cnt {

template<typename TList>
void move_to_front(TList& list, typename TList::iterator& it)
{
    list.splice(list.begin(), list, it);
}

template<typename TKey, typename TValue,
         template<typename THType> class THasher = std::hash,
         template<typename TMKey, typename TMVal, typename TMHash> class TMap = std::unordered_map,
         template<typename TLType> class TList = std::list>
class lru_cache
{
    typedef TKey                _key_type;
    typedef TValue              _value_type;
    typedef THasher<_key_type>  _hasher;

    struct _map_value_t
    {
        explicit _map_value_t(const _value_type& v)
            : value(v)
        {}

        explicit _map_value_t(_value_type&& v)
            : value(std::move(v))
        {}

        _value_type value;
        typename TList<TKey>::iterator lru_it;
    };

    using _lru_list_t = TList<_key_type>;
    using _cache_map_t = TMap<_key_type, _map_value_t, _hasher>;

public:
    typedef _key_type           key_type;
    typedef _value_type         value_type;
    typedef size_t              size_type;
    typedef _hasher             hasher;
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

    bool find(const key_type& key, value_type& result)
    {
        typename _cache_map_t::iterator it = m_cache.find(key);
        if (it == m_cache.end()) {
            return false;
        }

        move_to_front(m_lru_list, it->second.lru_it);
        result = it->second.value;
        return true;
    }

    bool insert(const key_type& key, const value_type& val)
    {
        std::pair<typename _cache_map_t::iterator, bool> rc =
            m_cache.emplace(key, _map_value_t(val));
        if (! rc.second) {
            return false;
        }

        m_lru_list.emplace_front(key);
        rc.first->second.lru_it = m_lru_list.begin();

        if (m_size >= m_capacity) {
            if (m_lru_list.empty()) {
                return false;
            }
            typename _cache_map_t::iterator it = m_cache.find(m_lru_list.back());
            m_lru_list.pop_back();
            if (it != m_cache.end()) {
                m_cache.erase(it);
            }
        } else {
            ++m_size;
        }
        return true;
    }

    void update(const key_type& key, const value_type& val)
    {
        std::pair<typename _cache_map_t::iterator, bool> rc =
            m_cache.emplace(key, _map_value_t(val));
        if (! rc.second) {
            rc.first->second.value = val;
            move_to_front(m_lru_list, rc.first->second.lru_it);
            return;
        }

        m_lru_list.emplace_front(key);
        rc.first->second.lru_it = m_lru_list.begin();

        if (m_size >= m_capacity) {
            if (m_lru_list.empty()) {
                return;
            }
            typename _cache_map_t::iterator it = m_cache.find(m_lru_list.back());
            m_lru_list.pop_back();
            if (it != m_cache.end()) {
                m_cache.erase(it);
            }
        } else {
            ++m_size;
        }
    }

/*    void update(const key_type& key, value_type&& val)
    {
        std::pair<typename _cache_map_t::iterator, bool> rc =
            m_cache.emplace(key, _map_value_t(val));
        if (! rc.second) {
            rc.first->second.value = std::move(val);
            move_to_front(m_lru_list, rc.first->second.lru_it);
            return;
        }

        m_lru_list.emplace_front(key);
        rc.first->second.lru_it = m_lru_list.begin();

        if (m_size >= m_capacity) {
            if (m_lru_list.empty()) {
                return;
            }
            typename _cache_map_t::iterator it = m_cache.find(m_lru_list.back());
            m_lru_list.pop_back();
            if (it != m_cache.end()) {
                m_cache.erase(it);
            }
        } else {
            ++m_size;
        }
    }
*/
private:
    const size_t m_capacity;

    size_t m_size;
    _cache_map_t m_cache;
    _lru_list_t m_lru_list;
};

} // namespace cnt
} // namespace wstux

#endif /* _LRU_CACHE_LRU_CACHE_H */

