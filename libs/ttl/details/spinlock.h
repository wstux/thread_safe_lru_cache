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

#ifndef _TTL_CACHE_SPINLOCK_H
#define _TTL_CACHE_SPINLOCK_H

#include <atomic>

namespace wstux {
namespace ttl {
namespace details {

/*
 *  The spinlock implementation described in the links is used:
 *  https://www.talkinghightech.com/en/implementing-a-spinlock-in-c/
 *  https://rigtorp.se/spinlock/
 */
class spinlock
{
public:
    void lock()
    {
        for (;;)
        {
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

} // namespace details
} // namespace ttl
} // namespace wstux

#endif /* _TTL_CACHE_SPINLOCK_H */

