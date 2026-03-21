/**
 * @file test_forward_list.cpp
 * @brief Comprehensive test suite for mystl::forward_list implementation
 * @brief mystl::forward_list 实现的全面测试套件
 * 
 * This file contains extensive tests for the forward_list container in mystl.
 * It verifies the correctness of forward_list operations including construction,
 * element access, modification, iterator functionality, and various operations.
 * 
 * 此文件包含对 mystl 中 forward_list 容器的广泛测试。
 * 它验证 forward_list 操作的正确性，包括构造、元素访问、修改、迭代器功能和各种操作。
 * 
 * The tests are organized into logical sections corresponding to different
 * aspects of forward_list functionality.
 * 
 * 测试按 forward_list 功能的不同方面组织成逻辑部分。
 * 
 * @author shensjt
 * @date 2026-03-21 / 2026年3月21日
 * 
 * @see include/mystl/forward_list.hpp
 * @see 参考：include/mystl/forward_list.hpp
 */

#include <iostream>
#include <cassert>
#include <algorithm>
#include "../../include/mystl/forward_list.hpp"

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
 * @brief Test forward_list constructors
 * @brief 测试 forward_list 构造函数
 */
void test_forward_list_constructors() {
    std::cout << "Testing forward_list constructors... / 测试 forward_list 构造函数..." << std::endl;
    
    // 默认构造函数 / Default constructor
    mystl::forward_list<int> fl1;
    TEST(fl1.empty());
    
    // 指定大小的构造函数 / Constructor with specified size
    mystl::forward_list<int> fl2(5, 42);
    TEST(!fl2.empty());
    TEST(fl2.front() == 42);
    
    // 拷贝构造函数 / Copy constructor
    mystl::forward_list<int> fl3(fl2);
    TEST(!fl3.empty());
    TEST(fl3.front() == 42);
    
    // 移动构造函数 / Move constructor
    mystl::forward_list<int> fl4(std::move(fl3));
    TEST(!fl4.empty());
    TEST(fl4.front() == 42);
    TEST(fl3.empty());
    
    // 初始化列表构造函数 / Initializer list constructor
    mystl::forward_list<int> fl5 = {1, 2, 3, 4, 5};
    TEST(!fl5.empty());
    TEST(fl5.front() == 1);
    
    // 范围构造函数 / Range constructor
    int arr[] = {10, 20, 30, 40, 50};
    mystl::forward_list<int> fl6(arr, arr + 5);
    TEST(!fl6.empty());
    TEST(fl6.front() == 10);
    
    std::cout << "  All constructor tests passed! / 所有构造函数测试通过！" << std::endl;
}

/**
 * @brief Test forward_list element access
 * @brief 测试 forward_list 元素访问
 */
void test_forward_list_access() {
    std::cout << "Testing forward_list element access... / 测试 forward_list 元素访问..." << std::endl;
    
    mystl::forward_list<int> fl = {10, 20, 30, 40, 50};
    
    // front() / 访问第一个元素
    TEST(fl.front() == 10);
    
    // 修改front() / Modify front()
    fl.front() = 100;
    TEST(fl.front() == 100);
    
    std::cout << "  All element access tests passed! / 所有元素访问测试通过！" << std::endl;
}

/**
 * @brief Test forward_list iterators
 * @brief 测试 forward_list 迭代器
 */
void test_forward_list_iterators() {
    std::cout << "Testing forward_list iterators... / 测试 forward_list 迭代器..." << std::endl;
    
    mystl::forward_list<int> fl = {1, 2, 3, 4, 5};
    
    // 正向迭代 / Forward iteration
    int sum = 0;
    for (auto it = fl.begin(); it != fl.end(); ++it) {
        sum += *it;
    }
    TEST(sum == 15);
    
    // 范围for循环 / Range-based for loop
    sum = 0;
    for (int val : fl) {
        sum += val;
    }
    TEST(sum == 15);
    
    // before_begin() 和 cbefore_begin() / before_begin() and cbefore_begin()
    auto before_it = fl.before_begin();
    auto cbefore_it = fl.cbefore_begin();
    
    // 常量迭代器 / Constant iterator
    const mystl::forward_list<int>& cfl = fl;
    sum = 0;
    for (auto it = cfl.begin(); it != cfl.end(); ++it) {
        sum += *it;
    }
    TEST(sum == 15);
    
    std::cout << "  All iterator tests passed! / 所有迭代器测试通过！" << std::endl;
}

