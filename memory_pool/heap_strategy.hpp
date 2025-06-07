#ifndef HEAP_STRATEGY_HPP_HEADER_GUARD
#define HEAP_STRATEGY_HPP_HEADER_GUARD

#include <stdint.h>
#include <algorithm>

template <typename T, unsigned int N>
class mempool_std_heap
{
private:
    // states of the heap nodes
    enum State
    {
        FREE = 1,
        TAKEN = 0
    };

    // 内存块的元数据
    struct Entry
    {
        State state; // the state of the memory chunk
        T *p;        // pointer to the memory chunk

        // comparsion operator, needed for heap book keeping
        // make_heap 默认创建最大堆
        // 这里重载 operator<，让 State 值更大的元素（Free，空闲内存块）放到堆的顶部
        bool operator<(const Entry &other) const
        {
            return state < other.state;
        }
    };

    typedef unsigned int size_type; // convenience

private:
    size_type available;        // number of memory chunks available
    Entry a[N];                 // book keeping
    // 使用 uint8_t 而不是 T 分配内存池空间的原因是:
    // 
    uint8_t buf[sizeof(T) * N]; // the actual memory, here will the objects be stored

public:
    void init()
    {
        // number of available memory chunks is the size of the memory pool
        available = N;

        // all memory chunks are free, pointers are initialized
        for (size_type i = 0; i < N; ++i)
        {
            a[i].state = FREE;
            a[i].p = reinterpret_cast<T *>(&buf[sizeof(T) * i]);
        }

        // make heap of book keeping array
        std::make_heap(a, a + N);
    }

    void* allocate()
    {
        // allocation not possible if memory pool has no more space
        if (available <= 0 || available > N)
            throw std::bad_alloc();

        // the first memory chunk is always on index 0
        Entry e = a[0];

        // remove first entry from heap
        std::pop_heap(a, a + available);

        // one memory chunk allocated, no more available
        --available;

        // mark the removed chunk
        a[available].state = TAKEN;
        a[available].p = NULL;

        // return pointer to the allocated memory
        return e.p;
    }

    void deallocate(void *ptr)
    {
        // invalid pointers are ignored
        if (!ptr || available >= N)
            return;

        // mark freed memory as such
        a[available].state = FREE;
        a[available].p = reinterpret_cast<T *>(ptr);

        // freed memory chunk, one more available
        ++available;

        // heap book keeping
        std::push_heap(a, a + available);
    }
};

#endif

