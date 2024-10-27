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

#ifndef _TTL_CACHE_INTRUSIVE_BASE_TTL_CACHE_H
#define _TTL_CACHE_INTRUSIVE_BASE_TTL_CACHE_H

#include <chrono>
#include <memory>
#include <vector>

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/unordered_set.hpp>

namespace wstux {
namespace ttl {
namespace details {

namespace bi = boost::intrusive;

typedef bi::link_mode<bi::normal_link>          link_mode;
typedef bi::list_base_hook<link_mode>           list_hook;
typedef bi::unordered_set_base_hook<link_mode>  hash_tbl_hook;

template<typename TKey, typename TValue>
struct ttl_node : public list_hook
                , public hash_tbl_hook
{
    typedef TKey        key_type;
    typedef TValue      value_type;

    typedef std::chrono::steady_clock   _clock_t;
    typedef _clock_t::time_point        _time_point_t;

    ttl_node(key_type&& k, value_type&& v)
        : key(std::move(k))
        , value(std::move(v))
        , time_point(_clock_t::now())
    {}

    key_type key;
    value_type value;
    _time_point_t time_point;
};

template<typename THash>
struct ttl_node_hash
{
    template<typename T>
    size_t operator()(const T& t) const { return THash{}(t); }

    template<typename TKey, typename TValue>
    size_t operator()(const ttl_node<TKey, TValue>& n) const { return THash{}(n.key); }
};

template<typename TKey>
const TKey& _key(const TKey& k) { return k; }

template<typename TKey, typename TValue>
const TKey& _key(const ttl_node<TKey, TValue>& n) { return n.key; }

template<typename TKeyEqual>
struct ttl_node_equal
{
    template<typename T1, typename T2>
    bool operator()(const T1& l, const T2& r) const { return TKeyEqual{}(_key(l), _key(r)); }
};

template<typename TKey, typename TValue, typename THash, typename TKeyEqual>
struct intrusive_traits
{
    typedef TKey                key_type;
    typedef TValue              value_type;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;

    typedef size_t              size_type;
    typedef THash               hasher;
    typedef TKeyEqual           key_equal;

    typedef ttl_node<key_type, value_type>  _ttl_node;
    typedef bi::list<_ttl_node, bi::constant_time_size<false>>  ttl_list;

    typedef bi::constant_time_size<true>            _is_ct_size;
    typedef bi::hash<ttl_node_hash<hasher>>         _intrusive_hash;
    typedef bi::equal<ttl_node_equal<key_equal>>    _intrusive_key_equal;
    typedef bi::unordered_set<_ttl_node, _is_ct_size, _intrusive_hash, _intrusive_key_equal> hash_table;

    typedef typename hash_table::bucket_type    _bucket_type;
    typedef typename hash_table::bucket_traits  _bucket_traits;
};

/**
 *  @brief  Implementation of ttl_cache based on intrusive containers.
 *  @details    The idea of a cache based on intrusive containers is taken from
 *              https://www.youtube.com/watch?v=60XhYzkXu1M&t=2358s
 */
template<typename TKey, typename TValue, class THash, class TKeyEqual>
class base_ttl_cache
{
private:
    typedef intrusive_traits<TKey, TValue, THash, TKeyEqual>    traits;

protected:
    typedef typename traits::key_type           key_type;
    typedef typename traits::value_type         value_type;
    typedef typename traits::size_type          size_type;
    typedef typename traits::hasher             hasher;
    typedef typename traits::key_equal          key_equal;
    typedef typename traits::reference          reference;
    typedef typename traits::const_reference    const_reference;
    typedef typename traits::pointer            pointer;
    typedef typename traits::const_pointer      const_pointer;

    typedef typename traits::_ttl_node::_clock_t        _clock_t;
    typedef typename traits::_ttl_node::_time_point_t   _time_point_t;
    typedef typename traits::ttl_list                   _ttl_list_t;
    typedef typename traits::hash_table                 _hash_table_t;

    explicit base_ttl_cache(size_type ttl_msecs, size_type capacity)
        : m_capacity(capacity)
        , m_time_to_live(std::chrono::milliseconds(ttl_msecs))
        , m_buckets(m_capacity)
        , m_hash_tbl(_bucket_traits_t(m_buckets.data(), m_buckets.capacity()))
    {}

