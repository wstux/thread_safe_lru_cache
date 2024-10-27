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

#ifndef _TTL_CACHE_STD_BASE_TTL_CACHE_H
#define _TTL_CACHE_STD_BASE_TTL_CACHE_H

#include <chrono>
#include <list>
#include <unordered_map>

namespace wstux {
namespace ttl {
namespace details {

template<typename TKey, typename TValue, class THash, class TKeyEqual>
class base_ttl_cache
{
protected:
    typedef TKey                        key_type;
    typedef TValue                      value_type;
    typedef size_t                      size_type;
    typedef THash                       hasher;
    typedef TKeyEqual                   key_equal;
    typedef value_type&                 reference;
    typedef const value_type&           const_reference;
    typedef value_type*                 pointer;
    typedef const value_type*           const_pointer;

    typedef std::chrono::steady_clock   _clock_t;
    typedef _clock_t::time_point        _time_point_t;
    typedef std::list<key_type>         _ttl_list_t;

    struct _hash_table_value
    {
        template<typename... TArgs>
        explicit _hash_table_value(TArgs&&... args)
            : value(std::forward<TArgs>(args)...)
            , time_point(_clock_t::now())
        {}

        value_type value;
        _time_point_t time_point;
        typename _ttl_list_t::iterator ttl_it;
    };
    typedef _hash_table_value       _table_value_t;
    typedef std::unordered_map<key_type, _table_value_t, hasher, key_equal> _hash_table_t;

    explicit base_ttl_cache(size_type ttl_msecs, size_type capacity)
        : m_capacity(capacity)
        , m_time_to_live(std::chrono::milliseconds(ttl_msecs))
    {
        m_hash_tbl.reserve(capacity);
    }

    virtual ~base_ttl_cache() {}

    size_type capacity() const { return m_capacity; }

    void clear()
    {
        m_hash_tbl.clear();
        m_ttl_list.clear();
    }

    void erase(const key_type& key)
    {
        typename _hash_table_t::iterator it = m_hash_tbl.find(key);
        if (it != m_hash_tbl.end()) {
            m_ttl_list.erase(it->second.ttl_it);
            m_hash_tbl.erase(it);
        }
    }

    void erase(typename _hash_table_t::iterator& it)
    {
        m_ttl_list.erase(it->second.ttl_it);
        m_hash_tbl.erase(it);
    }

    typename _hash_table_t::iterator find_in_tbl(const key_type& key)
    {
        return m_hash_tbl.find(key);
    }

    template<typename... TArgs>
    void insert(const key_type& key, TArgs&&... args)
    {
        if (size() >= m_capacity) {
            m_hash_tbl.erase(m_ttl_list.front());
            m_ttl_list.front() = key;
            std::pair<typename _hash_table_t::iterator, bool> rc =
                m_hash_tbl.emplace(key, _table_value_t(std::forward<TArgs>(args)...));
            rc.first->second.ttl_it = m_ttl_list.begin();
            move_to_top(rc.first);
        } else {
            typename _ttl_list_t::iterator it = m_ttl_list.emplace(m_ttl_list.end(), key);
            std::pair<typename _hash_table_t::iterator, bool> rc =
                m_hash_tbl.emplace(key, _table_value_t(std::forward<TArgs>(args)...));
            rc.first->second.ttl_it = it;
        }
    }

    bool is_expired(typename _hash_table_t::iterator& it) const
    {
        const std::chrono::milliseconds exp_secs =
            std::chrono::duration_cast<std::chrono::milliseconds>(_clock_t::now() - it->second.time_point);
        return (exp_secs.count() > m_time_to_live.count());
    } 

    bool is_find(typename _hash_table_t::iterator& it) const { return (it != m_hash_tbl.end()); }

    void move_to_top(typename _hash_table_t::iterator& it)
    {
        m_ttl_list.splice(m_ttl_list.end(), m_ttl_list, it->second.ttl_it);
        it->second.time_point = _clock_t::now();
    }

    void reset(size_type ttl_msecs, size_type new_capacity)
    {
        clear();

        m_capacity = new_capacity;
        m_time_to_live = std::chrono::milliseconds(ttl_msecs);
        m_hash_tbl.reserve(m_capacity);
    }

    inline size_type size() const { return m_hash_tbl.size(); }

    static const value_type& load(const typename _hash_table_t::iterator& it)
    {
        return it->second.value;
    }

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
    std::chrono::milliseconds m_time_to_live;

    _hash_table_t m_hash_tbl;
    _ttl_list_t m_ttl_list;
};

} // namespace details
} // namespace ttl
} // namespace wstux

#endif /* _TTL_CACHE_STD_BASE_TTL_CACHE_H */

