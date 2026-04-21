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

#ifndef _THREAD_SAFE_CACHE_LIBS_CACHE_LFU_POLICY_H_
#define _THREAD_SAFE_CACHE_LIBS_CACHE_LFU_POLICY_H_

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    #include <boost/intrusive/set.hpp>
#else
    #include <map>
#endif

#include "cache/details/traits.h"

namespace wstux {
namespace cache {
namespace details {
namespace lfu {

////////////////////////////////////////////////////////////////////////////////
// Specifics of cache implementation

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
/**
 *  \brief  Implementations based on boost intrusive.
 */
namespace bi = boost::intrusive;

template<typename TKey, typename TValue>
struct node : public bi::set_base_hook<bi::link_mode<bi::normal_link>>
            , public bi::unordered_set_base_hook<bi::link_mode<bi::normal_link>>
{
    typedef TKey        key_type;
    typedef TValue      value_type;

    node(key_type&& k, value_type&& v)
        : key(std::move(k))
        , value(std::move(v))
        , freq(0)
    {}

    key_type key;
    value_type value;
    size_t freq;
};

template<typename TKey, typename TValue>
using _tbl_node = node<TKey, TValue>;

template<typename TKey, typename TValue>
struct key_extractor
{
    typedef TKey    type;
    const TKey& operator()(const node<TKey, TValue>& n) const { return cache::details::common::_key(n); }
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
    typedef std::multimap<std::size_t, key_type, TAllocator>    _frequency_map_t;

    template<typename... TArgs>
    explicit hash_table_value(TArgs&&... args)
        : value(std::forward<TArgs>(args)...)
        , freq(0)
    {}

    value_type value;
    typename _frequency_map_t::iterator freq_it;
    size_t freq;
};

template<typename TKey, typename TValue, typename TAllocator>
using _tbl_node = hash_table_value<TKey, TValue, TAllocator>;
#endif

////////////////////////////////////////////////////////////////////////////////
// struct lfu_policy

template<typename TTraits>
struct lfu_policy
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

    typedef cache::details::common::hash_table_traits<TTraits, _tbl_node>   _tbl_traits;
    typedef typename _tbl_traits::_hash_table_t     _hash_table_t;

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    typedef typename _tbl_traits::_node_t           _node_t;
    typedef typename _tbl_traits::_node_allocator_t _node_allocator_t;

    typedef typename _tbl_traits::_bucket_traits_t  _bucket_traits_t;
    typedef typename _tbl_traits::_buckets_list_t   _buckets_list_t;

    typedef bi::multiset<_node_t, bi::key_of_value<key_extractor<key_type, value_type>>>    _frequency_map_t;
#else
    typedef typename _tbl_traits::_table_value_t    _table_value_t;
    typedef std::multimap<size_type, key_type, key_equal, allocator_type>                   _frequency_map_t;
#endif

    lfu_policy(size_type capacity, const allocator_type& alloc)
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        : allocator(alloc)
        , buckets(capacity, alloc)
        , hash_tbl(_bucket_traits_t(buckets.data(), buckets.capacity()))
#else
        : hash_tbl(alloc)
        , freq_tbl(alloc)
#endif
    {
#if ! defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        hash_tbl.reserve(capacity);
#endif
    }

    void clear() { freq_tbl.clear(); }

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    _node_t* erase(typename _hash_table_t::iterator& it)
#else
    void erase(typename _hash_table_t::iterator& it)
#endif
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        freq_tbl.erase(freq_tbl.iterator_to(*it));
        _node_t* p = &(*it);
        hash_tbl.erase(it);
        return p;
#else
        freq_tbl.erase(it->second.freq_it);
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
        typename _frequency_map_t::iterator it = freq_tbl.begin();
        _node_t* p_node = &*it;
        hash_tbl.erase(hash_tbl.iterator_to(*it));
        freq_tbl.erase(it);
        return p_node;
#else
    #if __cplusplus >= 201703
        typename _hash_table_t::node_type node = hash_tbl.extract(freq_tbl.begin()->second);
        return node;
    #else
        hash_tbl.erase(freq_tbl.begin()->second);
        freq_tbl.erase(freq_tbl.begin());
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
        ++p_node->freq;
        hash_tbl.insert(*p_node);
        freq_tbl.insert(*p_node);
#else
        std::pair<typename _hash_table_t::iterator, bool> rc =
            hash_tbl.emplace(key, _table_value_t(std::forward<TArgs>(args)...));
        ++rc.first->second.freq;
        rc.first->second.freq_it = freq_tbl.emplace(rc.first->second.freq, key);
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
        p_node->freq = 1;
        hash_tbl.insert(*p_node);
        freq_tbl.insert(*p_node);
#else
    #if __cplusplus >= 201703
        node.key() = key;
        typename _hash_table_t::insert_return_type rc = hash_tbl.insert(std::move(node));
        rc.position->second.value = std::move(value_type(std::forward<TArgs>(args)...));
        rc.position->second.freq = 1;

        typename _frequency_map_t::node_type freq_node = freq_tbl.extract(rc.position->second.freq_it);
        freq_node.key() = rc.position->second.freq;
        typename _frequency_map_t::iterator freq_it = freq_tbl.insert(std::move(freq_node));
        freq_it->second = key;
        rc.position->second.freq_it = freq_it;
    #else
        std::pair<typename _hash_table_t::iterator, bool> rc =
            hash_tbl.emplace(key, typename TTraits::_table_value_t(std::forward<TArgs>(args)...));
        rc.first->second.freq = 1;
        rc.first->second.freq_it = freq_tbl.emplace(rc.first->second.freq, key);
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

    void touch(typename _hash_table_t::iterator& it)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        freq_tbl.erase(freq_tbl.iterator_to(*it));
        ++it->freq;
        _node_t* p_node = &*it;
        freq_tbl.insert(*p_node);
#else
        ++it->second.freq;
    #if __cplusplus >= 201703
        typename _frequency_map_t::node_type freq_node = freq_tbl.extract(it->second.freq_it);
        freq_node.key() = it->second.freq;
        it->second.freq_it = freq_tbl.insert(std::move(freq_node));
    #else
        freq_tbl.erase(it->second.freq_it);
        it->second.freq_it = freq_tbl.emplace(it->second.freq, it->first);
    #endif
#endif
    }

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    _node_allocator_t allocator;
    _buckets_list_t buckets;
#endif
    _hash_table_t hash_tbl;
    _frequency_map_t freq_tbl;
};

} // namespace lfu
} // namespace details
} // namespace cache
} // namespace wstux

#endif /* _THREAD_SAFE_CACHE_LIBS_CACHE_LFU_POLICY_H_ */