    virtual ~base_ttl_cache() {}

    size_type capacity() const { return m_capacity; }

    void clear()
    {
        m_hash_tbl.clear();
        m_ttl_list.clear();
    }

    void erase(const key_type& key)
    {
        typename _hash_table_t::iterator it =
            m_hash_tbl.find(key, m_hash_tbl.hash_function(), m_hash_tbl.key_eq());
        if (it != m_hash_tbl.end()) {
            m_ttl_list.erase(m_ttl_list.iterator_to(*it));
            m_hash_tbl.erase(it);
        }
    }

    void erase(typename _hash_table_t::iterator& it)
    {
        m_ttl_list.erase(m_ttl_list.iterator_to(*it));
        m_hash_tbl.erase(it);
    }

    typename _hash_table_t::iterator find_in_tbl(const key_type& key)
    {
        return m_hash_tbl.find(key, m_hash_tbl.hash_function(), m_hash_tbl.key_eq());
    }

    template<typename... TArgs>
    void insert(const key_type& key, TArgs&&... args)
    {
        if (size() >= m_capacity) {
            _ttl_node_ptr_t p_node = extract_node(m_ttl_list.begin());
            p_node->key = key;
            p_node->value = std::move(value_type(std::forward<TArgs>(args)...));
            insert_node(std::move(p_node));
        } else {
            _ttl_node_ptr_t p_node = std::make_unique<_ttl_node_t>(key_type(key), value_type(std::forward<TArgs>(args)...));
            insert_node(std::move(p_node));
        }
    }

    bool is_expired(typename _hash_table_t::iterator& it) const
    {
        const std::chrono::milliseconds exp_secs =
            std::chrono::duration_cast<std::chrono::milliseconds>(_clock_t::now() - it->time_point);
        return (exp_secs.count() > m_time_to_live.count());
    } 

    bool is_find(typename _hash_table_t::iterator& it) const { return (it != m_hash_tbl.end()); } 

    void move_to_top(typename _hash_table_t::iterator& it)
    {
        m_ttl_list.splice(m_ttl_list.end(), m_ttl_list, m_ttl_list.iterator_to(*it));
    }

    void reset(size_type ttl_msecs, size_type new_capacity)
    {
        clear();

        m_capacity = new_capacity;
        m_time_to_live = std::chrono::milliseconds(ttl_msecs);
        _buckets_list_t buckets(m_capacity);
        m_hash_tbl.rehash(_bucket_traits_t(buckets.data(), buckets.capacity()));
        m_buckets = std::move(buckets);
    }

    inline size_type size() const { return m_hash_tbl.size(); }

    static const value_type& load(const typename _hash_table_t::iterator& it)
    {
        return it->value;
    }

    static void load(const typename _hash_table_t::iterator& it, value_type& res)
    {
        res = it->value;
    }

    static void store(const typename _hash_table_t::iterator& it, value_type&& val)
    {
        it->value = val;
    }

private:
    typedef typename traits::_bucket_type       _bucket_type_t;
    typedef typename traits::_bucket_traits     _bucket_traits_t;
    typedef std::vector<_bucket_type_t>         _buckets_list_t;
    typedef typename traits::_ttl_node          _ttl_node_t;
    typedef std::unique_ptr<_ttl_node_t>        _ttl_node_ptr_t;

    _ttl_node_ptr_t extract_node(typename _ttl_list_t::iterator it)
    {
        _ttl_node_ptr_t p_node(&*it);
        m_hash_tbl.erase(m_hash_tbl.iterator_to(*it));
        m_ttl_list.erase(it);
        return p_node;
    }

    void insert_node(_ttl_node_ptr_t p_node)
    {
        m_hash_tbl.insert(*p_node);
        m_ttl_list.insert(m_ttl_list.end(), *p_node);
        [[maybe_unused]] ttl_node<TKey, TValue>* p_ignore = p_node.release();
    }

private:
    size_t m_capacity;
    std::chrono::milliseconds m_time_to_live;

    _buckets_list_t m_buckets;
    _hash_table_t m_hash_tbl;
    _ttl_list_t m_ttl_list;
};

} // namespace details
} // namespace ttl
} // namespace wstux

#endif /* _TTL_CACHE_INTRUSIVE_BASE_TTL_CACHE_H */

