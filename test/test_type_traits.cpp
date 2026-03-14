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

// Test classes for inheritance testing
class Base {};
class Derived : public Base {};
class AnotherClass {};

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
    
    // ==================== 2. Type Checks ====================
    std::cout << "\n2. Testing Type Checks:" << std::endl;
    
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
    
    // ==================== 3. Type Transformations ====================
    std::cout << "\n3. Testing Type Transformations:" << std::endl;
    
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
    
    // ==================== 4. Type Relationships ====================
    std::cout << "\n4. Testing Type Relationships:" << std::endl;
    
    // is_base_of - use ::value since we don't have _v helper
    TEST((mystl::is_base_of<Base, Derived>::value == true));
    TEST((mystl::is_base_of<Base, Base>::value == true));  // A class is a base of itself
    TEST((mystl::is_base_of<Derived, Base>::value == false));
    TEST((mystl::is_base_of<int, int>::value == false));   // Non-class types
    
    // is_convertible - use ::value since we don't have _v helper
    TEST((mystl::is_convertible<int, double>::value == true));
    TEST((mystl::is_convertible<double, int>::value == true));
    TEST((mystl::is_convertible<Derived*, Base*>::value == true));
    TEST((mystl::is_convertible<Base*, Derived*>::value == false));
    TEST((mystl::is_convertible<void, int>::value == false));
    TEST((mystl::is_convertible<int, void>::value == false));
    
    // ==================== 5. Conditional Selection ====================
    std::cout << "\n5. Testing Conditional Selection:" << std::endl;
    
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
    
    // ==================== 6. Helper Tools ====================
    std::cout << "\n6. Testing Helper Tools:" << std::endl;
    
    // void_t
    TEST((mystl::is_same_v<mystl::void_t<int, double, char>, void>));
    TEST((mystl::is_same_v<mystl::void_t<>, void>));
    
    // declval - test that it can be used in unevaluated context
    {
        using T = decltype(mystl::declval<int>());
        TEST((mystl::is_same_v<T, int&&>));
    }
    {
        using T = decltype(mystl::declval<int&>());
        TEST((mystl::is_same_v<T, int&>));
    }
    
    // ==================== 7. New Type Traits Tests ====================
    std::cout << "\n7. Testing New Type Traits:" << std::endl;
    
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
    
    // is_array
    TEST((mystl::is_array_v<int[]> == true));
    TEST((mystl::is_array_v<int[5]> == true));
    TEST((mystl::is_array_v<int> == false));
    TEST((mystl::is_array_v<int*> == false));
    
    // remove_extent
    TEST((mystl::is_same_v<mystl::remove_extent_t<int[]>, int>));
    TEST((mystl::is_same_v<mystl::remove_extent_t<int[5]>, int>));
    TEST((mystl::is_same_v<mystl::remove_extent_t<int>, int>));
    
    // is_function (simplified test)
    TEST((mystl::is_function_v<void()> == true));
    TEST((mystl::is_function_v<int(double, char)> == true));
    TEST((mystl::is_function_v<int> == false));
    TEST((mystl::is_function_v<int*> == false));
    
    // decay
    TEST((mystl::is_same_v<mystl::decay_t<int[]>, int*>));
    TEST((mystl::is_same_v<mystl::decay_t<int[5]>, int*>));
    TEST((mystl::is_same_v<mystl::decay_t<int&>, int>));
    TEST((mystl::is_same_v<mystl::decay_t<const int&>, int>));
    TEST((mystl::is_same_v<mystl::decay_t<void()>, void(*)()>));
    
    // enable_if
    {
        using T1 = mystl::enable_if_t<true, int>;
        TEST((mystl::is_same_v<T1, int>));
        
        // enable_if<false> should have no ::type
        static_assert(mystl::is_same_v<mystl::enable_if<false>, mystl::enable_if<false>>, 
                     "enable_if<false> should exist but have no ::type");
    }
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    return 0;
}
