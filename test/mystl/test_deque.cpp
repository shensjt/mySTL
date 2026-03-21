/**
 * @file test_deque.cpp
 * @brief Comprehensive test suite for mystl::deque implementation
 * @brief mystl::deque 实现的全面测试套件
 * 
 * This file contains extensive tests for the deque container in mystl.
 * It verifies the correctness of deque operations including construction,
 * element access, modification, iterator functionality, and various operations.
 * 
 * 此文件包含对 mystl 中 deque 容器的广泛测试。
 * 它验证 deque 操作的正确性，包括构造、元素访问、修改、迭代器功能和各种操作。
 * 
 * The tests are organized into logical sections corresponding to different
 * aspects of deque functionality.
 * 
 * 测试按 deque 功能的不同方面组织成逻辑部分。
 * 
 * @author shensjt
 * @date 2026-03-21 / 2026年3月21日
 * 
 * @see include/mystl/deque.hpp
 * @see 参考：include/mystl/deque.hpp
 */

#include <iostream>
#include <cassert>
#include <algorithm>
#include "../../include/mystl/deque.hpp"
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
 * @brief Test deque constructors
 * @brief 测试 deque 构造函数
 */
void test_deque_constructors() {
    std::cout << "Testing deque constructors... / 测试 deque 构造函数..." << std::endl;
    
    // 默认构造函数 / Default constructor
    mystl::deque<int> dq1;
    TEST(dq1.empty());
    TEST(dq1.size() == 0);
    
    // 指定大小的构造函数 / Constructor with specified size
    mystl::deque<int> dq2(5, 42);
    TEST(dq2.size() == 5);
    for (size_t i = 0; i < dq2.size(); ++i) {
        TEST(dq2[i] == 42);
    }
    
    // 拷贝构造函数 / Copy constructor
    mystl::deque<int> dq3(dq2);
    TEST(dq3.size() == 5);
    for (size_t i = 0; i < dq3.size(); ++i) {
        TEST(dq3[i] == 42);
    }
    
    // 移动构造函数 / Move constructor
    mystl::deque<int> dq4(std::move(dq3));
    TEST(dq4.size() == 5);
    TEST(dq3.empty());
    TEST(dq3.size() == 0);
    
    // 初始化列表构造函数 / Initializer list constructor
    mystl::deque<int> dq5 = {1, 2, 3, 4, 5};
    TEST(dq5.size() == 5);
    TEST(dq5[0] == 1);
    TEST(dq5[4] == 5);
    
    // 初始化列表构造函数 / Initializer list constructor
    mystl::deque<int> dq6({10, 20, 30, 40, 50});
    TEST(dq6.size() == 5);
    for (size_t i = 0; i < dq6.size(); ++i) {
        TEST(dq6[i] == static_cast<int>(10 * (i + 1)));
    }
    
    std::cout << "  All constructor tests passed! / 所有构造函数测试通过！" << std::endl;
}

/**
 * @brief Test deque element access
 * @brief 测试 deque 元素访问
 */
void test_deque_access() {
    std::cout << "Testing deque element access... / 测试 deque 元素访问..." << std::endl;
    
    mystl::deque<int> dq = {10, 20, 30, 40, 50};
    
    // operator[]
    TEST(dq[0] == 10);
    TEST(dq[2] == 30);
    TEST(dq[4] == 50);
    
    // front() and back()
    TEST(dq.front() == 10);
    TEST(dq.back() == 50);
    
    // at() with valid index
    TEST(dq.at(1) == 20);
    TEST(dq.at(3) == 40);
    
    // at() with invalid index (should throw)
    bool caught = false;
    try {
        dq.at(10);
    } catch (const mystl::out_of_range&) {
        caught = true;
    }
    TEST(caught);
    
    std::cout << "  All element access tests passed! / 所有元素访问测试通过！" << std::endl;
}

/**
 * @brief Test deque iterators
 * @brief 测试 deque 迭代器
 */
