/**
 * @file test_insert_iterators.cpp
 * @brief Test suite for insert iterator adaptors
 * @brief 插入迭代器适配器的测试套件
 * 
 * This file contains comprehensive tests for the insert iterator adaptors:
 * - back_insert_iterator: inserts elements at the end of a container
 * - front_insert_iterator: inserts elements at the beginning of a container
 * - insert_iterator: inserts elements at a specific position in a container
 * 
 * 本文件包含插入迭代器适配器的全面测试：
 * - back_insert_iterator：在容器末尾插入元素
 * - front_insert_iterator：在容器开头插入元素
 * - insert_iterator：在容器的特定位置插入元素
 * 
 * @author shensjt
 * @date 2026-03-19
 * 
 * @see back_insert_iterator.hpp
 * @see front_insert_iterator.hpp
 * @see insert_iterator.hpp
 */

#include "../../include/iterator/back_insert_iterator.hpp"
#include "../../include/iterator/front_insert_iterator.hpp"
#include "../../include/iterator/insert_iterator.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <algorithm>

int test_count = 0;      ///< Global test counter / 全局测试计数器
int fail_count = 0;      ///< Global failure counter / 全局失败计数器

/**
 * @brief Test assertion macro
 * @brief 测试断言宏
 * 
 * @param expr Expression to evaluate
 * @param expr 要评估的表达式
 * 
 * @details Checks if the expression is true. If false, logs the failure
 * with line number and expression. Increments test_count and fail_count accordingly.
 * 
 * @details 检查表达式是否为真。如果为假，记录失败信息（包括行号和表达式）。
 * 相应地递增 test_count 和 fail_count。
 */
#define CHECK(expr) \
    do { \
        ++test_count; \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            ++fail_count; \
        } else { \
            std::cout << "Test passed: " << #expr << std::endl; \
        } \
    } while(0)

/**
 * @brief Test back_insert_iterator functionality
 * @brief 测试 back_insert_iterator 功能
 * 
 * @details Tests the back_insert_iterator with various container types:
 * - std::vector: random access container
 * - std::list: bidirectional container
 * - std::deque: random access container with different memory layout
 * 
 * @details 使用各种容器类型测试 back_insert_iterator：
 * - std::vector：随机访问容器
 * - std::list：双向容器
 * - std::deque：具有不同内存布局的随机访问容器
 * 
 * Tests include:
 * - Basic insertion using operator=, operator*, operator++
 * - Usage with std::copy algorithm
 * - Multiple container types
 * 
 * 测试包括：
 * - 使用 operator=、operator*、operator++ 的基本插入
 * - 与 std::copy 算法的使用
 * - 多种容器类型
 */
void test_back_insert_iterator() {
    std::cout << "\n=== Testing back_insert_iterator ===" << std::endl;
    
    // Test with std::vector
    {
        std::vector<int> vec;
        mystl::back_insert_iterator<std::vector<int>> back_it = mystl::back_inserter(vec);
        
        *back_it = 1;
        ++back_it;
        *back_it = 2;
        back_it++ = 3;
        
        CHECK(vec.size() == 3);
        CHECK(vec[0] == 1);
        CHECK(vec[1] == 2);
        CHECK(vec[2] == 3);
        
        // Test with std::copy
        std::vector<int> source = {4, 5, 6};
        std::copy(source.begin(), source.end(), mystl::back_inserter(vec));
        
        CHECK(vec.size() == 6);
        CHECK(vec[3] == 4);
        CHECK(vec[4] == 5);
        CHECK(vec[5] == 6);
    }
    
    // Test with std::list
    {
        std::list<int> lst;
        auto back_it = mystl::back_inserter(lst);
        
        *back_it = 10;
        *back_it = 20;
        *back_it = 30;
        
        CHECK(lst.size() == 3);
        auto it = lst.begin();
        CHECK(*it == 10); ++it;
        CHECK(*it == 20); ++it;
        CHECK(*it == 30);
    }
    
    // Test with std::deque
    {
        std::deque<int> dq;
        auto back_it = mystl::back_inserter(dq);
        
        for (int i = 0; i < 5; ++i) {
            *back_it = i * 10;
        }
        
        CHECK(dq.size() == 5);
        for (int i = 0; i < 5; ++i) {
            CHECK(dq[i] == i * 10);
        }
    }
}

