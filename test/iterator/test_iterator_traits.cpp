/**
 * @file test_iterator_traits.cpp
 * @brief Test for iterator traits system
 * @brief 迭代器特性系统测试
 * 
 * @details This file contains comprehensive tests for the iterator traits system
 * defined in include/iterator/iterator_traits.hpp. It verifies the correctness
 * of iterator type trait extraction, pointer specializations, type aliases,
 * compile-time checks, and extended functionality.
 * 
 * @details 本文件包含对 include/iterator/iterator_traits.hpp 中定义的迭代器特性系统的
 * 全面测试。它验证了迭代器类型特征提取、指针特化、类型别名、编译时检查和扩展功能的正确性。
 * 
 * @note Tests cover custom iterators, raw pointers, const/volatile pointers,
 * C arrays, and runtime behavior.
 * 
 * @note 测试涵盖自定义迭代器、原始指针、const/volatile 指针、C 数组和运行时行为。
 * 
 * @author mySTL shensjt
 * @date 2026-03-17
 * 
 * @see include/iterator/iterator_traits.hpp
 * @see include/utility/type_traits.hpp
 */

#include "../include/iterator/iterator_traits.hpp"
#include "../include/utility/type_traits.hpp"
#include <iostream>

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
 * @brief Example custom iterator: random access iterator
 * @brief 示例自定义迭代器：随机访问迭代器
 * 
 * @tparam T Element type
 * @tparam T 元素类型
 * 
 * @details Implements a random access iterator for testing iterator traits.
 * Provides all required operations for random access iterator category.
 * 
 * @details 实现用于测试迭代器特性的随机访问迭代器。
 * 提供随机访问迭代器类别所需的所有操作。
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

// ==================== Test iterator_traits ====================
// ==================== 测试 iterator_traits ====================

/**
 * @brief Test iterator_traits type trait extraction
 * @brief 测试 iterator_traits 类型特征提取
 * 
 * @details Tests iterator_traits for custom iterators, raw pointers,
 * and const pointers.
 * 
 * @details 测试自定义迭代器、原始指针和 const 指针的 iterator_traits。
 */
void test_iterator_traits() {
    std::cout << "\n=== Testing iterator_traits ===" << std::endl;
    std::cout << "\n=== 测试 iterator_traits ===" << std::endl;
    
    // Test iterator_traits for custom iterator / 测试自定义迭代器的 iterator_traits
    CHECK((mystl::is_same_v<mystl::iterator_category_t<ArrayIterator<int>>, 
                           mystl::random_access_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::value_type_t<ArrayIterator<int>>, int>));
    CHECK((mystl::is_same_v<mystl::difference_type_t<ArrayIterator<int>>, std::ptrdiff_t>));
    CHECK((mystl::is_same_v<mystl::pointer_t<ArrayIterator<int>>, int*>));
    CHECK((mystl::is_same_v<mystl::reference_t<ArrayIterator<int>>, int&>));
    
    // Test pointer specialization / 测试指针特化
    CHECK((mystl::is_same_v<mystl::iterator_category_t<int*>, 
                           mystl::random_access_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::value_type_t<int*>, int>));
    CHECK((mystl::is_same_v<mystl::difference_type_t<int*>, std::ptrdiff_t>));
    CHECK((mystl::is_same_v<mystl::pointer_t<int*>, int*>));
    CHECK((mystl::is_same_v<mystl::reference_t<int*>, int&>));
    
    // Test const pointer specialization / 测试 const 指针特化
    CHECK((mystl::is_same_v<mystl::iterator_category_t<const int*>, 
                           mystl::random_access_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::value_type_t<const int*>, int>));
    CHECK((mystl::is_same_v<mystl::pointer_t<const int*>, const int*>));
    CHECK((mystl::is_same_v<mystl::reference_t<const int*>, const int&>));
    
    std::cout << "All iterator_traits tests passed! / 所有 iterator_traits 测试通过！" << std::endl;
}

// ==================== Test iterator_traits Main Template ====================
// ==================== 测试 iterator_traits 主模板 ====================