void test_deque_iterators() {
    std::cout << "Testing deque iterators... / 测试 deque 迭代器..." << std::endl;
    
    mystl::deque<int> dq = {1, 2, 3, 4, 5};
    
    // Forward iteration
    int sum = 0;
    for (auto it = dq.begin(); it != dq.end(); ++it) {
        sum += *it;
    }
    TEST(sum == 15);
    
    // Range-based for loop
    sum = 0;
    for (int val : dq) {
        sum += val;
    }
    TEST(sum == 15);
    
    // Reverse iteration
    sum = 0;
    for (auto it = dq.rbegin(); it != dq.rend(); ++it) {
        sum += *it;
    }
    TEST(sum == 15);
    
    // Const iterators
    const mystl::deque<int>& cdq = dq;
    sum = 0;
    for (auto it = cdq.begin(); it != cdq.end(); ++it) {
        sum += *it;
    }
    TEST(sum == 15);
    
    // 测试迭代器算术运算 / Test iterator arithmetic operations
    auto it1 = dq.begin();
    auto it2 = dq.begin() + 3;
    TEST(*it1 == 1);
    TEST(*it2 == 4);
    TEST(it2 - it1 == 3);
    
    // 测试迭代器下标访问 / Test iterator subscript access
    TEST(it1[0] == 1);
    TEST(it1[2] == 3);
    TEST(it2[-1] == 3);
    
    std::cout << "  All iterator tests passed! / 所有迭代器测试通过！" << std::endl;
}

/**
 * @brief Test deque capacity
 * @brief 测试 deque 容量
 */
void test_deque_capacity() {
    std::cout << "Testing deque capacity... / 测试 deque 容量..." << std::endl;
    
    mystl::deque<int> dq;
    
    // empty() and size()
    TEST(dq.empty());
    TEST(dq.size() == 0);
    
    // max_size()
    TEST(dq.max_size() > 0);
    
    // push_back to test growth
    for (int i = 0; i < 5; ++i) {
        dq.push_back(i);
        TEST(dq.size() == static_cast<size_t>(i + 1));
        TEST(dq.back() == i);
    }
    
    // pop_back
    dq.pop_back();
    TEST(dq.size() == 4);
    TEST(dq.back() == 3);
    
    // push_front
    dq.push_front(-1);
    TEST(dq.size() == 5);
    TEST(dq.front() == -1);
    
    // pop_front
    dq.pop_front();
    TEST(dq.size() == 4);
    TEST(dq.front() == 0);
    
    std::cout << "  Basic capacity tests passed! / 基本容量测试通过！" << std::endl;
}

/**
 * @brief Test deque modifiers
 * @brief 测试 deque 修改操作
 */
void test_deque_modifiers() {
    std::cout << "Testing deque modifiers... / 测试 deque 修改操作..." << std::endl;
    
    mystl::deque<int> dq1 = {1, 2, 3};
    mystl::deque<int> dq2 = {4, 5, 6};
    
    // swap()
    dq1.swap(dq2);
    TEST(dq1[0] == 4);
    TEST(dq1[2] == 6);
    TEST(dq2[0] == 1);
    TEST(dq2[2] == 3);
    
    // clear()
    dq1.clear();
    TEST(dq1.empty());
    TEST(dq1.size() == 0);
    
    std::cout << "  Basic modifier tests passed! / 基本修改操作测试通过！" << std::endl;
}

/**
 * @brief Test deque insert and erase
 * @brief 测试 deque 插入和删除
 */
