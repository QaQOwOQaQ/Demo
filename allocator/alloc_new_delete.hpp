#ifndef __ALLOC_NEW_DELETE__HPP__
#define __ALLOC_NEW_DELETE__HPP__

#include <limits>
#include <cstddef>
#include <bits/allocator.h>

template <typename T> class alloc_new_delete
{
    public:
        typedef T value_type;
        typedef value_type * pointer;
        typedef const value_type * const_pointer;
        typedef value_type & reference;
        typedef const value_type const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

    public:
        template <typename U> struct rebind
        {
            typedef alloc_new_delete<U> other;
        };

    public:
        explicit alloc_new_delete()
        {}

        ~alloc_new_delete()
        {}

        explicit alloc_new_delete(const alloc_new_delete &)
        {}

        template <typename U> explicit alloc_new_delete(alloc_new_delete<U> const &)
        {}

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
            return reinterpret_cast<pointer>(::operator new(n * sizeof(T)));
        }

        void deallocate(pointer p, size_type)
        {
            ::operator delete(p);
        }

    public:
        size_type max_size() const
        {
            return std::numeric_limits<size_type>::max() / sizeof(T);
        }

    public:
        void construct(pointer p, const T & t)
        {
            new(p) T(t);
        }

        void destroy(pointer p)
        {
            p->~T();
        }

    public:
        bool operator==(const alloc_new_delete &)
        {
            return true;
        }

        bool operator!=(const alloc_new_delete & a)
        {
            return !operator==(a);
        }
};

#endif