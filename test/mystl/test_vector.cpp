/**
 * @file test_vector.cpp
 * @brief Comprehensive test suite for mystl::vector implementation
 * @brief mystl::vector 实现的全面测试套件
 * 
 * This file contains extensive tests for the vector container in mystl.
 * It verifies the correctness of vector operations including construction,
 * element access, modification, iterator functionality, and various operations.
 * 
 * 此文件包含对 mystl 中 vector 容器的广泛测试。
 * 它验证 vector 操作的正确性，包括构造、元素访问、修改、迭代器功能和各种操作。
 * 
 * The tests are organized into logical sections corresponding to different
 * aspects of vector functionality.
 * 
 * 测试按 vector 功能的不同方面组织成逻辑部分。
 * 
 * @author shensjt
 * @date 2026-03-21 / 2026年3月21日
 * 
 * @see include/mystl/vector.hpp
 * @see 参考：include/mystl/vector.hpp
 */

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include "../../include/mystl/vector.hpp"

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
 * @brief Test vector constructors
 * @brief 测试 vector 构造函数
 */
void test_vector_constructors() {
    std::cout << "Testing vector constructors... / 测试 vector 构造函数..." << std::endl;
    
    // 默认构造函数 / Default constructor
    mystl::vector<int> v1;
    TEST(v1.empty());
    TEST(v1.size() == 0);
    TEST(v1.capacity() == 0);
    
    // 指定大小的构造函数 / Constructor with specified size
    mystl::vector<int> v2(5, 42);
    TEST(v2.size() == 5);
    TEST(v2.capacity() == 5);
    for (size_t i = 0; i < v2.size(); ++i) {
        TEST(v2[i] == 42);
    }
    
    // 拷贝构造函数 / Copy constructor
    mystl::vector<int> v3(v2);
    TEST(v3.size() == 5);
    TEST(v3.capacity() == 5);
    for (size_t i = 0; i < v3.size(); ++i) {
        TEST(v3[i] == 42);
    }
    
    // 移动构造函数 / Move constructor
    mystl::vector<int> v4(std::move(v3));
    TEST(v4.size() == 5);
    TEST(v4.capacity() == 5);
    TEST(v3.empty());
    TEST(v3.capacity() == 0);
    
    // 初始化列表构造函数 / Initializer list constructor
    mystl::vector<int> v5 = {1, 2, 3, 4, 5};
    TEST(v5.size() == 5);
    TEST(v5[0] == 1);
    TEST(v5[4] == 5);
    
    std::cout << "  All constructor tests passed! / 所有构造函数测试通过！" << std::endl;
}

/**
 * @brief Test vector element access
 * @brief 测试 vector 元素访问
 */
void test_vector_access() {
    std::cout << "Testing vector element access... / 测试 vector 元素访问..." << std::endl;
    
    mystl::vector<int> v = {10, 20, 30, 40, 50};
    
    // operator[]
    TEST(v[0] == 10);
    TEST(v[2] == 30);
    TEST(v[4] == 50);
    
    // front() and back()
    TEST(v.front() == 10);
    TEST(v.back() == 50);
    
    // data()
    int* data = v.data();
    TEST(data[0] == 10);
    TEST(data[4] == 50);
    
    // at() with valid index
    TEST(v.at(1) == 20);
    TEST(v.at(3) == 40);
    
    // at() with invalid index (should throw)
    bool caught = false;
    try {
        v.at(10);
    } catch (const mystl::out_of_range&) {
        caught = true;
    }
    TEST(caught);
    
    std::cout << "  All element access tests passed! / 所有元素访问测试通过！" << std::endl;
}

/**
 * @brief Test vector iterators
 * @brief 测试 vector 迭代器
 */
void test_vector_iterators() {
    std::cout << "Testing vector iterators... / 测试 vector 迭代器..." << std::endl;
    
    mystl::vector<int> v = {1, 2, 3, 4, 5};
    
    // Forward iteration
    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it) {
        sum += *it;
    }
    TEST(sum == 15);
    
    // Range-based for loop
    sum = 0;
    for (int val : v) {
        sum += val;
    }
    TEST(sum == 15);
    
    // Reverse iteration
    sum = 0;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        sum += *it;
    }
    TEST(sum == 15);
    
    // Const iterators
    const mystl::vector<int>& cv = v;
    sum = 0;
    for (auto it = cv.begin(); it != cv.end(); ++it) {
        sum += *it;
    }
    TEST(sum == 15);
    
    std::cout << "  All iterator tests passed! / 所有迭代器测试通过！" << std::endl;
}

