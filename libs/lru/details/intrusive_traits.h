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

#ifndef _LRU_CACHE_INTRUSIVE_TRAITS_H
#define _LRU_CACHE_INTRUSIVE_TRAITS_H

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
    typedef TValue      mapped_type;

    lru_node(key_type&& k, mapped_type&& v)
        : key(std::move(k))
        , value(std::move(v))
    {}

    key_type key;
    mapped_type value;
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
    typedef TValue              mapped_type;
    typedef size_t              size_type;
    typedef THash               hasher;
    typedef TKeyEqual           key_equal;

    typedef lru_node<key_type, mapped_type>                     _lru_node;
    typedef bi::list<_lru_node, bi::constant_time_size<false>>  lru_list;

    typedef bi::constant_time_size<true>            _is_ct_size;
    typedef bi::hash<lru_node_hash<hasher>>         _intrusive_hash;
    typedef bi::equal<lru_node_equal<key_equal>>    _intrusive_key_equal;
    typedef bi::unordered_set<_lru_node, _is_ct_size, _intrusive_hash, _intrusive_key_equal> hash_table;

    typedef typename hash_table::bucket_type    _bucket_type;
    typedef typename hash_table::bucket_traits  _bucket_traits;

    typedef typename hash_table::value_type     value_type;
    typedef value_type&                         reference;
    typedef const value_type&                   const_reference;
    typedef value_type*                         pointer;
    typedef const value_type*                   const_pointer;
};

} // namespace details
} // namespace lru
} // namespace wstux

#endif /* _LRU_CACHE_INTRUSIVE_TRAITS_H */

