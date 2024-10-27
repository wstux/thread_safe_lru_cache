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

#ifndef _TTL_CACHE_THREAD_SAFE_TTL_CACHE_H
#define _TTL_CACHE_THREAD_SAFE_TTL_CACHE_H

#include <atomic>
#include <memory>
#include <mutex>
#include <type_traits>
#include <vector>
#include <utility>

#include "ttl/ttl_cache.h"
#include "ttl/details/spinlock.h"

namespace wstux {
namespace ttl {

/**
 *  \brief  Thread safe TTL cache.
 *  \tparam TKey - cache key.
 *  \tparam TValue - cache value.
 *  \tparam THash - function object for hashing the cache key.
 *  \tparam TKeyEqual - function object for performing comparisons the cache key.
 *  \tparam TLock - synchronization primitive for protecting shared data from
 *          being accessed by multiple threads simultaneously.
 *
 *  \details    The cache is implemented through a thread-safe wrapper, so no
 *          locks or synchronizations are required in the calling code.
 *
 *          Thread safe TTL cache is implemented using sharding. Thread safe TTL
 *          cache creates several TTL shards inside itself to store data. When
 *          inserting and searching in the cache, the container is selected based
 *          on the key hash. This approach allows for less frequent blocking of
 *          shared data (the container in which the cache data is stored) when
 *          adding new data, since there is less chance that different data will
 *          be placed in the same container.
 *
 *                     ┌─────────────────────────────────────┐
 *                     │        thread_safe_ttl_cache        │
 *                     ├─────────────────────────────────────┤
 *                     ||========|========|========|========||
 *                     || Shrd_1 | Shrd_2 | Shrd_3 | Shrd_4 ||
 *                     ||========|========|========|========||
 *                     || data_1 | data_2 | data_3 | data_4 ||
 *                     ||--------|--------|--------|--------||
 *                     ||        |        |        |        ||
 *                     ||--------|--------|--------|--------||
 *                     └─────────────────────────────────────┘
 *                         /                  |         \
 *           Thread 1 was given Cont_1 to     |       Thread 3 was given Cont_4
 *             write data to. Shrd_1          |        to write data to. Shrd_4
 *              is locked by a mutex.         |         is locked by a mutex.
 *                                            |
 *                         Thread 2 was given Shrd_3 write data to.
 *                             Cont_3 is locked by a mutex.
 *
 *          The cache depth is divided equally between all shards. If the cache
 *          depth is divisible by the remainder by the number of shards, the
 *          remainder is written to the first container.
 *          Example:
 *          1) With 2 shards and a cache depth of 10, 2 shards will be created,
 *          each of which will have a size of 5 elements.
 *          2) With 4 shards and a cache depth of 11, 4 shards will be created,
 *          the first shard will have a size of 3 elements, and the rest - 2
 *          elements.
 *
 *          When an element is added to the cache, a decision is made based on
 *          the hash of its key, in which shard it (the element) should be placed.
 *          The shard for adding data is selected based on the remainder of the
 *          integer division of the hash by the number of shards. When there is
 *          no space left in the container to add a new element, the oldest element
 *          is pushed out and a new one is added. Thus, a situation is possible
 *          when one of the shards is full, and the others still have free space,
 *          but based on the hash of the key, a filled shard was selected for
 *          storage, as a result of which old data will be pushed out from it,
 *          and new data will be placed, while the remaining shards will remain
 *          completely unfilled.
 *
 *                                      +--------+
 *                                      | data_4 |
 *                                      +--------+
 *                                        /
 *                                       / Adding a new element
 *                                      /
 *                      |========|========|                  |========|========|
 *                      | Shrd_1 | Shrd_2 |                  | Shrd_2 | Cont_2 |
 *                      |========|========|                  |========|========|
 *                      | data_1 | data_2 |    =========>    | data_1 | data_4 |
 *                      |--------|--------|                  |--------|--------|
 *                      |        | data_3 |                  |        | data_2 |
 *                      |--------|--------|                  |--------|--------|
 *                     /             |
 *                    /              | The old element is being
 *          Shard 1 has a free       |   pushed out of shard 2
 *               position        +--------+
 *                               | data_3 |
 *                               +--------+
 *
 */
template<typename TKey, typename TValue,
         class THash = std::hash<TKey>, class TKeyEqual = std::equal_to<TKey>,
         class TLock = details::spinlock>
class thread_safe_ttl_cache
{
    typedef ttl_cache<TKey, TValue, THash, TKeyEqual>   _shard_type;

public:
    typedef typename _shard_type::key_type          key_type;
    typedef typename _shard_type::value_type        value_type;
    typedef typename _shard_type::size_type         size_type;
    typedef typename _shard_type::hasher            hasher;
    typedef typename _shard_type::key_equal         key_equal;
    typedef TLock                                   lock_type;
    typedef typename _shard_type::reference         reference;
    typedef typename _shard_type::const_reference   const_reference;
    typedef typename _shard_type::pointer           pointer;
    typedef typename _shard_type::const_pointer     const_pointer;

