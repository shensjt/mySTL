/**
 * @file test_list.cpp
 * @brief Comprehensive test suite for mystl::list implementation
 * @brief mystl::list 实现的全面测试套件
 * 
 * This file contains extensive tests for the list container in mystl.
 * It verifies the correctness of list operations including construction,
 * element access, modification, iterator functionality, and various operations.
 * 
 * 此文件包含对 mystl 中 list 容器的广泛测试。
 * 它验证 list 操作的正确性，包括构造、元素访问、修改、迭代器功能和各种操作。
 * 
 * The tests are organized into logical sections corresponding to different
 * aspects of list functionality.
 * 
 * 测试按 list 功能的不同方面组织成逻辑部分。
 * 
 * @author shensjt
 * @date 2026-03-21 / 2026年3月21日
 * 
 * @see include/mystl/list.hpp
 * @see 参考：include/mystl/list.hpp
 */

#include <iostream>
#include <cassert>
#include "../../include/mystl/list.hpp"

// Helper macro for testing / 测试辅助宏
#define TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            std::cerr << "测试失败，行号 " << __LINE__ << ": " << #expr << std::endl; \
            assert(false && #expr); \
        } \
    } while(0)

/**
 * @brief Test list constructors
 * @brief 测试 list 构造函数
 */
void test_list_constructors() {
    std::cout << "Testing list constructors... / 测试 list 构造函数..." << std::endl;
    
    // 默认构造函数 / Default constructor
    mystl::list<int> l1;
    TEST(l1.empty());
    TEST(l1.size() == 0);
    
    // 指定大小的构造函数 / Constructor with specified size
    mystl::list<int> l2(static_cast<std::size_t>(5), 42);
    TEST(l2.size() == 5);
    TEST(!l2.empty());
    
    // 检查元素值 / Check element values
    int count = 0;
    for (int val : l2) {
        TEST(val == 42);
        ++count;
    }
    TEST(count == 5);
    
    // 拷贝构造函数 / Copy constructor
    mystl::list<int> l3(l2);
    TEST(l3.size() == 5);
    count = 0;
    for (int val : l3) {
        TEST(val == 42);
        ++count;
    }
    TEST(count == 5);
    
    // 移动构造函数 / Move constructor
    mystl::list<int> l4(std::move(l3));
    TEST(l4.size() == 5);
    TEST(l3.empty());
    TEST(l3.size() == 0);
    
    // 初始化列表构造函数 / Initializer list constructor
    mystl::list<int> l5 = {1, 2, 3, 4, 5};
    TEST(l5.size() == 5);
    
    int expected = 1;
    for (int val : l5) {
        TEST(val == expected);
        ++expected;
    }
    
    std::cout << "  All constructor tests passed! / 所有构造函数测试通过！" << std::endl;
}

/**
 * @brief Test list element access
 * @brief 测试 list 元素访问
 */
void test_list_access() {
    std::cout << "Testing list element access... / 测试 list 元素访问..." << std::endl;
    
    mystl::list<int> l = {10, 20, 30, 40, 50};
    
    // front() and back()
    TEST(l.front() == 10);
    TEST(l.back() == 50);
    
    // 修改元素 / Modify elements
    l.front() = 100;
    l.back() = 500;
    TEST(l.front() == 100);
    TEST(l.back() == 500);
    
    std::cout << "  All element access tests passed! / 所有元素访问测试通过！" << std::endl;
}

/**
 * @brief Test list iterators
 * @brief 测试 list 迭代器
 */
void test_list_iterators() {
    std::cout << "Testing list iterators... / 测试 list 迭代器..." << std::endl;
    
    mystl::list<int> l = {1, 2, 3, 4, 5};
    
    // 正向遍历 / Forward traversal
    int sum = 0;
    for (auto it = l.begin(); it != l.end(); ++it) {
        sum += *it;
    }
    TEST(sum == 15);
    
    // 范围for循环 / Range-based for loop
    sum = 0;
    for (int val : l) {
        sum += val;
    }
    TEST(sum == 15);
    
    // 手动反向遍历（不使用rbegin/rend） / Manual reverse traversal (without rbegin/rend)
    sum = 0;
    auto it = l.end();
    while (it != l.begin()) {
        --it;
        sum += *it;
    }
    TEST(sum == 15);
    
    std::cout << "  All iterator tests passed! / 所有迭代器测试通过！" << std::endl;
}

/**
 * @brief Test list modifiers
 * @brief 测试 list 修改操作
 */
