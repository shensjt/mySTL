/**
 * @file test_map.cpp
 * @brief Comprehensive test suite for mystl::map implementation
 * @brief mystl::map 实现的全面测试套件
 * 
 * This file contains extensive tests for the map container in mystl.
 * It verifies the correctness of map operations including construction,
 * element access, modification, iterator functionality, and comparison operations.
 * 
 * 此文件包含对 mystl 中 map 容器的广泛测试。
 * 它验证 map 操作的正确性，包括构造、元素访问、修改、迭代器功能和比较操作。
 * 
 * The tests are organized into logical sections corresponding to different
 * aspects of map functionality.
 * 
 * 测试按 map 功能的不同方面组织成逻辑部分。
 * 
 * @author shensjt
 * @date 2026-03-30
 * 
 * @see include/mystl/map.hpp
 * @see 参考：include/mystl/map.hpp
 */

#include "../../include/mystl/map.hpp"
#include "../../include/mystl/vector.hpp"
#include <iostream>
#include <string>
#include <cassert>

// Helper macro for testing / 测试辅助宏
#define TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            std::cerr << "测试失败，行号 " << __LINE__ << ": " << #expr << std::endl; \
            assert(false && #expr); \
        } \
    } while(0)

using namespace mystl;

/**
 * @brief Test basic map operations
 * @brief 测试基本map操作
 */
void test_map_basic() {
    std::cout << "=== Testing basic map operations === / === 测试基本map操作 ===" << std::endl;
    
    // 默认构造函数
    map<int, std::string> m1;
    assert(m1.empty());
    assert(m1.size() == 0);
    
    // 插入元素
    auto result1 = m1.insert({1, "one"});
    assert(result1.second);
    assert(result1.first->first == 1);
    assert(result1.first->second == "one");
    assert(m1.size() == 1);
    
    // 插入重复键
    auto result2 = m1.insert({1, "ONE"});
    assert(!result2.second);  // 应该失败
    assert(m1.size() == 1);   // 大小不变
    assert(m1[1] == "one");   // 值不变
    
    // operator[] 访问和插入
    m1[2] = "two";
    assert(m1.size() == 2);
    assert(m1[2] == "two");
    
    // operator[] 访问不存在的键（插入默认值）
    std::string& value = m1[3];
    assert(value == "");  // 默认构造的std::string
    assert(m1.size() == 3);
    
    // at() 访问
    assert(m1.at(1) == "one");
    assert(m1.at(2) == "two");
    
    // at() 访问不存在的键（应该抛出异常）
    bool exception_thrown = false;
    try {
        m1.at(99);
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    
    // const at() 访问
    const map<int, std::string>& cm1 = m1;
    assert(cm1.at(1) == "one");
    
    std::cout << "Basic operations test passed! / 基本操作测试通过！" << std::endl;
}

/**
 * @brief Test map iteration
 * @brief 测试map迭代
 */
void test_map_iteration() {
    std::cout << "\n=== Testing map iteration === / === 测试map迭代 ===" << std::endl;
    
    map<int, std::string> m;
    m[1] = "one";
    m[2] = "two";
    m[3] = "three";
    m[4] = "four";
    
    // 正向迭代
    int expected_keys[] = {1, 2, 3, 4};
    std::string expected_values[] = {"one", "two", "three", "four"};
    int i = 0;
    
    for (auto it = m.begin(); it != m.end(); ++it) {
        assert(it->first == expected_keys[i]);
        assert(it->second == expected_values[i]);
        ++i;
    }
    assert(i == 4);
    
    // 常量迭代
    i = 0;
    const map<int, std::string>& cm = m;
    for (auto it = cm.begin(); it != cm.end(); ++it) {
        assert(it->first == expected_keys[i]);
        assert(it->second == expected_values[i]);
        ++i;
    }
    assert(i == 4);
    
    // 反向迭代
    i = 3;
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
        assert(it->first == expected_keys[i]);
        assert(it->second == expected_values[i]);
        --i;
    }
    assert(i == -1);
    
    std::cout << "Iteration test passed! / 迭代测试通过！" << std::endl;
}

/**
 * @brief Test map find operations
 * @brief 测试map查找操作
 */
void test_map_find() {
    std::cout << "\n=== Testing map find operations === / === 测试map查找操作 ===" << std::endl;
    
    map<int, std::string> m;
    m[1] = "one";
    m[2] = "two";
    m[3] = "three";
    
    // find 存在的键
    auto it1 = m.find(2);
    assert(it1 != m.end());
    assert(it1->first == 2);
    assert(it1->second == "two");
    
    // find 不存在的键
    auto it2 = m.find(99);
    assert(it2 == m.end());
    
    // count
    assert(m.count(1) == 1);
    assert(m.count(2) == 1);
    assert(m.count(99) == 0);
    
    // lower_bound
    auto lb1 = m.lower_bound(2);
    assert(lb1 != m.end());
    assert(lb1->first == 2);
    
    auto lb2 = m.lower_bound(0);
    assert(lb2 != m.end());
    assert(lb2->first == 1);
    
    auto lb3 = m.lower_bound(5);
    assert(lb3 == m.end());
    
    // upper_bound
    auto ub1 = m.upper_bound(2);
    assert(ub1 != m.end());
    assert(ub1->first == 3);
    
    auto ub2 = m.upper_bound(3);
    assert(ub2 == m.end());
    
    // equal_range
    auto range1 = m.equal_range(2);
    assert(range1.first != m.end());
    assert(range1.first->first == 2);
    assert(range1.second != m.end());
    assert(range1.second->first == 3);
    
    auto range2 = m.equal_range(99);
    assert(range2.first == m.end());
    assert(range2.second == m.end());
    
    std::cout << "Find operations test passed! / 查找操作测试通过！" << std::endl;
}

