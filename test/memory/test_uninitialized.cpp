/**
 * @file test_uninitialized.cpp
 * @brief Comprehensive test suite for mystl uninitialized memory operations
 * @brief mystl未初始化内存操作的综合测试套件
 * 
 * This file contains tests for uninitialized memory operations in mystl.
 * It verifies the correctness of functions for constructing, copying, moving,
 * filling, and destroying objects in uninitialized memory.
 * 
 * 此文件包含对mystl中未初始化内存操作的测试。
 * 验证在未初始化内存中构造、复制、移动、填充和销毁对象函数的正确性。
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/uninitialized.hpp
 */

#include <iostream>
#include <cassert>
#include <vector>
#include <stdexcept>
#include "../../include/memory/uninitialized.hpp"
#include "../../include/memory/allocator.hpp"

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

// Test class with constructor/destructor tracking
// 用于跟踪构造函数/析构函数的测试类
class TrackedType {
public:
    static int constructor_count;    ///< 构造函数调用计数 / Constructor call count
    static int destructor_count;     ///< 析构函数调用计数 / Destructor call count
    static int copy_count;           ///< 拷贝构造函数调用计数 / Copy constructor call count
    static int move_count;           ///< 移动构造函数调用计数 / Move constructor call count
    
    int value;                       ///< 对象值 / Object value
    bool throw_on_copy;              ///< 是否在拷贝时抛出异常 / Whether to throw exception on copy
    
    TrackedType(int v = 0, bool throw_copy = false) 
        : value(v), throw_on_copy(throw_copy) { 
        ++constructor_count;
    }
    
    ~TrackedType() { 
        ++destructor_count;
    }
    
    TrackedType(const TrackedType& other) 
        : value(other.value), throw_on_copy(other.throw_on_copy) { 
        ++constructor_count;
        ++copy_count;
        if (throw_on_copy) {
            throw std::runtime_error("Copy constructor intentionally failed / 拷贝构造函数故意失败");
        }
    }
    
    TrackedType(TrackedType&& other) noexcept 
        : value(other.value), throw_on_copy(other.throw_on_copy) { 
        ++constructor_count;
        ++move_count;
        other.value = -1;
    }
    
    static void reset_counts() {
        constructor_count = 0;
        destructor_count = 0;
        copy_count = 0;
        move_count = 0;
    }
    
    static void print_counts() {
        std::cout << "    Constructed: " << constructor_count 
                  << ", Destructed: " << destructor_count
                  << ", Copied: " << copy_count
                  << ", Moved: " << move_count << std::endl;
        std::cout << "    已构造: " << constructor_count 
                  << ", 已析构: " << destructor_count
                  << ", 已拷贝: " << copy_count
                  << ", 已移动: " << move_count << std::endl;
    }
};

int TrackedType::constructor_count = 0;
int TrackedType::destructor_count = 0;
int TrackedType::copy_count = 0;
int TrackedType::move_count = 0;

/**
 * @brief Main test function for uninitialized memory operations
 * @brief 未初始化内存操作的主测试函数
 * 
 * Tests various aspects of mystl uninitialized memory operations.
 * 测试mystl未初始化内存操作的各个方面。
 * 
 * @return 0 if all tests pass, 1 otherwise
 *         如果所有测试通过返回0，否则返回1
 */