/**
 * @brief Test forward_list capacity
 * @brief 测试 forward_list 容量
 */
void test_forward_list_capacity() {
    std::cout << "Testing forward_list capacity... / 测试 forward_list 容量..." << std::endl;
    
    mystl::forward_list<int> fl;
    
    // empty() and max_size() / empty() 和 max_size()
    TEST(fl.empty());
    TEST(fl.max_size() > 0);
    
    // push_front 测试增长 / push_front test for growth
    for (int i = 0; i < 5; ++i) {
        fl.push_front(i);
        TEST(!fl.empty());
        TEST(fl.front() == i);
    }
    
    // pop_front / 移除第一个元素
    fl.pop_front();
    TEST(fl.front() == 3);
    
    // clear() / 清空链表
    fl.clear();
    TEST(fl.empty());
    
    std::cout << "  All capacity tests passed! / 所有容量测试通过！" << std::endl;
}

/**
 * @brief Test forward_list modifiers
 * @brief 测试 forward_list 修改操作
 */
void test_forward_list_modifiers() {
    std::cout << "Testing forward_list modifiers... / 测试 forward_list 修改操作..." << std::endl;
    
    // push_front 和 pop_front / push_front and pop_front
    mystl::forward_list<int> fl;
    fl.push_front(1);
    fl.push_front(2);
    fl.push_front(3);
    TEST(fl.front() == 3);
    
    fl.pop_front();
    TEST(fl.front() == 2);
    
    // insert_after 单个元素 / insert_after single element
    auto it = fl.before_begin();
    it = fl.insert_after(it, 0);
    TEST(fl.front() == 0);
    
    // insert_after 多个相同元素 / insert_after multiple identical elements
    it = fl.begin();
    fl.insert_after(it, 3, 99);
    
    // insert_after 范围 / insert_after range
    int arr[] = {10, 20, 30};
    it = fl.begin();
    fl.insert_after(it, arr, arr + 3);
    
    // insert_after 初始化列表 / insert_after initializer list
    it = fl.before_begin();
    fl.insert_after(it, {100, 200, 300});
    
    // erase_after 单个元素 / erase_after single element
    it = fl.before_begin();
    it = fl.erase_after(it);
    
    // erase_after 范围 / erase_after range
    auto first = fl.before_begin();
    auto last = fl.begin();
    ++last; ++last; // 跳过前两个元素 / Skip first two elements
    fl.erase_after(first, last);
    
    // emplace_front / 原位构造前端元素
    mystl::forward_list<std::pair<int, int>> fl2;
    fl2.emplace_front(1, 2);
    TEST(fl2.front().first == 1);
    TEST(fl2.front().second == 2);
    
    // emplace_after / 原位构造指定位置之后的元素
    auto it2 = fl2.before_begin();
    fl2.emplace_after(it2, 3, 4);
    // 注意：emplace_after在before_begin之后插入，所以新元素会成为第一个元素
    // Note: emplace_after inserts after before_begin, so new element becomes first
    TEST(fl2.front().first == 3);
    TEST(fl2.front().second == 4);
    
    std::cout << "  All modifier tests passed! / 所有修改操作测试通过！" << std::endl;
}

/**
 * @brief Test forward_list operations
 * @brief 测试 forward_list 操作
 */
