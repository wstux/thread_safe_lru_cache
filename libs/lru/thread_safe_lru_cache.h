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

#ifndef _LRU_CACHE_THREAD_SAFE_LRU_CACHE_H
#define _LRU_CACHE_THREAD_SAFE_LRU_CACHE_H

#include <memory>
#include <mutex>
#include <type_traits>
#include <vector>
#include <utility>

#include "lru/lru_cache.h"
#include "lru/details/spinlock.h"

namespace wstux {
namespace lru {

template<typename TKey, typename TValue,
         class THash = std::hash<TKey>, class TKeyEqual = std::equal_to<TKey>,
         class TLock = details::spinlock>
class thread_safe_lru_cache
{
    typedef lru_cache<TKey, TValue, THash, TKeyEqual>   _shard_type;

public:
    typedef typename _shard_type::key_type          key_type;
    typedef typename _shard_type::mapped_type       mapped_type;
    typedef typename _shard_type::size_type         size_type;
    typedef typename _shard_type::hasher            hasher;
    typedef typename _shard_type::key_equal         key_equal;
    typedef TLock                                   lock_type;
    typedef typename _shard_type::reference         reference;
    typedef typename _shard_type::const_reference   const_reference;
    typedef typename _shard_type::pointer           pointer;
    typedef typename _shard_type::const_pointer     const_pointer;

    explicit thread_safe_lru_cache(size_t capacity, size_t shards_count)
        : m_capacity(capacity)
        , m_shards((m_capacity > shards_count) ? shards_count : m_capacity)
    {
        shards_count = m_shards.size();
        for (size_t i = 0; i < shards_count; i++) {
            const size_t shard_capacity = (i != 0)
                ? (m_capacity / shards_count)
                : ((m_capacity / shards_count) + (m_capacity % shards_count));
            m_shards[i].first = std::make_shared<_shard_type>(shard_capacity);
        }
    }

    thread_safe_lru_cache(const thread_safe_lru_cache&) = delete;
    thread_safe_lru_cache& operator=(const thread_safe_lru_cache&) = delete;

    size_type capacity() const { return m_capacity; }

    void clear()
    {
        for (_shard_guard& sh : m_shards) {
            wrapper(sh, &_shard_type::clear);
        }
    }

    bool contains(const key_type& key)
    {
        return wrapper(get_shard(key), &_shard_type::contains, key);
    }

    /**
     *  \attention  This method constructs the object in place, so it may take a
     *              long time to complete. The lock will be held during the
     *              construction of the object, so it is not recommended to use
     *              this method in a multi-threaded application.
     */
    template<typename... TArgs>
    bool emplace(const key_type& key, TArgs&&... args)
    {
        // Fixing error: no matching function for call to
        // 'wrapper(_safe_shard_type&, <unresolved overloaded function type>)'
        return wrapper(get_shard(key), &_shard_type::template emplace<TArgs...>,
                       key, std::forward<TArgs>(args)...);
    }

    bool empty() const
    {
        for (const _shard_guard& sh : m_shards) {
            if (! wrapper(sh, &_shard_type::empty)) {
                return false;
            }
        }
        return true;
    }

    void erase(const key_type& key)
    {
        wrapper(get_shard(key), &_shard_type::erase, key);
    }

    bool find(const key_type& key, mapped_type& result)
    {
        return wrapper(get_shard(key), &_shard_type::find, key, result);
    }

    bool insert(const key_type& key, const mapped_type& val)
    {
        return wrapper(get_shard(key), &_shard_type::insert, key, val);
    }

#if defined(LRU_CACHE_ENABLE_STD_OPTIONAL)
    std::optional<mapped_type> get(const key_type& key)
    {
        return wrapper(get_shard(key), &_shard_type::get, key);
    }
#endif

    void reserve(size_type new_capacity)
    {
        //m_capacity = new_capacity;
        //m_cache.reserve(m_capacity);
        size_t shards_count = m_shards.size();
        for (size_t i = 0; i < shards_count; i++) {
            const size_t shard_capacity = (i != 0)
                ? (new_capacity / shards_count)
                : ((new_capacity / shards_count) + (new_capacity % shards_count));
            //m_shards[i].first = std::make_shared<_shard_type>(shard_capacity);
            wrapper(m_shards[i], &_shard_type::reserve, shard_capacity);
        }
        m_capacity = new_capacity;
    }

    size_type shards_size() const { return m_shards.size(); }

    size_type size() const
    {
        size_type s = 0;
        for (const _shard_guard& sh : m_shards) {
            s += wrapper(sh, &_shard_type::size);
        }
        return s;
    }

    void update(const key_type& key, const mapped_type& val)
    {
        //wrapper(get_shard(key), &_shard_type::update, key, val);
        // Fixing error: no matching function for call to
        // 'wrapper(_safe_shard_type&, <unresolved overloaded function type>)'
        constexpr void (_shard_type::*p_update)(const key_type&, const mapped_type&) = &_shard_type::update;
        wrapper(get_shard(key), p_update, key, std::forward<decltype(val)>(val));
    }

    void update(const key_type& key, mapped_type&& val)
    {
        //wrapper(get_shard(key), &_shard_type::update, key, val);
        // Fixing error: no matching function for call to
        // 'wrapper(_safe_shard_type&, <unresolved overloaded function type>)'
        constexpr void (_shard_type::*p_update)(const key_type&, mapped_type&&) = &_shard_type::update;
        wrapper(get_shard(key), p_update, key, std::forward<decltype(val)>(val));
    }

private:
    typedef std::shared_ptr<_shard_type>                    _shard_ptr_type;
//    typedef std::pair<_shard_ptr_type, TLock>               _shard_guard;
    struct _shard_guard
    {
        _shard_ptr_type first;
        mutable lock_type second;
    };

private:
    inline const _shard_guard& get_shard(const TKey& key) const
    {
        //constexpr size_t shift = std::numeric_limits<size_t>::digits - 16;
        //return m_shards[(hasher{}(key) >> shift) % m_shards.size()];
        return m_shards[(hasher{}(key)) % m_shards.size()];
    }

    inline _shard_guard& get_shard(const TKey& key)
    {
        //constexpr size_t shift = std::numeric_limits<size_t>::digits - 16;
        //return m_shards[(hasher{}(key) >> shift) % m_shards.size()];
        return m_shards[(hasher{}(key)) % m_shards.size()];
    }

    template<typename T, typename TFn, typename... TArgs>
    inline typename std::result_of<TFn(_shard_type, TArgs...)>::type wrapper(T& p, const TFn& func, TArgs&&... args)
    {
        std::unique_lock<decltype(p.second)> lock(p.second);
        return (p.first.get()->*func)(std::forward<TArgs>(args)...);
    }

    template<typename T, typename TFn, typename... TArgs>
    inline typename std::result_of<TFn(_shard_type, TArgs...)>::type wrapper(const T& p, const TFn& func, TArgs&&... args) const
    {
        std::unique_lock<decltype(p.second)> lock(p.second);
        return (p.first.get()->*func)(std::forward<TArgs>(args)...);
    }

private:
    size_t m_capacity;
    std::vector<_shard_guard> m_shards;
};

} // namespace lru
} // namespace wstux

#endif /* _LRU_CACHE_THREAD_SAFE_LRU_CACHE_H */