/**
 * @brief Test front_insert_iterator functionality
 * @brief 测试 front_insert_iterator 功能
 * 
 * @details Tests the front_insert_iterator with containers that support push_front():
 * - std::list: bidirectional container
 * - std::deque: random access container
 * 
 * @details 使用支持 push_front() 的容器测试 front_insert_iterator：
 * - std::list：双向容器
 * - std::deque：随机访问容器
 * 
 * Tests include:
 * - Basic insertion at the front of containers
 * - Reverse order of inserted elements (last inserted appears first)
 * - Usage with std::copy algorithm
 * 
 * 测试包括：
 * - 在容器前部的基本插入
 * - 插入元素的相反顺序（最后插入的元素出现在最前面）
 * - 与 std::copy 算法的使用
 */
void test_front_insert_iterator() {
    std::cout << "\n=== Testing front_insert_iterator ===" << std::endl;
    
    // Test with std::list
    {
        std::list<int> lst;
        mystl::front_insert_iterator<std::list<int>> front_it = mystl::front_inserter(lst);
        
        *front_it = 1;
        ++front_it;
        *front_it = 2;
        front_it++ = 3;
        
        CHECK(lst.size() == 3);
        auto it = lst.begin();
        CHECK(*it == 3); ++it;  // Last inserted is at front
        CHECK(*it == 2); ++it;
        CHECK(*it == 1);
        
        // Test with std::copy
        std::list<int> source = {4, 5, 6};
        std::copy(source.begin(), source.end(), mystl::front_inserter(lst));
        
        CHECK(lst.size() == 6);
        it = lst.begin();
        CHECK(*it == 6); ++it;
        CHECK(*it == 5); ++it;
        CHECK(*it == 4); ++it;
        CHECK(*it == 3); ++it;
        CHECK(*it == 2); ++it;
        CHECK(*it == 1);
    }
    
    // Test with std::deque
    {
        std::deque<int> dq;
        auto front_it = mystl::front_inserter(dq);
        
        *front_it = 100;
        *front_it = 200;
        *front_it = 300;
        
        CHECK(dq.size() == 3);
        CHECK(dq[0] == 300);  // Last inserted is at front
        CHECK(dq[1] == 200);
        CHECK(dq[2] == 100);
    }
}

/**
 * @brief Test insert_iterator functionality
 * @brief 测试 insert_iterator 功能
 * 
 * @details Tests the insert_iterator with various container types and insertion positions:
 * - std::vector: insertion at middle and beginning
 * - std::list: insertion at middle position
 * - std::deque: insertion at middle position
 * 
 * @details 使用各种容器类型和插入位置测试 insert_iterator：
 * - std::vector：在中间和开头插入
 * - std::list：在中间位置插入
 * - std::deque：在中间位置插入
 * 
 * Tests include:
 * - Insertion at specific positions
 * - Automatic position advancement after each insertion
 * - Usage with std::copy algorithm
 * - Multiple container types
 * 
 * 测试包括：
 * - 在特定位置插入
 * - 每次插入后自动位置前进
 * - 与 std::copy 算法的使用
 * - 多种容器类型
 */
