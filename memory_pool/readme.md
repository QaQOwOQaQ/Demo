This tutorial shows how to use memory pools to increase memory allocation/deallocation and preventing fragmentation. Useful in high performance systems or systems with limited resources, e.g. embedded and/or realtime systems.

It is implemented in C++ using templates.

A memory pool is, in general, a preallocated chunk of memory to be used by an application. The preallocation prevents the application to ask the operating system for memory (through new/delete or malloc/free). This way, preallocated memory saves system calls and defies the non-deterministic nature of memory allocation.

To prevent fragmentation, the easy solution is to have every object within the memory pool the same size. This sounds like an unnatural requirement but in practice (embedded realtime systems) the communication within the application is often done using messages of a defined size.

In practice it is often useful to define multiple memory pools for different kind of messages or for different communication channels (e.g. synchronization points like message queues).

The memory pool shown in this tutorial does the exactly the things stated above.

Note: to make the tutorial focus on the imporant parts, this implementation is **not thread-safe**.

In Practice
Considering the following code, it is a simple application of dynamic memory usage, creating object from a class A:

int main(int, char **)
{
    // array to hold objects
    A * a[64];

    // create objects, allocate memory
    for (int i = 0; i < 8; ++i)
        a[i] = new A(i);

    // delete objects, free memory
    for (int i = 0; i < 8; ++i) {
        delete a[i];
        a[i] = NULL;
    }

    return 0;
}
The usage of a memory pool should be transparent for the rest of the application. Let’s have a look at the class A. For easier inspection, the code is written inline.

class A
{
    private:
        int data; // some data
    public:
        // initiailzation of the object.
        A(int data) : data(data) {}

        // cleanup
        virtual ~A() {}
};
This code already works but does not use a memory pool, it uses the standard new/delete mechanism for memory management.

The following code shows the extension to be made to use the memory pool for objects of the class A. All other classes are still handled by standard memory management.

class A
{
    private:
        typedef memory_pool<A, mempool_malloc<A> > MemoryPool;
        static MemoryPool mem;
    private:
        int data; // some data
    public:
        // initiailzation of the object.
        A(int data) : data(data) {}

        // cleanup
        virtual ~A() {}

        // operator 'new' and 'delete' overloaded to redirect any memory management,
        // in this case delegated to the memory pool
        void * operator new(unsigned int n)
        {
            return mem.alloc(n);
        }

        void operator delete(void * p)
        {
            mem.free(p);
        }
};

A::MemoryPool A::mem; // instance of the static memory pool
The memory pool is static for the class A, so all objects use the memory pool. Other classes are not affected. From this point it is not possible to have some objects of this class in a memory pool and some not.

Memory Pool
The implementation of the memory pool is rather simple:

template <typename T, class Strategy> class memory_pool
{
    private:
        Strategy s;
    public:
        memory_pool()
        {
            s.init();
        }

        void * alloc(unsigned int n)
        {
            if (sizeof(T) != n) throw std::bad_alloc();
            return s.allocate();
        }

        void free(void * p)
        {
            s.deallocate(p);
        }
};
The important and interesting part is done in the Strategy. The design is intentional, to make the behaviour of the memory pool configurable.

Note: the memory pool (or the strategy respectively) is initialized only once. Since the pool is a static member of the class A, the constructor of the memory pool is called only once.

The next chapter gets into the different strategies.

Strategies
This chapter shows different strategies to configure the memoy pool. This way it is very easy to extend the memory pool with different strategies.

malloc/free
This strategy is simply to use the default memory management, using malloc and free.

template <typename T> class mempool_malloc
{
    public:
        void init() {}

        void * allocate()
        {
            return ::malloc(sizeof(T));
        }

        void deallocate(void * p)
        {
            ::free(p);
        }
};
There is nothing to initialize (may be different in certain embedded operating systems). The allocation and deallocation is done by calling malloc and free.

The template parameter simply takes the type of data to be managed.

A thing to note: using this strategy the memory pool is not really a pool anymore. Also the memory is limited by what has the operating system to offer.

Simple Array
A simple array to handle memory is not a very good idea in practice, but it helps to show different types of strategies and for performance comparsions later on.

template <typename T, unsigned int N> class mempool_linear
{
    private:
        typedef unsigned int size_type; // convinience
    private:
        uint8_t buf[sizeof(T)*N]; // the memory, the objects are stored here
        bool state[N]; // true if the memory chunk is occupied, false if it is free
    public:
        void init()
        {
            // initially all memory chunks are free
            for (size_type i = 0; i < N; ++i) state[i] = false;
        }

        void * allocate()
        {
            // search linearily through the array to find an unused
            // memory chunk, take it and mark it occupied
            for (size_type i = 0; i < N; ++i)
                if (!state[i]) {
                    state[i] = true;
                    return &buf[sizeof(T)*i];
                }

            // no free memory chunks could be found
            throw std::bad_alloc();
        }

        void deallocate(void * p)
        {
            // search all memory chunks to find the one to be freed,
            // then mark it as not occupied.
            // please note: this could have been done with pointer arithmetic,
            // to gain more efficiency, but is not done here intentionally.
            for (size_type i = 0; i < N; ++i)
                if (&buf[sizeof(T)*i] == p) {
                    state[i] = false;
                    return;
                }
        }
};
The template parameters are T the type of data to be managed and N the number of memory chunks to be reserved for the memory pool to be used. It is not possible to use more objects of the class T than N.

