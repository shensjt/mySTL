/**
 * @file test_rb_tree.cpp
 * @brief Simplified test suite for mystl::rb_tree implementation
 * @brief mystl::rb_tree 实现的简化测试套件
 * 
 * This file contains essential tests for the red-black tree container in mystl.
 * It verifies the correctness of rb_tree operations including construction,
 * element access, modification, and iterator functionality.
 * 
 * 此文件包含对 mystl 中红黑树容器的基本测试。
 * 它验证红黑树操作的正确性，包括构造、元素访问、修改和迭代器功能。
 * 
 * @author shensjt
 * @date 2026-03-30
 * 
 * @see include/mystl/rb_tree.hpp
 * @see include/mystl/detail/rb_tree_node.hpp
 */

#include "../../include/mystl/rb_tree.hpp"
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
 * @brief Test basic operations of rb_tree
 * @brief 测试rb_tree的基本操作
 * 
 * This function tests basic operations including construction, insertion,
 * search, deletion, and iterator functionality.
 * 
 * 此函数测试基本操作，包括构造、插入、搜索、删除和迭代器功能。
 */
void test_rb_tree_basic() {
    std::cout << "Testing rb_tree basic operations... / 测试 rb_tree 基本操作..." << std::endl;
    
    // 默认构造函数
    mystl::rb_tree<int, int> tree;
    TEST(tree.empty());
    TEST(tree.size() == 0);
    
    // 插入元素
    auto result = tree.insert(42);
    TEST(result.second);
    TEST(*result.first == 42);
    TEST(tree.size() == 1);
    
    // 查找元素
    auto it = tree.find(42);
    TEST(it != tree.end());
    TEST(*it == 42);
    
    // 插入重复元素（不允许重复）
    result = tree.insert(42);
    TEST(!result.second);
    TEST(tree.size() == 1);
    
    // 插入更多元素
    tree.insert(10);
    tree.insert(30);
    tree.insert(20);
    TEST(tree.size() == 4);
    
    // 计数
    TEST(tree.count(10) == 1);
    TEST(tree.count(100) == 0);
    
    // 边界操作
    auto lb = tree.lower_bound(15);
    TEST(lb != tree.end());
    TEST(*lb == 20);
    
    auto ub = tree.upper_bound(15);
    TEST(ub != tree.end());
    TEST(*ub == 20);
    
    // 删除元素
    size_t erased = tree.erase(10);
    TEST(erased == 1);
    TEST(tree.size() == 3);
    TEST(tree.find(10) == tree.end());
    
    // 迭代器
    int sum = 0;
    for (auto val : tree) {
        sum += val;
    }
    TEST(sum == 42 + 20 + 30);
    
    std::cout << "rb_tree basic tests passed! / rb_tree 基本测试通过！" << std::endl;
}

/**
 * @brief Test copy and move operations of rb_tree
 * @brief 测试rb_tree的拷贝和移动操作
 * 
 * This function tests copy construction, move construction, copy assignment,
 * and move assignment operations.
 * 
 * 此函数测试拷贝构造、移动构造、拷贝赋值和移动赋值操作。
 */
void test_rb_tree_copy_move() {
    std::cout << "Testing rb_tree copy and move operations... / 测试 rb_tree 拷贝和移动操作..." << std::endl;
    
    mystl::rb_tree<int, int> tree1;
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    
    // 拷贝构造
    mystl::rb_tree<int, int> tree2(tree1);
    TEST(tree1.size() == 3);
    TEST(tree2.size() == 3);
    
    // 移动构造
    mystl::rb_tree<int, int> tree3(std::move(tree1));
    TEST(tree1.empty());
    TEST(tree3.size() == 3);
    
    // 拷贝赋值
    mystl::rb_tree<int, int> tree4;
    tree4 = tree2;
    TEST(tree4.size() == 3);
    
    // 移动赋值
    mystl::rb_tree<int, int> tree5;
    tree5 = std::move(tree2);
    TEST(tree2.empty());
    TEST(tree5.size() == 3);
    
    std::cout << "rb_tree copy/move tests passed! / rb_tree 拷贝/移动测试通过！" << std::endl;
}

/**
 * @brief Main function for rb_tree tests
 * @brief rb_tree测试的主函数
 * 
 * This is the entry point for the rb_tree test suite. It runs all test
 * functions and reports the results.
 * 
 * 这是rb_tree测试套件的入口点。它运行所有测试函数并报告结果。
 * 
 * @return int 0 if all tests pass, 1 otherwise
 * @return int 如果所有测试通过则返回0，否则返回1
 */
int main() {
    std::cout << "=== Testing mystl rb_tree === / === 测试 mystl rb_tree ===" << std::endl;
    
    try {
        test_rb_tree_basic();
        test_rb_tree_copy_move();
        
        std::cout << "\n=== All rb_tree tests passed successfully! / 所有 rb_tree 测试成功通过！ ===" << std::endl;
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