void test_deque_insert_erase() {
    std::cout << "Testing deque insert and erase... / 测试 deque 插入和删除..." << std::endl;
    
    mystl::deque<int> dq = {1, 2, 3, 4, 5};
    
    // 在开头插入 / Insert at beginning
    dq.insert(dq.begin(), 0);
    TEST(dq.size() == 6);
    TEST(dq[0] == 0);
    TEST(dq[1] == 1);
    
    // 在中间插入 / Insert in middle
    dq.insert(dq.begin() + 3, 99);
    TEST(dq.size() == 7);
    TEST(dq[3] == 99);
    
    // 在结尾插入 / Insert at end
    dq.insert(dq.end(), 100);
    TEST(dq.size() == 8);
    TEST(dq[7] == 100);
    
    // 插入多个元素 / Insert multiple elements
    dq.insert(dq.begin() + 2, 3, 88);
    TEST(dq.size() == 11);
    TEST(dq[2] == 88);
    TEST(dq[3] == 88);
    TEST(dq[4] == 88);
    
    // 删除中间元素 / Erase middle element
    dq.erase(dq.begin() + 3);
    TEST(dq.size() == 10);
    TEST(dq[3] == 88);  // 原来的第4个88 / Original 4th 88
    
    // 删除范围 / Erase range
    dq.erase(dq.begin() + 1, dq.begin() + 4);
    TEST(dq.size() == 7);
    TEST(dq[1] == 2);  // 删除元素1,88,88后，位置1的元素是2 / After deleting elements 1,88,88, element at position 1 is 2
    
    // 测试insert范围函数 / Test insert range function
    mystl::deque<int> dq2 = {1, 2, 3};
    mystl::vector<int> vec = {10, 20, 30};
    dq2.insert(dq2.begin() + 1, vec.begin(), vec.end());
    TEST(dq2.size() == 6);
    TEST(dq2[0] == 1);
    TEST(dq2[1] == 10);
    TEST(dq2[2] == 20);
    TEST(dq2[3] == 30);
    TEST(dq2[4] == 2);
    TEST(dq2[5] == 3);
    
    // 测试insert初始化列表函数 / Test insert initializer list function
    mystl::deque<int> dq3 = {1, 2, 3};
    dq3.insert(dq3.begin() + 1, {4, 5, 6});
    TEST(dq3.size() == 6);
    TEST(dq3[0] == 1);
    TEST(dq3[1] == 4);
    TEST(dq3[2] == 5);
    TEST(dq3[3] == 6);
    TEST(dq3[4] == 2);
    TEST(dq3[5] == 3);
    
    std::cout << "  All insert and erase tests passed! / 所有插入和删除测试通过！" << std::endl;
}

/**
 * @brief Test deque emplace
 * @brief 测试 deque 原位构造
 */
void test_deque_emplace() {
    std::cout << "Testing deque emplace... / 测试 deque 原位构造..." << std::endl;
    
    mystl::deque<int> dq;
    
    // emplace_back
    dq.emplace_back(1);
    dq.emplace_back(2);
    dq.emplace_back(3);
    TEST(dq.size() == 3);
    TEST(dq[0] == 1);
    TEST(dq[1] == 2);
    TEST(dq[2] == 3);
    
    // emplace_front
    dq.emplace_front(0);
    dq.emplace_front(-1);
    TEST(dq.size() == 5);
    TEST(dq[0] == -1);
    TEST(dq[1] == 0);
    
    // emplace at position
    dq.emplace(dq.begin() + 2, 99);
    TEST(dq.size() == 6);
    TEST(dq[0] == -1);
    TEST(dq[1] == 0);
    TEST(dq[2] == 99);
    TEST(dq[3] == 1);
    TEST(dq[4] == 2);
    TEST(dq[5] == 3);
    
    // emplace at end
    dq.emplace(dq.end(), 100);
    TEST(dq.size() == 7);
    TEST(dq[6] == 100);
    
    // emplace at beginning
    dq.emplace(dq.begin(), -2);
    TEST(dq.size() == 8);
    TEST(dq[0] == -2);
    TEST(dq[1] == -1);
    
    // 测试复杂类型的emplace / Test emplace with complex type
    struct Point {
        int x, y;
        Point(int x, int y) : x(x), y(y) {}
    };
    
    mystl::deque<Point> point_dq;
    point_dq.emplace_back(10, 20);
    point_dq.emplace_back(30, 40);
    point_dq.emplace_front(0, 0);
    
    TEST(point_dq.size() == 3);
    TEST(point_dq[0].x == 0 && point_dq[0].y == 0);
    TEST(point_dq[1].x == 10 && point_dq[1].y == 20);
    TEST(point_dq[2].x == 30 && point_dq[2].y == 40);
    
    // 测试复杂类型的emplace at position / Test emplace at position with complex type
    point_dq.emplace(point_dq.begin() + 1, 5, 5);
    TEST(point_dq.size() == 4);
    TEST(point_dq[0].x == 0 && point_dq[0].y == 0);
    TEST(point_dq[1].x == 5 && point_dq[1].y == 5);
    TEST(point_dq[2].x == 10 && point_dq[2].y == 20);
    TEST(point_dq[3].x == 30 && point_dq[3].y == 40);
    
    std::cout << "  All emplace tests passed! / 所有原位构造测试通过！" << std::endl;
}

