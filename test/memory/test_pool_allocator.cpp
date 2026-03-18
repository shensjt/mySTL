/**
 * @file test_pool_allocator.cpp
 * @brief Comprehensive test suite for mystl::pool_allocator implementation
 * 
 * This file contains tests for the pool_allocator library in mystl.
 * It verifies the correctness of memory pool allocator operations.
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/pool_allocator.hpp
 */

#include "../../include/memory/pool_allocator.hpp"
#include <iostream>
#include <cassert>
#include <vector>

#define TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  Test passed: " << #expr << std::endl; \
        } \
    } while(0)

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Testing mystl::pool_allocator Implementation" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // 1. Basic type definitions
    std::cout << "1. Testing Type Definitions:" << std::endl;
    {
        mystl::pool_allocator<int> alloc;
        static_assert(std::is_same_v<decltype(alloc)::value_type, int>, 
                      "value_type should be int");
        std::cout << "  Type definitions OK" << std::endl;
    }
    
    // 2. Basic allocation and deallocation
    std::cout << "\n2. Testing Basic Allocation and Deallocation:" << std::endl;
    {
        mystl::pool_allocator<int> alloc;
        int* p = alloc.allocate(10);
        TEST(p != nullptr);
        alloc.deallocate(p, 10);
        std::cout << "  Basic allocation/deallocation OK" << std::endl;
    }
    
    // 3. Object construction and destruction
    std::cout << "\n3. Testing Object Construction and Destruction:" << std::endl;
    {
        mystl::pool_allocator<std::string> alloc;
        std::string* p = alloc.allocate(3);
        
        alloc.construct(p, "Hello");
        alloc.construct(p + 1, "World");
        alloc.construct(p + 2, "!");
        
        TEST(p[0] == "Hello");
        TEST(p[1] == "World");
        TEST(p[2] == "!");
        
        alloc.destroy(p);
        alloc.destroy(p + 1);
        alloc.destroy(p + 2);
        
        alloc.deallocate(p, 3);
        std::cout << "  Object construction/destruction OK" << std::endl;
    }
    
    // 4. Compatibility with standard containers
    std::cout << "\n4. Testing Compatibility with Standard Containers:" << std::endl;
    {
        std::vector<int, mystl::pool_allocator<int>> vec;
        for (int i = 0; i < 10; ++i) {
            vec.push_back(i);
        }
        TEST(vec.size() == 10);
        TEST(vec[0] == 0);
        TEST(vec[9] == 9);
        std::cout << "  Works with std::vector OK" << std::endl;
    }
    
    // 5. Rebind mechanism
    std::cout << "\n5. Testing Rebind Mechanism:" << std::endl;
    {
        mystl::pool_allocator<int> int_alloc;
        using rebind_alloc = typename mystl::pool_allocator<int>::template rebind<double>::other;
        rebind_alloc double_alloc(int_alloc);
        
        double* p = double_alloc.allocate(5);
        TEST(p != nullptr);
        double_alloc.deallocate(p, 5);
        std::cout << "  Rebind mechanism OK" << std::endl;
    }
    
    std::cout << "\n=========================================" << std::endl;
    std::cout << "All tests PASSED!" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    return 0;
}
