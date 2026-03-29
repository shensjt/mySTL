/**
 * @file test_multimap.cpp
 * @brief Comprehensive test suite for mystl::multimap implementation
 * @brief mystl::multimap 实现的全面测试套件
 * 
 * This file contains extensive tests for the multimap container in mystl.
 * It verifies the correctness of multimap operations including construction,
 * element access with duplicate keys, modification, iterator functionality, and comparison operations.
 * 
 * 此文件包含对 mystl 中 multimap 容器的广泛测试。
 * 它验证 multimap 操作的正确性，包括构造、带重复键的元素访问、修改、迭代器功能和比较操作。
 * 
 * The tests are organized into logical sections corresponding to different
 * aspects of multimap functionality, with special focus on duplicate key handling.
 * 
 * 测试按 multimap 功能的不同方面组织成逻辑部分，特别关注重复键处理。
 * 
 * @author shensjt
 * @date 2026-03-30
 * 
 * @see include/mystl/multimap.hpp
 * @see 参考：include/mystl/multimap.hpp
 */

#include "../../include/mystl/multimap.hpp"
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
 * @brief Test basic multimap operations
 * @brief 测试基本multimap操作
 */
void test_multimap_basic() {
    std::cout << "=== Testing basic multimap operations === / === 测试基本multimap操作 ===" << std::endl;
    
    // 默认构造函数
    multimap<int, std::string> mm1;
    assert(mm1.empty());
    assert(mm1.size() == 0);
    
    // 插入元素
    auto it1 = mm1.insert({1, "one"});
    assert(it1 != mm1.end());
    assert(it1->first == 1);
    assert(it1->second == "one");
    assert(mm1.size() == 1);
    
    // 插入重复键（multimap允许）
    auto it2 = mm1.insert({1, "ONE"});
    assert(it2 != mm1.end());
    assert(it2->first == 1);
    assert(it2->second == "ONE");
    assert(mm1.size() == 2);  // 大小增加
    
    // 插入不同键
    mm1.insert({2, "two"});
    assert(mm1.size() == 3);
    
    std::cout << "Basic operations test passed! / 基本操作测试通过！" << std::endl;
}

/**
 * @brief Test multimap iteration with duplicate keys
 * @brief 测试带重复键的multimap迭代
 */
void test_multimap_iteration() {
    std::cout << "\n=== Testing multimap iteration with duplicate keys === / === 测试带重复键的multimap迭代 ===" << std::endl;
    
    multimap<int, std::string> mm;
    mm.insert({1, "one-a"});
    mm.insert({2, "two-a"});
    mm.insert({1, "one-b"});  // 重复键
    mm.insert({3, "three"});
    mm.insert({2, "two-b"});  // 重复键
    
    assert(mm.size() == 5);
    
    // 收集所有键值对
    vector<pair<int, std::string>> pairs;
    for (const auto& p : mm) {
        pairs.push_back(pair<int, std::string>(p.first, p.second));
    }
    
    // 验证顺序和内容
    assert(pairs.size() == 5);
    
    // 验证键的顺序
    assert(pairs[0].first == 1);
    assert(pairs[1].first == 1);  // 相同键连续
    assert(pairs[2].first == 2);
    assert(pairs[3].first == 2);  // 相同键连续
    assert(pairs[4].first == 3);
    
    // 验证值
    // 注意：相同键的值按插入顺序（实际实现可能不同，但至少应该存在）
    bool found_one_a = false, found_one_b = false;
    bool found_two_a = false, found_two_b = false;
    
    for (const auto& p : pairs) {
        if (p.first == 1) {
            if (p.second == "one-a") found_one_a = true;
            if (p.second == "one-b") found_one_b = true;
        } else if (p.first == 2) {
            if (p.second == "two-a") found_two_a = true;
            if (p.second == "two-b") found_two_b = true;
        } else if (p.first == 3) {
            assert(p.second == "three");
        }
    }
    
    assert(found_one_a && found_one_b);
    assert(found_two_a && found_two_b);
    
    // 反向迭代
    int key_count = 0;
    for (auto it = mm.rbegin(); it != mm.rend(); ++it) {
        ++key_count;
    }
    assert(key_count == 5);
    
    std::cout << "Iteration test passed! / 迭代测试通过！" << std::endl;
}

