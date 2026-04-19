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

#ifndef _THREAD_SAFE_CACHE_LIBS_CACHE_BASE_CACHE_H_
#define _THREAD_SAFE_CACHE_LIBS_CACHE_BASE_CACHE_H_

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
namespace common {

////////////////////////////////////////////////////////////////////////////////
// class type_traits

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

////////////////////////////////////////////////////////////////////////////////
// class base_cache

template<typename TKey, typename TValue, class THash, class TKeyEqual, class TAllocator, template<typename...> class TPolicy>
class base_cache
{
private:
    typedef type_traits<TKey, TValue, THash, TKeyEqual, TAllocator> _traits_t;
    typedef TPolicy<_traits_t>                                      _policy_t;
    typedef typename _policy_t::_hash_table_t                       _hash_table_t;
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    typedef typename _policy_t::_node_t                             _node_t;
    typedef typename _policy_t::_node_allocator_t                   _node_allocator_t;
#endif

protected:
    typedef _policy_t                               policy_type;
    typedef typename _policy_t::_hash_table_t       hash_table_type;

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

    template<typename... TArgs>
    base_cache(size_type capacity, const allocator_type& alloc, TArgs&&... args)
        : m_capacity(capacity)
        , m_policy(m_capacity, alloc, std::forward<TArgs>(args)...)
    {}

    virtual ~base_cache() {}

    inline size_type capacity() const { return m_capacity; }

    void clear()
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        m_policy.hash_tbl.clear_and_dispose(std::bind(&base_cache::deallocate<_node_t>, this, std::placeholders::_1));
#else
        m_policy.hash_tbl.clear();
#endif
        m_policy.clear();
    }

    void erase(const key_type& key)
    {
        typename _hash_table_t::iterator it = find_in_tbl(key);
        if (it != m_policy.hash_tbl.cend()) {
            erase(it);
        }
    }

    void erase(typename _hash_table_t::iterator& it)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        _node_t* p = m_policy.erase(it);
        deallocate<_node_t>(p);
#else
        m_policy.erase(it);
#endif
    }

    inline typename _hash_table_t::iterator find_in_tbl(const key_type& key)
    {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
        return m_policy.hash_tbl.find(key, m_policy.hash_tbl.hash_function(), m_policy.hash_tbl.key_eq());
#else
        return m_policy.hash_tbl.find(key);
#endif
    }

    template<typename... TArgs>
    void insert(const key_type& key, TArgs&&... args)
    {
        if (size() >= m_capacity) {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
            _node_t* p_node = m_policy.evict();
            m_policy.insert_node(p_node, key, std::forward<TArgs>(args)...);
#else
    #if __cplusplus >= 201703
            typename _hash_table_t::node_type node = m_policy.evict();
            m_policy.insert_node(std::move(node), key, std::forward<TArgs>(args)...);
    #else
            m_policy.evict();
            m_policy.insert_node(key, std::forward<TArgs>(args)...);
    #endif
#endif
        } else {
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
            _node_t* p_node = allocate<_node_t>(std::move(key_type(key)), std::move(value_type(std::forward<TArgs>(args)...)));
            m_policy.insert(p_node);
#else
            m_policy.insert(key, std::forward<TArgs>(args)...);
#endif
        }
    }

    inline bool is_find(typename _hash_table_t::iterator& it) const
    {
        return (it != m_policy.hash_tbl.end());
    }

    const policy_type& policy() const { return m_policy; }

    policy_type& policy() { return m_policy; }

    template<typename... TArgs>
    void reset(size_type new_capacity, TArgs&&... args)
    {
        clear();

        m_capacity = new_capacity;
        m_policy.reset(m_capacity, std::forward<TArgs>(args)...);
    }

    inline size_type size() const { return m_policy.hash_tbl.size(); }

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

private:
#if defined(THREAD_SAFE_CACHE_USE_BOOST_INTRUSIVE)
    template<typename T, typename... TArgs>
    T* allocate(TArgs&&... args)
    {
        using allocator_traits_t = std::allocator_traits<_node_allocator_t>;

        T* p_raw = allocator_traits_t::allocate(m_policy.allocator, 1);
        allocator_traits_t::construct(m_policy.allocator, p_raw, std::forward<TArgs>(args)...);
        return p_raw;
    }

    template<typename T>
    void deallocate(T* p_raw)
    {
        using allocator_traits_t = std::allocator_traits<_node_allocator_t>;

        allocator_traits_t::destroy(m_policy.allocator, p_raw);
        allocator_traits_t::deallocate(m_policy.allocator, p_raw, 1);
    }
#endif

private:
    size_type m_capacity;
    _policy_t m_policy;
};

} // namespace common
} // namespace details
} // namespace cache
} // namespace wstux

#endif /* _THREAD_SAFE_CACHE_LIBS_CACHE_BASE_CACHE_H_ */
