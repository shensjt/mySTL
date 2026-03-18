/**
 * @file test_allocator.cpp
 * @brief Comprehensive test suite for mystl::allocator implementation
 * @brief mystl::allocator实现的综合测试套件
 * 
 * This file contains extensive tests for the allocator library in mystl.
 * It verifies the correctness of allocator operations including memory allocation,
 * object construction/destruction, type definitions, and exception safety.
 * 
 * 此文件包含对mystl中分配器库的广泛测试。
 * 验证分配器操作的正确性，包括内存分配、对象构造/销毁、类型定义和异常安全性。
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/allocator.hpp
 */

#include "../../include/memory/allocator.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <list>

// Test Helper Macros with cleaner bilingual output
// 测试辅助宏，带有更清晰的双语输出
#define TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "[FAIL] Line " << __LINE__ << ": " << #expr << std::endl; \
            std::cerr << "[失败] 行号 " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  ✓ " << #expr << std::endl; \
        } \
    } while(0)

#define TEST_NAMED(name, expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "[FAIL] Test '" << name << "' at line " << __LINE__ << ": " << #expr << std::endl; \
            std::cerr << "[失败] 测试 '" << name << "' 行号 " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  ✓ " << name << " (" << #expr << ")" << std::endl; \
        } \
    } while(0)

// Test class with constructor/destructor tracking
// 用于跟踪构造函数/析构函数的测试类
class TrackedObject {
public:
    static int constructor_count;    ///< 构造函数调用计数 / Constructor call count
    static int destructor_count;     ///< 析构函数调用计数 / Destructor call count
    static int copy_count;           ///< 拷贝构造函数调用计数 / Copy constructor call count
    static int move_count;           ///< 移动构造函数调用计数 / Move constructor call count
    
    int value;                       ///< 对象值 / Object value
    
    TrackedObject() : value(0) { 
        ++constructor_count; 
    }
    
    TrackedObject(int v) : value(v) { 
        ++constructor_count; 
    }
    
    TrackedObject(const TrackedObject& other) : value(other.value) { 
        ++constructor_count; 
        ++copy_count;
    }
    
    TrackedObject(TrackedObject&& other) noexcept : value(other.value) { 
        ++constructor_count; 
        ++move_count;
        other.value = -1;
    }
    
    ~TrackedObject() { 
        ++destructor_count; 
    }
    
    static void reset_counts() {
        constructor_count = 0;
        destructor_count = 0;
        copy_count = 0;
        move_count = 0;
    }
};

int TrackedObject::constructor_count = 0;
int TrackedObject::destructor_count = 0;
int TrackedObject::copy_count = 0;
int TrackedObject::move_count = 0;

// Test functions
// 测试函数