/**
 * @brief Test vector capacity
 * @brief 测试 vector 容量
 */
void test_vector_capacity() {
    std::cout << "Testing vector capacity... / 测试 vector 容量..." << std::endl;
    
    mystl::vector<int> v;
    
    // empty() and size()
    TEST(v.empty());
    TEST(v.size() == 0);
    
    // reserve()
    v.reserve(10);
    TEST(v.capacity() >= 10);
    TEST(v.empty());
    TEST(v.size() == 0);
    
    // push_back to test growth
    for (int i = 0; i < 5; ++i) {
        v.push_back(i);
        TEST(v.size() == static_cast<size_t>(i + 1));
        TEST(v.back() == i);
    }
    
    // pop_back
    v.pop_back();
    TEST(v.size() == 4);
    TEST(v.back() == 3);
    
    std::cout << "  Basic capacity tests passed! / 基本容量测试通过！" << std::endl;
}

/**
 * @brief Test vector modifiers
 * @brief 测试 vector 修改操作
 */
void test_vector_modifiers() {
    std::cout << "Testing vector modifiers... / 测试 vector 修改操作..." << std::endl;
    
    mystl::vector<int> v1 = {1, 2, 3};
    mystl::vector<int> v2 = {4, 5, 6};
    
    // swap()
    v1.swap(v2);
    TEST(v1[0] == 4);
    TEST(v1[2] == 6);
    TEST(v2[0] == 1);
    TEST(v2[2] == 3);
    
    // clear()
    v1.clear();
    TEST(v1.empty());
    TEST(v1.size() == 0);
    
    // insert测试 / insert test
    mystl::vector<int> v3 = {1, 2, 3, 4, 5};
    
    // 在开头插入 / Insert at beginning
    v3.insert(v3.begin(), 0);
    TEST(v3.size() == 6);
    TEST(v3[0] == 0);
    TEST(v3[1] == 1);
    
    // 在中间插入 / Insert in middle
    v3.insert(v3.begin() + 3, 99);
    TEST(v3.size() == 7);
    TEST(v3[3] == 99);
    
    // 在结尾插入 / Insert at end
    v3.insert(v3.end(), 100);
    TEST(v3.size() == 8);
    TEST(v3[7] == 100);
    
    // erase测试 / erase test
    // 删除中间元素 / Erase middle element
    v3.erase(v3.begin() + 3);
    TEST(v3.size() == 7);
    TEST(v3[3] == 3);  // 原来的第4个元素 / Original 4th element
    
    // 删除范围 / Erase range
    v3.erase(v3.begin() + 1, v3.begin() + 3);
    TEST(v3.size() == 5);
    TEST(v3[1] == 3);  // 原来的第4个元素 / Original 4th element
    
    std::cout << "  Basic modifier tests passed! / 基本修改操作测试通过！" << std::endl;
}

/**
 * @brief Test vector edge cases
 * @brief 测试 vector 边界情况
 */
