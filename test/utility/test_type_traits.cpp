/**
 * @file test_type_traits.cpp
 * @brief Comprehensive test suite for mystl::type_traits implementation
 * 
 * This file contains extensive tests for the type traits library in mystl.
 * It verifies the correctness of compile-time type introspection, classification,
 * and transformation traits against expected behavior.
 * 
 * The tests are organized into logical sections corresponding to different
 * categories of type traits, making it easy to locate and understand specific tests.
 * 
 * @author shensjt
 * @date 2026-03-16
 * 
 * @see include/utility/type_traits.hpp
 */

#include "../include/utility/type_traits.hpp"
#include <iostream>
#include <cassert>

// Helper macro for testing
#define TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "Test passed: " << #expr << std::endl; \
        } \
    } while(0)

// ==================== Test Classes for Inheritance Testing ====================

/**
 * @brief Base class for inheritance testing
 */
class Base {};

/**
 * @brief Derived class for inheritance testing
 */
class Derived : public Base {};

/**
 * @brief Unrelated class for inheritance testing
 */
class AnotherClass {};

// ==================== Test Classes for Construction/Assignment Testing ====================

/**
 * @brief Fully constructible and assignable class
 */
class Constructible {
public:
    Constructible() = default;
    Constructible(const Constructible&) = default;
    Constructible(Constructible&&) = default;
    Constructible& operator=(const Constructible&) = default;
    Constructible& operator=(Constructible&&) = default;
    ~Constructible() = default;
};

/**
 * @brief Non-copyable but movable class
 */
class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
    ~NonCopyable() = default;
};

/**
 * @brief Class without default constructor
 */
class NonDefaultConstructible {
public:
    NonDefaultConstructible(int) {}
    NonDefaultConstructible(const NonDefaultConstructible&) = default;
    ~NonDefaultConstructible() = default;
};

// ==================== Test Classes for Type Properties ====================

/**
 * @brief Abstract base class with virtual functions
 */
class PolymorphicBase {
public:
    virtual ~PolymorphicBase() = default;
    virtual void foo() = 0;
};

/**
 * @brief Concrete derived class implementing virtual functions
 */
class PolymorphicDerived : public PolymorphicBase {
public:
    void foo() override {}
};

/**
 * @brief Empty class (no members)
 */
class EmptyClass {};

// ==================== Test Structs for is_empty Testing ====================

/**
 * @brief Non-empty struct with data member
 */
struct NonEmptyStruct { int x; };

/**
 * @brief Empty struct (no members)
 */
struct EmptyStruct {};

/**
 * @brief Struct with virtual destructor (not empty due to vtable)
 */
struct EmptyWithVirtual { virtual ~EmptyWithVirtual() = default; };

/**
 * @brief Final empty class
 */
struct EmptyFinal final {};

// ==================== Additional Test Classes for New Traits ====================

/**
 * @brief Trivial class (POD type)
 */
struct TrivialStruct {
    int x;
    double y;
};

/**
 * @brief Non-trivial class (has user-defined constructor)
 */
struct NonTrivialStruct {
    NonTrivialStruct() : x(0) {}
    int x;
};

/**
 * @brief Standard layout struct (all members have same access control)
 */
struct StandardLayoutStruct {
    int x;
    double y;
};

/**
 * @brief Non-standard layout struct (has virtual function)
 */
struct NonStandardLayoutStruct {
    virtual ~NonStandardLayoutStruct() = default;
    int x;
};

/**
 * @brief Non-standard layout struct (mixed access control)
 */
struct NonStandardLayoutMixedAccess {
    int x;
private:
    int y;  // Different access control breaks standard layout
};

/**
 * @brief Final class for is_final testing
 */
class FinalClass final {};

/**
 * @brief Non-final class for is_final testing
 */
class NonFinalClass {};

/**
 * @brief Enum for testing is_enum and underlying_type
 */
enum TestEnum { VALUE1, VALUE2, VALUE3 };

/**
 * @brief Scoped enum for testing
 */
enum class ScopedEnum { A, B, C };

/**
 * @brief Enum with specified underlying type
 */
enum class EnumWithType : unsigned long { X, Y, Z };

// ==================== Test Functions ====================