/**
 * @brief Test map erase operations
 * @brief 测试map删除操作
 */
void test_map_erase() {
    std::cout << "\n=== Testing map erase operations === / === 测试map删除操作 ===" << std::endl;
    
    map<int, std::string> m;
    for (int i = 1; i <= 10; ++i) {
        m[i] = "value" + std::to_string(i);
    }
    assert(m.size() == 10);
    
    // 通过键删除
    size_t erased = m.erase(5);
    assert(erased == 1);
    assert(m.size() == 9);
    assert(m.find(5) == m.end());
    
    // 删除不存在的键
    erased = m.erase(99);
    assert(erased == 0);
    assert(m.size() == 9);
    
    // 通过迭代器删除
    auto it = m.find(3);
    assert(it != m.end());
    auto next_it = m.erase(it);
    assert(m.size() == 8);
    assert(m.find(3) == m.end());
    
    // 通过范围删除
    auto first = m.find(7);
    auto last = m.find(10);
    m.erase(first, last);
    assert(m.size() == 5);  // 剩下 1, 2, 4, 6, 10
    
    // 清空
    m.clear();
    assert(m.empty());
    assert(m.size() == 0);
    
    std::cout << "Erase operations test passed! / 删除操作测试通过！" << std::endl;
}

/**
 * @brief Test map copy and move operations
 * @brief 测试map拷贝和移动操作
 */
void test_map_copy_move() {
    std::cout << "\n=== Testing map copy and move operations === / === 测试map拷贝和移动操作 ===" << std::endl;
    
    map<int, std::string> m1;
    m1[1] = "one";
    m1[2] = "two";
    m1[3] = "three";
    
    // 拷贝构造函数
    map<int, std::string> m2(m1);
    assert(m2.size() == 3);
    assert(m2[1] == "one");
    assert(m2[2] == "two");
    assert(m2[3] == "three");
    
    // 修改原map，确保深拷贝
    m1[1] = "ONE";
    assert(m2[1] == "one");  // m2不应受影响
    
    // 拷贝赋值运算符
    map<int, std::string> m3;
    m3 = m2;
    assert(m3.size() == 3);
    assert(m3[1] == "one");
    
    // 移动构造函数
    map<int, std::string> m4(std::move(m3));
    assert(m4.size() == 3);
    assert(m4[1] == "one");
    assert(m3.empty());  // m3应该被移动
    
    // 移动赋值运算符
    map<int, std::string> m5;
    m5 = std::move(m4);
    assert(m5.size() == 3);
    assert(m5[1] == "one");
    assert(m4.empty());  // m4应该被移动
    
    std::cout << "Copy and move operations test passed! / 拷贝和移动操作测试通过！" << std::endl;
}

/**
 * @brief Test map comparison operators
 * @brief 测试map比较操作符
 */
void test_map_comparison() {
    std::cout << "\n=== Testing map comparison operators === / === 测试map比较操作符 ===" << std::endl;
    
    map<int, std::string> m1;
    m1[1] = "one";
    m1[2] = "two";
    
    map<int, std::string> m2;
    m2[1] = "one";
    m2[2] = "two";
    
    map<int, std::string> m3;
    m3[1] = "one";
    m3[2] = "two";
    m3[3] = "three";
    
    map<int, std::string> m4;
    m4[1] = "one";
    m4[2] = "TWO";  // 不同的值
    
    // 相等比较
    assert(m1 == m2);
    assert(!(m1 == m3));
    assert(!(m1 == m4));
    
    // 不等比较
    assert(m1 != m3);
    assert(m1 != m4);
    assert(!(m1 != m2));
    
    // 小于比较
    assert(m1 < m3);   // m1是m3的真子集
    assert(!(m3 < m1));
    assert(!(m1 < m2)); // 相等，不小于
    
    // 其他比较
    assert(m1 <= m2);
    assert(m1 <= m3);
    assert(m3 > m1);
    assert(m3 >= m1);
    
    std::cout << "Comparison operators test passed! / 比较操作符测试通过！" << std::endl;
}

/**
 * @brief Test map with custom comparator
 * @brief 测试带自定义比较器的map
 */