void test_vector_edge_cases() {
    std::cout << "Testing vector edge cases... / 测试 vector 边界情况..." << std::endl;
    
    // 空vector的各种操作 / Various operations on empty vector
    mystl::vector<int> empty_vec;
    TEST(empty_vec.empty());
    TEST(empty_vec.size() == 0);
    TEST(empty_vec.capacity() == 0);
    
    // 单个元素的vector / Single element vector
    mystl::vector<int> single_vec = {42};
    TEST(!single_vec.empty());
    TEST(single_vec.size() == 1);
    TEST(single_vec.front() == 42);
    TEST(single_vec.back() == 42);
    TEST(single_vec[0] == 42);
    TEST(single_vec.at(0) == 42);
    
    // 大量元素测试 / Large number of elements test
    mystl::vector<int> large_vec;
    const int LARGE_SIZE = 1000;
    for (int i = 0; i < LARGE_SIZE; ++i) {
        large_vec.push_back(i);
    }
    TEST(large_vec.size() == LARGE_SIZE);
    for (int i = 0; i < LARGE_SIZE; ++i) {
        TEST(large_vec[i] == i);
    }
    
    // 测试capacity增长策略（2倍扩容） / Test capacity growth strategy (2x expansion)
    mystl::vector<int> growth_vec;
    size_t prev_capacity = growth_vec.capacity();
    for (int i = 0; i < 100; ++i) {
        growth_vec.push_back(i);
        if (growth_vec.size() > prev_capacity) {
            // 扩容应该至少翻倍 / Expansion should at least double
            TEST(growth_vec.capacity() >= prev_capacity * 2);
            prev_capacity = growth_vec.capacity();
        }
    }
    
    // shrink_to_fit测试 / shrink_to_fit test
    growth_vec.shrink_to_fit();
    TEST(growth_vec.capacity() == growth_vec.size());
    
    // 测试resize()函数 / Test resize() function
    mystl::vector<int> resize_vec = {1, 2, 3};
    resize_vec.resize(5, 99);
    TEST(resize_vec.size() == 5);
    TEST(resize_vec[0] == 1);
    TEST(resize_vec[1] == 2);
    TEST(resize_vec[2] == 3);
    TEST(resize_vec[3] == 99);
    TEST(resize_vec[4] == 99);
    
    resize_vec.resize(2);
    TEST(resize_vec.size() == 2);
    TEST(resize_vec[0] == 1);
    TEST(resize_vec[1] == 2);
    
    // 测试assign函数 / Test assign function
    mystl::vector<int> assign_vec;
    assign_vec.assign(5, 88);
    TEST(assign_vec.size() == 5);
    for (int val : assign_vec) {
        TEST(val == 88);
    }
    
    int arr[] = {1, 2, 3, 4, 5};
    assign_vec.assign(arr, arr + 5);
    TEST(assign_vec.size() == 5);
    for (size_t i = 0; i < assign_vec.size(); ++i) {
        TEST(assign_vec[i] == static_cast<int>(i + 1));
    }
    
    assign_vec.assign({10, 20, 30});
    TEST(assign_vec.size() == 3);
    TEST(assign_vec[0] == 10);
    TEST(assign_vec[1] == 20);
    TEST(assign_vec[2] == 30);
    
    // 测试emplace_back() / Test emplace_back()
    mystl::vector<int> emplace_vec;
    emplace_vec.emplace_back(1);
    emplace_vec.emplace_back(2);
    emplace_vec.emplace_back(3);
    TEST(emplace_vec.size() == 3);
    TEST(emplace_vec[0] == 1);
    TEST(emplace_vec[1] == 2);
    TEST(emplace_vec[2] == 3);
    
    struct Point {
        int x, y;
        Point(int x, int y) : x(x), y(y) {}
    };
    
    mystl::vector<Point> point_vec;
    point_vec.emplace_back(10, 20);
    point_vec.emplace_back(30, 40);
    TEST(point_vec.size() == 2);
    TEST(point_vec[0].x == 10);
    TEST(point_vec[0].y == 20);
    TEST(point_vec[1].x == 30);
    TEST(point_vec[1].y == 40);
    
    std::cout << "  All edge case tests passed! / 所有边界情况测试通过！" << std::endl;
}

/**
 * @brief Test vector exception safety
 * @brief 测试 vector 异常安全性
 */
void test_vector_exception_safety() {
    std::cout << "Testing vector exception safety... / 测试 vector 异常安全性..." << std::endl;
    
    // at() 异常测试 / at() exception test
    mystl::vector<int> v = {1, 2, 3};
    
    bool caught_upper = false;
    try {
        v.at(10);
    } catch (const mystl::out_of_range&) {
        caught_upper = true;
    }
    TEST(caught_upper);
    
    bool caught_lower = false;
    try {
        v.at(-1);  // 转换为size_t会变成很大的数 / Converted to size_t becomes a large number
    } catch (const mystl::out_of_range&) {
        caught_lower = true;
    }
    TEST(caught_lower);
    
    // 自赋值测试 / Self-assignment test
    v = v;
    TEST(v.size() == 3);
    TEST(v[0] == 1);
    TEST(v[2] == 3);
    
    std::cout << "  All exception safety tests passed! / 所有异常安全性测试通过！" << std::endl;
}

/**
 * @brief Test vector comparison operators
 * @brief 测试 vector 比较操作符
 */