int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Testing mystl Uninitialized Memory Operations" << std::endl;
    std::cout << "测试mystl未初始化内存操作" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // 1. Test uninitialized_copy
    // 1. 测试uninitialized_copy
    std::cout << "1. Testing uninitialized_copy / 测试uninitialized_copy:" << std::endl;
    {
        std::vector<int> src = {1, 2, 3, 4, 5};
        mystl::allocator<int> alloc;
        int* dest = alloc.allocate(src.size());
        
        try {
            mystl::uninitialized_copy(src.begin(), src.end(), dest);
            
            for (size_t i = 0; i < src.size(); ++i) {
                TEST(dest[i] == src[i]);
            }
            
            mystl::destroy(dest, dest + src.size());
            alloc.deallocate(dest, src.size());
            std::cout << "  uninitialized_copy OK / uninitialized_copy正常" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, src.size());
            throw;
        }
    }
    
    // 2. Test uninitialized_fill
    // 2. 测试uninitialized_fill
    std::cout << "\n2. Testing uninitialized_fill / 测试uninitialized_fill:" << std::endl;
    {
        const int fill_value = 42;
        const size_t count = 5;
        
        mystl::allocator<int> alloc;
        int* dest = alloc.allocate(count);
        
        try {
            mystl::uninitialized_fill(dest, dest + count, fill_value);
            
            for (size_t i = 0; i < count; ++i) {
                TEST(dest[i] == fill_value);
            }
            
            mystl::destroy(dest, dest + count);
            alloc.deallocate(dest, count);
            std::cout << "  uninitialized_fill OK / uninitialized_fill正常" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, count);
            throw;
        }
    }
    
    // 3. Test uninitialized_move
    // 3. 测试uninitialized_move
    std::cout << "\n3. Testing uninitialized_move / 测试uninitialized_move:" << std::endl;
    {
        TrackedType::reset_counts();
        std::vector<TrackedType> src;
        src.reserve(3);
        src.emplace_back(100);
        src.emplace_back(200);
        src.emplace_back(300);
        
        mystl::allocator<TrackedType> alloc;
        TrackedType* dest = alloc.allocate(src.size());
        
        try {
            mystl::uninitialized_move(src.begin(), src.end(), dest);
            
            for (size_t i = 0; i < src.size(); ++i) {
                TEST(dest[i].value == 100 + i * 100);
            }
            
            mystl::destroy(dest, dest + src.size());
            alloc.deallocate(dest, src.size());
            std::cout << "  uninitialized_move OK / uninitialized_move正常" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, src.size());
            throw;
        }
    }
    
    // 4. Test destroy
    // 4. 测试destroy
    std::cout << "\n4. Testing destroy / 测试destroy:" << std::endl;
    {
        TrackedType::reset_counts();
        const size_t count = 3;
        
        mystl::allocator<TrackedType> alloc;
        TrackedType* dest = alloc.allocate(count);
        
        try {
            for (size_t i = 0; i < count; ++i) {
                ::new (dest + i) TrackedType(static_cast<int>(i * 100));
            }
            
            int after_construct = TrackedType::constructor_count;
            
            mystl::destroy(dest, dest + count);
            
            TEST(TrackedType::destructor_count == after_construct);
            
            alloc.deallocate(dest, count);
            std::cout << "  destroy OK / destroy正常" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, count);
            throw;
        }
    }
    
    // 5. Test exception safety
    // 5. 测试异常安全性
    std::cout << "\n5. Testing Exception Safety / 测试异常安全性:" << std::endl;
    {
        std::vector<TrackedType> src;
        src.reserve(3);
        src.emplace_back(100, false);
        src.emplace_back(200, false);
        src.emplace_back(300, true);
        
        mystl::allocator<TrackedType> alloc;
        TrackedType* dest = alloc.allocate(src.size());
        
        TrackedType::reset_counts();
        
        try {
            mystl::uninitialized_copy(src.begin(), src.end(), dest);
            TEST(false && "Should have thrown exception / 应该抛出异常");
        } catch (const std::runtime_error& e) {
            std::cout << "  Caught expected exception: " << e.what() << std::endl;
            std::cout << "  捕获预期异常: " << e.what() << std::endl;
            TrackedType::print_counts();
            
            TEST(TrackedType::copy_count == 3);
            TEST(TrackedType::destructor_count == 2);
            std::cout << "  Exception safety OK / 异常安全性正常" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, src.size());
            throw;
        }
        
        alloc.deallocate(dest, src.size());
    }
    
    std::cout << "\n=========================================" << std::endl;
    std::cout << "All tests PASSED!" << std::endl;
    std::cout << "所有测试通过！" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    return 0;
}
