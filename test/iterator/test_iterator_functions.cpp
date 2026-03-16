/**
 * @file test_iterator_functions.cpp
 * @brief Test for iterator operation functions
 * @brief 迭代器操作函数测试
 * 
 * @details This file contains comprehensive tests for iterator operation functions
 * defined in include/iterator/iterator_functions.hpp. It verifies the correctness
 * of functions such as advance, distance, next, prev, iter_swap, and other
 * extended functionality.
 * 
 * @details 本文件包含对 include/iterator/iterator_functions.hpp 中定义的迭代器操作函数
 * 的全面测试。它验证了 advance、distance、next、prev、iter_swap 及其他扩展功能的正确性。
 * 
 * @note Tests use custom iterators and raw pointers to avoid dependency on
 * standard library containers.
 * 
 * @note 测试使用自定义迭代器和原始指针，避免依赖标准库容器。
 * 
 * @author mySTL shensjt
 * @date 2026-03-17
 * 
 * @see include/iterator/iterator_functions.hpp
 * @see include/iterator/iterator_traits.hpp
 * @see include/utility/type_traits.hpp
 */

#include "../include/iterator/iterator_functions.hpp"
#include "../include/utility/type_traits.hpp"
#include <iostream>
#include <cstring>  // for strlen in string tests

// ==================== Test Helper Functions ====================
// ==================== 测试辅助函数 ====================

/**
 * @brief Global test counter
 * @brief 全局测试计数器
 */
int test_count = 0;

/**
 * @brief Global failure counter
 * @brief 全局失败计数器
 */
int fail_count = 0;

/**
 * @brief Test assertion macro
 * @brief 测试断言宏
 * 
 * @details Checks an expression and updates test statistics.
 * Prints success/failure messages to appropriate output streams.
 * 
 * @details 检查表达式并更新测试统计信息。
 * 将成功/失败消息打印到适当的输出流。
 * 
 * @param expr Expression to check
 * @param expr 要检查的表达式
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

// ==================== Custom Test Iterator ====================
// ==================== 自定义测试迭代器 ====================

/**
 * @brief Simple array iterator (random access iterator)
 * @brief 简单数组迭代器（随机访问迭代器）
 * 
 * @tparam T Element type
 * @tparam T 元素类型
 * 
 * @details Implements a random access iterator for array-like structures.
 * Used to test iterator functions without relying on standard containers.
 * 
 * @details 为类似数组的结构实现随机访问迭代器。
 * 用于测试迭代器函数，不依赖标准容器。
 */
template<typename T>
struct ArrayIterator {
    using iterator_category = mystl::random_access_iterator_tag;  ///< Iterator category / 迭代器类别
    using value_type        = T;                                  ///< Value type / 值类型
    using difference_type   = std::ptrdiff_t;                     ///< Difference type / 差值类型
    using pointer           = T*;                                 ///< Pointer type / 指针类型
    using reference         = T&;                                 ///< Reference type / 引用类型
    
    T* ptr;  ///< Pointer to current element / 指向当前元素的指针
    
    /**
     * @brief Constructor
     * @brief 构造函数
     * 
     * @param p Pointer to element, defaults to nullptr
     * @param p 指向元素的指针，默认为 nullptr
     */
    ArrayIterator(T* p = nullptr) : ptr(p) {}
    
    // Basic iterator operations / 基本迭代器操作
    reference operator*() const { return *ptr; }          ///< Dereference operator / 解引用运算符
    pointer operator->() const { return ptr; }            ///< Member access operator / 成员访问运算符
    
    // Forward iterator operations / 前向迭代器操作
    ArrayIterator& operator++() { ++ptr; return *this; }  ///< Pre-increment / 前置递增
    ArrayIterator operator++(int) { ArrayIterator tmp = *this; ++ptr; return tmp; }  ///< Post-increment / 后置递增
    
    // Bidirectional iterator operations / 双向迭代器操作
    ArrayIterator& operator--() { --ptr; return *this; }  ///< Pre-decrement / 前置递减
    ArrayIterator operator--(int) { ArrayIterator tmp = *this; --ptr; return tmp; }  ///< Post-decrement / 后置递减
    
