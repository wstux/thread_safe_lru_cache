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

#ifndef _THREAD_SAFE_CACHE_LIBS_CACHE_TRAITS_H_
#define _THREAD_SAFE_CACHE_LIBS_CACHE_TRAITS_H_

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    #include <vector>

    #include <boost/intrusive/unordered_set.hpp>
#else
    #include <unordered_map>
#endif

namespace wstux {
namespace cache {
namespace details {
namespace common {

////////////////////////////////////////////////////////////////////////////////
// Specifics of cache implementation

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
/**
 *  \brief  Boost intrusive structures.
 */
namespace bi = boost::intrusive;

template<typename THash, template<typename, typename> class TNode>
struct node_hash
{
    template<typename T>
    size_t operator()(const T& t) const { return THash{}(t); }

    template<typename TKey, typename TValue>
    size_t operator()(const TNode<TKey, TValue>& n) const { return THash{}(n.key); }
};

template<typename TKey>
const TKey& _key(const TKey& k) { return k; }

template<template<typename, typename> class TNode, typename TKey, typename TValue>
const TKey& _key(const TNode<TKey, TValue>& n) { return n.key; }

template<typename TKeyEqual, template<typename, typename> class TNode>
struct node_equal
{
    template<typename T1, typename T2>
    bool operator()(const T1& l, const T2& r) const { return TKeyEqual{}(_key(l), _key(r)); }
};
#endif

////////////////////////////////////////////////////////////////////////////////
// struct hash_table_traits

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
template<typename TTraits, template<typename, typename> class TNode>
#else
template<typename TTraits, template<typename, typename, typename> class TTableValue>
#endif
struct hash_table_traits
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
    typedef TNode<key_type, value_type>             _node_t;
    typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<_node_t>          _node_allocator_t;

    typedef bi::constant_time_size<true>            _ct_size_t;
    typedef bi::hash<node_hash<hasher, TNode>>      _intr_hash_t;
    typedef bi::equal<node_equal<key_equal, TNode>> _intr_key_equal_t;
    typedef bi::unordered_set<_node_t, _ct_size_t, _intr_hash_t, _intr_key_equal_t> _hash_table_t;

    typedef typename _hash_table_t::bucket_type     _bucket_type_t;
    typedef typename _hash_table_t::bucket_traits   _bucket_traits_t;

    typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<_bucket_traits_t> _bucket_allocator_t;
    typedef std::vector<_bucket_type_t, _bucket_allocator_t>        _buckets_list_t;
#else
    typedef TTableValue<key_type, value_type, allocator_type>   _table_value_t;
    typedef std::unordered_map<key_type, _table_value_t, hasher, key_equal, allocator_type> _hash_table_t;
#endif

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    template<typename T, typename... TArgs>
    static T* allocate(_node_allocator_t& allocator, TArgs&&... args)
    {
        using allocator_traits_t = std::allocator_traits<_node_allocator_t>;

        T* p_raw = allocator_traits_t::allocate(allocator, 1);
        allocator_traits_t::construct(allocator, p_raw, std::forward<TArgs>(args)...);
        return p_raw;
    }

    template<typename T>
    static void deallocate(_node_allocator_t& allocator, T* p_raw)
    {
        using allocator_traits_t = std::allocator_traits<_node_allocator_t>;

        allocator_traits_t::destroy(allocator, p_raw);
        allocator_traits_t::deallocate(allocator, p_raw, 1);
    }
#endif
};

////////////////////////////////////////////////////////////////////////////////
// struct type_traits

template<typename TKey, typename TValue, typename THash, typename TKeyEqual, typename TAllocator>
struct type_traits
{
    typedef TAllocator          allocator_type;
    typedef TKey                key_type;
    typedef TValue              value_type;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef std::size_t         size_type;
    typedef THash               hasher;
    typedef TKeyEqual           key_equal;
};

} // namespace common
} // namespace details
} // namespace cache
} // namespace wstux

#endif /* _THREAD_SAFE_CACHE_LIBS_CACHE_TRAITS_H_ */
