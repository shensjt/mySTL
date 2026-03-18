/**
 * @file test_temporary_buffer.cpp
 * @brief Comprehensive test suite for mystl::temporary_buffer implementation
 * @brief mystl::temporary_buffer实现的综合测试套件
 * 
 * This file contains tests for the temporary_buffer utility in mystl.
 * It verifies the correctness of temporary buffer allocation and usage.
 * 
 * 此文件包含对mystl中临时缓冲区工具的测试。
 * 验证临时缓冲区分配和使用的正确性。
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
            std::cerr << "[FAIL] Line " << __LINE__ << ": " << #expr << std::endl; \
            std::cerr << "[失败] 行号 " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  ✓ " << #expr << std::endl; \
        } \
    } while(0)

/**
 * @brief Main test function for temporary_buffer
 * @brief temporary_buffer的主测试函数
 * 
 * Tests various aspects of mystl::temporary_buffer implementation.
 * 测试mystl::temporary_buffer实现的各个方面。
 * 
 * @return 0 if all tests pass, 1 otherwise
 *         如果所有测试通过返回0，否则返回1
 */
int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Testing mystl::temporary_buffer Implementation" << std::endl;
    std::cout << "测试mystl::temporary_buffer实现" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // 1. Basic construction and destruction
    // 1. 基本构造和销毁
    std::cout << "1. Testing Basic Construction and Destruction / 测试基本构造和销毁:" << std::endl;
    {
        mystl::temporary_buffer<int> buf(10);
        TEST(buf.size() >= 10);
        TEST(buf.begin() != nullptr);
        TEST(buf.end() == buf.begin() + buf.size());
        std::cout << "  Basic construction OK / 基本构造正常" << std::endl;
    }
    
    // 2. Zero size buffer
    // 2. 零大小缓冲区
    std::cout << "\n2. Testing Zero Size Buffer / 测试零大小缓冲区:" << std::endl;
    {
        mystl::temporary_buffer<int> buf(0);
        TEST(buf.size() == 0);
        TEST(buf.begin() == nullptr);
        TEST(buf.end() == nullptr);
        std::cout << "  Zero size buffer OK / 零大小缓冲区正常" << std::endl;
    }
    
    // 3. Buffer usage as workspace
    // 3. 缓冲区作为工作空间使用
    std::cout << "\n3. Testing Buffer Usage as Workspace / 测试缓冲区作为工作空间使用:" << std::endl;
    {
        const size_t N = 100;
        mystl::temporary_buffer<int> buf(N);
        
        std::fill(buf.begin(), buf.end(), 42);
        
        for (size_t i = 0; i < buf.size(); ++i) {
            TEST(buf.begin()[i] == 42);
        }
        std::cout << "  Buffer usage as workspace OK / 缓冲区作为工作空间使用正常" << std::endl;
    }
    
    // 4. Buffer with different types
    // 4. 不同数据类型的缓冲区
    std::cout << "\n4. Testing Buffer with Different Types / 测试不同数据类型的缓冲区:" << std::endl;
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
        
        std::cout << "  Different types OK / 不同数据类型正常" << std::endl;
    }
    
    // 5. Move semantics
    // 5. 移动语义
    std::cout << "\n5. Testing Move Semantics / 测试移动语义:" << std::endl;
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
        
        std::cout << "  Move semantics OK / 移动语义正常" << std::endl;
    }
    
    // 6. get_temporary_buffer and return_temporary_buffer
    // 6. get_temporary_buffer和return_temporary_buffer
    std::cout << "\n6. Testing get_temporary_buffer and return_temporary_buffer / 测试get_temporary_buffer和return_temporary_buffer:" << std::endl;
    {
        auto [ptr, size] = mystl::get_temporary_buffer<int>(15);
        TEST(ptr != nullptr || size == 0);
        
        if (ptr && size > 0) {
            std::fill(ptr, ptr + size, 99);
            TEST(ptr[0] == 99);
            
            mystl::return_temporary_buffer(ptr);
            std::cout << "  get/return_temporary_buffer OK / get/return_temporary_buffer正常" << std::endl;
        } else {
            std::cout << "  get_temporary_buffer returned empty buffer (acceptable) / get_temporary_buffer返回空缓冲区（可接受）" << std::endl;
        }
    }
    
    // 7. Buffer as algorithm workspace
    // 7. 缓冲区作为算法工作空间
    std::cout << "\n7. Testing Buffer as Algorithm Workspace / 测试缓冲区作为算法工作空间:" << std::endl;
    {
        std::vector<int> data = {5, 2, 8, 1, 9, 3, 7, 4, 6, 0};
        mystl::temporary_buffer<int> workspace(data.size());
        
        if (workspace.size() >= data.size()) {
            std::copy(data.begin(), data.end(), workspace.begin());
            std::sort(workspace.begin(), workspace.begin() + data.size());
            std::copy(workspace.begin(), workspace.begin() + data.size(), data.begin());
            TEST(std::is_sorted(data.begin(), data.end()));
            std::cout << "  Algorithm workspace OK / 算法工作空间正常" << std::endl;
        } else {
            std::cout << "  Workspace too small for test (acceptable) / 工作空间太小无法测试（可接受）" << std::endl;
        }
    }
    
    // 8. Buffer reuse in loops
    // 8. 循环中重用缓冲区
    std::cout << "\n8. Testing Buffer Reuse in Loops / 测试循环中重用缓冲区:" << std::endl;
    {
        for (int i = 0; i < 3; ++i) {
            mystl::temporary_buffer<int> buf(10);
            std::fill(buf.begin(), buf.end(), i);
            TEST(buf.begin()[0] == i);
        }
        std::cout << "  Buffer reuse in loops OK / 循环中重用缓冲区正常" << std::endl;
    }
    
    std::cout << "\n=========================================" << std::endl;
    std::cout << "All tests PASSED!" << std::endl;
    std::cout << "所有测试通过！" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    return 0;
}