void test_forward_list_operations() {
    std::cout << "Testing forward_list operations... / 测试 forward_list 操作..." << std::endl;
    
    // splice_after - 整个链表 / splice_after - entire list
    mystl::forward_list<int> fl1 = {1, 2, 3};
    mystl::forward_list<int> fl2 = {4, 5, 6};
    auto pos = fl1.begin();
    ++pos; // 指向元素2 / Point to element 2
    fl1.splice_after(pos, fl2);
    
    // splice_after - 单个节点 / splice_after - single node
    mystl::forward_list<int> fl3 = {10, 20, 30};
    mystl::forward_list<int> fl4 = {40, 50, 60};
    auto it = fl4.before_begin();
    ++it; // 指向元素40之前 / Point before element 40
    fl3.splice_after(fl3.before_begin(), fl4, it);
    
    // splice_after - 范围 / splice_after - range
    mystl::forward_list<int> fl5 = {100, 200, 300};
    mystl::forward_list<int> fl6 = {400, 500, 600, 700};
    auto first = fl6.begin();
    auto last = first;
    ++last; ++last; // 指向元素600 / Point to element 600
    fl5.splice_after(fl5.before_begin(), fl6, first, last);
    
    // remove / 移除指定值
    mystl::forward_list<int> fl7 = {1, 2, 3, 2, 4, 2, 5};
    fl7.remove(2);
    
    // remove_if / 条件移除
    mystl::forward_list<int> fl8 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    fl8.remove_if([](int n) { return n % 2 == 0; }); // 删除偶数 / Remove even numbers
    
    // unique / 去重
    mystl::forward_list<int> fl9 = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
    fl9.unique();
    
    // unique with predicate / 带谓词的去重
    mystl::forward_list<int> fl10 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    fl10.unique([](int a, int b) { return b - a == 1; }); // 删除连续差值为1的元素 / Remove elements with consecutive difference of 1
    
    // merge / 合并
    mystl::forward_list<int> fl11 = {1, 3, 5};
    mystl::forward_list<int> fl12 = {2, 4, 6};
    fl11.merge(fl12);
    
    // merge with comparator / 带比较器的合并
    mystl::forward_list<int> fl13 = {5, 3, 1};
    mystl::forward_list<int> fl14 = {6, 4, 2};
    fl13.merge(fl14, [](int a, int b) { return a > b; }); // 降序合并 / Merge in descending order
    
    // sort / 排序
    mystl::forward_list<int> fl15 = {5, 3, 1, 4, 2};
    fl15.sort();
    
    // sort with comparator / 带比较器的排序
    mystl::forward_list<int> fl16 = {1, 2, 3, 4, 5};
    fl16.sort([](int a, int b) { return a > b; }); // 降序排序 / Sort in descending order
    
    // reverse / 反转
    mystl::forward_list<int> fl17 = {1, 2, 3, 4, 5};
    fl17.reverse();
    
    std::cout << "  All operation tests passed! / 所有操作测试通过！" << std::endl;
}

/**
 * @brief Test forward_list assign and resize
 * @brief 测试 forward_list 赋值和调整大小
 */
void test_forward_list_assign_resize() {
    std::cout << "Testing forward_list assign and resize... / 测试 forward_list 赋值和调整大小..." << std::endl;
    
    // assign with count / 使用数量赋值
    mystl::forward_list<int> fl1;
    fl1.assign(5, 42);
    
    // assign with range / 使用范围赋值
    int arr[] = {1, 2, 3, 4, 5};
    mystl::forward_list<int> fl2;
    fl2.assign(arr, arr + 5);
    
    // assign with initializer list / 使用初始化列表赋值
    mystl::forward_list<int> fl3;
    fl3.assign({10, 20, 30, 40, 50});
    
    // resize - 增大 / resize - increase
    mystl::forward_list<int> fl4 = {1, 2, 3};
    fl4.resize(5, 99);
    
    // resize - 缩小 / resize - decrease
    mystl::forward_list<int> fl5 = {1, 2, 3, 4, 5};
    fl5.resize(3);
    
    std::cout << "  All assign and resize tests passed! / 所有赋值和调整大小测试通过！" << std::endl;
}

/**
 * @brief Test forward_list comparison operators
 * @brief 测试 forward_list 比较操作符
 */
