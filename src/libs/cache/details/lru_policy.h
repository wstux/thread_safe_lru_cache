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

#ifndef _THREAD_SAFE_CACHE_LIBS_CACHE_LRU_POLICY_H_
#define _THREAD_SAFE_CACHE_LIBS_CACHE_LRU_POLICY_H_

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    #include <vector>

    #include <boost/intrusive/list.hpp>
    #include <boost/intrusive/unordered_set.hpp>
#else
    #include <list>
    #include <unordered_map>
#endif

namespace wstux {
namespace cache {
namespace details {
namespace lru {

////////////////////////////////////////////////////////////////////////////////
// Specifics of cache implementation

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
/**
 *  \brief  Implementations based on boost intrusive.
 */
namespace bi = boost::intrusive;

template<typename TKey, typename TValue>
struct node : public bi::list_base_hook<bi::link_mode<bi::normal_link>>
            , public bi::unordered_set_base_hook<bi::link_mode<bi::normal_link>>
{
    typedef TKey        key_type;
    typedef TValue      value_type;

    node(key_type&& k, value_type&& v)
        : key(std::move(k))
        , value(std::move(v))
    {}

    key_type key;
    value_type value;
};

template<typename THash>
struct node_hash
{
    template<typename T>
    size_t operator()(const T& t) const { return THash{}(t); }

    template<typename TKey, typename TValue>
    size_t operator()(const node<TKey, TValue>& n) const { return THash{}(n.key); }
};

template<typename TKey>
const TKey& _key(const TKey& k) { return k; }

template<typename TKey, typename TValue>
const TKey& _key(const node<TKey, TValue>& n) { return n.key; }

template<typename TKeyEqual>
struct node_equal
{
    template<typename T1, typename T2>
    bool operator()(const T1& l, const T2& r) const { return TKeyEqual{}(_key(l), _key(r)); }
};
#else
/**
 *  \brief  Implementations based on standard library.
 */
template<typename TKey, typename TValue, typename TAllocator>
struct hash_table_value
{
    typedef TKey                            key_type;
    typedef TValue                          value_type;
    typedef std::list<key_type, TAllocator> _list_t;

    template<typename... TArgs>
    explicit hash_table_value(TArgs&&... args)
        : value(std::forward<TArgs>(args)...)
    {}

    value_type value;
    typename _list_t::iterator list_it;
};
#endif

////////////////////////////////////////////////////////////////////////////////
// struct lru_policy

template<typename TTraits>
struct lru_policy
{
    typedef typename TTraits::allocator_type    allocator_type;
    typedef typename TTraits::key_type          key_type;
    typedef typename TTraits::value_type        value_type;
    typedef typename TTraits::reference         reference;
    typedef typename TTraits::const_reference   const_reference;
    typedef typename TTraits::pointer           pointer;
    typedef typename TTraits::const_pointer     const_pointer;
    typedef typename TTraits::size_type         size_type;
    typedef typename TTraits::hasher            hasher;
    typedef typename TTraits::key_equal         key_equal;

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    typedef node<key_type, value_type>              _node_t;
    typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<_node_t>          _node_allocator_t;

    typedef bi::list<_node_t, bi::constant_time_size<false>>    _list_t;

    typedef bi::constant_time_size<true>            _ct_size_t;
    typedef bi::hash<node_hash<hasher>>             _intr_hash_t;
    typedef bi::equal<node_equal<key_equal>>        _intr_key_equal_t;
    typedef bi::unordered_set<_node_t, _ct_size_t, _intr_hash_t, _intr_key_equal_t> _hash_table_t;

    typedef typename _hash_table_t::bucket_type     _bucket_type_t;
    typedef typename _hash_table_t::bucket_traits   _bucket_traits_t;

    typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<_bucket_traits_t> _bucket_allocator_t;
    typedef std::vector<_bucket_type_t, _bucket_allocator_t>        _buckets_list_t;
#else
    typedef hash_table_value<key_type, value_type, allocator_type>  _table_value_t;
    typedef typename _table_value_t::_list_t                        _list_t;
    typedef std::unordered_map<key_type, _table_value_t, hasher, key_equal, allocator_type> _hash_table_t;
#endif

