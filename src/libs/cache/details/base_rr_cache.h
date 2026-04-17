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

#ifndef _THREAD_SAFE_CACHE_LIBS_CACHE_BASE_RR_CACHE_H_
#define _THREAD_SAFE_CACHE_LIBS_CACHE_BASE_RR_CACHE_H_

#include <algorithm>
#include <random>
#include <vector>
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    #include <boost/intrusive/list.hpp>
    #include <boost/intrusive/unordered_set.hpp>
#else
    #include <unordered_map>
#endif

namespace wstux {
namespace cache {
namespace rr {
namespace details {

////////////////////////////////////////////////////////////////////////////////
// Specifics of cache implementation

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
/**
 *  \brief  Implementations based on boost intrusive.
 */
namespace bi = boost::intrusive;

template<typename TKey, typename TValue>
struct node : public bi::unordered_set_base_hook<bi::link_mode<bi::normal_link>>
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
#endif

////////////////////////////////////////////////////////////////////////////////
// class type_traits

template<typename TKey, typename TValue, typename THash, typename TKeyEqual, class TAllocator>
struct type_traits
{
    typedef TAllocator          allocator_type;
    typedef TKey                key_type;
    typedef TValue              value_type;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;

    typedef size_t              size_type;
    typedef THash               hasher;
    typedef TKeyEqual           key_equal;

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    typedef node<key_type, value_type>              _node_t;
    typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<_node_t>  _node_allocator_t;

    typedef bi::constant_time_size<true>            _ct_size_t;
    typedef bi::hash<node_hash<hasher>>             _intr_hash_t;
    typedef bi::equal<node_equal<key_equal>>        _intr_key_equal_t;
    typedef bi::unordered_set<_node_t, _ct_size_t, _intr_hash_t, _intr_key_equal_t> _hash_table_t;

    typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<_node_t*> _key_allocator_t;
    typedef std::vector<_node_t*, _key_allocator_t> _keys_vector;

    typedef typename _hash_table_t::bucket_type     _bucket_type_t;
    typedef typename _hash_table_t::bucket_traits   _bucket_traits_t;
    typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<_bucket_traits_t> _bucket_allocator_t;
    typedef std::vector<_bucket_type_t, _bucket_allocator_t>    _buckets_list_t;
#else
    typedef std::unordered_map<key_type, value_type, hasher, key_equal, allocator_type> _hash_table_t;
    typedef std::vector<key_type, allocator_type>   _keys_vector;
#endif
};

////////////////////////////////////////////////////////////////////////////////
// class base_lru_cache

/**
 *  \brief  Implementation of LRU cache based on intrusive containers.
 *  \details    The idea of a cache based on intrusive containers is taken from
 *              https://www.youtube.com/watch?v=60XhYzkXu1M&t=2358s
 */
template<typename TKey, typename TValue, class THash, class TKeyEqual, class TAllocator>
class base_rr_cache
{
private:
    typedef type_traits<TKey, TValue, THash, TKeyEqual, TAllocator> _traits_t;

protected:
    typedef typename _traits_t::allocator_type      allocator_type;
    typedef typename _traits_t::key_type            key_type;
    typedef typename _traits_t::value_type          value_type;
    typedef typename _traits_t::size_type           size_type;
    typedef typename _traits_t::hasher              hasher;
    typedef typename _traits_t::key_equal           key_equal;
    typedef typename _traits_t::reference           reference;
    typedef typename _traits_t::const_reference     const_reference;
    typedef typename _traits_t::pointer             pointer;
    typedef typename _traits_t::const_pointer       const_pointer;

    typedef typename _traits_t::_hash_table_t       _hash_table_t;
    typedef typename _traits_t::_keys_vector        _keys_vector;
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    typedef typename _traits_t::_bucket_allocator_t _bucket_allocator_t;
    typedef typename _traits_t::_node_allocator_t   _node_allocator_t;

    typedef typename _traits_t::_bucket_traits_t    _bucket_traits_t;
    typedef typename _traits_t::_buckets_list_t     _buckets_list_t;
    typedef typename _traits_t::_node_t             _node_t;
#endif

    explicit base_rr_cache(size_type capacity, const allocator_type& alloc)
        : m_capacity(capacity)
        , m_rand_gen(std::random_device{}())
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        , m_allocator(alloc)
        , m_buckets(m_capacity, alloc)
        , m_hash_tbl(_bucket_traits_t(m_buckets.data(), m_buckets.capacity()))
#else
        , m_hash_tbl(alloc)
#endif
        , m_keys(alloc)
    {
        m_keys.reserve(capacity);
#if ! defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        m_hash_tbl.reserve(capacity);
#endif
    }

    virtual ~base_rr_cache() {}

    inline size_type capacity() const { return m_capacity; }

    void clear()
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        m_hash_tbl.clear_and_dispose(std::bind(&base_rr_cache::deallocate<_node_t>, this, std::placeholders::_1));
#else
        m_hash_tbl.clear();
#endif
        m_keys.clear();
    }