void test_map_custom_comparator() {
    std::cout << "\n=== Testing map with custom comparator === / === 测试带自定义比较器的map ===" << std::endl;
    
    // 使用greater作为比较器（降序）
    map<int, std::string, std::greater<int>> m;
    m[3] = "three";
    m[1] = "one";
    m[2] = "two";
    
    // 验证顺序是降序
    vector<int> keys;
    for (const auto& p : m) {
        keys.push_back(p.first);
    }
    
    assert(keys.size() == 3);
    assert(keys[0] == 3);
    assert(keys[1] == 2);
    assert(keys[2] == 1);
    
    std::cout << "Custom comparator test passed! / 自定义比较器测试通过！" << std::endl;
}

/**
 * @brief Test map emplace operations
 * @brief 测试map原位构造操作
 */
void test_map_emplace() {
    std::cout << "\n=== Testing map emplace operations === / === 测试map原位构造操作 ===" << std::endl;
    
    map<int, std::string> m;
    
    // emplace
    auto result1 = m.emplace(1, "one");
    assert(result1.second);
    assert(result1.first->first == 1);
    assert(result1.first->second == "one");
    
    // emplace 重复键
    auto result2 = m.emplace(1, "ONE");
    assert(!result2.second);
    assert(m[1] == "one");
    
    // emplace_hint
    auto hint = m.find(1);
    auto result3 = m.emplace_hint(hint, 2, "two");
    assert(result3->first == 2);
    assert(result3->second == "two");
    
    assert(m.size() == 2);
    
    std::cout << "Emplace operations test passed! / 原位构造操作测试通过！" << std::endl;
}

/**
 * @brief Test map swap operation
 * @brief 测试map交换操作
 */
void test_map_swap() {
    std::cout << "\n=== Testing map swap operation === / === 测试map交换操作 ===" << std::endl;
    
    map<int, std::string> m1;
    m1[1] = "one";
    m1[2] = "two";
    
    map<int, std::string> m2;
    m2[3] = "three";
    m2[4] = "four";
    m2[5] = "five";
    
    // 交换前
    assert(m1.size() == 2);
    assert(m2.size() == 3);
    
    // 交换
    m1.swap(m2);
    
    // 交换后
    assert(m1.size() == 3);
    assert(m2.size() == 2);
    assert(m1.find(3) != m1.end());
    assert(m2.find(1) != m2.end());
    
    // 全局swap函数
    swap(m1, m2);
    assert(m1.size() == 2);
    assert(m2.size() == 3);
    
    std::cout << "Swap operation test passed! / 交换操作测试通过！" << std::endl;
}

/**
 * @brief Test map initializer list
 * @brief 测试map初始化列表
 */
void test_map_initializer_list() {
    std::cout << "\n=== Testing map initializer list === / === 测试map初始化列表 ===" << std::endl;
    
    // 初始化列表构造函数
    map<int, std::string> m = {
        {1, "one"},
        {2, "two"},
        {3, "three"}
    };
    
    assert(m.size() == 3);
    assert(m[1] == "one");
    assert(m[2] == "two");
    assert(m[3] == "three");
    
    // 初始化列表赋值
    m = {
        {4, "four"},
        {5, "five"}
    };
    
    assert(m.size() == 2);
    assert(m[4] == "four");
    assert(m[5] == "five");
    assert(m.find(1) == m.end());  // 旧元素应该被清除
    
    std::cout << "Initializer list test passed! / 初始化列表测试通过！" << std::endl;
}

/**
 * @brief Test map value_comp and key_comp
 * @brief 测试map值比较和键比较
 */
void test_map_comparators() {
    std::cout << "\n=== Testing map value_comp and key_comp === / === 测试map值比较和键比较 ===" << std::endl;
    
    map<int, std::string> m;
    
    auto key_comp = m.key_comp();
    assert(key_comp(1, 2));   // 1 < 2
    assert(!key_comp(2, 1));  // 2 < 1 为假
    assert(!key_comp(1, 1));  // 1 < 1 为假
    
    auto value_comp = m.value_comp();
    assert(value_comp({1, "a"}, {2, "b"}));   // 比较键：1 < 2
    assert(!value_comp({2, "a"}, {1, "b"}));  // 比较键：2 < 1 为假
    
    std::cout << "Comparators test passed! / 比较器测试通过！" << std::endl;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 */
int main() {
    std::cout << "=== Testing mystl map === / === 测试 mystl map ===" << std::endl;
    std::cout << std::endl;
    
    try {
        test_map_basic();
        test_map_iteration();
        test_map_find();
        test_map_erase();
        test_map_copy_move();
        test_map_comparison();
        test_map_custom_comparator();
        test_map_emplace();
        test_map_swap();
        test_map_initializer_list();
        test_map_comparators();
        
        std::cout << "\n=== All map tests passed! / 所有map测试通过！ ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception caught during testing: " << e.what() << std::endl;
        std::cerr << "测试过程中捕获异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught during testing" << std::endl;
        std::cerr << "测试过程中捕获未知异常" << std::endl;
        return 1;
    }
}
