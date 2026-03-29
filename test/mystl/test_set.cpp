/**
 * @file test_set.cpp
 * @brief Simplified test suite for mystl::set implementation
 * @brief mystl::set 实现的简化测试套件
 * 
 * This file contains essential tests for the set container in mystl.
 * It verifies the correctness of set operations including construction,
 * element access, modification, iterator functionality, and set operations.
 * 
 * 此文件包含对 mystl 中集合容器的基本测试。
 * 它验证集合操作的正确性，包括构造、元素访问、修改、迭代器功能和集合操作。
 * 
 * @author shensjt
 * @date 2026-03-30
 * 
 * @see include/mystl/set.hpp
 * @see include/mystl/rb_tree.hpp
 */

#include "../../include/mystl/set.hpp"
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
 * @brief Test basic operations of set
 * @brief 测试set的基本操作
 * 
 * This function tests basic operations including construction, insertion,
 * search, deletion, and iterator functionality.
 * 
 * 此函数测试基本操作，包括构造、插入、搜索、删除和迭代器功能。
 */
void test_set_basic() {
    std::cout << "Testing set basic operations... / 测试 set 基本操作..." << std::endl;
    
    // 默认构造函数
    mystl::set<int> s;
    TEST(s.empty());
    TEST(s.size() == 0);
    
    // 插入元素
    auto result = s.insert(42);
    TEST(result.second);
    TEST(*result.first == 42);
    TEST(s.size() == 1);
    
    // 查找元素
    auto it = s.find(42);
    TEST(it != s.end());
    TEST(*it == 42);
    
    // 插入重复元素
    result = s.insert(42);
    TEST(!result.second);
    TEST(s.size() == 1);
    
    // 插入更多元素
    s.insert(10);
    s.insert(30);
    s.insert(20);
    TEST(s.size() == 4);
    
    // 计数
    TEST(s.count(10) == 1);
    TEST(s.count(100) == 0);
    
    // 边界操作
    auto lb = s.lower_bound(15);
    TEST(lb != s.end());
    TEST(*lb == 20);
    
    auto ub = s.upper_bound(15);
    TEST(ub != s.end());
    TEST(*ub == 20);
    
    // 删除元素
    size_t erased = s.erase(10);
    TEST(erased == 1);
    TEST(s.size() == 3);
    TEST(s.find(10) == s.end());
    
    // 迭代器
    int sum = 0;
    for (auto val : s) {
        sum += val;
    }
    TEST(sum == 42 + 20 + 30);
    
    std::cout << "set basic tests passed! / set 基本测试通过！" << std::endl;
}

/**
 * @brief Test copy and move operations of set
 * @brief 测试set的拷贝和移动操作
 * 
 * This function tests copy construction, move construction, copy assignment,
 * and move assignment operations.
 * 
 * 此函数测试拷贝构造、移动构造、拷贝赋值和移动赋值操作。
 */
void test_set_copy_move() {
    std::cout << "Testing set copy and move operations... / 测试 set 拷贝和移动操作..." << std::endl;
    
    mystl::set<int> s1;
    s1.insert(1);
    s1.insert(2);
    s1.insert(3);
    
    // 拷贝构造
    mystl::set<int> s2(s1);
    TEST(s1.size() == 3);
    TEST(s2.size() == 3);
    
    // 移动构造
    mystl::set<int> s3(std::move(s1));
    TEST(s1.empty());
    TEST(s3.size() == 3);
    
    // 拷贝赋值
    mystl::set<int> s4;
    s4 = s2;
    TEST(s4.size() == 3);
    
    // 移动赋值
    mystl::set<int> s5;
    s5 = std::move(s2);
    TEST(s2.empty());
    TEST(s5.size() == 3);
    
    std::cout << "set copy/move tests passed! / set 拷贝/移动测试通过！" << std::endl;
}

/**
 * @brief Test comparison operations of set
 * @brief 测试set的比较操作
 * 
 * This function tests equality, inequality, and less-than comparison
 * operations for set containers.
 * 
 * 此函数测试set容器的相等、不相等和小于比较操作。
 */
void test_set_comparisons() {
    std::cout << "Testing set comparison operations... / 测试 set 比较操作..." << std::endl;
    
    mystl::set<int> s1;
    s1.insert(1);
    s1.insert(2);
    s1.insert(3);
    
    mystl::set<int> s2;
    s2.insert(1);
    s2.insert(2);
    s2.insert(3);
    
    mystl::set<int> s3;
    s3.insert(1);
    s3.insert(2);
    
    // 相等比较
    TEST(s1 == s2);
    TEST(!(s1 == s3));
    
    // 不等比较
    TEST(!(s1 != s2));
    TEST(s1 != s3);
    
    // 小于比较
    TEST(s3 < s1);
    TEST(!(s1 < s3));
    
    std::cout << "set comparison tests passed! / set 比较测试通过！" << std::endl;
}

/**
 * @brief Main function for set tests
 * @brief set测试的主函数
 * 
 * This is the entry point for the set test suite. It runs all test
 * functions and reports the results.
 * 
 * 这是set测试套件的入口点。它运行所有测试函数并报告结果。
 * 
 * @return int 0 if all tests pass, 1 otherwise
 * @return int 如果所有测试通过则返回0，否则返回1
 */
int main() {
    std::cout << "=== Testing mystl set === / === 测试 mystl set ===" << std::endl;
    
    try {
        test_set_basic();
        test_set_copy_move();
        test_set_comparisons();
        
        std::cout << "\n=== All set tests passed successfully! / 所有 set 测试成功通过！ ===" << std::endl;
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