/**
 * @brief Test multimap find operations with duplicate keys
 * @brief 测试带重复键的multimap查找操作
 */
void test_multimap_find() {
    std::cout << "\n=== Testing multimap find operations with duplicate keys === / === 测试带重复键的multimap查找操作 ===" << std::endl;
    
    multimap<int, std::string> mm;
    mm.insert({1, "one-a"});
    mm.insert({2, "two-a"});
    mm.insert({1, "one-b"});
    mm.insert({2, "two-b"});
    mm.insert({2, "two-c"});
    
    // find 返回第一个匹配的迭代器
    auto it1 = mm.find(1);
    assert(it1 != mm.end());
    assert(it1->first == 1);
    // 可能是 "one-a" 或 "one-b"
    assert(it1->second == "one-a" || it1->second == "one-b");
    
    // find 不存在的键
    auto it2 = mm.find(99);
    assert(it2 == mm.end());
    
    // count 重复键
    assert(mm.count(1) == 2);
    assert(mm.count(2) == 3);
    assert(mm.count(3) == 0);
    assert(mm.count(99) == 0);
    
    // lower_bound
    auto lb1 = mm.lower_bound(2);
    assert(lb1 != mm.end());
    assert(lb1->first == 2);
    
    // upper_bound
    auto ub1 = mm.upper_bound(2);
    assert(ub1 == mm.end());  // 2之后没有更大的键
    
    auto ub2 = mm.upper_bound(1);
    assert(ub2 != mm.end());
    assert(ub2->first == 2);  // 1之后是2
    
    // equal_range 对于重复键
    auto range1 = mm.equal_range(2);
    auto start = range1.first;
    auto end = range1.second;
    
    int count = 0;
    while (start != end) {
        assert(start->first == 2);
        ++count;
        ++start;
    }
    assert(count == 3);  // 应该有3个键为2的元素
    
    // equal_range 对于不存在的键
    auto range2 = mm.equal_range(99);
    assert(range2.first == mm.end());
    assert(range2.second == mm.end());
    
    std::cout << "Find operations test passed! / 查找操作测试通过！" << std::endl;
}

/**
 * @brief Test multimap erase operations with duplicate keys
 * @brief 测试带重复键的multimap删除操作
 */
void test_multimap_erase() {
    std::cout << "\n=== Testing multimap erase operations with duplicate keys === / === 测试带重复键的multimap删除操作 ===" << std::endl;
    
    multimap<int, std::string> mm;
    for (int i = 1; i <= 3; ++i) {
        for (int j = 1; j <= 3; ++j) {  // 每个键有3个值
            mm.insert({i, "value" + std::to_string(i) + "-" + std::to_string(j)});
        }
    }
    assert(mm.size() == 9);
    
    // 通过键删除（删除所有匹配的键）
    size_t erased = mm.erase(2);
    assert(erased == 3);  // 删除了3个元素
    assert(mm.size() == 6);
    assert(mm.count(2) == 0);
    
    // 删除不存在的键
    erased = mm.erase(99);
    assert(erased == 0);
    assert(mm.size() == 6);
    
    // 通过迭代器删除
    auto it = mm.find(1);
    assert(it != mm.end());
    auto next_it = mm.erase(it);
    assert(mm.size() == 5);
    assert(mm.count(1) == 2);  // 原来有3个，删除1个，剩下2个
    
    // 通过范围删除
    auto first = mm.lower_bound(3);
    auto last = mm.upper_bound(3);
    mm.erase(first, last);
    assert(mm.size() == 2);  // 只剩下键为1的2个元素
    assert(mm.count(3) == 0);
    
    // 清空
    mm.clear();
    assert(mm.empty());
    assert(mm.size() == 0);
    
    std::cout << "Erase operations test passed! / 删除操作测试通过！" << std::endl;
}

/**
 * @brief Test multimap copy and move operations
 * @brief 测试multimap拷贝和移动操作
 */