/**
 * @brief Test deque edge cases
 * @brief 测试 deque 边界情况
 */
void test_deque_edge_cases() {
    std::cout << "Testing deque edge cases... / 测试 deque 边界情况..." << std::endl;
    
    // 空deque的各种操作 / Various operations on empty deque
    mystl::deque<int> empty_dq;
    TEST(empty_dq.empty());
    TEST(empty_dq.size() == 0);
    
    // 单个元素的deque / Single element deque
    mystl::deque<int> single_dq = {42};
    TEST(!single_dq.empty());
    TEST(single_dq.size() == 1);
    TEST(single_dq.front() == 42);
    TEST(single_dq.back() == 42);
    TEST(single_dq[0] == 42);
    TEST(single_dq.at(0) == 42);
    
    // 大量元素测试 / Large number of elements test
    mystl::deque<int> large_dq;
    const int LARGE_SIZE = 1000;
    for (int i = 0; i < LARGE_SIZE; ++i) {
        large_dq.push_back(i);
    }
    TEST(large_dq.size() == LARGE_SIZE);
    for (int i = 0; i < LARGE_SIZE; ++i) {
        TEST(large_dq[i] == i);
    }
    
    // 两端交替插入测试 / Alternating insertion at both ends test
    mystl::deque<int> alt_dq;
    for (int i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            alt_dq.push_front(i);
        } else {
            alt_dq.push_back(i);
        }
    }
    TEST(alt_dq.size() == 100);
    
    // 测试resize()函数 / Test resize() function
    mystl::deque<int> resize_dq = {1, 2, 3};
    resize_dq.resize(5, 99);
    TEST(resize_dq.size() == 5);
    TEST(resize_dq[0] == 1);
    TEST(resize_dq[1] == 2);
    TEST(resize_dq[2] == 3);
    TEST(resize_dq[3] == 99);
    TEST(resize_dq[4] == 99);
    
    resize_dq.resize(2);
    TEST(resize_dq.size() == 2);
    TEST(resize_dq[0] == 1);
    TEST(resize_dq[1] == 2);
    
    // 测试assign函数 / Test assign function
    mystl::deque<int> assign_dq;
    assign_dq.assign(5, 88);
    TEST(assign_dq.size() == 5);
    for (int val : assign_dq) {
        TEST(val == 88);
    }
    
    // 使用初始化列表进行assign测试 / Test assign with initializer list
    assign_dq.assign({1, 2, 3, 4, 5});
    TEST(assign_dq.size() == 5);
    for (size_t i = 0; i < assign_dq.size(); ++i) {
        TEST(assign_dq[i] == static_cast<int>(i + 1));
    }
    
    assign_dq.assign({10, 20, 30});
    TEST(assign_dq.size() == 3);
    TEST(assign_dq[0] == 10);
    TEST(assign_dq[1] == 20);
    TEST(assign_dq[2] == 30);
    
    // 测试shrink_to_fit / Test shrink_to_fit
    mystl::deque<int> shrink_dq;
    for (int i = 0; i < 100; ++i) {
        shrink_dq.push_back(i);
    }
    // 删除一些元素 / Delete some elements
    for (int i = 0; i < 50; ++i) {
        shrink_dq.pop_back();
    }
    // 注意：deque没有capacity()函数 / Note: deque doesn't have capacity() function
    shrink_dq.shrink_to_fit();
    // 检查shrink_to_fit后容器仍然正常工作 / Check container still works after shrink_to_fit
    TEST(shrink_dq.size() == 50);
    for (int i = 0; i < 50; ++i) {
        TEST(shrink_dq[i] == i);
    }
    
    // 测试get_allocator() / Test get_allocator()
    mystl::deque<int> alloc_dq;
    auto alloc = alloc_dq.get_allocator();
    // 确保get_allocator()返回有效的分配器 / Ensure get_allocator() returns valid allocator
    // 我们可以使用分配器来分配内存 / We can use allocator to allocate memory
    int* p = alloc.allocate(1);
    alloc.construct(p, 42);
    TEST(*p == 42);
    alloc.destroy(p);
    alloc.deallocate(p, 1);
    
    // 测试带自定义分配器的deque / Test deque with custom allocator
    mystl::deque<int, mystl::allocator<int>> custom_alloc_dq;
    auto custom_alloc = custom_alloc_dq.get_allocator();
    // 确保可以获取分配器 / Ensure allocator can be obtained
    static_assert(std::is_same<decltype(custom_alloc), mystl::allocator<int>>::value,
                  "get_allocator() should return the correct allocator type");
    
    // 测试带分配器的拷贝构造函数 / Test copy constructor with allocator
    mystl::deque<int> dq1 = {1, 2, 3, 4, 5};
    mystl::allocator<int> custom_alloc2;
    mystl::deque<int> dq2(dq1, custom_alloc2);
    TEST(dq2.size() == dq1.size());
    for (size_t i = 0; i < dq1.size(); ++i) {
        TEST(dq2[i] == dq1[i]);
    }
    
    // 测试带分配器的移动构造函数（分配器相等的情况） / Test move constructor with allocator (allocators equal)
    mystl::deque<int> dq3 = {1, 2, 3};
    mystl::deque<int> dq4(std::move(dq3), dq3.get_allocator());
    TEST(dq4.size() == 3);
    TEST(dq3.empty());
    TEST(dq4[0] == 1 && dq4[1] == 2 && dq4[2] == 3);
    
    // 测试带分配器的移动构造函数（分配器不相等的情况） / Test move constructor with allocator (allocators not equal)
    mystl::deque<int, mystl::allocator<int>> dq5 = {10, 20, 30};
    mystl::allocator<int> different_alloc;
    mystl::deque<int, mystl::allocator<int>> dq6(std::move(dq5), different_alloc);
    // 当分配器不相等时，应该进行拷贝而不是移动 / When allocators are not equal, should copy instead of move
    TEST(dq6.size() == 3);
    TEST(dq6[0] == 10 && dq6[1] == 20 && dq6[2] == 30);
    
    std::cout << "  All edge case tests passed! / 所有边界情况测试通过！" << std::endl;
}