void test_list_modifiers() {
    std::cout << "Testing list modifiers... / 测试 list 修改操作..." << std::endl;
    
    mystl::list<int> l;
    
    // push_back
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    TEST(l.size() == 3);
    TEST(l.front() == 1);
    TEST(l.back() == 3);
    
    // push_front
    l.push_front(0);
    TEST(l.size() == 4);
    TEST(l.front() == 0);
    
    // pop_back
    l.pop_back();
    TEST(l.size() == 3);
    TEST(l.back() == 2);
    
    // pop_front
    l.pop_front();
    TEST(l.size() == 2);
    TEST(l.front() == 1);
    
    // insert
    auto it = l.begin();
    ++it; // 指向第二个元素之前 / Point before second element
    l.insert(it, 99);
    TEST(l.size() == 3);
    
    // 检查插入后的顺序 / Check order after insertion
    auto check_it = l.begin();
    TEST(*check_it == 1);
    ++check_it;
    TEST(*check_it == 99);
    ++check_it;
    TEST(*check_it == 2);
    
    // erase
    it = l.begin();
    ++it; // 指向99 / Point to 99
    it = l.erase(it);
    TEST(l.size() == 2);
    TEST(*it == 2); // erase返回下一个元素的迭代器 / erase returns iterator to next element
    
    // clear
    l.clear();
    TEST(l.empty());
    TEST(l.size() == 0);
    
    std::cout << "  All modifier tests passed! / 所有修改操作测试通过！" << std::endl;
}

/**
 * @brief Test list swap
 * @brief 测试 list 交换操作
 */
void test_list_swap() {
    std::cout << "Testing list swap... / 测试 list 交换操作..." << std::endl;
    
    mystl::list<int> l1 = {1, 2, 3};
    mystl::list<int> l2 = {4, 5, 6, 7};
    
    l1.swap(l2);
    
    TEST(l1.size() == 4);
    TEST(l2.size() == 3);
    
    int sum1 = 0;
    for (int val : l1) {
        sum1 += val;
    }
    TEST(sum1 == 22); // 4+5+6+7
    
    int sum2 = 0;
    for (int val : l2) {
        sum2 += val;
    }
    TEST(sum2 == 6); // 1+2+3
    
    std::cout << "  All swap tests passed! / 所有交换操作测试通过！" << std::endl;
}

/**
 * @brief Test list algorithms
 * @brief 测试 list 算法操作
 */
void test_list_algorithms() {
    std::cout << "Testing list algorithms... / 测试 list 算法操作..." << std::endl;
    
    // 测试reverse() / Test reverse()
    mystl::list<int> l1 = {1, 2, 3, 4, 5};
    l1.reverse();
    
    int expected = 5;
    for (int val : l1) {
        TEST(val == expected);
        --expected;
    }
    TEST(expected == 0);
    
    std::cout << "  reverse() test passed! / reverse() 测试通过！" << std::endl;
}

/**
 * @brief Test list remove operations
 * @brief 测试 list 移除操作
 */
void test_list_remove_operations() {
    std::cout << "Testing list remove operations... / 测试 list 移除操作..." << std::endl;
    
    // 测试remove() / Test remove()
    mystl::list<int> l2 = {1, 2, 3, 2, 4, 2, 5};
    l2.remove(2);
    TEST(l2.size() == 4);
    for (int val : l2) {
        TEST(val != 2);
    }
    
    // 测试remove_if() / Test remove_if()
    mystl::list<int> l3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    l3.remove_if([](int x) { return x % 2 == 0; }); // 删除偶数 / Remove even numbers
    TEST(l3.size() == 5);
    for (int val : l3) {
        TEST(val % 2 == 1); // 只保留奇数 / Only odd numbers remain
    }
    
    std::cout << "  remove() and remove_if() tests passed! / remove() 和 remove_if() 测试通过！" << std::endl;
}

/**
 * @brief Test list unique operations
 * @brief 测试 list 去重操作
 */
void test_list_unique_operations() {
    std::cout << "Testing list unique operations... / 测试 list 去重操作..." << std::endl;
    
    // 测试unique() / Test unique()
    mystl::list<int> l4 = {1, 1, 2, 2, 2, 3, 3, 4, 5, 5};
    l4.unique();
    TEST(l4.size() == 5);
    int prev = 0;
    for (int val : l4) {
        TEST(val > prev); // 确保没有连续重复 / Ensure no consecutive duplicates
        prev = val;
    }
    
    // 测试带谓词的unique() / Test unique() with predicate
    mystl::list<int> l5 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    l5.unique([](int a, int b) { return (b - a) == 1; }); // 删除连续相差1的元素 / Remove elements with consecutive difference of 1
    // 这个谓词会删除连续相差1的元素，所以应该只剩下1, 3, 5, 7, 9
    // This predicate removes elements with consecutive difference of 1, so only 1, 3, 5, 7, 9 should remain
    TEST(l5.size() == 5);
    int check_val = 1;
    for (int val : l5) {
        TEST(val == check_val);
        check_val += 2;
    }
    
    std::cout << "  unique() tests passed! / unique() 测试通过！" << std::endl;
}

/**
 * @brief Test list splice operations
 * @brief 测试 list 拼接操作
 */
