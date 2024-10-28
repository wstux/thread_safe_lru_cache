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

#ifndef _TTL_CACHE_TTL_CACHE_H
#define _TTL_CACHE_TTL_CACHE_H

#if __cplusplus >= 201703
    #define _THREAD_SAFE_CACHE_ENABLE_OPTIONAL
#endif

#include <functional>

#if defined(_THREAD_SAFE_CACHE_ENABLE_OPTIONAL)
    #include <optional>
#endif

#include "ttl/details/base_ttl_cache.h"

namespace wstux {
namespace ttl {

/**
 *  \brief  TTL cache.
 *  \tparam TKey - cache key.
 *  \tparam TValue - cache value.
 *  \tparam THash - function object for hashing the cache key.
 *  \tparam TKeyEqual - function object for performing comparisons the cache key.
 *
 *  \todo   Add allocator as template parameter.
 *
 *  \details    TTL cache has two implementations - based on std::unordered_map
 *          and based boost::intrusive::unordered_set_base_hook. Since a hash
 *          table is used as a container, searching in it takes O(1) time.
 *
 *          When new elements are added, the hash table expands. Because the
 *          hash table requires a continuous area of ​​memory, when the hash table
 *          expands, the table itself is constantly copied (metadata is copied,
 *          not data). This constant copying can be very expensive,so the
 *          implementation of the cache storage reserves memory for the required
 *          number of elements for each storage.
 */
template<typename TKey, typename TValue,
         class THash = std::hash<TKey>, class TKeyEqual = std::equal_to<TKey>>
class ttl_cache : protected details::base_ttl_cache<TKey, TValue, THash, TKeyEqual>
{
private:
    typedef details::base_ttl_cache<TKey, TValue, THash, TKeyEqual>     base;

public:
    typedef typename base::key_type          key_type;
    typedef typename base::value_type        value_type;
    typedef typename base::size_type         size_type;
    typedef typename base::hasher            hasher;
    typedef typename base::key_equal         key_equal;
    typedef typename base::reference         reference;
    typedef typename base::const_reference   const_reference;
    typedef typename base::pointer           pointer;
    typedef typename base::const_pointer     const_pointer;

    /// \brief  Constructs a new container.
    /// \param  ttl_msecs - time to live milliseconds.
    /// \param  capacity - number of elements for which space has been allocated
    ///         in the container.
    explicit ttl_cache(size_type ttl_msecs, size_type capacity)
        : base(ttl_msecs, capacity)
    {}

    ttl_cache(const ttl_cache&) = delete;
    ttl_cache& operator=(const ttl_cache&) = delete;

    /// \brief  Return the number of items for which space has been allocated in
    ///         the container.
    /// \return The number of elements for which space has been allocated in the
    ///         container.
    size_type capacity() const { return base::capacity(); }

    /// \brief  Erases all elements from the container. After this call, size()
    ///         returns zero.
    void clear() { base::clear(); }

    /// \brief  Checks if there is an element with key equivalent to key in the
    ///         container.
    /// \param  key - key value of the element to search for.
    /// \return true if there is such an element, otherwise false.
    bool contains(const key_type& key)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it) && ! base::is_expired(it)) {
            base::move_to_top(it);
            return true;
        }
        return false;
    }

    /// \brief  Inserts a new element into the container constructed in-place
    ///         with the given args, if there is no element with the key in the
    ///         container.
    /// \param  key - key value of the element.
    /// \param  args - arguments to forward to the constructor of the element.
    /// \return true if the insertion took place, otherwise false.
    template<typename... TArgs>
    bool emplace(const key_type& key, TArgs&&... args)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            bool rc = false;
            if (base::is_expired(it)) {
                base::store(it, value_type(std::forward<TArgs>(args)...));
                rc = true;
            }
            base::move_to_top(it);
            return rc;
        }

        base::insert(key, std::forward<TArgs>(args)...);
        return true;
    }

    /// \brief  Checks if the container has no elements.
    /// \return true if the container is empty, false otherwise.
    bool empty() const { return (base::size() == 0); }

    /// \brief  Removes specified elements from the container. The order of the
    ///         remaining elements is preserved.
    /// \param  key - key value of the elements to remove.
    void erase(const key_type& key) { base::erase(key); }

    /// \brief  Finds an element with key equivalent to key.
    /// \param  key - key value of the element to search for.
    /// \param  result - variable that will contain the requested element if
    ///         such an element is found.
    /// \return true if element has been found, otherwise false.
    bool find(const key_type& key, value_type& result)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            if (base::is_expired(it)) {
                base::erase(it);
            } else {
                base::move_to_top(it);
                base::load(it, result);
                return true;
            }
        }

        return false;
    }

    /// \brief  Inserts element into the container, if the container doesn't
    ///         already contain an element with an equivalent key.
    /// \param  key - key value of the element.
    /// \param  val - element value to insert.
    /// \return true if the insertion took place, otherwise false.
    bool insert(const key_type& key, const value_type& val)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            bool rc = false;
            if (base::is_expired(it)) {
                base::store(it, value_type(val));
                rc = true;
            }
            base::move_to_top(it);
            return rc;
        }

        base::insert(key, val);
        return true;
    }

#if defined(_THREAD_SAFE_CACHE_ENABLE_OPTIONAL)
    /// \brief  Gets an element with key equivalent to key.
    /// \param  key - key value of the element to search for.
    /// \return Element if element has been found, otherwise std::nullopt.
    std::optional<value_type> get(const key_type& key)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it) && ! base::is_expired(it)) {
            base::move_to_top(it);
            return base::load(it);
        }

        return std::nullopt;
    }
#endif

    /// \brief  Clear cache contents and change the capacity of the cache.
    /// \param  ttl_msecs - time to live milliseconds.
    /// \param  new_capacity - new capacity of the cache, in number of elements.
    void reset(size_type ttl_msecs, size_type new_capacity)
    {
        base::reset(ttl_msecs, new_capacity);
    }

    /// \brief  Return the number of elements in the container.
    /// \return The number of elements in the container.
    size_type size() const { return base::size(); }

    /// \brief  Inserts element into the container, if the container doesn't
    ///         already contain an element with an equivalent key, otherwise
    //          updates value of the element.
    /// \param  key - key value of the element.
    /// \param  val - element value to update/insert.
    void update(const key_type& key, const value_type& val)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            base::store(it, value_type(val));
            base::move_to_top(it);
            return;
        }

        base::insert(key, val);
    }

    /// \brief  Inserts element into the container, if the container doesn't
    ///         already contain an element with an equivalent key, otherwise
    //          updates value of the element.
    /// \param  key - key value of the element.
    /// \param  val - element value to update/insert.
    void update(const key_type& key, value_type&& val)
    {
        typename _hash_table_t::iterator it = base::find_in_tbl(key);
        if (base::is_find(it)) {
            base::store(it, std::move(val));
            base::move_to_top(it);
            return;
        }

        base::insert(key, std::move(val));
    }

private:
    typedef typename base::_hash_table_t    _hash_table_t;
};

} // namespace ttl
} // namespace wstux

#endif /* _TTL_CACHE_TTL_CACHE_H */