    void erase(const key_type& key)
    {
        typename _hash_table_t::iterator it = find_in_tbl(key);
        if (it != m_hash_tbl.end()) {
            typename _keys_vector::iterator key_it = std::find_if(m_keys.begin(), m_keys.end(),
                [&key](const typename _keys_vector::value_type& v) -> bool {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
                    return (key == v->key);
#else
                    return (key == v);
#endif
                });
            std::swap(*key_it, m_keys[m_keys.size() - 1]);
            m_keys.pop_back();
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
            _node_t* p = &(*it);
            m_hash_tbl.erase(it);
            deallocate<_node_t>(p);
#else
            m_hash_tbl.erase(it);
#endif
        }
    }

    inline typename _hash_table_t::iterator find_in_tbl(const key_type& key)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        return m_hash_tbl.find(key, m_hash_tbl.hash_function(), m_hash_tbl.key_eq());
#else
        return m_hash_tbl.find(key);
#endif
    }

    template<typename... TArgs>
    void insert(const key_type& key, TArgs&&... args)
    {
        if (size() >= m_capacity) {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
            _node_t* p_node = extract_node();
            p_node->key = key;
            p_node->value = std::move(value_type(std::forward<TArgs>(args)...));
            insert_node(p_node);
#else
            std::uniform_int_distribution<size_t> dist(0, m_keys.size() - 1);
            const size_t idx = dist(m_rand_gen);
            const key_type& rand_key = m_keys[idx];
    #if __cplusplus >= 201703
            typename _hash_table_t::node_type node = m_hash_tbl.extract(rand_key);
            node.key() = key;
            typename _hash_table_t::insert_return_type rc = m_hash_tbl.insert(std::move(node));
            rc.position->second = std::move(value_type(std::forward<TArgs>(args)...));
    #else
            m_hash_tbl.erase(rand_key);
            m_hash_tbl.emplace(key, value_type(std::forward<TArgs>(args)...));
    #endif
            m_keys[idx] = key;
#endif
        } else {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
            _node_t* p_node = allocate<_node_t>(std::move(key_type(key)), std::move(value_type(std::forward<TArgs>(args)...)));
            insert_node(p_node);
#else
            m_hash_tbl.emplace(key, value_type(std::forward<TArgs>(args)...));
            m_keys.emplace_back(key);
#endif
        }
    }

    inline bool is_find(typename _hash_table_t::iterator& it) const
    {
        return (it != m_hash_tbl.end());
    }

    void reset(size_type new_capacity)
    {
        clear();

        m_capacity = new_capacity;
        m_keys.reserve(m_capacity);
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        _buckets_list_t buckets(m_capacity);
        m_hash_tbl.rehash(_bucket_traits_t(buckets.data(), buckets.capacity()));
        m_buckets = std::move(buckets);
#else
        m_hash_tbl.reserve(m_capacity);
#endif
    }

    inline size_type size() const { return m_hash_tbl.size(); }

    inline static const value_type& load(const typename _hash_table_t::iterator& it)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        return it->value;
#else
        return it->second;
#endif
    }

    inline static void load(const typename _hash_table_t::iterator& it, value_type& res)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        res = it->value;
#else
        res = it->second;
#endif
    }

    inline static void store(const typename _hash_table_t::iterator& it, value_type&& val)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        it->value = val;
#else
        it->second = val;
#endif
    }

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
private:
    template<typename T, typename... TArgs>
    T* allocate(TArgs&&... args)
    {
        using allocator_traits_t = std::allocator_traits<_node_allocator_t>;

        T* p_raw = allocator_traits_t::allocate(m_allocator, 1);
        allocator_traits_t::construct(m_allocator, p_raw, std::forward<TArgs>(args)...);
        return p_raw;
    }

    template<typename T>
    void deallocate(T* p_raw)
    {
        using allocator_traits_t = std::allocator_traits<_node_allocator_t>;

        allocator_traits_t::destroy(m_allocator, p_raw);
        allocator_traits_t::deallocate(m_allocator, p_raw, 1);
    }

    _node_t* extract_node()
    {
        std::uniform_int_distribution<size_t> dist(0, m_keys.size() - 1);
        const size_t idx = dist(m_rand_gen);

        _node_t* p_node = m_keys[idx];

        m_hash_tbl.erase(m_hash_tbl.iterator_to(*p_node));
        std::swap(m_keys[idx], m_keys[m_keys.size() - 1]);
        m_keys.pop_back();
        return p_node;
    }

    void insert_node(_node_t* p_node)
    {
        m_hash_tbl.insert(*p_node);
        m_keys.emplace_back(p_node);
    }
#endif

private:
    size_type m_capacity;

    std::mt19937 m_rand_gen;

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    _node_allocator_t m_allocator;
    _buckets_list_t m_buckets;
#endif
    _hash_table_t m_hash_tbl;
    _keys_vector m_keys;
};

} // namespace details
} // namespace rr
} // namespace cache
} // namespace wstux

#endif /* _THREAD_SAFE_CACHE_LIBS_CACHE_BASE_RR_CACHE_H_ */
