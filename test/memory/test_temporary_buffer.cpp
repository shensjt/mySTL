/**
 * @file test_temporary_buffer.cpp
 * @brief Comprehensive test suite for mystl::temporary_buffer implementation
 * 
 * This file contains tests for the temporary_buffer utility in mystl.
 * It verifies the correctness of temporary buffer allocation and usage.
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/temporary_buffer.hpp
 */

#include "../../include/memory/temporary_buffer.hpp"
#include <iostream>
#include <cassert>
#include <algorithm>
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
    std::cout << "Testing mystl::temporary_buffer Implementation" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // 1. Basic construction and destruction
    std::cout << "1. Testing Basic Construction and Destruction:" << std::endl;
    {
        mystl::temporary_buffer<int> buf(10);
        TEST(buf.size() >= 10);
        TEST(buf.begin() != nullptr);
        TEST(buf.end() == buf.begin() + buf.size());
        std::cout << "  Basic construction OK" << std::endl;
    }
    
    // 2. Zero size buffer
    std::cout << "\n2. Testing Zero Size Buffer:" << std::endl;
    {
        mystl::temporary_buffer<int> buf(0);
        TEST(buf.size() == 0);
        TEST(buf.begin() == nullptr);
        TEST(buf.end() == nullptr);
        std::cout << "  Zero size buffer OK" << std::endl;
    }
    
    // 3. Buffer usage as workspace
    std::cout << "\n3. Testing Buffer Usage as Workspace:" << std::endl;
    {
        const size_t N = 100;
        mystl::temporary_buffer<int> buf(N);
        
        std::fill(buf.begin(), buf.end(), 42);
        
        for (size_t i = 0; i < buf.size(); ++i) {
            TEST(buf.begin()[i] == 42);
        }
        std::cout << "  Buffer usage as workspace OK" << std::endl;
    }
    
    // 4. Buffer with different types
    std::cout << "\n4. Testing Buffer with Different Types:" << std::endl;
    {
        {
            mystl::temporary_buffer<double> buf(50);
            TEST(buf.size() >= 50);
            std::fill(buf.begin(), buf.end(), 3.14);
            TEST(buf.begin()[0] == 3.14);
        }
        
        {
            mystl::temporary_buffer<char> buf(100);
            TEST(buf.size() >= 100);
            std::fill(buf.begin(), buf.end(), 'A');
            TEST(buf.begin()[0] == 'A');
        }
        
        std::cout << "  Different types OK" << std::endl;
    }
    
    // 5. Move semantics
    std::cout << "\n5. Testing Move Semantics:" << std::endl;
    {
        mystl::temporary_buffer<int> buf1(20);
        std::fill(buf1.begin(), buf1.end(), 123);
        
        mystl::temporary_buffer<int> buf2(std::move(buf1));
        TEST(buf1.size() == 0);
        TEST(buf1.begin() == nullptr);
        TEST(buf2.size() >= 20);
        TEST(buf2.begin()[0] == 123);
        
        mystl::temporary_buffer<int> buf3(5);
        buf3 = std::move(buf2);
        TEST(buf2.size() == 0);
        TEST(buf2.begin() == nullptr);
        TEST(buf3.size() >= 20);
        TEST(buf3.begin()[0] == 123);
        
        std::cout << "  Move semantics OK" << std::endl;
    }
    
    // 6. get_temporary_buffer and return_temporary_buffer
    std::cout << "\n6. Testing get_temporary_buffer and return_temporary_buffer:" << std::endl;
    {
        auto [ptr, size] = mystl::get_temporary_buffer<int>(15);
        TEST(ptr != nullptr || size == 0);
        
        if (ptr && size > 0) {
            std::fill(ptr, ptr + size, 99);
            TEST(ptr[0] == 99);
            
            mystl::return_temporary_buffer(ptr);
            std::cout << "  get/return_temporary_buffer OK" << std::endl;
        } else {
            std::cout << "  get_temporary_buffer returned empty buffer (acceptable)" << std::endl;
        }
    }
    
    // 7. Buffer as algorithm workspace
    std::cout << "\n7. Testing Buffer as Algorithm Workspace:" << std::endl;
    {
        std::vector<int> data = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
        mystl::temporary_buffer<int> workspace(data.size());
        
        if (workspace.size() >= data.size()) {
            std::copy(data.begin(), data.end(), workspace.begin());
            std::sort(workspace.begin(), workspace.begin() + data.size());
            std::copy(workspace.begin(), workspace.begin() + data.size(), data.begin());
            TEST(std::is_sorted(data.begin(), data.end()));
            std::cout << "  Algorithm workspace OK" << std::endl;
        } else {
            std::cout << "  Workspace too small for test (acceptable)" << std::endl;
        }
    }
    
    // 8. Buffer reuse in loops
    std::cout << "\n8. Testing Buffer Reuse in Loops:" << std::endl;
    {
        for (int i = 0; i < 3; ++i) {
            mystl::temporary_buffer<int> buf(10);
            std::fill(buf.begin(), buf.end(), i);
            TEST(buf.begin()[0] == i);
        }
        std::cout << "  Buffer reuse in loops OK" << std::endl;
    }
    
    std::cout << "\n=========================================" << std::endl;
    std::cout << "All tests PASSED!" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    return 0;
}
