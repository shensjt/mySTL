/**
 * @file test_set.cpp
 * @brief Test suite for mystl::set and mystl::multiset
 * @brief mystl::set 和 mystl::multiset 测试套件
 * 
 * This file contains tests for the set and multiset containers in mystl.
 * It verifies the correctness of set/multiset operations including construction,
 * element access, modification, iterator functionality, and set operations.
 * 
 * 此文件包含对 mystl 中集合和多重集合容器的测试。
 * 它验证集合/多重集合操作的正确性，包括构造、元素访问、修改、迭代器功能和集合操作。
 * 
 * @author shensjt
 * @date 2026-03-29
 * 
 * @see include/mystl/set.hpp
 * @see include/mystl/multiset.hpp
 * @see include/algorithm/set_operations.hpp
 */

#include "../../include/mystl/set.hpp"
#include "../../include/mystl/multiset.hpp"
#include "../../include/algorithm/set_operations.hpp"
#include "../../include/mystl/vector.hpp"
#include <iostream>
#include <cassert>
#include <iterator>

// Helper macro for testing / 测试辅助宏
#define TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            std::cerr << "测试失败，行号 " << __LINE__ << ": " << #expr << std::endl; \
            assert(false && #expr); \
        } \
    } while(0)

// ==================== Test Functions for set ====================
// ==================== set 测试函数 ====================

/**
 * @brief Test set constructors and basic operations
 * @brief 测试集合构造函数和基本操作
 */
void test_set_constructors() {
    std::cout << "1. Testing set constructors and basic operations: / 1. 测试集合构造函数和基本操作:" << std::endl;
    
    // 默认构造函数 / Default constructor
    mystl::set<int> s1;
    TEST(s1.empty());
    TEST(s1.size() == 0);
    TEST(s1.begin() == s1.end());
    
    // 带比较函数的构造函数 / Constructor with comparison function
    mystl::less<int> comp;
    mystl::set<int, mystl::less<int>> s2(comp);
    TEST(s2.empty());
    
    // 插入元素 / Insert elements
    auto result1 = s1.insert(10);
    TEST(result1.second);  // 插入成功 / Insert successful
    TEST(*result1.first == 10);
    TEST(!s1.empty());
    TEST(s1.size() == 1);
    
    // 插入重复元素 / Insert duplicate element
    auto result2 = s1.insert(10);
    TEST(!result2.second);  // 插入失败 / Insert failed
    TEST(*result2.first == 10);
    TEST(s1.size() == 1);
    
    // 插入更多元素 / Insert more elements
    s1.insert(5);
    s1.insert(15);
    s1.insert(3);
    s1.insert(7);
    s1.insert(12);
    s1.insert(18);
    
    TEST(s1.size() == 7);
    
    // 范围构造函数 / Range constructor
    mystl::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    
    mystl::set<int> s3(vec.begin(), vec.end());
    TEST(s3.size() == 5);
    TEST(s3.find(1) != s3.end());
    TEST(s3.find(5) != s3.end());
    TEST(s3.find(6) == s3.end());
    
    // 初始化列表构造函数 / Initializer list constructor
    mystl::set<int> s4({1, 2, 3, 4, 5});
    TEST(s4.size() == 5);
    
    // 拷贝构造函数 / Copy constructor
    mystl::set<int> s5(s4);
    TEST(s5.size() == 5);
    TEST(s5 == s4);
    
    // 移动构造函数 / Move constructor
    mystl::set<int> s6(std::move(s5));
    TEST(s6.size() == 5);
    TEST(s5.empty());  // s5应该为空 / s5 should be empty
    
    std::cout << "  All set constructor tests passed! / 所有集合构造函数测试通过！" << std::endl;
}

/**
 * @brief Test set find and count operations
 * @brief 测试集合查找和计数操作
 */
