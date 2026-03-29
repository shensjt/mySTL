/**
 * @file test_rb_tree.cpp
 * @brief Comprehensive test suite for mystl::rb_tree implementation
 * @brief mystl::rb_tree 实现的全面测试套件
 * 
 * This file contains extensive tests for the red-black tree container in mystl.
 * It verifies the correctness of rb_tree operations including construction,
 * element access, modification, iterator functionality, and red-black tree properties.
 * 
 * 此文件包含对 mystl 中红黑树容器的广泛测试。
 * 它验证红黑树操作的正确性，包括构造、元素访问、修改、迭代器功能和红黑树性质。
 * 
 * The tests are organized into logical sections corresponding to different
 * aspects of rb_tree functionality.
 * 
 * 测试按红黑树功能的不同方面组织成逻辑部分。
 * 
 * @author shensjt
 * @date 2026-03-29
 * 
 * @see include/mystl/rb_tree.hpp
 * @see 参考：include/mystl/rb_tree.hpp
 */

#include "../../include/mystl/rb_tree.hpp"
#include "../../include/mystl/vector.hpp"
#include "../../include/algorithm/algorithm.hpp"
#include <iostream>
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

// ==================== Test Classes for Specific Testing ====================
// ==================== 用于特定测试的测试类 ====================

/**
 * @brief Simple test class with integer value
 * @brief 带有整数值的简单测试类
 */
class TestClass {
public:
    int value;
    
    TestClass() : value(0) {}
    explicit TestClass(int v) : value(v) {}
    
    bool operator<(const TestClass& other) const { return value < other.value; }
    bool operator==(const TestClass& other) const { return value == other.value; }
};

// ==================== Simple Pseudo-Random Number Generator ====================
// ==================== 简单伪随机数生成器 ====================

/**
 * @brief Simple pseudo-random number generator for testing
 * @brief 用于测试的简单伪随机数生成器
 */
class simple_rand {
private:
    unsigned long seed_;
    
public:
    explicit simple_rand(unsigned long seed = 12345) : seed_(seed) {}
    
    unsigned long operator()() {
        seed_ = seed_ * 1103515245 + 12345;
        return (seed_ / 65536) % 32768;
    }
    
    int operator()(int min, int max) {
        return min + (operator()() % (max - min + 1));
    }
};

// ==================== Helper Functions ====================
// ==================== 辅助函数 ====================

/**
 * @brief Check if value exists in vector
 * @brief 检查值是否存在于vector中
 */
template<typename T>
bool contains(const mystl::vector<T>& vec, const T& value) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Generate unique random numbers
 * @brief 生成唯一的随机数
 */
mystl::vector<int> generate_unique_numbers(int count, int min_val, int max_val) {
    mystl::vector<int> result;
    simple_rand rand_gen;
    
    while (result.size() < static_cast<size_t>(count)) {
        int val = rand_gen(min_val, max_val);
        if (!contains(result, val)) {
            result.push_back(val);
        }
    }
    
    return result;
}

// ==================== Test Functions ====================
// ==================== 测试函数 ====================

/**
 * @brief Test rb_tree constructors and basic operations
 * @brief 测试红黑树构造函数和基本操作
 */
void test_rb_tree_constructors() {
    std::cout << "1. Testing rb_tree constructors and basic operations: / 1. 测试红黑树构造函数和基本操作:" << std::endl;
    
    // 默认构造函数 / Default constructor
    mystl::rb_tree<int, int> tree1;
    TEST(tree1.empty());
    TEST(tree1.size() == 0);
    TEST(tree1.begin() == tree1.end());
    
    // 带比较函数的构造函数 / Constructor with comparison function
    mystl::less<int> comp;
    mystl::rb_tree<int, int, mystl::identity<int>, mystl::less<int>> tree2(comp);
    TEST(tree2.empty());
    
    // 插入元素 / Insert elements
    auto result1 = tree1.insert(10);
    TEST(result1.second);  // 插入成功 / Insert successful
    TEST(*result1.first == 10);
    TEST(!tree1.empty());
    TEST(tree1.size() == 1);
    
    // 插入重复元素 / Insert duplicate element
    auto result2 = tree1.insert(10);
    TEST(!result2.second);  // 插入失败 / Insert failed
    TEST(*result2.first == 10);
    TEST(tree1.size() == 1);
    
    // 插入更多元素 / Insert more elements
    tree1.insert(5);
    tree1.insert(15);
    tree1.insert(3);
    tree1.insert(7);
    tree1.insert(12);
    tree1.insert(18);
    
    TEST(tree1.size() == 7);
    
    std::cout << "  All constructor tests passed! / 所有构造函数测试通过！" << std::endl;
}

