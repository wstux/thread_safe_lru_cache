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

#ifndef _LRU_CACHE_STD_INTRUSIVE_LRU_CACHE_H
#define _LRU_CACHE_STD_INTRUSIVE_LRU_CACHE_H

#include <memory>
#include <vector>

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/unordered_set.hpp>

namespace wstux {
namespace lru {
namespace details {

namespace bi = boost::intrusive;

typedef bi::link_mode<bi::normal_link>          link_mode;
typedef bi::list_base_hook<link_mode>           list_hook;
typedef bi::unordered_set_base_hook<link_mode>  hash_tbl_hook;

template<typename TKey, typename TValue>
struct lru_node : public list_hook
                , public hash_tbl_hook
{
    typedef TKey        key_type;
    typedef TValue      value_type;

    lru_node(key_type&& k, value_type&& v)
        : key(std::move(k))
        , value(std::move(v))
    {}

    key_type key;
    value_type value;
};

template<typename THash>
struct lru_node_hash
{
    template<typename T>
    size_t operator()(const T& t) const { return THash{}(t); }

    template<typename TKey, typename TValue>
    size_t operator()(const lru_node<TKey, TValue>& n) const { return THash{}(n.key); }
};

template<typename TKey>
const TKey& _key(const TKey& k) { return k; }

template<typename TKey, typename TValue>
const TKey& _key(const lru_node<TKey, TValue>& n) { return n.key; }

template<typename TKeyEqual>
struct lru_node_equal
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

    typedef lru_node<key_type, value_type>  _lru_node;
    typedef bi::list<_lru_node, bi::constant_time_size<false>>  lru_list;

    //typedef value_type          hash_table_value;
    typedef bi::constant_time_size<true>            _is_ct_size;
    typedef bi::hash<lru_node_hash<hasher>>         _intrusive_hash;
    typedef bi::equal<lru_node_equal<key_equal>>    _intrusive_key_equal;
    typedef bi::unordered_set<_lru_node, _is_ct_size, _intrusive_hash, _intrusive_key_equal> hash_table;

    typedef typename hash_table::bucket_type    _bucket_type;
    typedef typename hash_table::bucket_traits  _bucket_traits;
};

/**
 *  @brief  Implementation of lru_cache based on intrusive containers.
 *  @details    The idea of a cache based on intrusive containers is taken from
 *              https://www.youtube.com/watch?v=60XhYzkXu1M&t=2358s
 */
template<typename TKey, typename TValue, class THash, class TKeyEqual>
class base_lru_cache
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

    typedef typename traits::lru_list           _lru_list_t;
    typedef typename traits::hash_table         _hash_table_t;

    explicit base_lru_cache(size_type capacity)
        : m_capacity(capacity)
        , m_buckets(m_capacity)
        , m_hash_tbl(_bucket_traits_t(m_buckets.data(), m_buckets.capacity()))
    {}

    virtual ~base_lru_cache() {}

    size_type capacity() const { return m_capacity; }

    void clear()
    {
        m_hash_tbl.clear();
        m_lru_list.clear();
    }

    void erase(const key_type& key)
    {
        typename _hash_table_t::iterator it =
            m_hash_tbl.find(key, m_hash_tbl.hash_function(), m_hash_tbl.key_eq());
        if (it != m_hash_tbl.end()) {
            m_lru_list.erase(m_lru_list.iterator_to(*it));
            m_hash_tbl.erase(it);
        }
    }

    typename _hash_table_t::iterator find_in_tbl(const key_type& key)
    {
        return m_hash_tbl.find(key, m_hash_tbl.hash_function(), m_hash_tbl.key_eq());
    }

    template<typename... TArgs>
    void insert(const key_type& key, TArgs&&... args)
    {
        if (size() >= m_capacity) {
            _lru_node_ptr_t p_node = extract_node(m_lru_list.begin());
            p_node->key = key;
            p_node->value = std::move(value_type(std::forward<TArgs>(args)...));
            insert_node(std::move(p_node));
        } else {
            _lru_node_ptr_t p_node = std::make_unique<_lru_node_t>(key_type(key), value_type(std::forward<TArgs>(args)...));
            insert_node(std::move(p_node));
        }
    }

    bool is_find(typename _hash_table_t::iterator& it) const { return (it != m_hash_tbl.end()); } 

    void move_to_top(typename _hash_table_t::iterator& it)
    {
        m_lru_list.splice(m_lru_list.end(), m_lru_list, m_lru_list.iterator_to(*it));
    }

    void reset(size_type new_capacity)
    {
        clear();

        m_capacity = new_capacity;
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
    typedef typename traits::_lru_node          _lru_node_t;
    typedef std::unique_ptr<_lru_node_t>        _lru_node_ptr_t;

    _lru_node_ptr_t extract_node(typename _lru_list_t::iterator it)
    {
        _lru_node_ptr_t p_node(&*it);
        m_hash_tbl.erase(m_hash_tbl.iterator_to(*it));
        m_lru_list.erase(it);
        return p_node;
    }

    void insert_node(_lru_node_ptr_t p_node)
    {
        m_hash_tbl.insert(*p_node);
        m_lru_list.insert(m_lru_list.end(), *p_node);
        [[maybe_unused]] lru_node<TKey, TValue>* p_ignore = p_node.release();
    }

private:
    size_t m_capacity;

    _buckets_list_t m_buckets;
    _hash_table_t m_hash_tbl;
    _lru_list_t m_lru_list;
};

} // namespace details
} // namespace lru
} // namespace wstux

#endif /* _LRU_CACHE_STD_INTRUSIVE_LRU_CACHE_H */