/**
 * @brief Test iterator_traits main template
 * @brief 测试 iterator_traits 主模板
 * 
 * @details Tests the main template of iterator_traits for type extraction.
 * 
 * @details 测试 iterator_traits 的主模板以进行类型提取。
 */
void test_iterator_traits_main_template() {
    std::cout << "\n=== Testing iterator_traits main template ===" << std::endl;
    std::cout << "\n=== 测试 iterator_traits 主模板 ===" << std::endl;
    
    // Test if iterator_traits main template can correctly extract types / 测试 iterator_traits 主模板是否能正确提取类型
    using Iter = ArrayIterator<double>;
    using Traits = mystl::iterator_traits<Iter>;
    
    CHECK((mystl::is_same_v<typename Traits::iterator_category, 
                           mystl::random_access_iterator_tag>));
    CHECK((mystl::is_same_v<typename Traits::value_type, double>));
    CHECK((mystl::is_same_v<typename Traits::difference_type, std::ptrdiff_t>));
    CHECK((mystl::is_same_v<typename Traits::pointer, double*>));
    CHECK((mystl::is_same_v<typename Traits::reference, double&>));
    
    std::cout << "iterator_traits main template tests passed! / iterator_traits 主模板测试通过！" << std::endl;
}

// ==================== Test iterator_traits Pointer Specialization ====================
// ==================== 测试 iterator_traits 指针特化 ====================

/**
 * @brief Test iterator_traits pointer specializations
 * @brief 测试 iterator_traits 指针特化
 * 
 * @details Tests pointer specializations for regular and const pointers.
 * 
 * @details 测试普通指针和 const 指针的指针特化。
 */
void test_iterator_traits_pointer_specialization() {
    std::cout << "\n=== Testing iterator_traits pointer specialization ===" << std::endl;
    std::cout << "\n=== 测试 iterator_traits 指针特化 ===" << std::endl;
    
    // Test regular pointer specialization / 测试普通指针特化
    using PtrTraits = mystl::iterator_traits<float*>;
    
    CHECK((mystl::is_same_v<typename PtrTraits::iterator_category, 
                           mystl::random_access_iterator_tag>));
    CHECK((mystl::is_same_v<typename PtrTraits::value_type, float>));
    CHECK((mystl::is_same_v<typename PtrTraits::difference_type, std::ptrdiff_t>));
    CHECK((mystl::is_same_v<typename PtrTraits::pointer, float*>));
    CHECK((mystl::is_same_v<typename PtrTraits::reference, float&>));
    
    // Test const pointer specialization / 测试 const 指针特化
    using ConstPtrTraits = mystl::iterator_traits<const float*>;
    
    CHECK((mystl::is_same_v<typename ConstPtrTraits::iterator_category, 
                           mystl::random_access_iterator_tag>));
    CHECK((mystl::is_same_v<typename ConstPtrTraits::value_type, float>));
    CHECK((mystl::is_same_v<typename ConstPtrTraits::pointer, const float*>));
    CHECK((mystl::is_same_v<typename ConstPtrTraits::reference, const float&>));
    
    std::cout << "iterator_traits pointer specialization tests passed! / iterator_traits 指针特化测试通过！" << std::endl;
}

// ==================== Test New Features ====================
// ==================== 测试新功能 ====================

/**
 * @brief Test new iterator traits features
 * @brief 测试新的迭代器特性功能
 * 
 * @details Tests volatile pointer specializations, C array specializations,
 * iterator concept checks, specific iterator category checks, C++20 style
 * traits, and safe iterator traits.
 * 
 * @details 测试 volatile 指针特化、C 数组特化、迭代器概念检查、
 * 具体迭代器类别检查、C++20 风格特性和安全迭代器特性。
 */
