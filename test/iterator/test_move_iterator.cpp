/**
 * @file test_move_iterator.cpp
 * @brief Comprehensive test suite for mystl::move_iterator implementation
 * @brief move_iterator 实现的全面测试套件
 * 
 * This file contains extensive tests for the move_iterator adaptor in mystl.
 * It verifies the correctness of move_iterator operations including construction,
 * move semantics, arithmetic operations, comparisons, and edge cases.
 * 
 * 本文件包含 mystl 中 move_iterator 适配器的全面测试。
 * 验证 move_iterator 操作的正确性，包括构造、移动语义、算术运算、比较和边界情况。
 * 
 * The tests are organized into logical sections corresponding to different
 * aspects of move_iterator functionality.
 * 
 * 测试按 move_iterator 功能的不同方面组织成逻辑部分。
 * 
 * @author mySTL shensjt
 * @date 2026-03-19
 * 
 * @see move_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/move_iterator
 */

#include "../../include/iterator/move_iterator.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>

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
 * @brief Test basic move_iterator functionality
 * @brief 测试基本的 move_iterator 功能
 * 
 * @details Tests basic operations of move_iterator including:
 * - Construction and initialization
 * - Dereferencing and increment/decrement
 * - base() function
 * - Copy construction and assignment
 * 
 * @details 测试 move_iterator 的基本操作，包括：
 * - 构造和初始化
 * - 解引用和递增/递减
 * - base() 函数
 * - 拷贝构造和赋值
 */
void test_basic_move_iterator() {
    std::cout << "\n=== Testing Basic Move Iterator ===" << std::endl;
    
    // Test with std::vector
    {
        std::vector<std::string> vec = {"one", "two", "three", "four", "five"};
        
        mystl::move_iterator<std::vector<std::string>::iterator> mit(vec.begin());
        
        CHECK(*mit == "one");
        ++mit;
        CHECK(*mit == "two");
        mit += 2;
        CHECK(*mit == "four");
        --mit;
        CHECK(*mit == "three");
        
        // Test base() function
        // mit points to "three" which is at index 2
        CHECK(mit.base() == vec.begin() + 2);
        
        // Test copy constructor
        auto mit2 = mit;
        CHECK(*mit2 == "three");
        CHECK(mit2.base() == vec.begin() + 2);
        
        // Test assignment
        mystl::move_iterator<std::vector<std::string>::iterator> mit3(vec.end());
        mit3 = mit;
        CHECK(*mit3 == "three");
        CHECK(mit3.base() == vec.begin() + 2);
    }
}

/**
 * @brief Test move semantics of move_iterator
 * @brief 测试 move_iterator 的移动语义
 * 
 * @details Verifies that move_iterator actually moves resources rather than copying:
 * - With std::unique_ptr (move-only type)
 * - With std::string (expensive to copy, cheap to move)
 * - Confirms source objects are left in moved-from state
 * 
 * @details 验证 move_iterator 实际上移动资源而不是复制：
 * - 使用 std::unique_ptr（仅移动类型）
 * - 使用 std::string（复制成本高，移动成本低）
 * - 确认源对象处于被移动后的状态
 */
void test_move_semantics() {
    std::cout << "\n=== Testing Move Semantics ===" << std::endl;
    
    // Test that move_iterator actually moves
    {
        std::vector<std::unique_ptr<int>> vec;
        vec.push_back(std::make_unique<int>(1));
        vec.push_back(std::make_unique<int>(2));
        vec.push_back(std::make_unique<int>(3));
        
        std::vector<std::unique_ptr<int>> dest;
        
        // Use move_iterator to move elements
        auto mit_begin = mystl::make_move_iterator(vec.begin());
        auto mit_end = mystl::make_move_iterator(vec.end());
        
        std::move(mit_begin, mit_end, std::back_inserter(dest));
        
        CHECK(dest.size() == 3);
        CHECK(*dest[0] == 1);
        CHECK(*dest[1] == 2);
        CHECK(*dest[2] == 3);
        
        // Original vector should now contain nullptrs
        CHECK(vec[0] == nullptr);
        CHECK(vec[1] == nullptr);
        CHECK(vec[2] == nullptr);
    }
    
    // Test with std::string
    {
        std::vector<std::string> vec = {"Hello", "World", "Test"};
        std::vector<std::string> dest;
        
        auto mit_begin = mystl::make_move_iterator(vec.begin());
        auto mit_end = mystl::make_move_iterator(vec.end());
        
        std::copy(mit_begin, mit_end, std::back_inserter(dest));
        
        CHECK(dest.size() == 3);
        CHECK(dest[0] == "Hello");
        CHECK(dest[1] == "World");
        CHECK(dest[2] == "Test");
        
        // Original strings should be empty (moved from)
        CHECK(vec[0].empty());
        CHECK(vec[1].empty());
        CHECK(vec[2].empty());
    }
}

