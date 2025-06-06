#include "alloc_new_delete.hpp"
#include "alloc_pool.hpp"
#include <iostream>
#include <vector>

void test_standard_vector()
{
    std::vector<int> v;
    std::cout << "#================ standard behaviour ==================#" << std::endl;

    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.push_back(40);
    v.push_back(50);
    v.push_back(60);
    v.push_back(70);

    std::cout << "after push 7 element-------------------->" << std::endl;
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

    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.push_back(40);
    v.push_back(50);
    v.push_back(60);
    v.push_back(70);

    std::cout << "after push 7 element-------------------->" << std::endl;
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