/**
 * @brief Test rb_tree find and count operations
 * @brief 测试红黑树查找和计数操作
 */
void test_rb_tree_find_count() {
    std::cout << "\n2. Testing rb_tree find and count operations: / 2. 测试红黑树查找和计数操作:" << std::endl;
    
    mystl::rb_tree<int, int> tree;
    
    // 插入测试数据 / Insert test data
    for (int i = 0; i < 10; ++i) {
        tree.insert(i * 2);  // 偶数 / Even numbers
    }
    
    TEST(tree.size() == 10);
    
    // 测试查找存在的元素 / Test finding existing elements
    for (int i = 0; i < 10; ++i) {
        auto it = tree.find(i * 2);
        TEST(it != tree.end());
        TEST(*it == i * 2);
    }
    
    // 测试查找不存在的元素 / Test finding non-existing elements
    for (int i = 0; i < 10; ++i) {
        auto it = tree.find(i * 2 + 1);  // 奇数 / Odd numbers
        TEST(it == tree.end());
    }
    
    // 测试计数 / Test count
    for (int i = 0; i < 10; ++i) {
        TEST(tree.count(i * 2) == 1);
        TEST(tree.count(i * 2 + 1) == 0);
    }
    
    std::cout << "  All find/count tests passed! / 所有查找/计数测试通过！" << std::endl;
}

/**
 * @brief Test rb_tree erase operations
 * @brief 测试红黑树删除操作
 */
void test_rb_tree_erase() {
    std::cout << "\n3. Testing rb_tree erase operations: / 3. 测试红黑树删除操作:" << std::endl;
    
    mystl::rb_tree<int, int> tree;
    
    // 插入测试数据 / Insert test data
    for (int i = 0; i < 10; ++i) {
        tree.insert(i);
    }
    
    TEST(tree.size() == 10);
    
    // 删除单个元素 / Erase single element
    size_t erased = tree.erase(5);
    TEST(erased == 1);
    TEST(tree.size() == 9);
    TEST(tree.find(5) == tree.end());
    
    // 删除不存在的元素 / Erase non-existing element
    erased = tree.erase(100);
    TEST(erased == 0);
    TEST(tree.size() == 9);
    
    // 通过迭代器删除 / Erase via iterator
    auto it = tree.find(3);
    TEST(it != tree.end());
    tree.erase(it);
    TEST(tree.size() == 8);
    TEST(tree.find(3) == tree.end());
    
    // 删除范围 / Erase range
    auto first = tree.find(7);
    auto last = tree.find(9);
    tree.erase(first, last);
    TEST(tree.size() == 6);  // 删除了7和8 / Erased 7 and 8
    
    // 清空树 / Clear tree
    tree.clear();
    TEST(tree.empty());
    TEST(tree.size() == 0);
    
    std::cout << "  All erase tests passed! / 所有删除测试通过！" << std::endl;
}

/**
 * @brief Test rb_tree copy and move operations
 * @brief 测试红黑树拷贝和移动操作
 */
void test_rb_tree_copy_move() {
    std::cout << "\n4. Testing rb_tree copy and move operations: / 4. 测试红黑树拷贝和移动操作:" << std::endl;
    
    mystl::rb_tree<int, int> tree1;
    
    // 插入测试数据 / Insert test data
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    
    // 拷贝构造 / Copy construction
    mystl::rb_tree<int, int> tree2(tree1);
    TEST(tree1.size() == 3);
    TEST(tree2.size() == 3);
    
    // 验证拷贝内容 / Verify copy content
    for (int i = 1; i <= 3; ++i) {
        TEST(tree2.find(i) != tree2.end());
    }
    
    // 拷贝赋值 / Copy assignment
    mystl::rb_tree<int, int> tree3;
    tree3 = tree1;
    TEST(tree3.size() == 3);
    
    // 移动构造 / Move construction
    mystl::rb_tree<int, int> tree4(std::move(tree1));
    TEST(tree1.empty());  // tree1应该为空 / tree1 should be empty
    TEST(tree4.size() == 3);
    
    // 移动赋值 / Move assignment
    mystl::rb_tree<int, int> tree5;
    tree5 = std::move(tree2);
    TEST(tree2.empty());  // tree2应该为空 / tree2 should be empty
    TEST(tree5.size() == 3);
    
    std::cout << "  All copy/move tests passed! / 所有拷贝/移动测试通过！" << std::endl;
}

