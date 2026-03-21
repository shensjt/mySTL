/**
 * @file test_reverse_iterator.cpp
 * @brief Comprehensive test suite for mystl::reverse_iterator implementation
 * @brief reverse_iterator 实现的全面测试套件
 * 
 * This file contains extensive tests for the reverse_iterator adaptor in mystl.
 * It verifies the correctness of reverse_iterator operations including construction,
 * traversal in reverse direction, arithmetic operations, comparisons, and edge cases.
 * 
 * 本文件包含 mystl 中 reverse_iterator 适配器的全面测试。
 * 验证 reverse_iterator 操作的正确性，包括构造、反向遍历、算术运算、比较和边界情况。
 * 
 * The tests are organized into logical sections corresponding to different
 * aspects of reverse_iterator functionality.
 * 
 * 测试按 reverse_iterator 功能的不同方面组织成逻辑部分。
 * 
 * @author shensjt
 * @date 2026-03-19
 * 
 * @see reverse_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/reverse_iterator
 */

#include "../../include/iterator/reverse_iterator.hpp"
#include "../../include/iterator/iterator_traits.hpp"
#include <iostream>
#include <vector>

int test_count = 0;      ///< Global test counter / 全局测试计数器
int fail_count = 0;      ///< Global failure counter / 全局失败计数器

/**
 * @brief Test assertion macro
 * @brief 测试断言宏
 * 
 * @param expr Expression to evaluate
 * @param expr 要评估的表达式
 * 
 * @details Checks if the expression is true. If false, logs the failure
 * with line number and expression. Increments test_count and fail_count accordingly.
 * 
 * @details 检查表达式是否为真。如果为假，记录失败信息（包括行号和表达式）。
 * 相应地递增 test_count 和 fail_count。
 */
#define CHECK(expr) \
    do { \
        ++test_count; \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            ++fail_count; \
        } else { \
            std::cout << "Test passed: " << #expr << std::endl; \
        } \
    } while(0)

/**
 * @brief Custom array iterator for testing reverse_iterator
 * @brief 用于测试 reverse_iterator 的自定义数组迭代器
 * 
 * @tparam T Element type
 * @tparam T 元素类型
 * 
 * @details Implements a random access iterator for testing reverse_iterator
 * with custom iterator types. This ensures reverse_iterator works correctly
 * with any iterator that meets the random access iterator requirements.
 * 
 * @details 实现随机访问迭代器，用于测试 reverse_iterator 与自定义迭代器类型。
 * 确保 reverse_iterator 适用于任何满足随机访问迭代器要求的迭代器。
 */
template<typename T>
struct ArrayIterator {
    using iterator_category = mystl::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    
    T* ptr;
    
    ArrayIterator(T* p = nullptr) : ptr(p) {}
    
    reference operator*() const { return *ptr; }
    pointer operator->() const { return ptr; }
    
    ArrayIterator& operator++() { ++ptr; return *this; }
    ArrayIterator operator++(int) { ArrayIterator tmp = *this; ++ptr; return tmp; }
    
    ArrayIterator& operator--() { --ptr; return *this; }
    ArrayIterator operator--(int) { ArrayIterator tmp = *this; --ptr; return tmp; }
    
    ArrayIterator& operator+=(difference_type n) { ptr += n; return *this; }
    ArrayIterator& operator-=(difference_type n) { ptr -= n; return *this; }
    ArrayIterator operator+(difference_type n) const { return ArrayIterator(ptr + n); }
    ArrayIterator operator-(difference_type n) const { return ArrayIterator(ptr - n); }
    difference_type operator-(const ArrayIterator& other) const { return ptr - other.ptr; }
    reference operator[](difference_type n) const { return ptr[n]; }
    
    bool operator==(const ArrayIterator& other) const { return ptr == other.ptr; }
    bool operator!=(const ArrayIterator& other) const { return ptr != other.ptr; }
    bool operator<(const ArrayIterator& other) const { return ptr < other.ptr; }
    bool operator>(const ArrayIterator& other) const { return ptr > other.ptr; }
    bool operator<=(const ArrayIterator& other) const { return ptr <= other.ptr; }
    bool operator>=(const ArrayIterator& other) const { return ptr >= other.ptr; }
};

/**
 * @brief Test basic reverse_iterator functionality
 * @brief 测试基本的 reverse_iterator 功能
 * 
 * @details Tests basic operations of reverse_iterator including:
 * - Construction and initialization
 * - Traversal in reverse direction (++ moves backward, -- moves forward)
 * - Copy construction and comparison
 * 
 * @details 测试 reverse_iterator 的基本操作，包括：
 * - 构造和初始化
 * - 反向遍历（++ 向后移动，-- 向前移动）
 * - 拷贝构造和比较
 */