    // Random access iterator operations / 随机访问迭代器操作
    ArrayIterator& operator+=(difference_type n) { ptr += n; return *this; }  ///< Compound addition assignment / 复合加法赋值
    ArrayIterator& operator-=(difference_type n) { ptr -= n; return *this; }  ///< Compound subtraction assignment / 复合减法赋值
    ArrayIterator operator+(difference_type n) const { return ArrayIterator(ptr + n); }  ///< Addition operator / 加法运算符
    ArrayIterator operator-(difference_type n) const { return ArrayIterator(ptr - n); }  ///< Subtraction operator / 减法运算符
    difference_type operator-(const ArrayIterator& other) const { return ptr - other.ptr; }  ///< Difference operator / 差值运算符
    reference operator[](difference_type n) const { return ptr[n]; }  ///< Subscript operator / 下标运算符
    
    // Comparison operations / 比较操作
    bool operator==(const ArrayIterator& other) const { return ptr == other.ptr; }  ///< Equality operator / 相等运算符
    bool operator!=(const ArrayIterator& other) const { return ptr != other.ptr; }  ///< Inequality operator / 不等运算符
    bool operator<(const ArrayIterator& other) const { return ptr < other.ptr; }    ///< Less-than operator / 小于运算符
    bool operator>(const ArrayIterator& other) const { return ptr > other.ptr; }    ///< Greater-than operator / 大于运算符
    bool operator<=(const ArrayIterator& other) const { return ptr <= other.ptr; }  ///< Less-than-or-equal operator / 小于等于运算符
    bool operator>=(const ArrayIterator& other) const { return ptr >= other.ptr; }  ///< Greater-than-or-equal operator / 大于等于运算符
};

// ==================== Test advance Function ====================
// ==================== 测试 advance 函数 ====================

/**
 * @brief Test the advance function
 * @brief 测试 advance 函数
 * 
 * @details Tests advance function with raw pointers and custom iterators.
 * Verifies both forward and backward movement for appropriate iterator types.
 * 
 * @details 使用原始指针和自定义迭代器测试 advance 函数。
 * 验证适当迭代器类型的向前和向后移动。
 */
void test_advance() {
    std::cout << "\n=== Testing advance function ===" << std::endl;
    std::cout << "\n=== 测试 advance 函数 ===" << std::endl;
    
    // Test random access iterator (raw pointer) / 测试随机访问迭代器（原始指针）
    {
        int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int* it = arr;
        
        mystl::advance(it, 3);
        CHECK(*it == 4);
        
        mystl::advance(it, -2);
        CHECK(*it == 2);
        
        mystl::advance(it, 7);
        CHECK(*it == 9);
        
        mystl::advance(it, -8);
        CHECK(*it == 1);
    }
    
    // Test random access iterator (custom iterator) / 测试随机访问迭代器（自定义迭代器）
    {
        int arr[] = {1, 2, 3, 4, 5};
        ArrayIterator<int> it(arr);
        
        mystl::advance(it, 2);
        CHECK(*it == 3);
        
        mystl::advance(it, -1);
        CHECK(*it == 2);
        
        mystl::advance(it, 3);
        CHECK(*it == 5);
    }
    
    std::cout << "All advance tests passed! / 所有 advance 测试通过！" << std::endl;
}

// ==================== Test distance Function ====================
// ==================== 测试 distance 函数 ====================

/**
 * @brief Test the distance function
 * @brief 测试 distance 函数
 * 
 * @details Tests distance function with raw pointers and custom iterators.
 * Verifies correct distance calculation for various iterator ranges.
 * 
 * @details 使用原始指针和自定义迭代器测试 distance 函数。
 * 验证各种迭代器范围的正确距离计算。
 */
void test_distance() {
    std::cout << "\n=== Testing distance function ===" << std::endl;
    std::cout << "\n=== 测试 distance 函数 ===" << std::endl;
    
    // Test random access iterator (raw pointer) / 测试随机访问迭代器（原始指针）
    {
        int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        auto d1 = mystl::distance(arr, arr + 10);
        CHECK(d1 == 10);
        
        auto d2 = mystl::distance(arr + 2, arr + 7);
        CHECK(d2 == 5);
    }
    
    // Test random access iterator (custom iterator) / 测试随机访问迭代器（自定义迭代器）
    {
        int arr[] = {1, 2, 3, 4, 5, 6, 7, 8};
        ArrayIterator<int> first(arr);
        ArrayIterator<int> last(arr + 8);
        auto d = mystl::distance(first, last);
        CHECK(d == 8);
    }
    
    std::cout << "All distance tests passed! / 所有 distance 测试通过！" << std::endl;
}