The allocation of the memory is done in a linear search through the array to find an unoccupied place. If the memory pool is very small, this might even be reasonable. However not recommended for larger memory pools.

The deallocation is the same: search for the place to be freed and do it.

Heap
This strategy uses a heap (see Heap @ wikipedia) to handle the memory pool. Operations (allocation and deallocation) are of logarithmic complexity. It is not part of this tutorial to show how a heap works.

template <typename T, unsigned int N> class mempool_heap
{
    private:
        typedef unsigned int size_type; // convinience
    private:
        size_type available; // number of memory chunks available
        T * a[N+1]; // book keeping, first place not used
        uint8_t buf[sizeof(T)*N]; // the memory itself, here will the objects be stored
    private:
        inline void swap(size_type i, size_type j)
        {
            T * t = a[i];
            a[i] = a[j];
            a[j] = t;
        }

        void up()
        {
            for (size_type n = available; n > 1; ) {
                size_type i = n / 2;
                if (!(a[i] && a[n]))
                    swap(i, n);
                n = i;
            }
        }

        void down()
        {
            size_type i = 1;
            size_type c = 2 * i;
            while (c <= available) {
                if (c+1 <= available)
                    if (!(a[c] && a[c+1]))
                        ++c;
                if (!(a[i] && a[c]))
                    swap(i, c);
                i = c;
                c = 2 * i;
            }
        }
    public:
        void init()
        {
            // number of available memory chunks is the size of the memory pool, naturally
            available = N;

            // first node is not used, convinience
            a[0] = NULL;

            // all other memory chunks are free, the pointers are initialized
            for (size_type i = 1; i <= N; ++i) {
                a[i] = reinterpret_cast<T *>(&buf[sizeof(T)*(i-1)]);
            }
        }

        void * allocate()
        {
            // allocation not possible if the memory pool has no more space
            if (available <= 0) throw std::bad_alloc();

            // the first memory chunk is always on index 1, this is guaranteed
            // by the heap
            T * t = a[1];

            // move the last free node to front
            a[1] = a[available];

            // mark the used chunk
            a[available] = NULL;

            // number of free chunks decreased
            --available;

            // book keeping for the heap
            down();

            // return a pointer to the allocated space
            return t;
        }

        void deallocate(void * p)
        {
            // invalid pointers are ignored
            if (!p || available >= N) return;

            // memory is freed, more space within the pool
            ++available;

            // the freed node is, well, free. status and pointer are restored.
            a[available] = reinterpret_cast<T *>(p);

            // book keeping for the heap
            up();
        }
};
The template parameters are the same as in chapter [[#Simple Array|Simple Array]]: data type and size of the memory pool.

Since the operations using the heap are of logarithmic nature, this strategy is very useful for large memory pools.

Standard Template Library Heap
The C++ Standard Library already provides functions to use heaps. This strategy is also a heap, using the functions provided by the standard library (make_heap, pop_heap, push_heap).

The implementation is very similar to the heap above. The disadvantage is the dependency to the algorithm header of the standard library.

template <typename T, unsigned int N> class mempool_std_heap
{
    private:
        // states of the heap nodes
        enum State { FREE = 1, TAKEN = 0 };

        struct Entry {
            State state; // the state of the memory chunk
            T * p; // pointer to the memory chunk

            // comparsion operator, needed for heap book keeping
            bool operator<(const Entry & other) const
            { return state < other.state; }
        };

        typedef unsigned int size_type; // convenience
    private:
        size_type available; // number of memory chunks available
        Entry a[N]; // book keeping
        uint8_t buf[sizeof(T)*N]; // the actual memory, here will the objects be stored
    public:
        void init()
        {
            // number of available memory chunks is the size of the memory pool
            available = N;

            // all memory chunks are free, pointers are initialized
            for (size_type i = 0; i < N; ++i) {
                a[i].state = FREE;
                a[i].p = reinterpret_cast<T *>(&buf[sizeof(T)*i]);
            }

            // make heap of book keeping array
            std::make_heap(a, a+N);
        }

        void * allocate()
        {
            // allocation not possible if memory pool has no more space
            if (available <= 0 || available > N) throw std::bad_alloc();

            // the first memory chunk is always on index 0
            Entry e = a[0];

            // remove first entry from heap
            std::pop_heap(a, a+N);

            // one memory chunk allocated, no more available
            --available;

            // mark the removed chunk
            a[available].state = TAKEN;
            a[available].p = NULL;

            // return pointer to the allocated memory
            return e.p;
        }

        void deallocate(void * ptr)
        {
            // invalid pointers are ignored
            if (!ptr || available >= N) return;

            // mark freed memory as such
            a[available].state = FREE;
            a[available].p = reinterpret_cast<T *>(ptr);

            // freed memory chunk, one more available
            ++available;

            // heap book keeping
            std::push_heap(a, a+N);
        }
};
Performance
This chapter compares the memory pool strategies shown above.

TODO