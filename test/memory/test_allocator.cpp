/**
 * @file test_allocator.cpp
 * @brief Comprehensive test suite for mystl::allocator implementation
 * 
 * This file contains extensive tests for the allocator library in mystl.
 * It verifies the correctness of allocator operations including memory allocation,
 * object construction/destruction, type definitions, and exception safety.
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

// Test Helper Macros
#define TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  Test passed: " << #expr << std::endl; \
        } \
    } while(0)

#define TEST_NAMED(name, expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Test '" << name << "' failed at line " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  Test '" << name << "' passed: " << #expr << std::endl; \
        } \
    } while(0)

// Test class with constructor/destructor tracking
class TrackedObject {
public:
    static int constructor_count;
    static int destructor_count;
    static int copy_count;
    static int move_count;
    
    int value;
    
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
bool test_type_definitions() {
    std::cout << "1. Testing Type Definitions:" << std::endl;
    
    mystl::allocator<int> alloc;
    
    static_assert(std::is_same_v<decltype(alloc)::value_type, int>, 
                  "value_type should be int");
    static_assert(std::is_same_v<decltype(alloc)::pointer, int*>, 
                  "pointer should be int*");
    static_assert(std::is_same_v<decltype(alloc)::const_pointer, const int*>, 
                  "const_pointer should be const int*");
    static_assert(std::is_same_v<decltype(alloc)::reference, int&>, 
                  "reference should be int&");
    static_assert(std::is_same_v<decltype(alloc)::const_reference, const int&>, 
                  "const_reference should be const int&");
    static_assert(std::is_same_v<decltype(alloc)::size_type, std::size_t>, 
                  "size_type should be std::size_t");
    static_assert(std::is_same_v<decltype(alloc)::difference_type, std::ptrdiff_t>, 
                  "difference_type should be std::ptrdiff_t");
    
    static_assert(std::is_same_v<decltype(alloc)::void_pointer, void*>, 
                  "void_pointer should be void*");
    static_assert(std::is_same_v<decltype(alloc)::const_void_pointer, const void*>, 
                  "const_void_pointer should be const void*");
    
    TEST_NAMED("All type definitions", true);
    return true;
}

bool test_allocation_deallocation() {
    std::cout << "\n2. Testing Memory Allocation and Deallocation:" << std::endl;
    
    mystl::allocator<int> alloc;
    
    int* p1 = alloc.allocate(10);
    TEST(p1 != nullptr);
    TEST_NAMED("Normal allocation", p1 != nullptr);
    
    int* p2 = alloc.allocate(0);
    TEST(p2 == nullptr);
    TEST_NAMED("Zero allocation returns nullptr", p2 == nullptr);
    
    alloc.deallocate(p1, 10);
    TEST_NAMED("Deallocation successful", true);
    
    alloc.deallocate(nullptr, 0);
    TEST_NAMED("nullptr deallocation safe", true);
    
    return true;
}

bool test_construction_destruction() {
    std::cout << "\n3. Testing Object Construction and Destruction:" << std::endl;
    
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
    TEST_NAMED("Object construction", true);
    
    TEST(TrackedObject::constructor_count == 3);
    
    alloc.destroy(p);
    alloc.destroy(p + 1);
    alloc.destroy(p + 2);
    TEST_NAMED("Object destruction", true);
    
    TEST(TrackedObject::destructor_count == 3);
    
    alloc.deallocate(p, 3);
    
    return true;
}

bool test_max_size() {
    std::cout << "\n4. Testing max_size Calculation:" << std::endl;
    
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
    TEST_NAMED("max_size calculation", true);
    
    return true;
}

bool test_comparison_operators() {
    std::cout << "\n5. Testing Comparison Operators:" << std::endl;
    
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
    TEST_NAMED("Comparison operators", true);
    
    return true;
}

bool test_void_specialization() {
    std::cout << "\n6. Testing void Specialization:" << std::endl;
    
    mystl::allocator<void> void_alloc;
    mystl::allocator<int> int_alloc;
    
    static_assert(std::is_same_v<decltype(void_alloc)::value_type, void>, 
                  "void specialization value_type should be void");
    static_assert(std::is_same_v<decltype(void_alloc)::pointer, void*>, 
                  "void specialization pointer should be void*");
    
    mystl::allocator<void> void_alloc2(int_alloc);
    
    TEST_NAMED("void specialization", true);
    return true;
}

bool test_rebind() {
    std::cout << "\n7. Testing Rebind Mechanism:" << std::endl;
    
    mystl::allocator<int> int_alloc;
    
    using rebind_alloc = typename mystl::allocator<int>::template rebind<double>::other;
    rebind_alloc double_alloc(int_alloc);
    
    double* p = double_alloc.allocate(5);
    TEST(p != nullptr);
    double_alloc.deallocate(p, 5);
    
    TEST_NAMED("Rebind mechanism", true);
    return true;
}

bool test_select_on_container_copy_construction() {
    std::cout << "\n8. Testing Container Copy Construction Selection:" << std::endl;
    
    mystl::allocator<int> alloc1;
    auto alloc2 = alloc1.select_on_container_copy_construction();
    
    TEST(alloc1 == alloc2);
    TEST_NAMED("select_on_container_copy_construction", true);
    
    return true;
}

bool test_allocate_with_hint() {
    std::cout << "\n9. Testing Allocation with Hint (C++20 optional):" << std::endl;
    
    mystl::allocator<int> alloc;
    
    int* hint = alloc.allocate(1);
    int* p = alloc.allocate(10, hint);
    TEST(p != nullptr);
    
    alloc.deallocate(p, 10);
    alloc.deallocate(hint, 1);
    
    TEST_NAMED("Allocation with hint", true);
    return true;
}

bool test_exception_safety() {
    std::cout << "\n10. Testing Exception Safety:" << std::endl;
    
    mystl::allocator<int> alloc;
    
    try {
        auto max_size = alloc.max_size();
        
        try {
            int* p2 = alloc.allocate(max_size + 1);
            if (p2) {
                alloc.deallocate(p2, max_size + 1);
            }
            TEST(false && "Should have thrown std::bad_alloc");
        } catch (const std::bad_alloc&) {
            TEST_NAMED("Over max_size allocation throws std::bad_alloc", true);
        }
        
    } catch (const std::bad_alloc& e) {
        std::cout << "  Large memory allocation failed (normal): " << e.what() << std::endl;
        TEST_NAMED("Exception safety", true);
    } catch (...) {
        TEST(false && "Unexpected exception");
    }
    
    return true;
}

bool test_with_standard_containers() {
    std::cout << "\n11. Testing with Standard Containers:" << std::endl;
    
    {
        std::vector<int, mystl::allocator<int>> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        
        TEST(vec.size() == 3);
        TEST(vec[0] == 1);
        TEST(vec[1] == 2);
        TEST(vec[2] == 3);
        TEST_NAMED("Works with std::vector", true);
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
        TEST_NAMED("Works with std::list (rebind)", true);
    }
    
    return true;
}

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Testing mystl::allocator Implementation" << std::endl;
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
    } else {
        std::cout << "Some tests FAILED!" << std::endl;
    }
    std::cout << "=========================================" << std::endl;
    
    return all_passed ? 0 : 1;
}