// ==================== Test next Function ====================
// ==================== 测试 next 函数 ====================

/**
 * @brief Test the next function
 * @brief 测试 next 函数
 * 
 * @details Tests both basic and boundary-checked versions of next function.
 * Verifies correct iterator advancement with and without bounds.
 * 
 * @details 测试 next 函数的基本版本和边界检查版本。
 * 验证带边界和不带边界的正确迭代器前进。
 */
void test_next() {
    std::cout << "\n=== Testing next function ===" << std::endl;
    std::cout << "\n=== 测试 next 函数 ===" << std::endl;
    
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Test basic version (raw pointer) / 测试基础版本（原始指针）
    auto it1 = mystl::next(arr, 3);
    CHECK(*it1 == 4);
    
    auto it2 = mystl::next(arr, 0);
    CHECK(*it2 == 1);
    
    auto it3 = mystl::next(arr, 9);
    CHECK(*it3 == 10);
    
    // Test boundary-checked version / 测试带边界检查版本
    auto it4 = mystl::next(arr, arr + 10, 3);
    CHECK(*it4 == 4);
    
    auto it5 = mystl::next(arr, arr + 10, 20);  // Exceeds boundary / 超过边界
    CHECK(it5 == arr + 10);
    
    auto it6 = mystl::next(arr, arr + 10, 0);
    CHECK(*it6 == 1);
    
    std::cout << "All next tests passed! / 所有 next 测试通过！" << std::endl;
}

// ==================== Test prev Function ====================
// ==================== 测试 prev 函数 ====================

/**
 * @brief Test the prev function
 * @brief 测试 prev 函数
 * 
 * @details Tests both basic and boundary-checked versions of prev function.
 * Verifies correct iterator movement backward with and without bounds.
 * 
 * @details 测试 prev 函数的基本版本和边界检查版本。
 * 验证带边界和不带边界的正确迭代器向后移动。
 */
void test_prev() {
    std::cout << "\n=== Testing prev function ===" << std::endl;
    std::cout << "\n=== 测试 prev 函数 ===" << std::endl;
    
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Test basic version (raw pointer) / 测试基础版本（原始指针）
    auto it1 = mystl::prev(arr + 10, 3);
    CHECK(*it1 == 8);
    
    auto it2 = mystl::prev(arr + 10, 0);
    CHECK(it2 == arr + 10);
    
    auto it3 = mystl::prev(arr + 10, 10);
    CHECK(*it3 == 1);
    
    // Test boundary-checked version / 测试带边界检查版本
    auto it4 = mystl::prev(arr + 10, arr, 3);
    CHECK(*it4 == 8);
    
    auto it5 = mystl::prev(arr + 10, arr, 20);  // Exceeds boundary / 超过边界
    CHECK(it5 == arr);
    
    auto it6 = mystl::prev(arr + 10, arr, 0);
    CHECK(it6 == arr + 10);
    
    std::cout << "All prev tests passed! / 所有 prev 测试通过！" << std::endl;
}

// ==================== Test iter_swap Function ====================
// ==================== 测试 iter_swap 函数 ====================

/**
 * @brief Test the iter_swap function
 * @brief 测试 iter_swap 函数
 * 
 * @details Tests swapping of elements pointed to by iterators.
 * Verifies correct element exchange between different and same arrays.
 * 
 * @details 测试迭代器指向的元素的交换。
 * 验证不同数组和相同数组之间的正确元素交换。
 */