/**
 * @brief Test arithmetic operations on move_iterator
 * @brief 测试 move_iterator 的算术运算
 * 
 * @details Tests random access iterator operations:
 * - Subscript operator []
 * - Addition and subtraction with integers
 * - Compound assignment operators (+=, -=)
 * - Difference between iterators
 * 
 * @details 测试随机访问迭代器操作：
 * - 下标运算符 []
 * - 与整数的加法和减法
 * - 复合赋值运算符（+=, -=）
 * - 迭代器之间的差值
 */
void test_move_iterator_arithmetic() {
    std::cout << "\n=== Testing Move Iterator Arithmetic ===" << std::endl;
    
    std::vector<int> vec = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    
    mystl::move_iterator<std::vector<int>::iterator> mit(vec.begin());
    
    CHECK(mit[0] == 10);
    CHECK(mit[1] == 20);
    CHECK(mit[4] == 50);
    CHECK(mit[9] == 100);
    
    auto mit2 = mit + 3;
    CHECK(*mit2 == 40);
    
    mit2 += 2;
    CHECK(*mit2 == 60);
    
    auto mit3 = mit2 - 1;
    CHECK(*mit3 == 50);
    
    mit3 -= 2;
    CHECK(*mit3 == 30);
    
    // Test difference
    CHECK((mit2 - mit) == 5);
    CHECK((mit - mit2) == -5);
}

/**
 * @brief Test comparison operations on move_iterator
 * @brief 测试 move_iterator 的比较操作
 * 
 * @details Tests all comparison operators:
 * - Equality (==) and inequality (!=)
 * - Less than (<) and greater than (>)
 * - Less than or equal (<=) and greater than or equal (>=)
 * 
 * @details 测试所有比较运算符：
 * - 相等（==）和不相等（!=）
 * - 小于（<）和大于（>）
 * - 小于等于（<=）和大于等于（>=）
 */
void test_move_iterator_comparisons() {
    std::cout << "\n=== Testing Move Iterator Comparisons ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    auto mit1 = mystl::make_move_iterator(vec.begin());
    auto mit2 = mystl::make_move_iterator(vec.begin() + 2);
    auto mit3 = mystl::make_move_iterator(vec.begin() + 2);
    
    CHECK(mit1 == mit1);
    CHECK(mit1 != mit2);
    CHECK(mit2 == mit3);
    CHECK(mit1 < mit2);
    CHECK(mit2 > mit1);
    CHECK(mit1 <= mit2);
    CHECK(mit2 >= mit1);
    CHECK(mit2 <= mit3);
    CHECK(mit2 >= mit3);
}

/**
 * @brief Test move_iterator with custom iterator types
 * @brief 测试 move_iterator 与自定义迭代器类型
 * 
 * @details Tests move_iterator with raw pointers and custom iterator types
 * to ensure it works with any iterator that meets the requirements.
 * 
 * @details 测试 move_iterator 与原始指针和自定义迭代器类型，
 * 确保它适用于任何满足要求的迭代器。
 */
void test_move_iterator_with_custom_iterator() {
    std::cout << "\n=== Testing Move Iterator with Custom Iterator ===" << std::endl;
    
    // Simple array iterator
    int arr[] = {100, 200, 300, 400, 500};
    
    mystl::move_iterator<int*> mit(arr);
    
    CHECK(*mit == 100);
    ++mit;
    CHECK(*mit == 200);
    
    auto mit2 = mit + 2;
    CHECK(*mit2 == 400);
    
    CHECK((mit2 - mit) == 2);
}