void test_set_find_count() {
    std::cout << "\n2. Testing set find and count operations: / 2. 测试集合查找和计数操作:" << std::endl;
    
    mystl::set<int> s;
    
    // 插入测试数据 / Insert test data
    for (int i = 0; i < 10; ++i) {
        s.insert(i * 2);  // 偶数 / Even numbers
    }
    
    TEST(s.size() == 10);
    
    // 测试查找存在的元素 / Test finding existing elements
    for (int i = 0; i < 10; ++i) {
        auto it = s.find(i * 2);
        TEST(it != s.end());
        TEST(*it == i * 2);
    }
    
    // 测试查找不存在的元素 / Test finding non-existing elements
    for (int i = 0; i < 10; ++i) {
        auto it = s.find(i * 2 + 1);  // 奇数 / Odd numbers
        TEST(it == s.end());
    }
    
    // 测试计数 / Test count
    for (int i = 0; i < 10; ++i) {
        TEST(s.count(i * 2) == 1);
        TEST(s.count(i * 2 + 1) == 0);
    }
    
    // 测试边界操作 / Test boundary operations
    auto lb = s.lower_bound(5);
    TEST(lb != s.end());
    TEST(*lb == 6);  // 第一个不小于5的元素是6 / First element not less than 5 is 6
    
    auto ub = s.upper_bound(5);
    TEST(ub != s.end());
    TEST(*ub == 6);  // 第一个大于5的元素是6 / First element greater than 5 is 6
    
    auto range = s.equal_range(6);
    TEST(range.first != s.end());
    TEST(*range.first == 6);
    TEST(range.second != s.end());
    TEST(*range.second == 8);
    
    std::cout << "  All set find/count tests passed! / 所有集合查找/计数测试通过！" << std::endl;
}

/**
 * @brief Test set erase operations
 * @brief 测试集合删除操作
 */
void test_set_erase() {
    std::cout << "\n3. Testing set erase operations: / 3. 测试集合删除操作:" << std::endl;
    
    mystl::set<int> s;
    
    // 插入测试数据 / Insert test data
    for (int i = 0; i < 10; ++i) {
        s.insert(i);
    }
    
    TEST(s.size() == 10);
    
    // 删除单个元素 / Erase single element
    size_t erased = s.erase(5);
    TEST(erased == 1);
    TEST(s.size() == 9);
    TEST(s.find(5) == s.end());
    
    // 删除不存在的元素 / Erase non-existing element
    erased = s.erase(100);
    TEST(erased == 0);
    TEST(s.size() == 9);
    
    // 通过迭代器删除 / Erase via iterator
    auto it = s.find(3);
    TEST(it != s.end());
    s.erase(it);
    TEST(s.size() == 8);
    TEST(s.find(3) == s.end());
    
    // 删除范围 / Erase range
    auto first = s.find(7);
    auto last = s.find(9);
    s.erase(first, last);
    TEST(s.size() == 6);  // 删除了7和8 / Erased 7 and 8
    
    // 清空集合 / Clear set
    s.clear();
    TEST(s.empty());
    TEST(s.size() == 0);
    
    std::cout << "  All set erase tests passed! / 所有集合删除测试通过！" << std::endl;
}

/**
 * @brief Test set iterator functionality
 * @brief 测试集合迭代器功能
 */
void test_set_iterator() {
    std::cout << "\n4. Testing set iterator functionality: / 4. 测试集合迭代器功能:" << std::endl;
    
    mystl::set<int> s;
    
    // 插入测试数据 / Insert test data
    mystl::vector<int> values;
    values.push_back(5);
    values.push_back(3);
    values.push_back(7);
    values.push_back(1);
    values.push_back(4);
    values.push_back(6);
    values.push_back(8);
    values.push_back(2);
    
    for (size_t i = 0; i < values.size(); ++i) {
        s.insert(values[i]);
    }
    
    TEST(s.size() == 8);
    
    // 手动排序values / Manually sort values
    for (size_t i = 0; i < values.size(); ++i) {
        for (size_t j = i + 1; j < values.size(); ++j) {
            if (values[j] < values[i]) {
                int temp = values[i];
                values[i] = values[j];
                values[j] = temp;
            }
        }
    }
    
    // 测试正向迭代 / Test forward iteration
    auto it = s.begin();
    for (size_t i = 0; i < values.size(); ++i) {
        TEST(it != s.end());
        TEST(*it == values[i]);
        ++it;
    }
    TEST(it == s.end());
    
    // 测试范围for循环 / Test range-based for loop
    size_t count = 0;
    for (int val : s) {
        TEST(val == values[count]);
        ++count;
    }
    TEST(count == 8);
    
    // 测试反向迭代器 / Test reverse iterator
    count = values.size() - 1;
    for (auto rit = s.rbegin(); rit != s.rend(); ++rit) {
        TEST(*rit == values[count]);
        --count;
    }
    
    std::cout << "  All set iterator tests passed! / 所有集合迭代器测试通过！" << std::endl;
}

// ==================== Test Functions for multiset ====================
// ==================== multiset 测试函数 ====================

/**
 * @brief Test multiset constructors and basic operations
 * @brief 测试多重集合构造函数和基本操作
 */