void test_iter_swap() {
    std::cout << "\n=== Testing iter_swap function ===" << std::endl;
    std::cout << "\n=== 测试 iter_swap 函数 ===" << std::endl;
    
    // Test swapping elements between two arrays / 测试交换两个数组的元素
    {
        int arr1[] = {1, 2, 3};
        int arr2[] = {4, 5, 6};
        
        mystl::iter_swap(arr1, arr2);
        CHECK(arr1[0] == 4);
        CHECK(arr2[0] == 1);
        
        mystl::iter_swap(arr1 + 1, arr2 + 1);
        CHECK(arr1[1] == 5);
        CHECK(arr2[1] == 2);
        
        // Test swapping elements within same array / 测试交换相同数组的元素
        mystl::iter_swap(arr1, arr1 + 2);
        CHECK(arr1[0] == 3);
        CHECK(arr1[2] == 4);
    }
    
    std::cout << "All iter_swap tests passed! / 所有 iter_swap 测试通过！" << std::endl;
}

// ==================== Test Extended Functions ====================
// ==================== 测试扩展功能 ====================

/**
 * @brief Test extended iterator functions
 * @brief 测试扩展迭代器函数
 * 
 * @details Tests additional functions like iter_move, size, empty,
 * advance_unrolled, safe_advance, and type traits.
 * 
 * @details 测试其他函数，如 iter_move、size、empty、
 * advance_unrolled、safe_advance 和类型特性。
 */
void test_extended_functions() {
    std::cout << "\n=== Testing extended functions ===" << std::endl;
    std::cout << "\n=== 测试扩展功能 ===" << std::endl;
    
    // Test iter_move (using custom string array) / 测试 iter_move（使用自定义字符串数组）
    {
        // Simple string structure for testing / 用于测试的简单字符串结构
        struct SimpleString {
            char data[32];
            SimpleString() { data[0] = '\0'; }
            SimpleString(const char* s) { std::strcpy(data, s); }
            bool empty() const { return data[0] == '\0'; }
            bool operator==(const char* str) const { return std::strcmp(data, str) == 0; }
        };
        
        SimpleString strs[] = {SimpleString("hello"), SimpleString("world"), SimpleString("test")};
        auto moved = mystl::iter_move(strs);
        CHECK(moved == "hello");
        // Note: After iter_move, original string state is undefined / 注意：iter_move 移动后，原字符串状态未定义
    }
    
    // Test size function / 测试 size 函数
    {
        int arr[] = {1, 2, 3, 4, 5};
        CHECK(mystl::size(arr) == 5);
        
        int arr2[] = {1, 2, 3, 4, 5, 6};
        CHECK(mystl::size(arr2) == 6);
        
        // Test iterator range size / 测试迭代器范围大小
        CHECK(mystl::size(arr, arr + 5) == 5);
    }
    
    // Test empty function / 测试 empty 函数
    {
        int arr[] = {1, 2, 3};
        CHECK(!mystl::empty(arr));
        
        // Test iterator range emptiness / 测试迭代器范围是否为空
        int* empty_ptr = nullptr;
        CHECK(mystl::empty(empty_ptr, empty_ptr));
        
        // Test iterator range non-emptiness / 测试迭代器范围非空
        CHECK(!mystl::empty(arr, arr + 3));
    }
    
    // Test advance_unrolled function / 测试 advance_unrolled 函数
    {
        int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int* it = arr;
        
        mystl::advance_unrolled(it, 8);
        CHECK(*it == 9);
        
        // Test positive distance / 测试正数距离
        int* it2 = arr;
        mystl::advance_unrolled(it2, 4);
        CHECK(*it2 == 5);
    }
    
    // Test safe_advance function / 测试 safe_advance 函数
    {
        int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int* it = arr;
        
        // Test forward movement (success) / 测试向前移动（成功）
        bool success1 = mystl::safe_advance(it, arr + 10, 3);
        CHECK(success1);
        CHECK(*it == 4);
        
        // Test forward movement (failure, out of bounds) / 测试向前移动（失败，越界）
        bool success2 = mystl::safe_advance(it, arr + 10, 10);
        CHECK(!success2);
        CHECK(*it == 4);  // it should remain unchanged / it 应该保持不变
        
        // Test bidirectional movement (backward success) / 测试双向移动（向后移动成功）
        bool success3 = mystl::safe_advance(it, arr, arr + 10, -2);
        CHECK(success3);
        CHECK(*it == 2);
        
        // Test bidirectional movement (backward failure, out of bounds) / 测试双向移动（向后移动失败，越界）
        bool success4 = mystl::safe_advance(it, arr, arr + 10, -10);
        CHECK(!success4);
        CHECK(*it == 2);  // it should remain unchanged / it 应该保持不变
        
        // Test zero distance movement / 测试零距离移动
        bool success5 = mystl::safe_advance(it, arr + 10, 0);
        CHECK(success5);
        CHECK(*it == 2);
        
        // Test bidirectional movement zero distance / 测试双向移动零距离
        bool success6 = mystl::safe_advance(it, arr, arr + 10, 0);
        CHECK(success6);
        CHECK(*it == 2);
    }
    
    // Test type traits / 测试类型特性
    {
        // Test iterator type traits (using raw pointers and custom iterators) / 测试迭代器类型特性（使用原始指针和自定义迭代器）
        CHECK((mystl::is_input_iterator_v<int*>));
        CHECK((mystl::is_forward_iterator_v<int*>));
        CHECK((mystl::is_bidirectional_iterator_v<int*>));
        CHECK((mystl::is_random_access_iterator_v<int*>));
        
        CHECK((mystl::is_input_iterator_v<ArrayIterator<int>>));
        CHECK((mystl::is_forward_iterator_v<ArrayIterator<int>>));
        CHECK((mystl::is_bidirectional_iterator_v<ArrayIterator<int>>));
        CHECK((mystl::is_random_access_iterator_v<ArrayIterator<int>>));
    }
    
    std::cout << "All extended functions tests passed! / 所有扩展功能测试通过！" << std::endl;
}