    /// \brief  Constructs a new container.
    /// \param  ttl_msecs - time to live milliseconds.
    /// \param  capacity - number of elements for which space has been allocated
    ///         in the container.
    /// \param  shards_count - shards count.
    explicit thread_safe_ttl_cache(size_type ttl_msecs, size_t capacity, size_t shards_count)
        : m_capacity(capacity)
        , m_shards((m_capacity > shards_count) ? shards_count : m_capacity)
    {
        shards_count = m_shards.size();
        for (size_t i = 0; i < shards_count; i++) {
            const size_t shard_capacity = (i != 0)
                ? (m_capacity / shards_count)
                : ((m_capacity / shards_count) + (m_capacity % shards_count));
            m_shards[i].first = std::make_shared<_shard_type>(ttl_msecs, shard_capacity);
        }
    }

    thread_safe_ttl_cache(const thread_safe_ttl_cache&) = delete;
    thread_safe_ttl_cache& operator=(const thread_safe_ttl_cache&) = delete;

    /// \brief  Return the number of items for which space has been allocated in
    ///         the container.
    /// \return The number of elements for which space has been allocated in the
    ///         container.
    size_type capacity() const { return m_capacity; }

    /// \brief  Erases all elements from the container. After this call, size()
    ///         returns zero.
    void clear()
    {
        for (_shard_guard& sh : m_shards) {
            wrapper(sh, &_shard_type::clear);
        }
    }

    /// \brief  Checks if there is an element with key equivalent to key in the
    ///         container.
    /// \param  key - key value of the element to search for.
    /// \return true if there is such an element, otherwise false.
    bool contains(const key_type& key)
    {
        return wrapper(get_shard(key), &_shard_type::contains, key);
    }

    /// \brief  Inserts a new element into the container constructed in-place
    ///         with the given args, if there is no element with the key in the
    ///         container.
    /// \param  key - key value of the element.
    /// \param  args - arguments to forward to the constructor of the element.
    /// \return true if the insertion took place, otherwise false.
    ///
    /// \attention  This method constructs the object in place, so it may take a
    ///         long time to complete. The lock will be held during the
    ///         construction of the object, so it is not recommended to use this
    ///         method in a multi-threaded application.
    template<typename... TArgs>
    bool emplace(const key_type& key, TArgs&&... args)
    {
        return wrapper(get_shard(key), &_shard_type::template emplace<TArgs...>,
                       key, std::forward<TArgs>(args)...);
    }

    /// \brief  Checks if the container has no elements.
    /// \return true if the container is empty, false otherwise.
    bool empty() const
    {
        for (const _shard_guard& sh : m_shards) {
            if (! wrapper(sh, &_shard_type::empty)) {
                return false;
            }
        }
        return true;
    }

    /// \brief  Removes specified elements from the container. The order of the
    ///         remaining elements is preserved.
    /// \param  key - key value of the elements to remove.
    void erase(const key_type& key)
    {
        wrapper(get_shard(key), &_shard_type::erase, key);
    }

    /// \brief  Finds an element with key equivalent to key.
    /// \param  key - key value of the element to search for.
    /// \param  result - variable that will contain the requested element if
    ///         such an element is found.
    /// \return true if element has been found, otherwise false.
    bool find(const key_type& key, value_type& result)
    {
        return wrapper(get_shard(key), &_shard_type::find, key, result);
    }

