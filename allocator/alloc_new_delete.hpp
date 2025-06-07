#ifndef __ALLOC_NEW_DELETE__HPP__
#define __ALLOC_NEW_DELETE__HPP__

#include <memory>
#include <cstddef>
#include <limits>
#include <iostream>

template <typename T>
class alloc_new_delete
{
public:
    typedef T                  value_type;
    typedef value_type*        pointer;
    typedef const value_type*  const_pointer;
    typedef value_type&        reference;
    typedef const value_type&  const_reference; 
    typedef std::size_t        size_type;
    typedef std::ptrdiff_t     difference_type;

public:
    // 允许一个分配器为不同类型的对象分配内存，而无需创建全新的分配器实例
    template <typename U>
    struct rebind
    {
        typedef alloc_new_delete<U> other;
    };

public:
    explicit alloc_new_delete() {}

    ~alloc_new_delete() {}

    explicit alloc_new_delete(const alloc_new_delete &) {}

    template <typename U>
    explicit alloc_new_delete(alloc_new_delete<U> const &) {}

public:
    pointer address(reference a) { return &a; }

    const_pointer address(const_reference a) { return &a; }

public:
    // ? why use reinterrept_cast but static_cast
    // 这里的目的是将 ::operator new 返回的 void* 指针转换为 pointer 类型的指针
    // 对于 static_cast 来说，虽然可以将 void* 转换为 pointer，但是这是建立在该 void* 最初是通过 pointer 得到的
    // 换句话说，static_cast 会默认认为这种转换是安全的，因为它假设你是在恢复原始的类型信息
    // 但其实在这里，我们没有办法做出这种保证
    // reinterpret_cast 的设计初衷就是为了处理这种“不安全”或“低级”的位模式重新解释
    // 它告诉编译器：“我明白你不能保证这个转换的类型安全性，但我知道底层内存的布局，请按我的要求重新解释这个指针。”
    pointer allocate(size_type n,
                     typename std::allocator<void>::const_pointer = NULL)
    {
        // std::cout << "call allocator::allocate: " << n << std::endl;
        return reinterpret_cast<pointer>(::operator new(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type) 
    { 
        // std::cout << "call allocator::deallocate" << std::endl;
        ::operator delete(p); 
    }

public:
    size_type max_size() const
    {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

public:
    void construct(pointer p, const T &t) { new (p) T(t); }

    void destroy(pointer p) { p->~T(); }

public:
    bool operator==(const alloc_new_delete &) { return true; }

    bool operator!=(const alloc_new_delete &a) { return !operator==(a); }
};

#endif