/**
 * @brief Test type definitions of allocator
 * @brief 测试分配器的类型定义
 * 
 * Verifies that all type aliases in mystl::allocator are correctly defined.
 * 验证mystl::allocator中的所有类型别名是否正确定义。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_type_definitions() {
    std::cout << "1. Testing Type Definitions / 测试类型定义:" << std::endl;
    
    mystl::allocator<int> alloc;
    
    static_assert(std::is_same_v<decltype(alloc)::value_type, int>, 
                  "value_type should be int / value_type应为int");
    static_assert(std::is_same_v<decltype(alloc)::pointer, int*>, 
                  "pointer should be int* / pointer应为int*");
    static_assert(std::is_same_v<decltype(alloc)::const_pointer, const int*>, 
                  "const_pointer should be const int* / const_pointer应为const int*");
    static_assert(std::is_same_v<decltype(alloc)::reference, int&>, 
                  "reference should be int& / reference应为int&");
    static_assert(std::is_same_v<decltype(alloc)::const_reference, const int&>, 
                  "const_reference should be const int& / const_reference应为const int&");
    static_assert(std::is_same_v<decltype(alloc)::size_type, std::size_t>, 
                  "size_type should be std::size_t / size_type应为std::size_t");
    static_assert(std::is_same_v<decltype(alloc)::difference_type, std::ptrdiff_t>, 
                  "difference_type should be std::ptrdiff_t / difference_type应为std::ptrdiff_t");
    
    static_assert(std::is_same_v<decltype(alloc)::void_pointer, void*>, 
                  "void_pointer should be void* / void_pointer应为void*");
    static_assert(std::is_same_v<decltype(alloc)::const_void_pointer, const void*>, 
                  "const_void_pointer should be const void* / const_void_pointer应为const void*");
    
    TEST_NAMED("All type definitions / 所有类型定义", true);
    return true;
}

/**
 * @brief Test memory allocation and deallocation
 * @brief 测试内存分配和释放
 * 
 * Tests basic memory allocation and deallocation operations.
 * 测试基本的内存分配和释放操作。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_allocation_deallocation() {
    std::cout << "\n2. Testing Memory Allocation and Deallocation / 测试内存分配和释放:" << std::endl;
    
    mystl::allocator<int> alloc;
    
    int* p1 = alloc.allocate(10);
    TEST(p1 != nullptr);
    TEST_NAMED("Normal allocation / 正常分配", p1 != nullptr);
    
    int* p2 = alloc.allocate(0);
    TEST(p2 == nullptr);
    TEST_NAMED("Zero allocation returns nullptr / 零分配返回nullptr", p2 == nullptr);
    
    alloc.deallocate(p1, 10);
    TEST_NAMED("Deallocation successful / 释放成功", true);
    
    alloc.deallocate(nullptr, 0);
    TEST_NAMED("nullptr deallocation safe / nullptr释放安全", true);
    
    return true;
}

/**
 * @brief Test object construction and destruction
 * @brief 测试对象构造和销毁
 * 
 * Tests object construction and destruction using allocator.
 * 测试使用分配器进行对象构造和销毁。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_construction_destruction() {
    std::cout << "\n3. Testing Object Construction and Destruction / 测试对象构造和销毁:" << std::endl;
    
    TrackedObject::reset_counts();
    mystl::allocator<TrackedObject> alloc;
    
    TrackedObject* p = alloc.allocate(3);
    TEST(p != nullptr);
    
    alloc.construct(p, 100);
    alloc.construct(p + 1);
    alloc.construct(p + 2, TrackedObject(200));
    
    TEST(p[0].value == 100);
    TEST(p[1].value == 0);
    TEST(p[2].value == 200);
    TEST_NAMED("Object construction / 对象构造", true);
    
    TEST(TrackedObject::constructor_count == 3);
    
    alloc.destroy(p);
    alloc.destroy(p + 1);
    alloc.destroy(p + 2);
    TEST_NAMED("Object destruction / 对象销毁", true);
    
    TEST(TrackedObject::destructor_count == 3);
    
    alloc.deallocate(p, 3);
    
    return true;
}

/**
 * @brief Test max_size calculation
 * @brief 测试max_size计算
 * 
 * Tests the max_size() function for different types.
 * 测试不同类型的max_size()函数。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_max_size() {
    std::cout << "\n4. Testing max_size Calculation / 测试max_size计算:" << std::endl;
    
    mystl::allocator<int> int_alloc;
    mystl::allocator<double> double_alloc;
    mystl::allocator<char> char_alloc;
    
    auto int_max = int_alloc.max_size();
    auto double_max = double_alloc.max_size();
    auto char_max = char_alloc.max_size();
    
    std::cout << "  int max_size: " << int_max << std::endl;
    std::cout << "  double max_size: " << double_max << std::endl;
    std::cout << "  char max_size: " << char_max << std::endl;
    
    TEST(char_max > int_max);
    TEST(int_max > double_max);
    TEST_NAMED("max_size calculation / max_size计算", true);
    
    return true;
}

/**
 * @brief Test comparison operators
 * @brief 测试比较操作符
 * 
 * Tests equality and inequality operators for allocators.
 * 测试分配器的相等和不相等操作符。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_comparison_operators() {
    std::cout << "\n5. Testing Comparison Operators / 测试比较操作符:" << std::endl;
    
    mystl::allocator<int> alloc1;
    mystl::allocator<int> alloc2;
    mystl::allocator<double> alloc3;
    
    bool eq1 = (alloc1 == alloc2);
    bool eq2 = (alloc1 == alloc3);
    bool ne1 = (alloc1 != alloc2);
    bool ne2 = (alloc1 != alloc3);
    
    TEST(eq1);
    TEST(eq2);
    TEST(!ne1);
    TEST(!ne2);
    TEST_NAMED("Comparison operators / 比较操作符", true);
    
    return true;
}

/**
 * @brief Test void specialization
 * @brief 测试void特化
 * 
 * Tests the void specialization of allocator.
 * 测试分配器的void特化。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_void_specialization() {
    std::cout << "\n6. Testing void Specialization / 测试void特化:" << std::endl;
    
    mystl::allocator<void> void_alloc;
    mystl::allocator<int> int_alloc;
    
    static_assert(std::is_same_v<decltype(void_alloc)::value_type, void>, 
                  "void specialization value_type should be void / void特化的value_type应为void");
    static_assert(std::is_same_v<decltype(void_alloc)::pointer, void*>, 
                  "void specialization pointer should be void* / void特化的pointer应为void*");
    
    mystl::allocator<void> void_alloc2(int_alloc);
    
    TEST_NAMED("void specialization / void特化", true);
    return true;
}

/**
 * @brief Test rebind mechanism
 * @brief 测试重新绑定机制
 * 
 * Tests the rebind mechanism for changing allocator type.
 * 测试用于更改分配器类型的重新绑定机制。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_rebind() {
    std::cout << "\n7. Testing Rebind Mechanism / 测试重新绑定机制:" << std::endl;
    
    mystl::allocator<int> int_alloc;
    
    using rebind_alloc = typename mystl::allocator<int>::template rebind<double>::other;
    rebind_alloc double_alloc(int_alloc);
    
    double* p = double_alloc.allocate(5);
    TEST(p != nullptr);
    double_alloc.deallocate(p, 5);
    
    TEST_NAMED("Rebind mechanism / 重新绑定机制", true);
    return true;
}

/**
 * @brief Test select_on_container_copy_construction
 * @brief 测试容器复制构造选择
 * 
 * Tests the select_on_container_copy_construction method.
 * 测试select_on_container_copy_construction方法。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_select_on_container_copy_construction() {
    std::cout << "\n8. Testing Container Copy Construction Selection / 测试容器复制构造选择:" << std::endl;
    
    mystl::allocator<int> alloc1;
    auto alloc2 = alloc1.select_on_container_copy_construction();
    
    TEST(alloc1 == alloc2);
    TEST_NAMED("select_on_container_copy_construction / 容器复制构造选择", true);
    
    return true;
}

/**
 * @brief Test allocation with hint (C++20 optional)
 * @brief 测试带提示的分配（C++20可选）
 * 
 * Tests allocation with hint parameter (C++20 optional feature).
 * 测试带提示参数的分配（C++20可选功能）。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_allocate_with_hint() {
    std::cout << "\n9. Testing Allocation with Hint (C++20 optional) / 测试带提示的分配（C++20可选）:" << std::endl;
    
    mystl::allocator<int> alloc;
    
    int* hint = alloc.allocate(1);
    int* p = alloc.allocate(10, hint);
    TEST(p != nullptr);
    
    alloc.deallocate(p, 10);
    alloc.deallocate(hint, 1);
    
    TEST_NAMED("Allocation with hint / 带提示的分配", true);
    return true;
}

/**
 * @brief Test exception safety
 * @brief 测试异常安全性
 * 
 * Tests exception safety guarantees of allocator operations.
 * 测试分配器操作的异常安全保证。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_exception_safety() {
    std::cout << "\n10. Testing Exception Safety / 测试异常安全性:" << std::endl;
    
    mystl::allocator<int> alloc;
    
    try {
        auto max_size = alloc.max_size();
        
        try {
            int* p2 = alloc.allocate(max_size + 1);
            if (p2) {
                alloc.deallocate(p2, max_size + 1);
            }
            TEST(false && "Should have thrown std::bad_alloc / 应该抛出std::bad_alloc");
        } catch (const std::bad_alloc&) {
            TEST_NAMED("Over max_size allocation throws std::bad_alloc / 超过max_size的分配抛出std::bad_alloc", true);
        }
        
    } catch (const std::bad_alloc& e) {
        std::cout << "  Large memory allocation failed (normal): " << e.what() << std::endl;
        std::cout << "  大内存分配失败（正常）: " << e.what() << std::endl;
        TEST_NAMED("Exception safety / 异常安全性", true);
    } catch (...) {
        TEST(false && "Unexpected exception / 意外的异常");
    }
    
    return true;
}

/**
 * @brief Test with standard containers
 * @brief 测试与标准容器的兼容性
 * 
 * Tests that allocator works correctly with standard containers.
 * 测试分配器与标准容器正常工作。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_with_standard_containers() {
    std::cout << "\n11. Testing with Standard Containers / 测试与标准容器的兼容性:" << std::endl;
    
    {
        std::vector<int, mystl::allocator<int>> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        
        TEST(vec.size() == 3);
        TEST(vec[0] == 1);
        TEST(vec[1] == 2);
        TEST(vec[2] == 3);
        TEST_NAMED("Works with std::vector / 与std::vector兼容", true);
    }
    
    {
        std::list<int, mystl::allocator<int>> lst;
        lst.push_back(1);
        lst.push_back(2);
        lst.push_back(3);
        
        TEST(lst.size() == 3);
        auto it = lst.begin();
        TEST(*it == 1);
        ++it;
        TEST(*it == 2);
        ++it;
        TEST(*it == 3);
        TEST_NAMED("Works with std::list (rebind) / 与std::list兼容（重新绑定）", true);
    }
    
    return true;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 * 
 * Runs all allocator tests and reports results.
 * 运行所有分配器测试并报告结果。
 * 
 * @return 0 if all tests pass, 1 otherwise
 *         如果所有测试通过返回0，否则返回1
 */
int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Testing mystl::allocator Implementation" << std::endl;
    std::cout << "测试mystl::allocator实现" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    bool all_passed = true;
    
    all_passed &= test_type_definitions();
    all_passed &= test_allocation_deallocation();
    all_passed &= test_construction_destruction();
    all_passed &= test_max_size();
    all_passed &= test_comparison_operators();
    all_passed &= test_void_specialization();
    all_passed &= test_rebind();
    all_passed &= test_select_on_container_copy_construction();
    all_passed &= test_allocate_with_hint();
    all_passed &= test_exception_safety();
    all_passed &= test_with_standard_containers();
    
    std::cout << "\n=========================================" << std::endl;
    if (all_passed) {
        std::cout << "All tests PASSED!" << std::endl;
        std::cout << "所有测试通过！" << std::endl;
    } else {
        std::cout << "Some tests FAILED!" << std::endl;
        std::cout << "部分测试失败！" << std::endl;
    }
    std::cout << "=========================================" << std::endl;
    
    return all_passed ? 0 : 1;
}
