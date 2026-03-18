/**
 * @file test_pool_allocator.cpp
 * @brief Comprehensive test suite for mystl::pool_allocator implementation
 * @brief mystl::pool_allocator实现的综合测试套件
 * 
 * This file contains tests for the pool_allocator library in mystl.
 * It verifies the correctness of memory pool allocator operations.
 * 
 * 此文件包含对mystl中内存池分配器库的测试。
 * 验证内存池分配器操作的正确性。
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
            std::cerr << "[FAIL] Line " << __LINE__ << ": " << #expr << std::endl; \
            std::cerr << "[失败] 行号 " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  ✓ " << #expr << std::endl; \
        } \
    } while(0)

/**
 * @brief Main test function for pool_allocator
 * @brief pool_allocator的主测试函数
 * 
 * Tests various aspects of mystl::pool_allocator implementation.
 * 测试mystl::pool_allocator实现的各个方面。
 * 
 * @return 0 if all tests pass, 1 otherwise
 *         如果所有测试通过返回0，否则返回1
 */
int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Testing mystl::pool_allocator Implementation" << std::endl;
    std::cout << "测试mystl::pool_allocator实现" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // 1. Basic type definitions
    // 1. 基本类型定义
    std::cout << "1. Testing Type Definitions / 测试类型定义:" << std::endl;
    {
        mystl::pool_allocator<int> alloc;
        static_assert(std::is_same_v<decltype(alloc)::value_type, int>, 
                      "value_type should be int / value_type应为int");
        std::cout << "  Type definitions OK / 类型定义正常" << std::endl;
    }
    
    // 2. Basic allocation and deallocation
    // 2. 基本分配和释放
    std::cout << "\n2. Testing Basic Allocation and Deallocation / 测试基本分配和释放:" << std::endl;
    {
        mystl::pool_allocator<int> alloc;
        int* p = alloc.allocate(10);
        TEST(p != nullptr);
        alloc.deallocate(p, 10);
        std::cout << "  Basic allocation/deallocation OK / 基本分配/释放正常" << std::endl;
    }
    
    // 3. Object construction and destruction
    // 3. 对象构造和销毁
    std::cout << "\n3. Testing Object Construction and Destruction / 测试对象构造和销毁:" << std::endl;
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
        std::cout << "  Object construction/destruction OK / 对象构造/销毁正常" << std::endl;
    }
    
    // 4. Compatibility with standard containers
    // 4. 与标准容器的兼容性
    std::cout << "\n4. Testing Compatibility with Standard Containers / 测试与标准容器的兼容性:" << std::endl;
    {
        std::vector<int, mystl::pool_allocator<int>> vec;
        for (int i = 0; i < 10; ++i) {
            vec.push_back(i);
        }
        TEST(vec.size() == 10);
        TEST(vec[0] == 0);
        TEST(vec[9] == 9);
        std::cout << "  Works with std::vector OK / 与std::vector兼容正常" << std::endl;
    }
    
    // 5. Rebind mechanism
    // 5. 重新绑定机制
    std::cout << "\n5. Testing Rebind Mechanism / 测试重新绑定机制:" << std::endl;
    {
        mystl::pool_allocator<int> int_alloc;
        using rebind_alloc = typename mystl::pool_allocator<int>::template rebind<double>::other;
        rebind_alloc double_alloc(int_alloc);
        
        double* p = double_alloc.allocate(5);
        TEST(p != nullptr);
        double_alloc.deallocate(p, 5);
        std::cout << "  Rebind mechanism OK / 重新绑定机制正常" << std::endl;
    }
    
    std::cout << "\n=========================================" << std::endl;
    std::cout << "All tests PASSED!" << std::endl;
    std::cout << "所有测试通过！" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    return 0;
}