void test_basic_reverse_iterator() {
    std::cout << "\n=== Testing Basic Reverse Iterator ===" << std::endl;
    
    int arr[] = {1, 2, 3, 4, 5};
    
    mystl::reverse_iterator<int*> rit(arr + 5);
    
    CHECK(*rit == 5);
    ++rit;
    CHECK(*rit == 4);
    ++rit;
    CHECK(*rit == 3);
    --rit;
    CHECK(*rit == 4);
    
    mystl::reverse_iterator<int*> rit2 = rit;
    CHECK(*rit2 == 4);
    
    CHECK(rit == rit2);
    ++rit2;
    CHECK(rit != rit2);
    CHECK(rit < rit2);
    CHECK(rit2 > rit);
}

/**
 * @brief Test reverse_iterator with custom iterator types
 * @brief 测试 reverse_iterator 与自定义迭代器类型
 * 
 * @details Tests reverse_iterator with the custom ArrayIterator to ensure
 * it works correctly with user-defined iterator types that meet the requirements.
 * 
 * @details 使用自定义 ArrayIterator 测试 reverse_iterator，确保它适用于
 * 满足要求的用户定义迭代器类型。
 */
void test_reverse_iterator_with_custom_iterator() {
    std::cout << "\n=== Testing Reverse Iterator with Custom Iterator ===" << std::endl;
    
    int arr[] = {10, 20, 30, 40, 50};
    ArrayIterator<int> it(arr);
    
    mystl::reverse_iterator<ArrayIterator<int>> rit(it + 5);
    
    CHECK(*rit == 50);
    ++rit;
    CHECK(*rit == 40);
    rit += 2;
    CHECK(*rit == 20);
    rit -= 1;
    CHECK(*rit == 30);
    
    auto rit2 = rit + 2;
    CHECK(*rit2 == 10);
    
    auto rit3 = rit2 - 1;
    CHECK(*rit3 == 20);
    
    CHECK((rit2 - rit3) == 1);
    CHECK((rit3 - rit2) == -1);
}

/**
 * @brief Test arithmetic operations on reverse_iterator
 * @brief 测试 reverse_iterator 的算术运算
 * 
 * @details Tests random access iterator operations on reverse_iterator:
 * - Subscript operator []
 * - Addition and subtraction with integers
 * - Compound assignment operators (+=, -=)
 * - Difference between iterators
 * 
 * @details 测试 reverse_iterator 的随机访问迭代器操作：
 * - 下标运算符 []
 * - 与整数的加法和减法
 * - 复合赋值运算符（+=, -=）
 * - 迭代器之间的差值
 */
void test_reverse_iterator_arithmetic() {
    std::cout << "\n=== Testing Reverse Iterator Arithmetic ===" << std::endl;
    
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    mystl::reverse_iterator<int*> rit(arr + 10);
    
    CHECK(rit[0] == 10);
    CHECK(rit[1] == 9);
    CHECK(rit[4] == 6);
    CHECK(rit[9] == 1);
    
    auto rit2 = rit + 3;
    CHECK(*rit2 == 7);
    
    rit2 += 2;
    CHECK(*rit2 == 5);
    
    rit2 -= 1;
    CHECK(*rit2 == 6);
    
    auto rit3 = rit2 - 2;
    CHECK(*rit3 == 8);
}

/**
 * @brief Test comparison operations on reverse_iterator
 * @brief 测试 reverse_iterator 的比较操作
 * 
 * @details Tests all comparison operators with reverse_iterator,
 * verifying that comparisons work correctly in reverse order.
 * 
 * @details 测试 reverse_iterator 的所有比较运算符，
 * 验证比较在反向顺序中正常工作。
 */
void test_reverse_iterator_comparisons() {
    std::cout << "\n=== Testing Reverse Iterator Comparisons ===" << std::endl;
    
    int arr[] = {1, 2, 3, 4, 5};
    
    mystl::reverse_iterator<int*> rit1(arr + 5);  // points to 5
    mystl::reverse_iterator<int*> rit2(arr + 4);  // points to 4
    mystl::reverse_iterator<int*> rit3(arr + 3);  // points to 3
    
    CHECK(rit1 == rit1);
    CHECK(rit1 != rit2);
    CHECK(rit1 < rit2);   // 5 comes before 4 in reverse order? Wait, let me think...
    // Actually: rit1 points to 5, rit2 points to 4
    // In reverse order: 5, 4, 3, 2, 1
    // So 5 comes before 4, so rit1 < rit2 should be true
    CHECK(rit2 > rit1);   // 4 comes after 5 in reverse order
    CHECK(rit1 <= rit2);
    CHECK(rit2 >= rit1);
    CHECK(rit2 < rit3);   // 4 comes before 3 in reverse order
    CHECK(rit3 > rit2);   // 3 comes after 4 in reverse order
    CHECK(rit2 <= rit3);
    CHECK(rit3 >= rit2);
}