/**
 * @brief Test rb_tree comparison operators
 * @brief 测试红黑树比较操作符
 */
void test_rb_tree_comparison() {
    std::cout << "\n5. Testing rb_tree comparison operators: / 5. 测试红黑树比较操作符:" << std::endl;
    
    mystl::rb_tree<int, int> tree1;
    mystl::rb_tree<int, int> tree2;
    
    // 插入相同元素 / Insert same elements
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    
    tree2.insert(1);
    tree2.insert(2);
    tree2.insert(3);
    
    // 测试相等 / Test equality
    TEST(tree1 == tree2);
    TEST(!(tree1 != tree2));
    
    // 修改tree2 / Modify tree2
    tree2.insert(4);
    TEST(tree1 != tree2);
    TEST(tree1 < tree2);   // tree1是tree2的前缀 / tree1 is prefix of tree2
    TEST(tree1 <= tree2);
    TEST(tree2 > tree1);
    TEST(tree2 >= tree1);
    
    std::cout << "  All comparison tests passed! / 所有比较测试通过！" << std::endl;
}

/**
 * @brief Test rb_tree swap operation
 * @brief 测试红黑树交换操作
 */
void test_rb_tree_swap() {
    std::cout << "\n6. Testing rb_tree swap operation: / 6. 测试红黑树交换操作:" << std::endl;
    
    mystl::rb_tree<int, int> tree1;
    mystl::rb_tree<int, int> tree2;
    
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    
    tree2.insert(4);
    tree2.insert(5);
    
    size_t size1 = tree1.size();
    size_t size2 = tree2.size();
    
    // 交换 / Swap
    tree1.swap(tree2);
    
    TEST(tree1.size() == size2);
    TEST(tree2.size() == size1);
    TEST(tree1.find(4) != tree1.end());
    TEST(tree2.find(1) != tree2.end());
    
    // 使用非成员函数swap / Use non-member function swap
    mystl::swap(tree1, tree2);
    TEST(tree1.size() == size1);
    TEST(tree2.size() == size2);
    
    std::cout << "  All swap tests passed! / 所有交换测试通过！" << std::endl;
}

/**
 * @brief Test rb_tree iterator functionality
 * @brief 测试红黑树迭代器功能
 */
void test_rb_tree_iterator() {
    std::cout << "\n7. Testing rb_tree iterator functionality: / 7. 测试红黑树迭代器功能:" << std::endl;
    
    mystl::rb_tree<int, int> tree;
    
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
        tree.insert(values[i]);
    }
    
    TEST(tree.size() == 8);
    
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
    auto it = tree.begin();
    for (size_t i = 0; i < values.size(); ++i) {
        TEST(it != tree.end());
        TEST(*it == values[i]);
        ++it;
    }
    TEST(it == tree.end());
    
    // 测试范围for循环 / Test range-based for loop
    size_t count = 0;
    for (int val : tree) {
        TEST(val == values[count]);
        ++count;
    }
    TEST(count == 8);
    
    std::cout << "  All iterator tests passed! / 所有迭代器测试通过！" << std::endl;
}

/**
 * @brief Test rb_tree boundary operations
 * @brief 测试红黑树边界操作
 */
void test_rb_tree_boundary() {
    std::cout << "\n8. Testing rb_tree boundary operations: / 8. 测试红黑树边界操作:" << std::endl;
    
    mystl::rb_tree<int, int> tree;
    
    // 测试空树的边界操作 / Test boundary operations on empty tree
    TEST(tree.lower_bound(42) == tree.end());
    TEST(tree.upper_bound(42) == tree.end());
    auto range = tree.equal_range(42);
    TEST(range.first == tree.end());
    TEST(range.second == tree.end());
    
    // 插入测试数据 / Insert test data
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);
    
    // 测试lower_bound / Test lower_bound
    auto lb = tree.lower_bound(25);
    TEST(lb != tree.end());
    TEST(*lb == 30);  // 第一个不小于25的元素是30 / First element not less than 25 is 30
    
    lb = tree.lower_bound(30);
    TEST(lb != tree.end());
    TEST(*lb == 30);  // 第一个不小于30的元素是30 / First element not less than 30 is 30
    
    // 测试upper_bound / Test upper_bound
    auto ub = tree.upper_bound(35);
    TEST(ub != tree.end());
    TEST(*ub == 40);  // 第一个大于35的元素是40 / First element greater than 35 is 40
    
    ub = tree.upper_bound(30);
    TEST(ub != tree.end());
    TEST(*ub == 40);  // 第一个大于30的元素是40 / First element greater than 30 is 40
    
    // 测试equal_range / Test equal_range
    range = tree.equal_range(30);
    TEST(range.first != tree.end());
    TEST(*range.first == 30);
    TEST(range.second != tree.end());
    TEST(*range.second == 40);
    
    std::cout << "  All boundary tests passed! / 所有边界测试通过！" << std::endl;
}

