/**
 * @file test_multiset.cpp
 * @brief Simplified test suite for mystl::multiset implementation
 * @brief mystl::multiset 实现的简化测试套件
 * 
 * This file contains essential tests for the multiset container in mystl.
 * It verifies the correctness of multiset operations including construction,
 * element access, modification, iterator functionality, and multiset-specific operations.
 * 
 * 此文件包含对 mystl 中多重集合容器的基本测试。
 * 它验证多重集合操作的正确性，包括构造、元素访问、修改、迭代器功能和多重集合特定操作。
 * 
 * @author shensjt
 * @date 2026-03-30
 * 
 * @see include/mystl/multiset.hpp
 * @see include/mystl/rb_tree.hpp
 */

#include "../../include/mystl/multiset.hpp"
#include "../../include/mystl/vector.hpp"
#include <iostream>
#include <cassert>

#define TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            assert(false && #expr); \
        } \
    } while(0)

/**
 * @brief Test basic operations of multiset
 * @brief 测试multiset的基本操作
 * 
 * This function tests basic operations including construction, insertion,
 * search, deletion, and iterator functionality for multiset.
 * 
 * 此函数测试multiset的基本操作，包括构造、插入、搜索、删除和迭代器功能。
 */
void test_multiset_basic() {
    std::cout << "Testing multiset basic operations... / 测试 multiset 基本操作..." << std::endl;
    
    // 默认构造函数
    mystl::multiset<int> ms;
    TEST(ms.empty());
    TEST(ms.size() == 0);
    
    // 插入元素
    auto it = ms.insert(42);
    TEST(*it == 42);
    TEST(ms.size() == 1);
    
    // 插入重复元素（允许重复）
    it = ms.insert(42);
    TEST(*it == 42);
    TEST(ms.size() == 2);  // 允许重复
    
    // 查找元素
    auto found = ms.find(42);
    TEST(found != ms.end());
    TEST(*found == 42);
    
    // 插入更多元素
    ms.insert(10);
    ms.insert(30);
    ms.insert(20);
    ms.insert(10);  // 再次插入10
    TEST(ms.size() == 6);  // 总共6个元素
    
    // 计数（多重集合可以计数重复元素）
    TEST(ms.count(42) == 2);
    TEST(ms.count(10) == 2);
    TEST(ms.count(30) == 1);
    TEST(ms.count(100) == 0);
    
    // 边界操作
    auto lb = ms.lower_bound(15);
    TEST(lb != ms.end());
    TEST(*lb == 20);
    
    auto ub = ms.upper_bound(15);
    TEST(ub != ms.end());
    TEST(*ub == 20);
    
    // 删除元素（删除所有等于该值的元素）
    size_t erased = ms.erase(10);
    TEST(erased == 2);  // 删除了2个10
    TEST(ms.size() == 4);
    TEST(ms.find(10) == ms.end());
    
    // 迭代器
    int sum = 0;
    for (auto val : ms) {
        sum += val;
    }
    // 剩余元素：42, 42, 20, 30
    TEST(sum == 42 + 42 + 20 + 30);
    
    std::cout << "multiset basic tests passed! / multiset 基本测试通过！" << std::endl;
}

/**
 * @brief Test copy and move operations of multiset
 * @brief 测试multiset的拷贝和移动操作
 * 
 * This function tests copy construction, move construction, copy assignment,
 * and move assignment operations for multiset.
 * 
 * 此函数测试multiset的拷贝构造、移动构造、拷贝赋值和移动赋值操作。
 */
void test_multiset_copy_move() {
    std::cout << "Testing multiset copy and move operations... / 测试 multiset 拷贝和移动操作..." << std::endl;
    
    mystl::multiset<int> ms1;
    ms1.insert(1);
    ms1.insert(1);  // 重复元素
    ms1.insert(2);
    ms1.insert(3);
    
    // 拷贝构造
    mystl::multiset<int> ms2(ms1);
    TEST(ms1.size() == 4);
    TEST(ms2.size() == 4);
    TEST(ms1.count(1) == 2);
    TEST(ms2.count(1) == 2);
    
    // 移动构造
    mystl::multiset<int> ms3(std::move(ms1));
    TEST(ms1.empty());
    TEST(ms3.size() == 4);
    TEST(ms3.count(1) == 2);
    
    // 拷贝赋值
    mystl::multiset<int> ms4;
    ms4 = ms2;
    TEST(ms4.size() == 4);
    TEST(ms4.count(1) == 2);
    
    // 移动赋值
    mystl::multiset<int> ms5;
    ms5 = std::move(ms2);
    TEST(ms2.empty());
    TEST(ms5.size() == 4);
    TEST(ms5.count(1) == 2);
    
    std::cout << "multiset copy/move tests passed! / multiset 拷贝/移动测试通过！" << std::endl;
}

/**
 * @brief Test equal_range operation of multiset
 * @brief 测试multiset的equal_range操作
 * 
 * This function tests the equal_range operation which returns a range
 * containing all elements with a given key in multiset.
 * 
 * 此函数测试equal_range操作，它返回包含multiset中所有具有给定键的元素的区间。
 */
void test_multiset_equal_range() {
    std::cout << "Testing multiset equal_range operation... / 测试 multiset equal_range 操作..." << std::endl;
    
    mystl::multiset<int> ms;
    
    // 插入重复元素
    ms.insert(5);
    ms.insert(5);
    ms.insert(5);
    ms.insert(10);
    ms.insert(15);
    
    // 测试equal_range
    auto range = ms.equal_range(5);
    size_t count = 0;
    for (auto it = range.first; it != range.second; ++it) {
        TEST(*it == 5);
        ++count;
    }
    TEST(count == 3);  // 应该有3个5
    
    // 测试不存在的元素
    range = ms.equal_range(7);
    TEST(range.first == range.second);  // 范围应该为空
    TEST(range.first != ms.end());  // 但迭代器应该指向插入位置
    
    std::cout << "multiset equal_range tests passed! / multiset equal_range 测试通过！" << std::endl;
}

/**
 * @brief Main function for multiset tests
 * @brief multiset测试的主函数
 * 
 * This is the entry point for the multiset test suite. It runs all test
 * functions and reports the results.
 * 
 * 这是multiset测试套件的入口点。它运行所有测试函数并报告结果。
 * 
 * @return int 0 if all tests pass, 1 otherwise
 * @return int 如果所有测试通过则返回0，否则返回1
 */
int main() {
    std::cout << "=== Testing mystl multiset === / === 测试 mystl multiset ===" << std::endl;
    
    try {
        test_multiset_basic();
        test_multiset_copy_move();
        test_multiset_equal_range();
        
        std::cout << "\n=== All multiset tests passed successfully! / 所有 multiset 测试成功通过！ ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        std::cerr << "测试失败，异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        std::cerr << "测试失败，未知异常" << std::endl;
        return 1;
    }
}
