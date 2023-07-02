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

#ifndef _LRU_CACHE_INTRUSIVE_NODE_H
#define _LRU_CACHE_INTRUSIVE_NODE_H

#include <memory>

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/unordered_set.hpp>

namespace wstux {
namespace lru {
namespace details {

typedef boost::intrusive::link_mode<boost::intrusive::normal_link>  link_mode;
typedef boost::intrusive::list_base_hook<link_mode>             lru_list_hook;
typedef boost::intrusive::unordered_set_base_hook<link_mode>    lru_uset_hook;

template<typename TKey, typename TValue>
struct lru_node : public lru_list_hook
                , public lru_uset_hook
{
//    lru_node(TKey k)
//        : key(std::move(k))
//    {}

    lru_node(TKey&& k, TValue&& v)
        : key(std::move(k))
        , value(std::move(v))
    {}

    TKey key;
    TValue value;
};

template<typename TKey>
const TKey& get_key(const TKey& k) { return k; }

template<typename TKey, typename TValue>
const TKey& get_key(const lru_node<TKey, TValue>& n) { return n.key; }

struct lru_node_hash
{
    template<typename T>
    size_t operator()(const T& t) const { return std::hash<T>{}(t); }
    //size_t operator()(const T& t) const { return std::hash<T>{}(get_key(t)); }

    template<typename TKey, typename TValue>
    size_t operator()(const lru_node<TKey, TValue>& n) const { return std::hash<TKey>{}(n.key); }
};

struct lru_node_equal
{
    template<typename T1, typename T2>
    bool operator()(const T1& l, const T2& r) const { return get_key(l) == get_key(r); }
};

//struct _lru_bucket_traits
//{
//    _lru_bucket_ptr bucket_begin() const { return p_buckets->data(); }
//    size_t bucket_count() const { return p_buckets->size(); }
//
//    std::vector<_lru_bucket_type>* p_buckets;
//};

template<typename TKey, typename TValue>
using lru_node_ptr = std::unique_ptr<lru_node<TKey, TValue>>;

template<typename TKey, typename TValue>
using _lru_list = boost::intrusive::list<lru_node<TKey, TValue>, boost::intrusive::constant_time_size<false>>;

template<typename TKey, typename TValue>
using _lru_map = boost::intrusive::unordered_set<lru_node<TKey, TValue>, boost::intrusive::constant_time_size<true>,
        boost::intrusive::hash<lru_node_hash>, boost::intrusive::equal<lru_node_equal>>;

template<typename TKey, typename TValue>
using _lru_bucket_type = typename _lru_map<TKey, TValue>::bucket_type;
template<typename TKey, typename TValue>
using _lru_bucket_traits = typename _lru_map<TKey, TValue>::bucket_traits;

//typedef boost::intrusive::list<lru_node, boost::intrusive::constant_time_size<false>> lru_list;
//typedef boost::intrusive::unordered_set<lru_node, boost::intrusive::constant_time_size<true>,
//        boost::intrusive::hash<lru_node_hash>, boost::intrusive::equal<lru_node_equal>> cache_map;

template<typename TKey, typename TValue>
lru_node_ptr<TKey, TValue> extract_node(_lru_map<TKey, TValue>& map, _lru_list<TKey, TValue>& list, typename _lru_list<TKey, TValue>::iterator it)
{
    lru_node_ptr<TKey, TValue> p_node(&*it);
    map.erase(map.iterator_to(*it));
    list.erase(it);
    return p_node;
}

template<typename TKey, typename TValue>
void insert_node(_lru_map<TKey, TValue>& map, _lru_list<TKey, TValue>& list, lru_node_ptr<TKey, TValue> p_node)
{
    if (! p_node) {
        return;
    }
    map.insert(*p_node);
    list.insert(list.end(), *p_node);
    [[maybe_unused]] lru_node<TKey, TValue>* p_ignore = p_node.release();
}

//using _lru_hook_ption = boost::intrusive::base_hook<boost::intrusive::unordered_set_base_hook<>>;
//using _lru_bucket_type = boost::intrusive::unordered_bucket<_lru_hook_ption>::type;
//using _lru_bucket_ptr = boost::intrusive::unordered_bucket_ptr<_lru_hook_ption>::type;

} // namespace details
} // namespace lru
} // namespace wstux

#endif /* _LRU_CACHE_INTRUSIVE_NODE_H */