void test_multiset_constructors() {
    std::cout << "\n5. Testing multiset constructors and basic operations: / 5. 测试多重集合构造函数和基本操作:" << std::endl;
    
    // 默认构造函数 / Default constructor
    mystl::multiset<int> ms1;
    TEST(ms1.empty());
    TEST(ms1.size() == 0);
    TEST(ms1.begin() == ms1.end());
    
    // 插入元素 / Insert elements
    auto it1 = ms1.insert(10);
    TEST(*it1 == 10);
    TEST(!ms1.empty());
    TEST(ms1.size() == 1);
    
    // 插入重复元素 / Insert duplicate element
    auto it2 = ms1.insert(10);
    TEST(*it2 == 10);
    TEST(ms1.size() == 2);  // 允许重复 / Allows duplicates
    
    // 插入更多元素 / Insert more elements
    ms1.insert(5);
    ms1.insert(15);
    ms1.insert(3);
    ms1.insert(7);
    ms1.insert(12);
    ms1.insert(18);
    
    TEST(ms1.size() == 8);  // 总共8个元素 / Total 8 elements
    
    // 范围构造函数 / Range constructor
    mystl::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(2);  // 重复元素 / Duplicate element
    vec.push_back(3);
    vec.push_back(3);
    vec.push_back(3);  // 多个重复 / Multiple duplicates
    
    mystl::multiset<int> ms2(vec.begin(), vec.end());
    TEST(ms2.size() == 6);
    TEST(ms2.count(1) == 1);
    TEST(ms2.count(2) == 2);
    TEST(ms2.count(3) == 3);
    
    // 初始化列表构造函数 / Initializer list constructor
    mystl::multiset<int> ms3({1, 2, 2, 3, 3, 3});
    TEST(ms3.size() == 6);
    
    std::cout << "  All multiset constructor tests passed! / 所有多重集合构造函数测试通过！" << std::endl;
}

/**
 * @brief Test multiset find and count operations
 * @brief 测试多重集合查找和计数操作
 */
void test_multiset_find_count() {
    std::cout << "\n6. Testing multiset find and count operations: / 6. 测试多重集合查找和计数操作:" << std::endl;
    
    mystl::multiset<int> ms;
    
    // 插入测试数据（包含重复） / Insert test data (with duplicates)
    for (int i = 0; i < 5; ++i) {
        ms.insert(i);
        ms.insert(i);  // 每个元素插入两次 / Insert each element twice
    }
    
    TEST(ms.size() == 10);
    
    // 测试查找存在的元素 / Test finding existing elements
    for (int i = 0; i < 5; ++i) {
        auto it = ms.find(i);
        TEST(it != ms.end());
        TEST(*it == i);
    }
    
    // 测试计数 / Test count
    for (int i = 0; i < 5; ++i) {
        TEST(ms.count(i) == 2);  // 每个元素出现两次 / Each element appears twice
    }
    
    // 测试不存在的元素 / Test non-existing elements
    TEST(ms.count(10) == 0);
    TEST(ms.find(10) == ms.end());
    
    // 测试边界操作 / Test boundary operations
    ms.insert(5);
    ms.insert(5);
    ms.insert(5);  // 插入3个5 / Insert three 5s
    
    auto lb = ms.lower_bound(5);
    TEST(lb != ms.end());
    TEST(*lb == 5);
    
    auto ub = ms.upper_bound(5);
    // 没有大于5的元素，所以ub应该是end()
    // No element greater than 5, so ub should be end()
    TEST(ub == ms.end());
    
    auto range = ms.equal_range(5);
    size_t count = 0;
    for (auto it = range.first; it != range.second; ++it) {
        TEST(*it == 5);
        ++count;
    }
    TEST(count == 3);  // 应该有3个5 / Should have three 5s
    
    std::cout << "  All multiset find/count tests passed! / 所有多重集合查找/计数测试通过！" << std::endl;
}

/**
 * @brief Test multiset erase operations
 * @brief 测试多重集合删除操作
 */