    /// \brief  Inserts element into the container, if the container doesn't
    ///         already contain an element with an equivalent key.
    /// \param  key - key value of the element.
    /// \param  val - element value to insert.
    /// \return true if the insertion took place, otherwise false.
    bool insert(const key_type& key, const value_type& val)
    {
        return wrapper(get_shard(key), &_shard_type::insert, key, val);
    }

#if defined(_THREAD_SAFE_TTL_CACHE_ENABLE_OPTIONAL)
    /// \brief  Gets an element with key equivalent to key.
    /// \param  key - key value of the element to search for.
    /// \return Element if element has been found, otherwise std::nullopt.
    std::optional<value_type> get(const key_type& key)
    {
        return wrapper(get_shard(key), &_shard_type::get, key);
    }
#endif

    /// \brief  Clear cache contents and change the capacity of the cache.
    /// \param  ttl_msecs - time to live milliseconds.
    /// \param  new_capacity - new capacity of the cache, in number of elements.
    void reset(size_type ttl_msecs, size_type new_capacity)
    {
        m_capacity = new_capacity;
        size_t shards_count = m_shards.size();
        for (size_t i = 0; i < shards_count; i++) {
            const size_t shard_capacity = (i != 0)
                ? (new_capacity / shards_count)
                : ((new_capacity / shards_count) + (new_capacity % shards_count));
            wrapper(m_shards[i], &_shard_type::reset, ttl_msecs, shard_capacity);
        }
    }

    /// \brief  Return the number of shards.
    /// \return The number of shards.
    size_type shards_size() const { return m_shards.size(); }

    /// \brief  Return the number of elements in the container.
    /// \return The number of elements in the container.
    size_type size() const
    {
        size_type s = 0;
        for (const _shard_guard& sh : m_shards) {
            s += wrapper(sh, &_shard_type::size);
        }
        return s;
    }

    /// \brief  Inserts element into the container, if the container doesn't
    ///         already contain an element with an equivalent key, otherwise
    //          updates value of the element.
    /// \param  key - key value of the element.
    /// \param  val - element value to update/insert.
    void update(const key_type& key, const value_type& val)
    {
        constexpr void (_shard_type::*p_update)(const key_type&, const value_type&) =
            &_shard_type::update;
        wrapper(get_shard(key), p_update, key, std::forward<decltype(val)>(val));
    }

    /// \brief  Inserts element into the container, if the container doesn't
    ///         already contain an element with an equivalent key, otherwise
    //          updates value of the element.
    /// \param  key - key value of the element.
    /// \param  val - element value to update/insert.
    void update(const key_type& key, value_type&& val)
    {
        constexpr void (_shard_type::*p_update)(const key_type&, value_type&&) =
            &_shard_type::update;
        wrapper(get_shard(key), p_update, key, std::forward<decltype(val)>(val));
    }

private:
    typedef std::shared_ptr<_shard_type>                    _shard_ptr_type;
    struct _shard_guard
    {
        _shard_ptr_type first;
        mutable lock_type second;
    };

private:
    inline const _shard_guard& get_shard(const TKey& key) const
    {
        return m_shards[(hasher{}(key)) % m_shards.size()];
    }

    inline _shard_guard& get_shard(const TKey& key)
    {
        return m_shards[(hasher{}(key)) % m_shards.size()];
    }

    template<typename T, typename TFn, typename... TArgs>
    inline typename std::result_of<TFn(_shard_type, TArgs...)>::type
        wrapper(T& p, const TFn& func, TArgs&&... args)
    {
        std::unique_lock<decltype(p.second)> lock(p.second);
        return (p.first.get()->*func)(std::forward<TArgs>(args)...);
    }

    template<typename T, typename TFn, typename... TArgs>
    inline typename std::result_of<TFn(_shard_type, TArgs...)>::type
        wrapper(const T& p, const TFn& func, TArgs&&... args) const
    {
        std::unique_lock<decltype(p.second)> lock(p.second);
        return (p.first.get()->*func)(std::forward<TArgs>(args)...);
    }

private:
    size_t m_capacity;
    std::vector<_shard_guard> m_shards;
};

} // namespace ttl
} // namespace wstux

#endif /* _TTL_CACHE_THREAD_SAFE_TTL_CACHE_H */