// ==================== Test Type Traits ====================
// ==================== 测试类型特性 ====================

/**
 * @brief Test type traits checking
 * @brief 测试类型特性检查
 * 
 * @details Tests iterator type traits for various iterator types.
 * 
 * @details 测试各种迭代器类型的迭代器类型特性。
 */
void test_type_traits() {
    std::cout << "\n=== Testing type traits ===" << std::endl;
    std::cout << "\n=== 测试类型特性 ===" << std::endl;
    
    // Test iterator type traits (using raw pointers and custom iterators) / 测试迭代器类型特性（使用原始指针和自定义迭代器）
    CHECK((mystl::is_input_iterator_v<int*>));
    CHECK((mystl::is_forward_iterator_v<int*>));
    CHECK((mystl::is_bidirectional_iterator_v<int*>));
    CHECK((mystl::is_random_access_iterator_v<int*>));
    
    CHECK((mystl::is_input_iterator_v<ArrayIterator<int>>));
    CHECK((mystl::is_forward_iterator_v<ArrayIterator<int>>));
    CHECK((mystl::is_bidirectional_iterator_v<ArrayIterator<int>>));
    CHECK((mystl::is_random_access_iterator_v<ArrayIterator<int>>));
    
    std::cout << "All type traits tests passed! / 所有类型特性测试通过！" << std::endl;
}

// ==================== Main Function ====================
// ==================== 主函数 ====================

/**
 * @brief Main test function
 * @brief 主测试函数
 * 
 * @details Runs all test suites and reports results.
 * 
 * @details 运行所有测试套件并报告结果。
 * 
 * @return 0 if all tests pass, 1 otherwise
 * @return 如果所有测试通过则返回 0，否则返回 1
 */
int main() {
    std::cout << "Starting iterator operation functions tests..." << std::endl;
    std::cout << "开始测试迭代器操作函数..." << std::endl;
    std::cout << "================================" << std::endl;
    
    test_count = 0;
    fail_count = 0;
    
    try {
        test_advance();
        test_distance();
        test_next();
        test_prev();
        test_iter_swap();
        test_extended_functions();
        test_type_traits();
        
        std::cout << "\n================================" << std::endl;
        std::cout << "Test statistics: / 测试统计：" << std::endl;
        std::cout << "Total tests: " << test_count << std::endl;
        std::cout << "Failures: " << fail_count << std::endl;
        std::cout << "Passes: " << (test_count - fail_count) << std::endl;
        
        if (fail_count == 0) {
            std::cout << "All iterator operation functions tests passed! / 所有迭代器操作函数测试通过！" << std::endl;
            return 0;
        } else {
            std::cout << fail_count << " test(s) failed! / " << fail_count << " 个测试失败！" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Test exception: " << e.what() << std::endl;
        std::cerr << "测试异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown test exception / 未知测试异常" << std::endl;
        return 1;
    }
}