void test_multiset_erase() {
    std::cout << "\n7. Testing multiset erase operations: / 7. 测试多重集合删除操作:" << std::endl;
    
    mystl::multiset<int> ms;
    
    // 插入测试数据（包含重复） / Insert test data (with duplicates)
    for (int i = 0; i < 5; ++i) {
        ms.insert(i);
        ms.insert(i);  // 每个元素插入两次 / Insert each element twice
    }
    
    TEST(ms.size() == 10);
    
    // 删除单个元素（删除所有等于该值的元素） / Erase single element (erase all elements with that value)
    size_t erased = ms.erase(2);
    TEST(erased == 2);  // 删除了2个2 / Erased two 2s
    TEST(ms.size() == 8);
    TEST(ms.find(2) == ms.end());
    
    // 删除不存在的元素 / Erase non-existing element
    erased = ms.erase(100);
    TEST(erased == 0);
    TEST(ms.size() == 8);
    
    // 通过迭代器删除 / Erase via iterator
    auto it = ms.find(3);
    TEST(it != ms.end());
    ms.erase(it);
    TEST(ms.size() == 7);
    TEST(ms.count(3) == 1);  // 还剩下一个3 / One 3 remains
    
    // 删除范围 / Erase range
    auto first = ms.find(0);
    auto last = ms.find(4);
    ms.erase(first, last);
    TEST(ms.size() == 3);  // 删除了0, 1, 3 / Erased 0, 1, 3
    
    // 清空多重集合 / Clear multiset
    ms.clear();
    TEST(ms.empty());
    TEST(ms.size() == 0);
    
    std::cout << "  All multiset erase tests passed! / 所有多重集合删除测试通过！" << std::endl;
}

// ==================== Test Functions for set operations ====================
// ==================== 集合操作测试函数 ====================

/**
 * @brief Test set operation algorithms
 * @brief 测试集合操作算法
 */
void test_set_operations() {
    std::cout << "\n8. Testing set operation algorithms: / 8. 测试集合操作算法:" << std::endl;
    
    mystl::vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);
    v1.push_back(4);
    v1.push_back(5);
    
    mystl::vector<int> v2;
    v2.push_back(3);
    v2.push_back(4);
    v2.push_back(5);
    v2.push_back(6);
    v2.push_back(7);
    
    // 测试并集 / Test union
    mystl::vector<int> result_union;
    mystl::set_union(v1.begin(), v1.end(),
                     v2.begin(), v2.end(),
                     std::back_inserter(result_union));
    
    TEST(result_union.size() == 7);
    TEST(result_union[0] == 1);
    TEST(result_union[1] == 2);
    TEST(result_union[2] == 3);
    TEST(result_union[3] == 4);
    TEST(result_union[4] == 5);
    TEST(result_union[5] == 6);
    TEST(result_union[6] == 7);
    
    // 测试交集 / Test intersection
    mystl::vector<int> result_intersection;
    mystl::set_intersection(v1.begin(), v1.end(),
                            v2.begin(), v2.end(),
                            std::back_inserter(result_intersection));
    
    TEST(result_intersection.size() == 3);
    TEST(result_intersection[0] == 3);
    TEST(result_intersection[1] == 4);
    TEST(result_intersection[2] == 5);
    
    // 测试差集 / Test difference
    mystl::vector<int> result_difference;
    mystl::set_difference(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(result_difference));
    
    TEST(result_difference.size() == 2);
    TEST(result_difference[0] == 1);
    TEST(result_difference[1] == 2);
    
    // 测试对称差集 / Test symmetric difference
    mystl::vector<int> result_symmetric_difference;
    mystl::set_symmetric_difference(v1.begin(), v1.end(),
                                    v2.begin(), v2.end(),
                                    std::back_inserter(result_symmetric_difference));
    
    TEST(result_symmetric_difference.size() == 4);
    TEST(result_symmetric_difference[0] == 1);
    TEST(result_symmetric_difference[1] == 2);
    TEST(result_symmetric_difference[2] == 6);
    TEST(result_symmetric_difference[3] == 7);
    
    // 测试子集检查 / Test subset check
    mystl::vector<int> subset = {3, 4, 5};
    TEST(mystl::includes(v1.begin(), v1.end(),
                         subset.begin(), subset.end()));
    
    mystl::vector<int> not_subset = {3, 4, 6};
    TEST(!mystl::includes(v1.begin(), v1.end(),
                          not_subset.begin(), not_subset.end()));
    
    std::cout << "  All set operation tests passed! / 所有集合操作测试通过！" << std::endl;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 */
int main() {
    std::cout << "=== Testing mystl set and multiset === / === 测试 mystl 集合和多重集合 ===" << std::endl;
    std::cout << std::endl;
    
    try {
        test_set_constructors();
        test_set_find_count();
        test_set_erase();
        test_set_iterator();
        test_multiset_constructors();
        test_multiset_find_count();
        test_multiset_erase();
        test_set_operations();
        
        std::cout << std::endl;
        std::cout << "=== All set and multiset tests passed successfully! / 所有集合和多重集合测试成功通过！ ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        std::cerr << "测试失败，异常信息: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        std::cerr << "测试失败，未知异常" << std::endl;
        return 1;
    }
}