void test_vector_comparison_operators() {
    std::cout << "Testing vector comparison operators... / 测试 vector 比较操作符..." << std::endl;
    
    mystl::vector<int> v1 = {1, 2, 3};
    mystl::vector<int> v2 = {1, 2, 3};
    mystl::vector<int> v3 = {1, 2, 4};
    mystl::vector<int> v4 = {1, 2};
    mystl::vector<int> v5 = {1, 2, 3, 4};
    
    // 相等比较 / Equality comparison
    TEST(v1 == v2);
    TEST(!(v1 == v3));
    TEST(!(v1 == v4));
    TEST(!(v1 == v5));
    
    // 不等比较 / Inequality comparison
    TEST(!(v1 != v2));
    TEST(v1 != v3);
    TEST(v1 != v4);
    TEST(v1 != v5);
    
    // 小于比较 / Less than comparison
    TEST(v4 < v1);   // {1, 2} < {1, 2, 3}
    TEST(v1 < v3);   // {1, 2, 3} < {1, 2, 4}
    TEST(v1 < v5);   // {1, 2, 3} < {1, 2, 3, 4}
    TEST(!(v1 < v2)); // 相等时不小于 / Not less when equal
    TEST(!(v3 < v1)); // {1, 2, 4}不小于{1, 2, 3} / {1, 2, 4} not less than {1, 2, 3}
    
    // 小于等于比较 / Less than or equal comparison
    TEST(v4 <= v1);
    TEST(v1 <= v2);  // 相等时<=为true / <= is true when equal
    TEST(v1 <= v3);
    TEST(!(v3 <= v1));
    
    // 大于比较 / Greater than comparison
    TEST(v3 > v1);
    TEST(v5 > v1);
    TEST(!(v1 > v2));
    TEST(!(v1 > v3));
    
    // 大于等于比较 / Greater than or equal comparison
    TEST(v3 >= v1);
    TEST(v1 >= v2);  // 相等时>=为true / >= is true when equal
    TEST(v5 >= v1);
    TEST(!(v1 >= v3));
    
    // 空vector比较 / Empty vector comparison
    mystl::vector<int> empty1, empty2;
    TEST(empty1 == empty2);
    TEST(!(empty1 != empty2));
    TEST(!(empty1 < empty2));
    TEST(empty1 <= empty2);
    TEST(!(empty1 > empty2));
    TEST(empty1 >= empty2);
    
    // 空vector与非空vector比较 / Empty vector vs non-empty vector comparison
    TEST(empty1 < v1);
    TEST(empty1 <= v1);
    TEST(!(empty1 > v1));
    TEST(!(empty1 >= v1));
    TEST(v1 > empty1);
    TEST(v1 >= empty1);
    TEST(!(v1 < empty1));
    TEST(!(v1 <= empty1));
    
    std::cout << "  All comparison operator tests passed! / 所有比较操作符测试通过！" << std::endl;
}

/**
 * @brief Test vector with std algorithm
 * @brief 测试 vector 与标准算法
 */
void test_vector_with_std_algorithm() {
    std::cout << "Testing vector with std algorithms... / 测试 vector 与标准算法..." << std::endl;
    
    mystl::vector<int> v = {5, 3, 1, 4, 2};
    
    // 使用std::sort排序 / Use std::sort to sort
    std::sort(v.begin(), v.end());
    TEST(v.size() == 5);
    for (size_t i = 0; i < v.size(); ++i) {
        TEST(v[i] == static_cast<int>(i + 1));
    }
    
    // 使用std::find查找元素 / Use std::find to find element
    auto it = std::find(v.begin(), v.end(), 3);
    TEST(it != v.end());
    TEST(*it == 3);
    
    // 使用std::count计数 / Use std::count to count
    int count = std::count(v.begin(), v.end(), 3);
    TEST(count == 1);
    
    // 使用std::reverse反转 / Use std::reverse to reverse
    std::reverse(v.begin(), v.end());
    TEST(v[0] == 5);
    TEST(v[4] == 1);
    
    std::cout << "  All std algorithm tests passed! / 所有标准算法测试通过！" << std::endl;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 * 
 * @return int 0 if all tests pass, 1 if any test fails
 * @return int 如果所有测试通过返回 0，如果有任何测试失败返回 1
 */
int main() {
    std::cout << "=== Starting vector tests === / === 开始 vector 测试 ===" << std::endl;
    std::cout << std::endl;
    
    try {
        test_vector_constructors();
        test_vector_access();
        test_vector_iterators();
        test_vector_capacity();
        test_vector_modifiers();
        test_vector_edge_cases();
        test_vector_exception_safety();
        test_vector_comparison_operators();
        test_vector_with_std_algorithm();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught during testing: " << e.what() << std::endl;
        std::cerr << "测试过程中捕获异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught during testing" << std::endl;
        std::cerr << "测试过程中捕获未知异常" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== All vector tests passed! / 所有 vector 测试通过！ ===" << std::endl;
    return 0;
}
