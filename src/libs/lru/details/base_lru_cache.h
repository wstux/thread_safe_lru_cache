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

#ifndef _LIBS_LRU_BASE_LRU_CACHE_H_
#define _LIBS_LRU_BASE_LRU_CACHE_H_

#include <atomic>

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    #include <memory>
    #include <vector>

    #include <boost/intrusive/list.hpp>
    #include <boost/intrusive/unordered_set.hpp>
#else
    #include <list>
    #include <unordered_map>
#endif

namespace wstux {
namespace lru {
namespace details {

////////////////////////////////////////////////////////////////////////////////
// class spinlock
//
// The spinlock implementation described in the links is used:
// https://www.talkinghightech.com/en/implementing-a-spinlock-in-c/
// https://rigtorp.se/spinlock/
class spinlock
{
public:
    void lock()
    {
        for (;;) {
            if (! m_lock.exchange(true, std::memory_order_acquire)) { return; }
            while (m_lock.load(std::memory_order_relaxed)) {
                __builtin_ia32_pause();
            }
        }
    }

    bool try_lock()
    {
      return (! m_lock.load(std::memory_order_relaxed)) &&
             (! m_lock.exchange(true, std::memory_order_acquire));
    }

    void unlock() { m_lock.store(false, std::memory_order_release); }

private:
    std::atomic_bool m_lock = {false};
};

////////////////////////////////////////////////////////////////////////////////
// Specifics of cache implementation

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
namespace bi = boost::intrusive;

template<typename TKey, typename TValue>
struct lru_node : public bi::list_base_hook<bi::link_mode<bi::normal_link>>
                , public bi::unordered_set_base_hook<bi::link_mode<bi::normal_link>>
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
#else
template<typename TKey, typename TValue>
struct hash_table_value
{
    typedef TKey                    key_type;
    typedef TValue                  value_type;
    typedef std::list<key_type>     _lru_list_t;

    template<typename... TArgs>
    explicit hash_table_value(TArgs&&... args)
        : value(std::forward<TArgs>(args)...)
    {}

    value_type value;
    typename _lru_list_t::iterator lru_it;
};
#endif

////////////////////////////////////////////////////////////////////////////////
// class type_traits

template<typename TKey, typename TValue, typename THash, typename TKeyEqual>
struct type_traits
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

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    typedef lru_node<key_type, value_type>                          _lru_node_t;
    typedef bi::list<_lru_node_t, bi::constant_time_size<false>>    _lru_list_t;

    typedef bi::constant_time_size<true>            _is_ct_size_t;
    typedef bi::hash<lru_node_hash<hasher>>         _intr_hash_t;
    typedef bi::equal<lru_node_equal<key_equal>>    _intr_key_equal_t;
    typedef bi::unordered_set<_lru_node_t, _is_ct_size_t, _intr_hash_t, _intr_key_equal_t> _hash_table_t;

    typedef typename _hash_table_t::bucket_type     _bucket_type_t;
    typedef typename _hash_table_t::bucket_traits   _bucket_traits_t;
#else
    typedef hash_table_value<key_type, value_type>  _table_value_t;
    typedef typename _table_value_t::_lru_list_t    _lru_list_t;
    typedef std::unordered_map<key_type, _table_value_t, hasher, key_equal> _hash_table_t;
#endif
};

////////////////////////////////////////////////////////////////////////////////
// class base_lru_cache

/**
 *  \brief  Implementation of LRU cache based on intrusive containers.
 *  \details    The idea of a cache based on intrusive containers is taken from
 *              https://www.youtube.com/watch?v=60XhYzkXu1M&t=2358s
 */
template<typename TKey, typename TValue, class THash, class TKeyEqual>
class base_lru_cache
{
private:
    typedef type_traits<TKey, TValue, THash, TKeyEqual> _traits_t;

protected:
    typedef typename _traits_t::key_type            key_type;
    typedef typename _traits_t::value_type          value_type;
    typedef typename _traits_t::size_type           size_type;
    typedef typename _traits_t::hasher              hasher;
    typedef typename _traits_t::key_equal           key_equal;
    typedef typename _traits_t::reference           reference;
    typedef typename _traits_t::const_reference     const_reference;
    typedef typename _traits_t::pointer             pointer;
    typedef typename _traits_t::const_pointer       const_pointer;