/**
 * @brief Test rb_tree with custom types
 * @brief 测试红黑树与自定义类型
 */
void test_rb_tree_custom_type() {
    std::cout << "\n9. Testing rb_tree with custom types: / 9. 测试红黑树与自定义类型:" << std::endl;
    
    // 自定义键提取函数 / Custom key extraction function
    struct extract_key {
        int operator()(const TestClass& tc) const { return tc.value; }
    };
    
    mystl::rb_tree<int, TestClass, extract_key> tree;
    
    // 插入自定义类型对象 / Insert custom type objects
    tree.insert(TestClass(10));
    tree.insert(TestClass(5));
    tree.insert(TestClass(15));
    
    TEST(tree.size() == 3);
    
    // 查找测试 / Find test
    auto it = tree.find(10);
    TEST(it != tree.end());
    TEST(it->value == 10);
    
    it = tree.find(20);
    TEST(it == tree.end());
    
    std::cout << "  All custom type tests passed! / 所有自定义类型测试通过！" << std::endl;
}

/**
 * @brief Test rb_tree large-scale operations
 * @brief 测试红黑树大规模操作
 */
void test_rb_tree_large_scale() {
    std::cout << "\n10. Testing rb_tree large-scale operations: / 10. 测试红黑树大规模操作:" << std::endl;
    
    mystl::rb_tree<int, int> tree;
    
    // 生成1000个不重复的随机数 / Generate 1000 unique random numbers
    mystl::vector<int> data = generate_unique_numbers(1000, 1, 10000);
    
    // 插入所有数据 / Insert all data
    for (size_t i = 0; i < data.size(); ++i) {
        tree.insert(data[i]);
    }
    
    // 验证大小 / Verify size
    TEST(tree.size() == 1000);
    
    // 验证所有元素都存在 / Verify all elements exist
    for (size_t i = 0; i < data.size(); ++i) {
        TEST(tree.find(data[i]) != tree.end());
    }
    
    // 验证中序遍历是有序的 / Verify in-order traversal is sorted
    int prev = -1;
    for (int val : tree) {
        TEST(val >= prev);
        prev = val;
    }
    
    // 随机删除一半元素 / Randomly delete half of the elements
    simple_rand rand_gen;
    for (size_t i = 0; i < 500; ++i) {
        // 随机选择一个索引 / Randomly select an index
        size_t idx = rand_gen() % data.size();
        int val = data[idx];
        
        // 从vector中删除 / Remove from vector
        for (size_t j = idx; j < data.size() - 1; ++j) {
            data[j] = data[j + 1];
        }
        data.pop_back();
        
        // 从树中删除 / Remove from tree
        size_t erased = tree.erase(val);
        TEST(erased == 1);
    }
    
    // 验证删除后的大小 / Verify size after deletion
    TEST(tree.size() == 500);
    
    // 验证删除的元素不存在 / Verify deleted elements don't exist
    // 注意：我们已经从data中删除了这些元素，所以data现在只包含剩余的元素
    // Note: We've already removed deleted elements from data, so data now contains only remaining elements
    
    // 验证剩余的元素存在 / Verify remaining elements exist
    for (size_t i = 0; i < data.size(); ++i) {
        TEST(tree.find(data[i]) != tree.end());
    }
    
    std::cout << "  All large-scale tests passed! / 所有大规模测试通过！" << std::endl;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 * 
 * @return int 0 if all tests pass, 1 if any test fails
 * @return int 如果所有测试通过返回 0，如果有任何测试失败返回 1
 */
int main() {
    std::cout << "=== Testing mystl rb_tree === / === 测试 mystl 红黑树 ===" << std::endl;
    std::cout << std::endl;
    
    try {
        test_rb_tree_constructors();
        test_rb_tree_find_count();
        test_rb_tree_erase();
        test_rb_tree_copy_move();
        test_rb_tree_comparison();
        test_rb_tree_swap();
        test_rb_tree_iterator();
        test_rb_tree_boundary();
        test_rb_tree_custom_type();
        test_rb_tree_large_scale();
        
        std::cout << "\n=== All rb_tree tests passed successfully! / 所有红黑树测试成功通过！ ===" << std::endl;
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