    lru_policy(size_type capacity, const allocator_type& alloc)
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        : allocator(alloc)
        , buckets(capacity, alloc)
        , hash_tbl(_bucket_traits_t(buckets.data(), buckets.capacity()))
#else
        : hash_tbl(alloc)
        , list(alloc)
#endif
    {
#if ! defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        hash_tbl.reserve(capacity);
#endif
    }

    void clear() { list.clear(); }

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    _node_t* erase(typename _hash_table_t::iterator& it)
#else
    void erase(typename _hash_table_t::iterator& it)
#endif
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        list.erase(list.iterator_to(*it));
        _node_t* p = &(*it);
        hash_tbl.erase(it);
        return p;
#else
        list.erase(it->second.list_it);
        hash_tbl.erase(it);
#endif
    }

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    _node_t* evict()
#else
    #if __cplusplus >= 201703
    typename _hash_table_t::node_type evict()
    #else
    void evict()
    #endif
#endif
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        typename _list_t::iterator it = list.begin();
        _node_t* p_node = &*it;
        hash_tbl.erase(hash_tbl.iterator_to(*it));
        list.erase(it);
        return p_node;
#else
    #if __cplusplus >= 201703
        typename _hash_table_t::node_type node = hash_tbl.extract(list.front());
        return node;
    #else
        hash_tbl.erase(list.front());
    #endif
#endif
    }

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    void insert(_node_t* p_node)
#else
    template<typename... TArgs>
    void insert(const key_type& key, TArgs&&... args)
#endif
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        hash_tbl.insert(*p_node);
        list.insert(list.end(), *p_node);
#else
        typename _list_t::iterator it = list.emplace(list.end(), key);
        std::pair<typename _hash_table_t::iterator, bool> rc =
            hash_tbl.emplace(key, _table_value_t(std::forward<TArgs>(args)...));
        rc.first->second.list_it = it;
#endif
    }

    template<typename... TArgs>
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    void insert_node(_node_t* p_node, const key_type& key, TArgs&&... args)
#else
    #if __cplusplus >= 201703
    void insert_node(typename _hash_table_t::node_type&& node, const key_type& key, TArgs&&... args)
    #else
    void insert_node(const key_type& key, TArgs&&... args)
    #endif
#endif
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        p_node->key = key;
        p_node->value = std::move(value_type(std::forward<TArgs>(args)...));
        hash_tbl.insert(*p_node);
        list.insert(list.end(), *p_node);
#else
    #if __cplusplus >= 201703
        node.key() = key;
        typename _hash_table_t::insert_return_type rc = hash_tbl.insert(std::move(node));
        rc.position->second.value = std::move(value_type(std::forward<TArgs>(args)...));
        list.front() = key;
        move_to_top(list, rc.position);
    #else
        list.front() = key;
        std::pair<typename _hash_table_t::iterator, bool> rc =
            hash_tbl.emplace(key, typename TTraits::_table_value_t(std::forward<TArgs>(args)...));
        rc.first->second.list_it = list.begin();
        move_to_top(list, rc.first);
    #endif
#endif
    }

    void reset(size_type new_capacity)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        _buckets_list_t bucks(new_capacity);
        hash_tbl.rehash(_bucket_traits_t(bucks.data(), bucks.capacity()));
        buckets = std::move(bucks);
#else
        hash_tbl.reserve(new_capacity);
#endif
    }

    inline static void move_to_top(_list_t& list, typename _hash_table_t::iterator& it)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        list.splice(list.end(), list, list.iterator_to(*it));
#else
        list.splice(list.end(), list, it->second.list_it);
#endif
    }

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    _node_allocator_t allocator;
    _buckets_list_t buckets;
#endif
    _hash_table_t hash_tbl;
    _list_t list;
};

} // namespace lru
} // namespace details
} // namespace cache
} // namespace wstux

#endif /* _THREAD_SAFE_CACHE_LIBS_CACHE_LRU_POLICY_H_ */