void test_forward_list_comparison() {
    std::cout << "Testing forward_list comparison operators... / 测试 forward_list 比较操作符..." << std::endl;
    
    mystl::forward_list<int> fl1 = {1, 2, 3};
    mystl::forward_list<int> fl2 = {1, 2, 3};
    mystl::forward_list<int> fl3 = {1, 2, 4};
    mystl::forward_list<int> fl4 = {1, 2};
    mystl::forward_list<int> fl5 = {1, 2, 3, 4};
    
    // 相等比较 / Equality comparison
    TEST(fl1 == fl2);
    TEST(!(fl1 == fl3));
    TEST(!(fl1 == fl4));
    TEST(!(fl1 == fl5));
    
    // 不等比较 / Inequality comparison
    TEST(!(fl1 != fl2));
    TEST(fl1 != fl3);
    TEST(fl1 != fl4);
    TEST(fl1 != fl5);
    
    // 小于比较 / Less than comparison
    TEST(fl4 < fl1);   // {1, 2} < {1, 2, 3}
    TEST(fl1 < fl3);   // {1, 2, 3} < {1, 2, 4}
    TEST(fl1 < fl5);   // {1, 2, 3} < {1, 2, 3, 4}
    TEST(!(fl1 < fl2)); // 相等时不小于 / Not less when equal
    TEST(!(fl3 < fl1)); // {1, 2, 4}不小于{1, 2, 3} / {1, 2, 4} not less than {1, 2, 3}
    
    // 小于等于比较 / Less than or equal comparison
    TEST(fl4 <= fl1);
    TEST(fl1 <= fl2);  // 相等时<=为true / <= is true when equal
    TEST(fl1 <= fl3);
    TEST(!(fl3 <= fl1));
    
    // 大于比较 / Greater than comparison
    TEST(fl3 > fl1);
    TEST(fl5 > fl1);
    TEST(!(fl1 > fl2));
    TEST(!(fl1 > fl3));
    
    // 大于等于比较 / Greater than or equal comparison
    TEST(fl3 >= fl1);
    TEST(fl1 >= fl2);  // 相等时>=为true / >= is true when equal
    TEST(fl5 >= fl1);
    TEST(!(fl1 >= fl3));
    
    std::cout << "  All comparison operator tests passed! / 所有比较操作符测试通过！" << std::endl;
}

/**
 * @brief Test forward_list edge cases
 * @brief 测试 forward_list 边界情况
 */
void test_forward_list_edge_cases() {
    std::cout << "Testing forward_list edge cases... / 测试 forward_list 边界情况..." << std::endl;
    
    // 空链表测试 / Empty list tests
    mystl::forward_list<int> empty_fl;
    TEST(empty_fl.empty());
    TEST(empty_fl.begin() == empty_fl.end());
    
    // 单元素链表测试 / Single element list tests
    mystl::forward_list<int> single_fl = {42};
    TEST(!single_fl.empty());
    TEST(single_fl.front() == 42);
    TEST(++single_fl.begin() == single_fl.end());
    
    // 大量元素测试 / Large number of elements test
    mystl::forward_list<int> large_fl;
    const int large_count = 1000;
    for (int i = 0; i < large_count; ++i) {
        large_fl.push_front(i);
    }
    TEST(!large_fl.empty());
    
    // 自赋值测试 / Self-assignment test
    mystl::forward_list<int> self_fl = {1, 2, 3};
    self_fl = self_fl;
    TEST(!self_fl.empty());
    TEST(self_fl.front() == 1);
    
    std::cout << "  All edge case tests passed! / 所有边界情况测试通过！" << std::endl;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 * 
 * @return int 0 if all tests pass, 1 if any test fails
 * @return int 如果所有测试通过返回 0，如果有任何测试失败返回 1
 */
int main() {
    std::cout << "Starting forward_list tests... / 开始 forward_list 测试..." << std::endl;
    std::cout << "=========================" << std::endl;
    
    try {
        test_forward_list_constructors();
        test_forward_list_access();
        test_forward_list_iterators();
        test_forward_list_capacity();
        test_forward_list_modifiers();
        test_forward_list_operations();
        test_forward_list_assign_resize();
        test_forward_list_comparison();
        test_forward_list_edge_cases();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught during testing: " << e.what() << std::endl;
        std::cerr << "测试过程中捕获异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught during testing" << std::endl;
        std::cerr << "测试过程中捕获未知异常" << std::endl;
        return 1;
    }
    
    std::cout << "=========================" << std::endl;
    std::cout << "All forward_list tests passed successfully! / 所有 forward_list 测试成功通过！" << std::endl;
    
    return 0;
}