void test_new_features() {
    std::cout << "\n=== Testing new features ===" << std::endl;
    std::cout << "\n=== 测试新功能 ===" << std::endl;
    
    // Test volatile pointer specialization / 测试 volatile 指针特化
    CHECK((mystl::is_same_v<mystl::iterator_category_t<volatile int*>, 
                           mystl::random_access_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::value_type_t<volatile int*>, int>));
    CHECK((mystl::is_same_v<mystl::reference_t<volatile int*>, volatile int&>));
    
    // Test const volatile pointer specialization / 测试 const volatile 指针特化
    CHECK((mystl::is_same_v<mystl::value_type_t<const volatile int*>, int>));
    CHECK((mystl::is_same_v<mystl::reference_t<const volatile int*>, const volatile int&>));
    
    // Test C array specialization / 测试 C 数组特化
    CHECK((mystl::is_same_v<mystl::iterator_category_t<int[5]>, 
                           mystl::random_access_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::value_type_t<int[5]>, int>));
    CHECK((mystl::is_same_v<mystl::value_type_t<const int[5]>, int>));
    
    // Test iterator concept checks / 测试迭代器概念检查
    CHECK((mystl::is_iterator_v<ArrayIterator<int>>));
    CHECK((mystl::is_iterator_v<int*>));
    CHECK((!mystl::is_iterator_v<int>));
    
    // Test specific iterator category checks / 测试具体迭代器类别检查
    CHECK((mystl::is_random_access_iterator_v<ArrayIterator<int>>));
    CHECK((mystl::is_random_access_iterator_v<int*>));
    CHECK((mystl::is_input_iterator_v<ArrayIterator<int>>));
    CHECK((mystl::is_forward_iterator_v<ArrayIterator<int>>));
    CHECK((mystl::is_bidirectional_iterator_v<ArrayIterator<int>>));
    
    // Test C++20 style traits / 测试 C++20 风格特性
    CHECK((mystl::is_same_v<mystl::iter_value_t<ArrayIterator<int>>, int>));
    CHECK((mystl::is_same_v<mystl::iter_reference_t<ArrayIterator<int>>, int&>));
    CHECK((mystl::is_same_v<mystl::iter_value_t<const int*>, int>));
    
    // Test safe iterator traits / 测试安全迭代器特性
    CHECK((mystl::is_same_v<typename mystl::safe_iterator_traits<ArrayIterator<int>>::value_type, int>));
    
    std::cout << "All new features tests passed! / 所有新功能测试通过！" << std::endl;
}

// ==================== Test Runtime Behavior ====================
// ==================== 测试运行时行为 ====================

/**
 * @brief Test runtime behavior of custom iterator
 * @brief 测试自定义迭代器的运行时行为
 * 
 * @details Tests actual usage of custom iterator with runtime operations.
 * 
 * @details 测试自定义迭代器与运行时操作的实际使用。
 */
void test_runtime_behavior() {
    std::cout << "\n=== Testing runtime behavior ===" << std::endl;
    std::cout << "\n=== 测试运行时行为 ===" << std::endl;
    
    // Test actual usage of custom iterator / 测试自定义迭代器的实际使用
    int arr[] = {1, 2, 3, 4, 5};
    ArrayIterator<int> it(arr);
    
    CHECK(*it == 1);
    ++it;
    CHECK(*it == 2);
    --it;
    CHECK(*it == 1);
    
    it += 3;
    CHECK(*it == 4);
    
    ArrayIterator<int> it2 = it - 2;
    CHECK(*it2 == 2);
    
    CHECK((it - it2) == 2);
    
    std::cout << "All runtime tests passed! / 所有运行时测试通过！" << std::endl;
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
    std::cout << "Starting iterator traits system tests..." << std::endl;
    std::cout << "开始测试迭代器特性系统..." << std::endl;
    std::cout << "================================" << std::endl;
    
    test_count = 0;
    fail_count = 0;
    
    try {
        test_iterator_traits();
        test_iterator_traits_main_template();
        test_iterator_traits_pointer_specialization();
        test_runtime_behavior();
        test_new_features();
        
        std::cout << "\n================================" << std::endl;
        std::cout << "Test statistics: / 测试统计：" << std::endl;
        std::cout << "Total tests: " << test_count << std::endl;
        std::cout << "Failures: " << fail_count << std::endl;
        std::cout << "Passes: " << (test_count - fail_count) << std::endl;
        
        if (fail_count == 0) {
            std::cout << "All iterator traits tests passed! / 所有迭代器特性测试通过！" << std::endl;
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
