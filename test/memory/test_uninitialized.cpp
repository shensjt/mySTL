/**
 * @file test_uninitialized.cpp
 * @brief Comprehensive test suite for mystl uninitialized memory operations
 * 
 * This file contains tests for uninitialized memory operations in mystl.
 * It verifies the correctness of functions for constructing, copying, moving,
 * filling, and destroying objects in uninitialized memory.
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
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  Test passed: " << #expr << std::endl; \
        } \
    } while(0)

// Test class with constructor/destructor tracking
class TrackedType {
public:
    static int constructor_count;
    static int destructor_count;
    static int copy_count;
    static int move_count;
    
    int value;
    bool throw_on_copy;
    
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
            throw std::runtime_error("Copy constructor intentionally failed");
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
    }
};

int TrackedType::constructor_count = 0;
int TrackedType::destructor_count = 0;
int TrackedType::copy_count = 0;
int TrackedType::move_count = 0;

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Testing mystl Uninitialized Memory Operations" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // 1. Test uninitialized_copy
    std::cout << "1. Testing uninitialized_copy:" << std::endl;
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
            std::cout << "  uninitialized_copy OK" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, src.size());
            throw;
        }
    }
    
    // 2. Test uninitialized_fill
    std::cout << "\n2. Testing uninitialized_fill:" << std::endl;
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
            std::cout << "  uninitialized_fill OK" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, count);
            throw;
        }
    }
    
    // 3. Test uninitialized_move
    std::cout << "\n3. Testing uninitialized_move:" << std::endl;
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
            std::cout << "  uninitialized_move OK" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, src.size());
            throw;
        }
    }
    
    // 4. Test destroy
    std::cout << "\n4. Testing destroy:" << std::endl;
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
            std::cout << "  destroy OK" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, count);
            throw;
        }
    }
    
    // 5. Test exception safety
    std::cout << "\n5. Testing Exception Safety:" << std::endl;
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
            TEST(false && "Should have thrown exception");
        } catch (const std::runtime_error& e) {
            std::cout << "  Caught expected exception: " << e.what() << std::endl;
            TrackedType::print_counts();
            
            TEST(TrackedType::copy_count == 3);
            TEST(TrackedType::destructor_count == 2);
            std::cout << "  Exception safety OK" << std::endl;
        } catch (...) {
            alloc.deallocate(dest, src.size());
            throw;
        }
        
        alloc.deallocate(dest, src.size());
    }
    
    std::cout << "\n=========================================" << std::endl;
    std::cout << "All tests PASSED!" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    return 0;
}