/**
 * @brief Main test function for type_traits
 * 
 * @return int 0 if all tests pass, 1 if any test fails
 * 
 * @details This function organizes tests into logical sections and runs
 * comprehensive verification of all type traits in the mystl library.
 */
int main() {
    std::cout << "=== Testing mystl type_traits ===\n" << std::endl;
    
    // ==================== 1. Basic Utilities ====================
    std::cout << "1. Testing Basic Utilities:" << std::endl;
    
    // integral_constant
    TEST((mystl::integral_constant<int, 42>::value == 42));
    TEST((mystl::integral_constant<bool, true>::value == true));
    TEST((mystl::integral_constant<bool, false>::value == false));
    
    // true_type and false_type
    TEST((mystl::true_type::value == true));
    TEST((mystl::false_type::value == false));
    
    // ==================== 2. Type Transformations (Basic) ====================
    std::cout << "\n2. Testing Type Transformations (Basic):" << std::endl;
    
    // remove_const
    TEST((mystl::is_same_v<mystl::remove_const_t<const int>, int>));
    TEST((mystl::is_same_v<mystl::remove_const_t<int>, int>));
    TEST((mystl::is_same_v<mystl::remove_const_t<const volatile int>, volatile int>));
    
    // remove_volatile
    TEST((mystl::is_same_v<mystl::remove_volatile_t<volatile int>, int>));
    TEST((mystl::is_same_v<mystl::remove_volatile_t<int>, int>));
    TEST((mystl::is_same_v<mystl::remove_volatile_t<const volatile int>, const int>));
    
    // remove_cv
    TEST((mystl::is_same_v<mystl::remove_cv_t<const volatile int>, int>));
    TEST((mystl::is_same_v<mystl::remove_cv_t<const int>, int>));
    TEST((mystl::is_same_v<mystl::remove_cv_t<volatile int>, int>));
    TEST((mystl::is_same_v<mystl::remove_cv_t<int>, int>));
    
    // remove_reference
    TEST((mystl::is_same_v<mystl::remove_reference_t<int&>, int>));
    TEST((mystl::is_same_v<mystl::remove_reference_t<int&&>, int>));
    TEST((mystl::is_same_v<mystl::remove_reference_t<int>, int>));
    
    // ==================== 3. Type Checks ====================
    std::cout << "\n3. Testing Type Checks:" << std::endl;
    
    // is_same
    TEST((mystl::is_same_v<int, int> == true));
    TEST((mystl::is_same_v<int, double> == false));
    TEST((mystl::is_same_v<const int, int> == false));
    TEST((mystl::is_same_v<int&, int> == false));
    
    // is_const
    TEST((mystl::is_const_v<const int> == true));
    TEST((mystl::is_const_v<int> == false));
    TEST((mystl::is_const_v<const volatile int> == true));
    
    // is_volatile
    TEST((mystl::is_volatile_v<volatile int> == true));
    TEST((mystl::is_volatile_v<int> == false));
    TEST((mystl::is_volatile_v<const volatile int> == true));
    
    // is_pointer
    TEST((mystl::is_pointer_v<int*> == true));
    TEST((mystl::is_pointer_v<const int*> == true));
    TEST((mystl::is_pointer_v<int* const> == true));
    TEST((mystl::is_pointer_v<int> == false));
    TEST((mystl::is_pointer_v<int&> == false));
    
    // is_reference
    TEST((mystl::is_reference_v<int&> == true));
    TEST((mystl::is_reference_v<int&&> == true));
    TEST((mystl::is_reference_v<int> == false));
    
    // is_lvalue_reference
    TEST((mystl::is_lvalue_reference_v<int&> == true));
    TEST((mystl::is_lvalue_reference_v<int&&> == false));
    TEST((mystl::is_lvalue_reference_v<int> == false));
    
    // is_rvalue_reference
    TEST((mystl::is_rvalue_reference_v<int&&> == true));
    TEST((mystl::is_rvalue_reference_v<int&> == false));
    TEST((mystl::is_rvalue_reference_v<int> == false));
    
    // is_void
    TEST((mystl::is_void_v<void> == true));
    TEST((mystl::is_void_v<const void> == true));
    TEST((mystl::is_void_v<volatile void> == true));
    TEST((mystl::is_void_v<const volatile void> == true));
    TEST((mystl::is_void_v<int> == false));
    
    // is_null_pointer
    TEST((mystl::is_null_pointer_v<std::nullptr_t> == true));
    TEST((mystl::is_null_pointer_v<const std::nullptr_t> == true));
    TEST((mystl::is_null_pointer_v<int> == false));
    
    // is_class
    TEST((mystl::is_class_v<Base> == true));
    TEST((mystl::is_class_v<Derived> == true));
    TEST((mystl::is_class_v<AnotherClass> == true));
    TEST((mystl::is_class_v<int> == false));
    TEST((mystl::is_class_v<int*> == false));
    
    // ==================== 4. Integer and Floating Point Types ====================
    std::cout << "\n4. Testing Integer and Floating Point Types:" << std::endl;
    
    // is_integral
    TEST((mystl::is_integral_v<int> == true));
    TEST((mystl::is_integral_v<const int> == true));
    TEST((mystl::is_integral_v<unsigned long long> == true));
    TEST((mystl::is_integral_v<bool> == true));
    TEST((mystl::is_integral_v<char> == true));
    TEST((mystl::is_integral_v<float> == false));
    TEST((mystl::is_integral_v<double> == false));
    TEST((mystl::is_integral_v<int*> == false));
    
    // is_floating_point
    TEST((mystl::is_floating_point_v<float> == true));
    TEST((mystl::is_floating_point_v<double> == true));
    TEST((mystl::is_floating_point_v<long double> == true));
    TEST((mystl::is_floating_point_v<const double> == true));
    TEST((mystl::is_floating_point_v<int> == false));
    TEST((mystl::is_floating_point_v<void> == false));
    
    // is_arithmetic
    TEST((mystl::is_arithmetic_v<int> == true));
    TEST((mystl::is_arithmetic_v<float> == true));
    TEST((mystl::is_arithmetic_v<const double> == true));
    TEST((mystl::is_arithmetic_v<void> == false));
    TEST((mystl::is_arithmetic_v<int*> == false));
    TEST((mystl::is_arithmetic_v<Base> == false));
    
    // is_signed / is_unsigned
    TEST((mystl::is_signed_v<int> == true));
    TEST((mystl::is_signed_v<float> == true));
    TEST((mystl::is_signed_v<unsigned int> == false));
    TEST((mystl::is_unsigned_v<unsigned int> == true));
    TEST((mystl::is_unsigned_v<int> == false));
    TEST((mystl::is_unsigned_v<float> == false));
    
    // ==================== 5. Array and Function Types ====================
    std::cout << "\n5. Testing Array and Function Types:" << std::endl;
    
    // is_array
    TEST((mystl::is_array_v<int[]> == true));
    TEST((mystl::is_array_v<int[5]> == true));
    TEST((mystl::is_array_v<int> == false));
    TEST((mystl::is_array_v<int*> == false));
    
    // remove_extent
    TEST((mystl::is_same_v<mystl::remove_extent_t<int[]>, int>));
    TEST((mystl::is_same_v<mystl::remove_extent_t<int[5]>, int>));
    TEST((mystl::is_same_v<mystl::remove_extent_t<int>, int>));
    
    // remove_all_extents
    TEST((mystl::is_same_v<mystl::remove_all_extents_t<int[3][4]>, int>));
    TEST((mystl::is_same_v<mystl::remove_all_extents_t<int[2][3][4]>, int>));
    TEST((mystl::is_same_v<mystl::remove_all_extents_t<int[]>, int>));
    TEST((mystl::is_same_v<mystl::remove_all_extents_t<int>, int>));
    
    // is_function
    TEST((mystl::is_function_v<void()> == true));
    TEST((mystl::is_function_v<int(double, char)> == true));
    TEST((mystl::is_function_v<int> == false));
    TEST((mystl::is_function_v<int*> == false));
    
    // ==================== 6. Type Transformations (Advanced) ====================
    std::cout << "\n6. Testing Type Transformations (Advanced):" << std::endl;
    
    // add_const
    TEST((mystl::is_same_v<mystl::add_const_t<int>, const int>));
    TEST((mystl::is_same_v<mystl::add_const_t<const int>, const int>));
    
    // add_volatile
    TEST((mystl::is_same_v<mystl::add_volatile_t<int>, volatile int>));
    TEST((mystl::is_same_v<mystl::add_volatile_t<volatile int>, volatile int>));
    
    // add_cv
    TEST((mystl::is_same_v<mystl::add_cv_t<int>, const volatile int>));
    TEST((mystl::is_same_v<mystl::add_cv_t<const int>, const volatile int>));
    
    // add_pointer
    TEST((mystl::is_same_v<mystl::add_pointer_t<int>, int*>));
    TEST((mystl::is_same_v<mystl::add_pointer_t<int&>, int*>));
    TEST((mystl::is_same_v<mystl::add_pointer_t<int&&>, int*>));
    
    // add_lvalue_reference
    TEST((mystl::is_same_v<mystl::add_lvalue_reference_t<int>, int&>));
    TEST((mystl::is_same_v<mystl::add_lvalue_reference_t<int&>, int&>));
    
    // add_rvalue_reference
    TEST((mystl::is_same_v<mystl::add_rvalue_reference_t<int>, int&&>));
    TEST((mystl::is_same_v<mystl::add_rvalue_reference_t<int&&>, int&&>));
    
    // ==================== 7. Conditional Selection ====================
    std::cout << "\n7. Testing Conditional Selection:" << std::endl;
    
    // conditional
    TEST((mystl::is_same_v<mystl::conditional_t<true, int, double>, int>));
    TEST((mystl::is_same_v<mystl::conditional_t<false, int, double>, double>));
    
    // conjunction
    TEST((mystl::conjunction_v<mystl::true_type> == true));
    TEST((mystl::conjunction_v<mystl::false_type> == false));
    TEST((mystl::conjunction_v<mystl::true_type, mystl::true_type> == true));
    TEST((mystl::conjunction_v<mystl::true_type, mystl::false_type> == false));
    TEST((mystl::conjunction_v<mystl::false_type, mystl::true_type> == false));
    
    // disjunction
    TEST((mystl::disjunction_v<mystl::true_type> == true));
    TEST((mystl::disjunction_v<mystl::false_type> == false));
    TEST((mystl::disjunction_v<mystl::true_type, mystl::false_type> == true));
    TEST((mystl::disjunction_v<mystl::false_type, mystl::true_type> == true));
    TEST((mystl::disjunction_v<mystl::false_type, mystl::false_type> == false));
    
    // negation
    TEST((mystl::negation_v<mystl::true_type> == false));
    TEST((mystl::negation_v<mystl::false_type> == true));
    
    // ==================== 8. Type Transformations (Complex) ====================
    std::cout << "\n8. Testing Type Transformations (Complex):" << std::endl;
    
    // declval - test that it can be used in unevaluated context
    {
        using T = decltype(mystl::declval<int>());
        TEST((mystl::is_same_v<T, int&&>));
    }
    {
        using T = decltype(mystl::declval<int&>());
        TEST((mystl::is_same_v<T, int&>));
    }
    
    // decay
    TEST((mystl::is_same_v<mystl::decay_t<int[]>, int*>));
    TEST((mystl::is_same_v<mystl::decay_t<int[5]>, int*>));
    TEST((mystl::is_same_v<mystl::decay_t<int&>, int>));
    TEST((mystl::is_same_v<mystl::decay_t<const int&>, int>));
    TEST((mystl::is_same_v<mystl::decay_t<void()>, void(*)()>));
    
    // ==================== 9. Type Relationships ====================
    std::cout << "\n9. Testing Type Relationships:" << std::endl;
    
    // is_base_of
    TEST((mystl::is_base_of<Base, Derived>::value == true));
    TEST((mystl::is_base_of<Base, Base>::value == true));
    TEST((mystl::is_base_of<Derived, Base>::value == false));
    TEST((mystl::is_base_of<int, int>::value == false));
    
    // is_convertible
    TEST((mystl::is_convertible<int, double>::value == true));
    TEST((mystl::is_convertible<double, int>::value == true));
    TEST((mystl::is_convertible<Derived*, Base*>::value == true));
    TEST((mystl::is_convertible<Base*, Derived*>::value == false));
    TEST((mystl::is_convertible<void, int>::value == false));
    TEST((mystl::is_convertible<int, void>::value == true));
    
    // ==================== 10. Helper Tools ====================
    std::cout << "\n10. Testing Helper Tools:" << std::endl;
    
    // void_t
    TEST((mystl::is_same_v<mystl::void_t<int, double, char>, void>));
    TEST((mystl::is_same_v<mystl::void_t<>, void>));
    
    // ==================== 11. SFINAE Support ====================
    std::cout << "\n11. Testing SFINAE Support:" << std::endl;
    
    // enable_if
    {
        using T1 = mystl::enable_if_t<true, int>;
        TEST((mystl::is_same_v<T1, int>));
    }
    
    // ==================== 12. Construction and Assignment Traits ====================
    std::cout << "\n12. Testing Construction and Assignment Traits:" << std::endl;
    
    // is_constructible
    TEST((mystl::is_constructible_v<int> == true));
    TEST((mystl::is_constructible_v<int, int> == true));
    TEST((mystl::is_constructible_v<Constructible> == true));
    TEST((mystl::is_constructible_v<NonDefaultConstructible> == false));
    TEST((mystl::is_constructible_v<NonDefaultConstructible, int> == true));
    
    // is_default_constructible
    TEST((mystl::is_default_constructible_v<int> == true));
    TEST((mystl::is_default_constructible_v<Constructible> == true));
    TEST((mystl::is_default_constructible_v<NonDefaultConstructible> == false));
    TEST((mystl::is_default_constructible_v<void> == false));
    
    // is_copy_constructible
    TEST((mystl::is_copy_constructible_v<int> == true));
    TEST((mystl::is_copy_constructible_v<Constructible> == true));
    TEST((mystl::is_copy_constructible_v<NonCopyable> == false));
    TEST((mystl::is_copy_constructible_v<void> == false));
    
    // is_move_constructible
    TEST((mystl::is_move_constructible_v<int> == true));
    TEST((mystl::is_move_constructible_v<Constructible> == true));
    TEST((mystl::is_move_constructible_v<NonCopyable> == true));
    
    // is_copy_assignable
    TEST((mystl::is_copy_assignable_v<int> == true));
    TEST((mystl::is_copy_assignable_v<Constructible> == true));
    TEST((mystl::is_copy_assignable_v<NonCopyable> == false));
    TEST((mystl::is_copy_assignable_v<void> == false));
    
    // is_move_assignable
    TEST((mystl::is_move_assignable_v<int> == true));
    TEST((mystl::is_move_assignable_v<Constructible> == true));
    TEST((mystl::is_move_assignable_v<NonCopyable> == true));
    
    // is_destructible
    TEST((mystl::is_destructible_v<int> == true));
    TEST((mystl::is_destructible_v<Constructible> == true));
    TEST((mystl::is_destructible_v<void> == false));
    
    // ==================== 13. Type Properties ====================
    std::cout << "\n13. Testing Type Properties:" << std::endl;
    
    // is_polymorphic
    TEST((mystl::is_polymorphic_v<PolymorphicBase> == true));
    TEST((mystl::is_polymorphic_v<PolymorphicDerived> == true));
    TEST((mystl::is_polymorphic_v<Base> == false));
    TEST((mystl::is_polymorphic_v<int> == false));
    
    // is_abstract
    TEST((mystl::is_abstract_v<PolymorphicBase> == true));
    TEST((mystl::is_abstract_v<PolymorphicDerived> == false));
    TEST((mystl::is_abstract_v<Base> == false));
    TEST((mystl::is_abstract_v<int> == false));
    TEST((mystl::is_abstract_v<int*> == false));
    TEST((mystl::is_abstract_v<void> == false));
    
    // is_empty
    TEST((mystl::is_empty_v<EmptyClass> == true));
    TEST((mystl::is_empty_v<Base> == true));
    TEST((mystl::is_empty_v<int> == false));
    TEST((mystl::is_empty_v<EmptyStruct> == true));
    TEST((mystl::is_empty_v<NonEmptyStruct> == false));
    TEST((mystl::is_empty_v<EmptyWithVirtual> == false));
    
    // ==================== 14. Additional Type Transformations ====================
    std::cout << "\n14. Testing Additional Type Transformations:" << std::endl;
    
    // common_type
    TEST((mystl::is_same_v<mystl::common_type_t<int, double>, double>));
    TEST((mystl::is_same_v<mystl::common_type_t<int, int>, int>));
    TEST((mystl::is_same_v<mystl::common_type_t<int, long, double>, double>));
    TEST((mystl::is_same_v<mystl::common_type_t<int&, double&>, double>));
    TEST((mystl::is_same_v<mystl::common_type_t<const int, volatile double>, double>));
    TEST((mystl::is_same_v<mystl::common_type_t<int*, void*>, void*>));
    TEST((mystl::is_same_v<mystl::common_type_t<const int*, volatile int*>, const volatile int*>));
    
    // make_signed
    TEST((mystl::is_same_v<mystl::make_signed_t<unsigned int>, int>));
    TEST((mystl::is_same_v<mystl::make_signed_t<int>, int>));
    TEST((mystl::is_same_v<mystl::make_signed_t<const unsigned int>, const int>));
    TEST((mystl::is_same_v<mystl::make_signed_t<volatile unsigned long>, volatile long>));
    
    // make_unsigned
    TEST((mystl::is_same_v<mystl::make_unsigned_t<int>, unsigned int>));
    TEST((mystl::is_same_v<mystl::make_unsigned_t<unsigned int>, unsigned int>));
    TEST((mystl::is_same_v<mystl::make_unsigned_t<const int>, const unsigned int>));
    TEST((mystl::is_same_v<mystl::make_unsigned_t<volatile long>, volatile unsigned long>));
    
    // ==================== 15. New Traits Testing ====================
    std::cout << "\n15. Testing New Traits:" << std::endl;
    
    // is_trivial
    TEST((mystl::is_trivial_v<TrivialStruct> == true));
    TEST((mystl::is_trivial_v<NonTrivialStruct> == false));
    TEST((mystl::is_trivial_v<int> == true));
    
    // is_trivially_copyable
    TEST((mystl::is_trivially_copyable_v<TrivialStruct> == true));
    TEST((mystl::is_trivially_copyable_v<int> == true));
    
    // is_standard_layout
    TEST((mystl::is_standard_layout_v<StandardLayoutStruct> == true));
    TEST((mystl::is_standard_layout_v<NonStandardLayoutStruct> == false));
    TEST((mystl::is_standard_layout_v<int> == true));
    
    // is_final
    TEST((mystl::is_final_v<FinalClass> == true));
    TEST((mystl::is_final_v<NonFinalClass> == false));
    TEST((mystl::is_final_v<int> == false));
    
    // is_enum
    TEST((mystl::is_enum_v<TestEnum> == true));
    TEST((mystl::is_enum_v<ScopedEnum> == true));
    TEST((mystl::is_enum_v<EnumWithType> == true));
    TEST((mystl::is_enum_v<int> == false));
    TEST((mystl::is_enum_v<Base> == false));
    
    // underlying_type (compile-time check only)
    {
        using T1 = mystl::underlying_type_t<TestEnum>;
        using T2 = mystl::underlying_type_t<ScopedEnum>;
        using T3 = mystl::underlying_type_t<EnumWithType>;
        // Just check that the types can be obtained (compile-time test)
        static_assert(mystl::is_integral_v<T1>, "underlying_type should return integral type");
        static_assert(mystl::is_integral_v<T2>, "underlying_type should return integral type");
        static_assert(mystl::is_same_v<T3, unsigned long>, "underlying_type should match specified type");
    }
    
    // ==================== 16. Fixed Bugs and Edge Cases ====================
    std::cout << "\n16. Testing Fixed Bugs and Edge Cases:" << std::endl;
    
    // is_base_of fixed bugs
    TEST((mystl::is_base_of_v<Base, Derived> == true));
    TEST((mystl::is_base_of_v<Base, Base> == true));
    TEST((mystl::is_base_of_v<Derived, Base> == false));
    
    // is_convertible void conversions
    TEST((mystl::is_convertible_v<int, void> == true));
    TEST((mystl::is_convertible_v<void, void> == true));
    TEST((mystl::is_convertible_v<void, int> == false));
    TEST((mystl::is_convertible_v<int*, void*> == true));
    TEST((mystl::is_convertible_v<void*, int*> == true));
    
    // SFINAE and ill-formed cases
    std::cout << "  Note: common_type<int[5], double[5]> is correctly ill-formed (no common type after decay)" << std::endl;
    std::cout << "  This matches std::common_type behavior (tested separately)" << std::endl;
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    return 0;
}