/**
 * @brief Test deque exception safety
 * @brief 测试 deque 异常安全性
 */
void test_deque_exception_safety() {
    std::cout << "Testing deque exception safety... / 测试 deque 异常安全性..." << std::endl;
    
    // at() 异常测试 / at() exception test
    mystl::deque<int> dq = {1, 2, 3};
    
    bool caught_upper = false;
    try {
        dq.at(10);
    } catch (const mystl::out_of_range&) {
        caught_upper = true;
    }
    TEST(caught_upper);
    
    bool caught_lower = false;
    try {
        dq.at(-1);  // 转换为size_t会变成很大的数 / Converted to size_t becomes a large number
    } catch (const mystl::out_of_range&) {
        caught_lower = true;
    }
    TEST(caught_lower);
    
    // 自赋值测试 / Self-assignment test
    dq = dq;
    TEST(dq.size() == 3);
    TEST(dq[0] == 1);
    TEST(dq[2] == 3);
    
    std::cout << "  All exception safety tests passed! / 所有异常安全性测试通过！" << std::endl;
}

/**
 * @brief Test deque comparison operators
 * @brief 测试 deque 比较操作符
 */
void test_deque_comparison_operators() {
    std::cout << "Testing deque comparison operators... / 测试 deque 比较操作符..." << std::endl;
    
    mystl::deque<int> dq1 = {1, 2, 3};
    mystl::deque<int> dq2 = {1, 2, 3};
    mystl::deque<int> dq3 = {1, 2, 4};
    mystl::deque<int> dq4 = {1, 2};
    mystl::deque<int> dq5 = {1, 2, 3, 4};
    
    // 相等比较 / Equality comparison
    TEST(dq1 == dq2);
    TEST(!(dq1 == dq3));
    TEST(!(dq1 == dq4));
    TEST(!(dq1 == dq5));
    
    // 不等比较 / Inequality comparison
    TEST(!(dq1 != dq2));
    TEST(dq1 != dq3);
    TEST(dq1 != dq4);
    TEST(dq1 != dq5);
    
    // 小于比较 / Less than comparison
    TEST(dq4 < dq1);   // {1, 2} < {1, 2, 3}
    TEST(dq1 < dq3);   // {1, 2, 3} < {1, 2, 4}
    TEST(dq1 < dq5);   // {1, 2, 3} < {1, 2, 3, 4}
    TEST(!(dq1 < dq2)); // 相等时不小于 / Not less when equal
    TEST(!(dq3 < dq1)); // {1, 2, 4}不小于{1, 2, 3} / {1, 2, 4} not less than {1, 2, 3}
    
    // 小于等于比较 / Less than or equal comparison
    TEST(dq4 <= dq1);
    TEST(dq1 <= dq2);  // 相等时<=为true / <= is true when equal
    TEST(dq1 <= dq3);
    TEST(!(dq3 <= dq1));
    
    // 大于比较 / Greater than comparison
    TEST(dq3 > dq1);
    TEST(dq5 > dq1);
    TEST(!(dq1 > dq2));
    TEST(!(dq1 > dq3));
    
    // 大于等于比较 / Greater than or equal comparison
    TEST(dq3 >= dq1);
    TEST(dq1 >= dq2);  // 相等时>=为true / >= is true when equal
    TEST(dq5 >= dq1);
    TEST(!(dq1 >= dq3));
    
    // 空deque比较 / Empty deque comparison
    mystl::deque<int> empty1, empty2;
    TEST(empty1 == empty2);
    TEST(!(empty1 != empty2));
    TEST(!(empty1 < empty2));
    TEST(empty1 <= empty2);
    TEST(!(empty1 > empty2));
    TEST(empty1 >= empty2);
    
    // 空deque与非空deque比较 / Empty deque vs non-empty deque comparison
    TEST(empty1 < dq1);
    TEST(empty1 <= dq1);
    TEST(!(empty1 > dq1));
    TEST(!(empty1 >= dq1));
    TEST(dq1 > empty1);
    TEST(dq1 >= empty1);
    TEST(!(dq1 < empty1));
    TEST(!(dq1 <= empty1));
    
    std::cout << "  All comparison operator tests passed! / 所有比较操作符测试通过！" << std::endl;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 * 
 * @return int 0 if all tests pass, 1 if any test fails
 * @return int 如果所有测试通过返回 0，如果有任何测试失败返回 1
 */
int main() {
    std::cout << "Starting deque tests... / 开始 deque 测试..." << std::endl;
    std::cout << "=========================" << std::endl;
    
    try {
        test_deque_constructors();
        test_deque_access();
        test_deque_iterators();
        test_deque_capacity();
        test_deque_modifiers();
        test_deque_insert_erase();
        test_deque_emplace();
        test_deque_edge_cases();
        test_deque_exception_safety();
        test_deque_comparison_operators();
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
    std::cout << "All deque tests passed successfully! / 所有 deque 测试成功通过！" << std::endl;
    
    return 0;
}
