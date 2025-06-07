#include "alloc_new_delete.hpp"
#include "alloc_pool.hpp"
#include <iostream>
#include <vector>

void test_standard_vector()
{
    std::vector<int> v;
    std::cout << "#================ standard behaviour ==================#" << std::endl;
    int n = 17;
    for(int i = 0; i < 17; i ++ ) v.push_back(i);
    std::cout << "after push " << n << " element-------------------->" << std::endl;
    std::cout << "size(): " << v.size() << ' ' << std::endl;
    std::cout << "capacity(): " << v.capacity() << ' ' << std::endl;

    v.erase(v.begin());
    std::cout << "after erase 1 element-------------------->" << std::endl;
    std::cout << "size(): " << v.size() << ' ' << std::endl;
    std::cout << "capacity(): " << v.capacity() << ' ' << std::endl;
}

template <class Allocator>
void test()
{
    std::vector<int, Allocator> v;
    std::cout << "#================= customer allocator behaviour ===============#" << std::endl;

    int n = 17;
    for(int i = 0; i < 17; i ++ ) v.push_back(i);
    std::cout << "after push " << n << " element-------------------->" << std::endl;
    std::cout << "size(): " << v.size() << ' ' << std::endl;
    std::cout << "capacity(): " << v.capacity() << ' ' << std::endl;

    v.erase(v.begin());
    std::cout << "after erase 1 element-------------------->" << std::endl;
    std::cout << "size(): " << v.size() << ' ' << std::endl;
    std::cout << "capacity(): " << v.capacity() << ' ' << std::endl;
}

int main(int, char **)
{
    test_standard_vector();
    test<alloc_new_delete<int>>();
    test<alloc_pool<int, 16>>();
    return 0;
}