/**
 * @brief Test const conversion of move_iterator
 * @brief 测试 move_iterator 的常量转换
 * 
 * @details Tests conversion from non-const move_iterator to const move_iterator.
 * This is important for iterator interoperability and const-correctness.
 * 
 * @details 测试从非常量 move_iterator 到常量 move_iterator 的转换。
 * 这对于迭代器互操作性和常量正确性很重要。
 */
void test_move_iterator_const_conversion() {
    std::cout << "\n=== Testing Move Iterator Const Conversion ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3};
    
    // Test conversion from non-const to const move_iterator
    mystl::move_iterator<std::vector<int>::iterator> mit(vec.begin());
    mystl::move_iterator<std::vector<int>::const_iterator> cmit = mit;
    
    // For const iterators, we can't move, so we just compare bases
    CHECK(mit.base() == vec.begin());
    CHECK(cmit.base() == vec.begin());
    
    ++mit;
    ++cmit;
    CHECK(mit.base() == vec.begin() + 1);
    CHECK(cmit.base() == vec.begin() + 1);
    
    CHECK(mit.base() == cmit.base());
}

/**
 * @brief Test edge cases and special scenarios
 * @brief 测试边界情况和特殊场景
 * 
 * @details Tests edge cases including:
 * - Empty containers
 * - Single element containers
 * - Arrow operator (->) usage
 * - Move-only types
 * 
 * @details 测试边界情况，包括：
 * - 空容器
 * - 单元素容器
 * - 箭头运算符（->）使用
 * - 仅移动类型
 */
void test_move_iterator_edge_cases() {
    std::cout << "\n=== Testing Move Iterator Edge Cases ===" << std::endl;
    
    // Test with empty container
    {
        std::vector<int> empty_vec;
        auto mit_begin = mystl::make_move_iterator(empty_vec.begin());
        auto mit_end = mystl::make_move_iterator(empty_vec.end());
        
        CHECK(mit_begin == mit_end);
        CHECK(mit_begin.base() == empty_vec.begin());
        CHECK(mit_end.base() == empty_vec.end());
    }
    
    // Test with single element
    {
        std::vector<std::string> single_vec = {"single"};
        auto mit = mystl::make_move_iterator(single_vec.begin());
        
        CHECK(*mit == "single");
        CHECK(mit.base() == single_vec.begin());
        
        std::string moved = *mit;
        CHECK(moved == "single");
        CHECK(single_vec[0].empty());  // Should be moved from
    }
    
    // Test arrow operator
    {
        struct TestStruct {
            int value;
            TestStruct(int v) : value(v) {}
        };
        
        std::vector<TestStruct> vec = {TestStruct(42), TestStruct(99)};
        auto mit = mystl::make_move_iterator(vec.begin());
        
        CHECK(mit->value == 42);
        ++mit;
        CHECK(mit->value == 99);
    }
}

/**
 * @brief Main test runner
 * @brief 主测试运行器
 * 
 * @return 0 if all tests pass, 1 if any test fails
 * @return 如果所有测试通过则返回 0，如果有任何测试失败则返回 1
 * 
 * @details Runs all test suites for move_iterator and reports statistics.
 * Catches and reports any exceptions that occur during testing.
 * 
 * @details 运行 move_iterator 的所有测试套件并报告统计信息。
 * 捕获并报告测试期间发生的任何异常。
 */
int main() {
    std::cout << "Starting move_iterator tests..." << std::endl;
    std::cout << "================================" << std::endl;
    
    test_count = 0;
    fail_count = 0;
    
    try {
        test_basic_move_iterator();
        test_move_semantics();
        test_move_iterator_arithmetic();
        test_move_iterator_comparisons();
        test_move_iterator_with_custom_iterator();
        test_move_iterator_const_conversion();
        test_move_iterator_edge_cases();
        
        std::cout << "\n================================" << std::endl;
        std::cout << "Test statistics:" << std::endl;
        std::cout << "Total tests: " << test_count << std::endl;
        std::cout << "Failures: " << fail_count << std::endl;
        std::cout << "Passes: " << (test_count - fail_count) << std::endl;
        
        if (fail_count == 0) {
            std::cout << "All move_iterator tests passed!" << std::endl;
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