/**
 * @brief Test const conversion of reverse_iterator
 * @brief 测试 reverse_iterator 的常量转换
 * 
 * @details Tests conversion from non-const reverse_iterator to const reverse_iterator.
 * This is important for iterator interoperability and const-correctness.
 * 
 * @details 测试从非常量 reverse_iterator 到常量 reverse_iterator 的转换。
 * 这对于迭代器互操作性和常量正确性很重要。
 */
void test_reverse_iterator_const_conversion() {
    std::cout << "\n=== Testing Reverse Iterator Const Conversion ===" << std::endl;
    
    int arr[] = {1, 2, 3};
    
    mystl::reverse_iterator<int*> rit(arr + 3);
    mystl::reverse_iterator<const int*> crit = rit;
    
    CHECK(*rit == 3);
    CHECK(*crit == 3);
    
    ++rit;
    ++crit;
    CHECK(*rit == 2);
    CHECK(*crit == 2);
    
    CHECK(rit == crit);
}

/**
 * @brief Test reverse_iterator with standard library containers
 * @brief 测试 reverse_iterator 与标准库容器
 * 
 * @details Tests reverse_iterator with std::vector to ensure compatibility
 * with standard library containers and algorithms.
 * 
 * @details 使用 std::vector 测试 reverse_iterator，确保与标准库容器
 * 和算法的兼容性。
 */
void test_reverse_iterator_with_std_vector() {
    std::cout << "\n=== Testing Reverse Iterator with std::vector ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    mystl::reverse_iterator<std::vector<int>::iterator> rit(vec.end());
    
    CHECK(*rit == 5);
    ++rit;
    CHECK(*rit == 4);
    
    int sum = 0;
    for (auto it = mystl::reverse_iterator<std::vector<int>::iterator>(vec.end());
         it != mystl::reverse_iterator<std::vector<int>::iterator>(vec.begin());
         ++it) {
        sum += *it;
    }
    
    CHECK(sum == 15);
}

/**
 * @brief Test edge cases and special scenarios
 * @brief 测试边界情况和特殊场景
 * 
 * @details Tests edge cases including:
 * - Single element containers
 * - Empty containers
 * - Boundary conditions
 * 
 * @details 测试边界情况，包括：
 * - 单元素容器
 * - 空容器
 * - 边界条件
 */
void test_reverse_iterator_edge_cases() {
    std::cout << "\n=== Testing Reverse Iterator Edge Cases ===" << std::endl;
    
    int arr[] = {42};
    
    mystl::reverse_iterator<int*> rit(arr + 1);
    CHECK(*rit == 42);
    
    ++rit;
    CHECK(rit.base() == arr);
    
    int empty_arr[0] = {};
    mystl::reverse_iterator<int*> rit_empty(empty_arr);
    CHECK(rit_empty.base() == empty_arr);
}

/**
 * @brief Main test runner
 * @brief 主测试运行器
 * 
 * @return 0 if all tests pass, 1 if any test fails
 * @return 如果所有测试通过则返回 0，如果有任何测试失败则返回 1
 * 
 * @details Runs all test suites for reverse_iterator and reports statistics.
 * Catches and reports any exceptions that occur during testing.
 * 
 * @details 运行 reverse_iterator 的所有测试套件并报告统计信息。
 * 捕获并报告测试期间发生的任何异常。
 */
int main() {
    std::cout << "Starting reverse_iterator tests..." << std::endl;
    std::cout << "================================" << std::endl;
    
    test_count = 0;
    fail_count = 0;
    
    try {
        test_basic_reverse_iterator();
        test_reverse_iterator_with_custom_iterator();
        test_reverse_iterator_arithmetic();
        test_reverse_iterator_comparisons();
        test_reverse_iterator_const_conversion();
        test_reverse_iterator_with_std_vector();
        test_reverse_iterator_edge_cases();
        
        std::cout << "\n================================" << std::endl;
        std::cout << "Test statistics:" << std::endl;
        std::cout << "Total tests: " << test_count << std::endl;
        std::cout << "Failures: " << fail_count << std::endl;
        std::cout << "Passes: " << (test_count - fail_count) << std::endl;
        
        if (fail_count == 0) {
            std::cout << "All reverse_iterator tests passed!" << std::endl;
            return 0;
        } else {
            std::cout << fail_count << " test(s) failed!" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Test exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown test exception" << std::endl;
        return 1;
    }
}
