#ifndef __ALLOC_POOL__HPP__
#define __ALLOC_POOL__HPP__

#include <limits>
#include <cstddef>
#include <bits/allocator.h>

template <typename T, std::size_t N>
class alloc_pool
{
private:
    T data[N];
    bool state[N];

public:
    typedef T                  value_type;
    typedef value_type*        pointer;
    typedef const value_type*  const_pointer;
    typedef value_type&        reference;
    typedef const value_type&  const_reference;
    typedef std::size_t        size_type;
    typedef std::ptrdiff_t     difference_type;

public:
    template <typename U>
    struct rebind
    {
        typedef alloc_pool<U, N> other;
    };

public:
    explicit alloc_pool()
    {
        for(size_type i = 0; i < N; i ++ ) 
        {
            state[i] = false;
        }
    }

    ~alloc_pool()
    {
    }

// 禁止拷贝和赋值，避免多个内存池实例管理同一块内存
public:
    explicit alloc_pool(const alloc_pool &) = delete;

    alloc_pool &operator=(const alloc_pool &) = delete;

    template <typename U>
    explicit alloc_pool(alloc_pool<U, N> const &) = delete;

    template <typename U>
    alloc_pool<T, N> &operator=(const alloc_pool<U, N> &) = delete;

public:
    pointer address(reference a)
    {
        return &a;
    }

    const_pointer address(const_reference a)
    {
        return &a;
    }

public:
    pointer allocate(size_type n, typename std::allocator<void>::const_pointer = NULL)
    {
        pointer p;
        size_type j = 0;
        size_type num_free = 0;
        for (size_type i = 0; (i < N) && (num_free < n); ++i)
        {
            if (!state[i]) // First-Fir Alogirthm
            {
                if (num_free == 0) // 记录首地址信息
                {
                    p = &data[i];
                    j = i;
                }
                ++num_free;
            }
            else // 分配连续内存空间
            {
                num_free = 0;
            }
        }
        if (num_free < n)
            return pointer();
        for (; num_free > 0; --num_free, ++j)
            state[j] = true;
        return p;
    }

    void deallocate(pointer p, size_type n)
    {
        for (size_type i = 0; i < N; ++i)
        {
            if (&data[i] == p)
            {
                for (size_type j = 0; j < n; ++j, ++i)
                {
                    state[i] = false;
                }
                return;
            }
        }
    }

public:
    size_type max_size() const
    {
        return N;
    }

public:
    void construct(pointer p, const T &t)
    {
        new (p) T(t);
    }

    void destroy(pointer p)
    {
        p->~T();
    }

public:
    bool operator==(const alloc_pool &)
    {
        return true;
    }

    bool operator!=(const alloc_pool &a)
    {
        return !operator==(a);
    }
};

#endif