    typedef typename _traits_t::_lru_list_t         _lru_list_t;
    typedef typename _traits_t::_hash_table_t       _hash_table_t;

    explicit base_lru_cache(size_type capacity)
        : m_capacity(capacity)
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        , m_buckets(m_capacity)
        , m_hash_tbl(_bucket_traits_t(m_buckets.data(), m_buckets.capacity()))
#endif
    {
#if ! defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        m_hash_tbl.reserve(capacity);
#endif
    }

    virtual ~base_lru_cache() {}

    inline size_type capacity() const { return m_capacity; }

    void clear()
    {
        m_hash_tbl.clear();
        m_lru_list.clear();
    }

    void erase(const key_type& key)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        typename _hash_table_t::iterator it =
            m_hash_tbl.find(key, m_hash_tbl.hash_function(), m_hash_tbl.key_eq());
        if (it != m_hash_tbl.end()) {
            m_lru_list.erase(m_lru_list.iterator_to(*it));
            m_hash_tbl.erase(it);
        }
#else
        typename _hash_table_t::iterator it = m_hash_tbl.find(key);
        if (it != m_hash_tbl.end()) {
            m_lru_list.erase(it->second.lru_it);
            m_hash_tbl.erase(it);
        }
#endif
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
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        if (size() >= m_capacity) {
            _lru_node_ptr_t p_node = extract_node(m_lru_list.begin());
            p_node->key = key;
            p_node->value = std::move(value_type(std::forward<TArgs>(args)...));
            insert_node(std::move(p_node));
        } else {
            _lru_node_ptr_t p_node = std::make_unique<_lru_node_t>(key_type(key), value_type(std::forward<TArgs>(args)...));
            insert_node(std::move(p_node));
        }
#else
        if (size() >= m_capacity) {
            m_hash_tbl.erase(m_lru_list.front());
            m_lru_list.front() = key;
            std::pair<typename _hash_table_t::iterator, bool> rc =
                m_hash_tbl.emplace(key, typename _traits_t::_table_value_t(std::forward<TArgs>(args)...));
            rc.first->second.lru_it = m_lru_list.begin();
            move_to_top(rc.first);
        } else {
            typename _lru_list_t::iterator it = m_lru_list.emplace(m_lru_list.end(), key);
            std::pair<typename _hash_table_t::iterator, bool> rc =
                m_hash_tbl.emplace(key, typename _traits_t::_table_value_t(std::forward<TArgs>(args)...));
            rc.first->second.lru_it = it;
        }
#endif
    }

    inline bool is_find(typename _hash_table_t::iterator& it) const
    {
        return (it != m_hash_tbl.end());
    } 

    inline void move_to_top(typename _hash_table_t::iterator& it)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        m_lru_list.splice(m_lru_list.end(), m_lru_list, m_lru_list.iterator_to(*it));
#else
        m_lru_list.splice(m_lru_list.end(), m_lru_list, it->second.lru_it);
#endif
    }

    void reset(size_type new_capacity)
    {
        clear();

        m_capacity = new_capacity;
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
        return it->second.value;
#endif
    }

    inline static void load(const typename _hash_table_t::iterator& it, value_type& res)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        res = it->value;
#else
        res = it->second.value;
#endif
    }

    inline static void store(const typename _hash_table_t::iterator& it, value_type&& val)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        it->value = val;
#else
        it->second.value = val;
#endif
    }

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
private:
    typedef typename _traits_t::_bucket_traits_t            _bucket_traits_t;
    typedef std::vector<typename _traits_t::_bucket_type_t> _buckets_list_t;
    typedef typename _traits_t::_lru_node_t                 _lru_node_t;
    typedef std::unique_ptr<_lru_node_t>                    _lru_node_ptr_t;

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
#endif

private:
    size_type m_capacity;

#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    _buckets_list_t m_buckets;
#endif
    _hash_table_t m_hash_tbl;
    _lru_list_t m_lru_list;
};

} // namespace details
} // namespace lru
} // namespace wstux

#endif /* _LIBS_LRU_BASE_LRU_CACHE_H_ */