void test_multimap_copy_move() {
    std::cout << "\n=== Testing multimap copy and move operations === / === 测试multimap拷贝和移动操作 ===" << std::endl;
    
    multimap<int, std::string> mm1;
    mm1.insert({1, "one-a"});
    mm1.insert({1, "one-b"});
    mm1.insert({2, "two"});
    
    // 拷贝构造函数
    multimap<int, std::string> mm2(mm1);
    assert(mm2.size() == 3);
    assert(mm2.count(1) == 2);
    assert(mm2.count(2) == 1);
    
    // 修改原multimap，确保深拷贝
    mm1.insert({3, "three"});
    assert(mm2.size() == 3);  // mm2不应受影响
    assert(mm2.count(3) == 0);
    
    // 拷贝赋值运算符
    multimap<int, std::string> mm3;
    mm3 = mm2;
    assert(mm3.size() == 3);
    assert(mm3.count(1) == 2);
    
    // 移动构造函数
    multimap<int, std::string> mm4(std::move(mm3));
    assert(mm4.size() == 3);
    assert(mm4.count(1) == 2);
    assert(mm3.empty());  // mm3应该被移动
    
    // 移动赋值运算符
    multimap<int, std::string> mm5;
    mm5 = std::move(mm4);
    assert(mm5.size() == 3);
    assert(mm5.count(1) == 2);
    assert(mm4.empty());  // mm4应该被移动
    
    std::cout << "Copy and move operations test passed! / 拷贝和移动操作测试通过！" << std::endl;
}

/**
 * @brief Test multimap comparison operators
 * @brief 测试multimap比较操作符
 */
void test_multimap_comparison() {
    std::cout << "\n=== Testing multimap comparison operators === / === 测试multimap比较操作符 ===" << std::endl;
    
    multimap<int, std::string> mm1;
    mm1.insert({1, "one"});
    mm1.insert({2, "two"});
    mm1.insert({2, "TWO"});  // 重复键
    
    multimap<int, std::string> mm2;
    mm2.insert({1, "one"});
    mm2.insert({2, "two"});
    mm2.insert({2, "TWO"});
    
    multimap<int, std::string> mm3;
    mm3.insert({1, "one"});
    mm3.insert({2, "two"});  // 只有一个键为2的元素
    
    multimap<int, std::string> mm4;
    mm4.insert({1, "one"});
    mm4.insert({2, "two"});
    mm4.insert({2, "two"});  // 相同的值
    
    // 相等比较
    assert(mm1 == mm2);
    assert(!(mm1 == mm3));  // 不同数量的重复键
    assert(!(mm1 == mm4));  // 相同的键但值不同
    
    // 不等比较
    assert(mm1 != mm3);
    assert(mm1 != mm4);
    assert(!(mm1 != mm2));
    
    // 小于比较（字典序）
    assert(mm3 < mm1);   // mm3是mm1的真子集
    assert(!(mm1 < mm3));
    
    // 其他比较
    assert(mm1 <= mm2);
    assert(mm3 <= mm1);
    assert(mm1 > mm3);
    assert(mm1 >= mm3);
    
    std::cout << "Comparison operators test passed! / 比较操作符测试通过！" << std::endl;
}

/**
 * @brief Test multimap with custom comparator
 * @brief 测试带自定义比较器的multimap
 */
void test_multimap_custom_comparator() {
    std::cout << "\n=== Testing multimap with custom comparator === / === 测试带自定义比较器的multimap ===" << std::endl;
    
    // 使用greater作为比较器（降序）
    multimap<int, std::string, std::greater<int>> mm;
    mm.insert({3, "three-a"});
    mm.insert({1, "one"});
    mm.insert({3, "three-b"});  // 重复键
    mm.insert({2, "two"});
    
    // 验证顺序是降序，相同键连续
    vector<int> keys;
    for (const auto& p : mm) {
        keys.push_back(p.first);
    }
    
    assert(keys.size() == 4);
    assert(keys[0] == 3);
    assert(keys[1] == 3);  // 相同键连续
    assert(keys[2] == 2);
    assert(keys[3] == 1);
    
    std::cout << "Custom comparator test passed! / 自定义比较器测试通过！" << std::endl;
}

/**
 * @brief Test multimap emplace operations
 * @brief 测试multimap原位构造操作
 */
void test_multimap_emplace() {
    std::cout << "\n=== Testing multimap emplace operations === / === 测试multimap原位构造操作 ===" << std::endl;
    
    multimap<int, std::string> mm;
    
    // emplace
    auto it1 = mm.emplace(1, "one");
    assert(it1 != mm.end());
    assert(it1->first == 1);
    assert(it1->second == "one");
    
    // emplace 重复键（multimap允许）
    auto it2 = mm.emplace(1, "ONE");
    assert(it2 != mm.end());
    assert(it2->first == 1);
    assert(it2->second == "ONE");
    
    // emplace_hint
    auto hint = mm.find(1);
    auto it3 = mm.emplace_hint(hint, 2, "two");
    assert(it3->first == 2);
    assert(it3->second == "two");
    
    assert(mm.size() == 3);
    
    std::cout << "Emplace operations test passed! / 原位构造操作测试通过！" << std::endl;
}