void test_list_splice_operations() {
    std::cout << "Testing list splice operations... / 测试 list 拼接操作..." << std::endl;
    
    // 测试splice() - 整个链表 / Test splice() - entire list
    mystl::list<int> l6 = {1, 2, 3};
    mystl::list<int> l7 = {4, 5, 6};
    
    // 将整个l7拼接到l6的开头 / Splice entire l7 to beginning of l6
    l6.splice(l6.begin(), l7);
    TEST(l6.size() == 6);
    TEST(l7.empty());
    
    int expected6[] = {4, 5, 6, 1, 2, 3};
    int i = 0;
    for (int val : l6) {
        TEST(val == expected6[i]);
        ++i;
    }
    
    // 测试splice() - 单个节点 / Test splice() - single node
    mystl::list<int> l8 = {1, 2, 3};
    mystl::list<int> l9 = {4, 5, 6};
    auto it = l9.begin();
    ++it; // 指向5 / Point to 5
    
    l8.splice(l8.end(), l9, it);
    TEST(l8.size() == 4);
    TEST(l9.size() == 2);
    
    // 测试splice() - 范围 / Test splice() - range
    mystl::list<int> l10 = {1, 2, 3};
    mystl::list<int> l11 = {4, 5, 6, 7, 8};
    auto first = l11.begin();
    ++first; // 指向5 / Point to 5
    auto last = first;
    ++last; ++last; // 指向7 / Point to 7
    
    l10.splice(l10.end(), l11, first, last);
    TEST(l10.size() == 5);
    TEST(l11.size() == 3);
    
    std::cout << "  splice() tests passed! / splice() 测试通过！" << std::endl;
}

/**
 * @brief Test list merge operations
 * @brief 测试 list 合并操作
 */
void test_list_merge_operations() {
    std::cout << "Testing list merge operations... / 测试 list 合并操作..." << std::endl;
    
    // 测试merge() - 合并两个已排序的链表 / Test merge() - merge two sorted lists
    mystl::list<int> l12 = {1, 3, 5};
    mystl::list<int> l13 = {2, 4, 6};
    
    l12.merge(l13);
    TEST(l12.size() == 6);
    TEST(l13.empty());
    
    int prev2 = 0;
    for (int val : l12) {
        TEST(val > prev2);
        prev2 = val;
    }
    
    // 测试merge() - 空链表 / Test merge() - empty list
    mystl::list<int> l14 = {1, 2, 3};
    mystl::list<int> l15;
    l14.merge(l15);
    TEST(l14.size() == 3);
    TEST(l15.empty());
    
    // 测试merge() - 合并到空链表 / Test merge() - merge into empty list
    mystl::list<int> l16;
    mystl::list<int> l17 = {1, 2, 3};
    l16.merge(l17);
    TEST(l16.size() == 3);
    TEST(l17.empty());
    
    std::cout << "  merge() tests passed! / merge() 测试通过！" << std::endl;
}

/**
 * @brief Test list sort operations
 * @brief 测试 list 排序操作
 */
void test_list_sort_operations() {
    std::cout << "Testing list sort operations... / 测试 list 排序操作..." << std::endl;
    
    // 测试sort() - 升序排序 / Test sort() - ascending order
    mystl::list<int> l14 = {5, 3, 1, 4, 2};
    l14.sort();
    
    int prev3 = 0;
    for (int val : l14) {
        TEST(val > prev3);
        prev3 = val;
    }
    
    // 测试sort() - 降序排序 / Test sort() - descending order
    mystl::list<int> l15 = {1, 2, 3, 4, 5};
    l15.sort([](int a, int b) { return a > b; }); // 降序排序 / Descending order
    
    int prev4 = 100; // 一个大数 / A large number
    for (int val : l15) {
        TEST(val < prev4);
        prev4 = val;
    }
    
    // 测试sort() - 空链表 / Test sort() - empty list
    mystl::list<int> l16;
    l16.sort();
    TEST(l16.empty());
    
    // 测试sort() - 单个元素 / Test sort() - single element
    mystl::list<int> l17 = {42};
    l17.sort();
    TEST(l17.size() == 1);
    TEST(l17.front() == 42);
    
    std::cout << "  sort() tests passed! / sort() 测试通过！" << std::endl;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 * 
 * @return int 0 if all tests pass, 1 if any test fails
 * @return int 如果所有测试通过返回 0，如果有任何测试失败返回 1
 */
int main() {
    std::cout << "=== Starting list tests === / === 开始 list 测试 ===" << std::endl;
    std::cout << std::endl;
    
    try {
        // 基本功能测试 / Basic functionality tests
        test_list_constructors();
        test_list_access();
        test_list_iterators();
        test_list_modifiers();
        test_list_swap();
        
        // 算法操作测试 / Algorithm operation tests
        test_list_algorithms();
        test_list_remove_operations();
        test_list_unique_operations();
        test_list_splice_operations();
        test_list_merge_operations();
        test_list_sort_operations();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught during testing: " << e.what() << std::endl;
        std::cerr << "测试过程中捕获异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught during testing" << std::endl;
        std::cerr << "测试过程中捕获未知异常" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== All list tests passed! / 所有 list 测试通过！ ===" << std::endl;
    return 0;
}