void test_insert_iterator() {
    std::cout << "\n=== Testing insert_iterator ===" << std::endl;
    
    // Test with std::vector
    {
        std::vector<int> vec = {1, 2, 3, 7, 8, 9};
        auto it = vec.begin() + 3;  // Position before 7
        
        mystl::insert_iterator<std::vector<int>> insert_it = mystl::inserter(vec, it);
        
        *insert_it = 4;
        ++insert_it;
        *insert_it = 5;
        insert_it++ = 6;
        
        CHECK(vec.size() == 9);
        CHECK(vec[0] == 1);
        CHECK(vec[1] == 2);
        CHECK(vec[2] == 3);
        CHECK(vec[3] == 4);
        CHECK(vec[4] == 5);
        CHECK(vec[5] == 6);
        CHECK(vec[6] == 7);
        CHECK(vec[7] == 8);
        CHECK(vec[8] == 9);
        
        // Test with std::copy at beginning
        std::vector<int> source = {-2, -1, 0};
        std::copy(source.begin(), source.end(), mystl::inserter(vec, vec.begin()));
        
        CHECK(vec.size() == 12);
        CHECK(vec[0] == -2);
        CHECK(vec[1] == -1);
        CHECK(vec[2] == 0);
    }
    
    // Test with std::list
    {
        std::list<int> lst = {10, 20, 30};
        auto pos = lst.begin();
        ++pos;  // Position before 20
        
        auto insert_it = mystl::inserter(lst, pos);
        *insert_it = 15;
        *insert_it = 16;
        *insert_it = 17;
        
        CHECK(lst.size() == 6);
        auto it = lst.begin();
        CHECK(*it == 10); ++it;
        CHECK(*it == 15); ++it;
        CHECK(*it == 16); ++it;
        CHECK(*it == 17); ++it;
        CHECK(*it == 20); ++it;
        CHECK(*it == 30);
    }
    
    // Test with std::deque
    {
        std::deque<int> dq = {100, 200, 300};
        auto pos = dq.begin() + 1;  // Position before 200
        
        auto insert_it = mystl::inserter(dq, pos);
        *insert_it = 150;
        *insert_it = 175;
        
        CHECK(dq.size() == 5);
        CHECK(dq[0] == 100);
        CHECK(dq[1] == 150);
        CHECK(dq[2] == 175);
        CHECK(dq[3] == 200);
        CHECK(dq[4] == 300);
    }
}

/**
 * @brief Test move semantics support in insert iterators
 * @brief 测试插入迭代器中的移动语义支持
 * 
 * @details Tests that insert iterators properly support move semantics
 * for move-only or expensive-to-copy types. Verifies that:
 * - Values are moved rather than copied when possible
 * - Source objects are left in valid but unspecified state after move
 * 
 * @details 测试插入迭代器是否正确支持仅移动或复制成本高的类型的移动语义。
 * 验证：
 * - 在可能时值被移动而不是复制
 * - 移动后源对象处于有效但未指定的状态
 */
void test_move_semantics() {
    std::cout << "\n=== Testing Move Semantics ===" << std::endl;
    
    // Test move assignment with back_insert_iterator
    {
        std::vector<std::string> vec;
        auto back_it = mystl::back_inserter(vec);
        
        std::string str1 = "Hello";
        std::string str2 = "World";
        
        *back_it = std::move(str1);
        *back_it = std::move(str2);
        
        CHECK(vec.size() == 2);
        CHECK(vec[0] == "Hello");
        CHECK(vec[1] == "World");
        CHECK(str1.empty());  // Should be moved from
        CHECK(str2.empty());  // Should be moved from
    }
    
    // Test move assignment with insert_iterator
    {
        std::vector<std::string> vec = {"Start", "End"};
        auto insert_it = mystl::inserter(vec, vec.begin() + 1);
        
        std::string middle = "Middle";
        *insert_it = std::move(middle);
        
        CHECK(vec.size() == 3);
        CHECK(vec[0] == "Start");
        CHECK(vec[1] == "Middle");
        CHECK(vec[2] == "End");
        CHECK(middle.empty());  // Should be moved from
    }
}

/**
 * @brief Main test runner
 * @brief 主测试运行器
 * 
 * @return 0 if all tests pass, 1 if any test fails
 * @return 如果所有测试通过则返回 0，如果有任何测试失败则返回 1
 * 
 * @details Runs all test suites for insert iterators and reports statistics.
 * Catches and reports any exceptions that occur during testing.
 * 
 * @details 运行插入迭代器的所有测试套件并报告统计信息。
 * 捕获并报告测试期间发生的任何异常。
 */
int main() {
    std::cout << "Starting insert_iterator tests..." << std::endl;
    std::cout << "================================" << std::endl;
    
    test_count = 0;
    fail_count = 0;
    
    try {
        test_back_insert_iterator();
        test_front_insert_iterator();
        test_insert_iterator();
        test_move_semantics();
        
        std::cout << "\n================================" << std::endl;
        std::cout << "Test statistics:" << std::endl;
        std::cout << "Total tests: " << test_count << std::endl;
        std::cout << "Failures: " << fail_count << std::endl;
        std::cout << "Passes: " << (test_count - fail_count) << std::endl;
        
        if (fail_count == 0) {
            std::cout << "All insert_iterator tests passed!" << std::endl;
            return 0;
        } else {
            std::cout << fail_count << " test(s) failed!" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Test exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown test exception" << std::endl;
        return 1;
    }
}