/**
 * @brief Test multimap swap operation
 * @brief 测试multimap交换操作
 */
void test_multimap_swap() {
    std::cout << "\n=== Testing multimap swap operation === / === 测试multimap交换操作 ===" << std::endl;
    
    multimap<int, std::string> mm1;
    mm1.insert({1, "one-a"});
    mm1.insert({1, "one-b"});
    
    multimap<int, std::string> mm2;
    mm2.insert({3, "three"});
    mm2.insert({4, "four-a"});
    mm2.insert({4, "four-b"});
    
    // 交换前
    assert(mm1.size() == 2);
    assert(mm2.size() == 3);
    
    // 交换
    mm1.swap(mm2);
    
    // 交换后
    assert(mm1.size() == 3);
    assert(mm2.size() == 2);
    assert(mm1.count(4) == 2);  // 重复键
    assert(mm2.count(1) == 2);  // 重复键
    
    // 全局swap函数
    swap(mm1, mm2);
    assert(mm1.size() == 2);
    assert(mm2.size() == 3);
    
    std::cout << "Swap operation test passed! / 交换操作测试通过！" << std::endl;
}

/**
 * @brief Test multimap initializer list
 * @brief 测试multimap初始化列表
 */
void test_multimap_initializer_list() {
    std::cout << "\n=== Testing multimap initializer list === / === 测试multimap初始化列表 ===" << std::endl;
    
    // 初始化列表构造函数（允许重复键）
    multimap<int, std::string> mm = {
        {1, "one-a"},
        {1, "one-b"},  // 重复键
        {2, "two"},
        {2, "TWO"},    // 重复键
        {3, "three"}
    };
    
    assert(mm.size() == 5);
    assert(mm.count(1) == 2);
    assert(mm.count(2) == 2);
    assert(mm.count(3) == 1);
    
    // 初始化列表赋值
    mm = {
        {4, "four-a"},
        {4, "four-b"},  // 重复键
        {5, "five"}
    };
    
    assert(mm.size() == 3);
    assert(mm.count(4) == 2);
    assert(mm.count(5) == 1);
    assert(mm.count(1) == 0);  // 旧元素应该被清除
    
    std::cout << "Initializer list test passed! / 初始化列表测试通过！" << std::endl;
}

/**
 * @brief Test multimap value_comp and key_comp
 * @brief 测试multimap值比较和键比较
 */
void test_multimap_comparators() {
    std::cout << "\n=== Testing multimap value_comp and key_comp === / === 测试multimap值比较和键比较 ===" << std::endl;
    
    multimap<int, std::string> mm;
    
    auto key_comp = mm.key_comp();
    assert(key_comp(1, 2));   // 1 < 2
    assert(!key_comp(2, 1));  // 2 < 1 为假
    assert(!key_comp(1, 1));  // 1 < 1 为假
    
    auto value_comp = mm.value_comp();
    assert(value_comp(pair<const int, std::string>(1, "a"), 
                      pair<const int, std::string>(2, "b")));   // 比较键：1 < 2
    assert(!value_comp(pair<const int, std::string>(2, "a"), 
                       pair<const int, std::string>(1, "b")));  // 比较键：2 < 1 为假
    
    std::cout << "Comparators test passed! / 比较器测试通过！" << std::endl;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 */
int main() {
    std::cout << "=== Testing mystl multimap === / === 测试 mystl multimap ===" << std::endl;
    std::cout << std::endl;
    
    try {
        test_multimap_basic();
        test_multimap_iteration();
        test_multimap_find();
        test_multimap_erase();
        test_multimap_copy_move();
        test_multimap_comparison();
        test_multimap_custom_comparator();
        test_multimap_emplace();
        test_multimap_swap();
        test_multimap_initializer_list();
        test_multimap_comparators();
        
        std::cout << "\n=== All multimap tests passed! / 所有multimap测试通过！ ===" << std::endl;
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
