#pragma once
#include <cstddef>

/**
 * @file type_traits.hpp
 * @brief Type traits library for compile-time type introspection and transformation
 * @brief 编译时类型内省和转换的类型特性库
 * 
 * This header provides a comprehensive set of type traits for compile-time type introspection,
 * type classification, and type transformations. It implements the C++ standard library type
 * traits with additional utilities for template metaprogramming.
 * 
 * 本头文件提供了一套全面的类型特性，用于编译时类型内省、类型分类和类型转换。
 * 它实现了C++标准库类型特性，并提供了模板元编程的额外工具。
 * 
 * @author mySTL Team
 * @version 1.0.0
 * @date 2026-03-16
 * 
 * @see https://en.cppreference.com/w/cpp/header/type_traits
 * @see https://zh.cppreference.com/w/cpp/header/type_traits
 */

namespace mystl {
// ==================== 1. Basic Utilities ====================
// ==================== 1. 基础工具 ====================

/**
 * @brief Base class for all type traits providing a compile-time constant value
 * @brief 所有类型特性的基类，提供编译时常量值
 * 
 * @tparam T Type of the constant value
 * @tparam T 常量值的类型
 * @tparam v Constant value of type T
 * @tparam v 类型T的常量值
 * 
 * @details This class is the fundamental building block for all type traits in the library.
 * It provides a compile-time constant value of specified type and serves as the base
 * for true_type and false_type.
 * 
 * @details 此类是库中所有类型特性的基础构建块。
 * 它提供指定类型的编译时常量值，并作为true_type和false_type的基类。
 * 
 * @note Inherit from this class to create custom type traits with boolean values
 * @note 继承此类以创建具有布尔值的自定义类型特性
 * 
 * @see true_type
 * @see false_type
 */
template<typename T, T v>
struct integral_constant {
    static constexpr T value = v;             ///< The constant value / 常量值
    using value_type = T;                     ///< Type of the value / 值类型
    using type = integral_constant<T, v>;     ///< Self type / 自身类型

    /**
     * @brief Conversion operator to value type
     * @brief 转换为值类型的运算符
     * 
     * @return Constant value of type T
     * @return 类型T的常量值
     * 
     * @details Allows implicit conversion to the underlying value type.
     * This enables using integral_constant in contexts where a value is expected.
     * 
     * @details 允许隐式转换为底层值类型。
     * 这使得integral_constant可以在需要值的上下文中使用。
     */
    constexpr operator value_type() const noexcept {
        return value;
    }

    /**
     * @brief Function call operator
     * @brief 函数调用运算符
     * 
     * @return Constant value of type T
     * @return 类型T的常量值
     * 
     * @details Returns the constant value when called as a function.
     * Useful for metaprogramming patterns that require callable objects.
     * 
     * @details 当作为函数调用时返回常量值。
     * 对于需要可调用对象的元编程模式很有用。
     */
    constexpr value_type operator()() const noexcept {
        return value;
    }
};

/**
 * @brief Type alias for boolean true value in type traits
 * @brief 类型特性中表示真值的布尔类型别名
 * 
 * @details Inherits from integral_constant<bool, true>.
 * Used as the result type for type traits that evaluate to true.
 * 
 * @details 继承自integral_constant<bool, true>。
 * 用作评估为真的类型特性的结果类型。
 */
using true_type = integral_constant<bool, true>;

/**
 * @brief Type alias for boolean false value in type traits
 * @brief 类型特性中表示假值的布尔类型别名
 * 
 * @details Inherits from integral_constant<bool, false>.
 * Used as the result type for type traits that evaluate to false.
 * 
 * @details 继承自integral_constant<bool, false>。
 * 用作评估为假的类型特性的结果类型。
 */
using false_type = integral_constant<bool, false>;


// ==================== 2. Type Transformations (Basic) ====================
// ==================== 2. 基本类型转换 ====================

/**
 * @brief Remove const qualifier from a type
 * @brief 从类型中移除const限定符
 * 
 * @tparam T Input type
 * @tparam T 输入类型
 * 
 * @details Provides the type T without const qualifier.
 * If T is not const-qualified, type is T.
 * 
 * @details 提供没有const限定符的类型T。
 * 如果T不是const限定的，type就是T。
 */
template<typename T>
struct remove_const {
    using type = T;  ///< Type without const qualifier / 没有const限定符的类型
};

/**
 * @brief Specialization for const-qualified types
 * @brief const限定类型的特化
 */
template<typename T>
struct remove_const<const T> {
    using type = T;  ///< Remove const qualifier / 移除const限定符
};

/**
 * @brief Convenience alias for remove_const::type
 * @brief remove_const::type的便捷别名
 */
template<typename T>
using remove_const_t = typename remove_const<T>::type;

/**
 * @brief Remove volatile qualifier from a type
 * @brief 从类型中移除volatile限定符
 * 
 * @tparam T Input type
 * @tparam T 输入类型
 * 
 * @details Provides the type T without volatile qualifier.
 * If T is not volatile-qualified, type is T.
 * 
 * @details 提供没有volatile限定符的类型T。
 * 如果T不是volatile限定的，type就是T。
 */
template<typename T>
struct remove_volatile {
    using type = T;  ///< Type without volatile qualifier / 没有volatile限定符的类型
};

/**
 * @brief Specialization for volatile-qualified types
 * @brief volatile限定类型的特化
 */
template<typename T>
struct remove_volatile<volatile T> {
    using type = T;  ///< Remove volatile qualifier / 移除volatile限定符
};

/**
 * @brief Convenience alias for remove_volatile::type
 * @brief remove_volatile::type的便捷别名
 */
template<typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

/**
 * @brief Remove both const and volatile qualifiers from a type
 * @brief 从类型中同时移除const和volatile限定符
 * 
 * @tparam T Input type
 * @tparam T 输入类型
 * 
 * @details Provides the type T without const or volatile qualifiers.
 * Equivalent to remove_const<remove_volatile<T>>::type.
 * 
 * @details 提供没有const或volatile限定符的类型T。
 * 等价于remove_const<remove_volatile<T>>::type。
 */
template<typename T>
struct remove_cv {
    using type = T;  ///< Type without qualifiers / 没有限定符的类型
};

/**
 * @brief Specialization for const-qualified types
 * @brief const限定类型的特化
 */
template<typename T>
struct remove_cv<const T> {
    using type = T;  ///< Remove const qualifier / 移除const限定符
};

/**
 * @brief Specialization for volatile-qualified types
 * @brief volatile限定类型的特化
 */
template<typename T>
struct remove_cv<volatile T> {
    using type = T;  ///< Remove volatile qualifier / 移除volatile限定符
};

/**
 * @brief Specialization for const volatile-qualified types
 * @brief const volatile限定类型的特化
 */
template<typename T>
struct remove_cv<const volatile T> {
    using type = T;  ///< Remove both const and volatile qualifiers / 同时移除const和volatile限定符
};

/**
 * @brief Convenience alias for remove_cv::type
 * @brief remove_cv::type的便捷别名
 */
template<typename T>
using remove_cv_t = typename remove_cv<T>::type;

/**
 * @brief Remove reference from a type
 * @brief 从类型中移除引用
 * 
 * @tparam T Input type
 * @tparam T 输入类型
 * 
 * @details Provides the non-reference type underlying T.
 * If T is not a reference, type is T.
 * 
 * @details 提供T的基础非引用类型。
 * 如果T不是引用，type就是T。
 */
template<typename T>
struct remove_reference {
    using type = T;  ///< Non-reference type / 非引用类型
};

/**
 * @brief Specialization for lvalue references
 * @brief 左值引用的特化
 */
template<typename T>
struct remove_reference<T&> {
    using type = T;  ///< Remove lvalue reference / 移除左值引用
};

/**
 * @brief Specialization for rvalue references
 * @brief 右值引用的特化
 */
template<typename T>
struct remove_reference<T&&> {
    using type = T;  ///< Remove rvalue reference / 移除右值引用
};

/**
 * @brief Convenience alias for remove_reference::type
 * @brief remove_reference::type的便捷别名
 */
template<typename T>
using remove_reference_t = typename remove_reference<T>::type;


// ==================== 3. Type Checks ====================
// ==================== 3. 类型检查 ====================

/**
 * @brief Check if two types are identical
 * @brief 检查两个类型是否相同
 * 
 * @tparam T First type to compare
 * @tparam T 要比较的第一个类型
 * @tparam U Second type to compare
 * @tparam U 要比较的第二个类型
 * 
 * @details Determines whether two types are identical, including all qualifiers.
 * This is a fundamental type trait used extensively in template metaprogramming.
 * 
 * @details 确定两个类型是否完全相同，包括所有限定符。
 * 这是模板元编程中广泛使用的基本类型特性。
 * 
 * @note Inherits from true_type if T and U are the same type, false_type otherwise
 * @note 如果T和U是相同类型，则继承自true_type，否则继承自false_type
 */
template<typename T, typename U>
struct is_same : false_type {};

/**
 * @brief Specialization for identical types
 * @brief 相同类型的特化
 */
template<typename T>
struct is_same<T, T> : true_type {};

/**
 * @brief Convenience variable template for is_same::value
 * @brief is_same::value的便捷变量模板
 */
template<typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;

/**
 * @brief Check if a type is const-qualified
 * @brief 检查类型是否具有const限定符
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 */
template<typename T>
struct is_const : false_type {};

/**
 * @brief Specialization for const-qualified types
 * @brief const限定类型的特化
 */
template<typename T>
struct is_const<const T> : true_type {};

/**
 * @brief Convenience variable template for is_const::value
 * @brief is_const::value的便捷变量模板
 */
template<typename T>
inline constexpr bool is_const_v = is_const<T>::value;

/**
 * @brief Check if a type is volatile-qualified
 * @brief 检查类型是否具有volatile限定符
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 */
template<typename T>
struct is_volatile : false_type {};

/**
 * @brief Specialization for volatile-qualified types
 * @brief volatile限定类型的特化
 */
template<typename T>
struct is_volatile<volatile T> : true_type {};

/**
 * @brief Convenience variable template for is_volatile::value
 * @brief is_volatile::value的便捷变量模板
 */
template<typename T>
inline constexpr bool is_volatile_v = is_volatile<T>::value;

/**
 * @brief Check if a type is a pointer
 * @brief 检查类型是否为指针
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 * 
 * @details Detects all pointer types including const, volatile, and const volatile pointers.
 * 
 * @details 检测所有指针类型，包括const、volatile和const volatile指针。
 */
template<typename T>
struct is_pointer : false_type {};

/**
 * @brief Specialization for plain pointers
 * @brief 普通指针的特化
 */
template<typename T>
struct is_pointer<T*> : true_type {};

/**
 * @brief Specialization for const pointers
 * @brief const指针的特化
 */
template<typename T>
struct is_pointer<T* const> : true_type {};

/**
 * @brief Specialization for volatile pointers
 * @brief volatile指针的特化
 */
template<typename T>
struct is_pointer<T* volatile> : true_type {};

/**
 * @brief Specialization for const volatile pointers
 * @brief const volatile指针的特化
 */
template<typename T>
struct is_pointer<T* const volatile> : true_type {};

/**
 * @brief Convenience variable template for is_pointer::value
 * @brief is_pointer::value的便捷变量模板
 */
template<typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

/**
 * @brief Check if a type is a reference
 * @brief 检查类型是否为引用
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 * 
 * @details Detects both lvalue references (T&) and rvalue references (T&&).
 * 
 * @details 检测左值引用（T&）和右值引用（T&&）。
 */
template<typename T>
struct is_reference : false_type {};

/**
 * @brief Specialization for lvalue references
 * @brief 左值引用的特化
 */
template<typename T>
struct is_reference<T&> : true_type {};

/**
 * @brief Specialization for rvalue references
 * @brief 右值引用的特化
 */
template<typename T>
struct is_reference<T&&> : true_type {};

/**
 * @brief Convenience variable template for is_reference::value
 * @brief is_reference::value的便捷变量模板
 */
template<typename T>
inline constexpr bool is_reference_v = is_reference<T>::value;

/**
 * @brief Check if a type is an lvalue reference
 * @brief 检查类型是否为左值引用
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 */
template<typename T>
struct is_lvalue_reference : false_type {};

/**
 * @brief Specialization for lvalue references
 * @brief 左值引用的特化
 */
template<typename T>
struct is_lvalue_reference<T&> : true_type {};

/**
 * @brief Convenience variable template for is_lvalue_reference::value
 * @brief is_lvalue_reference::value的便捷变量模板
 */
template<typename T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

/**
 * @brief Check if a type is an rvalue reference
 * @brief 检查类型是否为右值引用
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 */
template<typename T>
struct is_rvalue_reference : false_type {};

/**
 * @brief Specialization for rvalue references
 * @brief 右值引用的特化
 */
template<typename T>
struct is_rvalue_reference<T&&> : true_type {};

/**
 * @brief Convenience variable template for is_rvalue_reference::value
 * @brief is_rvalue_reference::value的便捷变量模板
 */
template<typename T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

/**
 * @brief Check if a type is void
 * @brief 检查类型是否为void
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 * 
 * @details Detects void with all qualifier combinations (const, volatile, const volatile).
 * 
 * @details 检测所有限定符组合的void（const、volatile、const volatile）。
 */
template<typename T>
struct is_void : false_type {};

/**
 * @brief Specialization for plain void
 * @brief 普通void的特化
 */
template<>
struct is_void<void> : true_type {};

/**
 * @brief Specialization for const void
 * @brief const void的特化
 */
template<>
struct is_void<const void> : true_type {};

/**
 * @brief Specialization for volatile void
 * @brief volatile void的特化
 */
template<>
struct is_void<volatile void> : true_type {};

/**
 * @brief Specialization for const volatile void
 * @brief const volatile void的特化
 */
template<>
struct is_void<const volatile void> : true_type {};

/**
 * @brief Convenience variable template for is_void::value
 * @brief is_void::value的便捷变量模板
 */
template<typename T>
inline constexpr bool is_void_v = is_void<T>::value;

/**
 * @brief Check if a type is std::nullptr_t
 * @brief 检查类型是否为std::nullptr_t
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 * 
 * @details Detects std::nullptr_t with all qualifier combinations.
 * 
 * @details 检测所有限定符组合的std::nullptr_t。
 */
template<typename T>
struct is_null_pointer : false_type {};

/**
 * @brief Specialization for std::nullptr_t
 * @brief std::nullptr_t的特化
 */
template<>
struct is_null_pointer<std::nullptr_t> : true_type {};

/**
 * @brief Specialization for const std::nullptr_t
 * @brief const std::nullptr_t的特化
 */
template<>
struct is_null_pointer<const std::nullptr_t> : true_type {};

/**
 * @brief Specialization for volatile std::nullptr_t
 * @brief volatile std::nullptr_t的特化
 */
template<>
struct is_null_pointer<volatile std::nullptr_t> : true_type {};

/**
 * @brief Specialization for const volatile std::nullptr_t
 * @brief const volatile std::nullptr_t的特化
 */
template<>
struct is_null_pointer<const volatile std::nullptr_t> : true_type {};

/**
 * @brief Convenience variable template for is_null_pointer::value
 * @brief is_null_pointer::value的便捷变量模板
 */
template<typename T>
inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

/**
 * @brief Check if a type is a class or struct
 * @brief 检查类型是否为类或结构体
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 * 
 * @details Uses SFINAE technique to detect if a type has a member pointer.
 * This is a simplified implementation that works for most cases.
 * 
 * @details 使用SFINAE技术检测类型是否具有成员指针。
 * 这是一个适用于大多数情况的简化实现。
 */
namespace detail {
    template<typename T>
    char test_is_class(int T::*);  ///< if T is a class, this overload is viable / 如果T是类，此重载可行
    
    template<typename T>
    long test_is_class(...);       ///< fallback overload / 后备重载
}

template<typename T>
struct is_class : integral_constant<bool, sizeof(detail::test_is_class<T>(0)) == 1> {};

/**
 * @brief Convenience variable template for is_class::value
 * @brief is_class::value的便捷变量模板
 */
template<typename T>
inline constexpr bool is_class_v = is_class<T>::value;


// ==================== 4. Integer and Floating Point Types ====================
// ==================== 4. 整数和浮点类型 ====================

/// @brief is_integral - Check if a type is an integral type
/// @brief is_integral - 检查类型是否为整数类型
///
/// Determines whether a type is one of the standard integral types.
/// This includes bool, char, wchar_t, char16_t, char32_t, and all signed/unsigned integer types.
///
/// 确定类型是否是标准整数类型之一。
/// 包括bool、char、wchar_t、char16_t、char32_t以及所有有符号/无符号整数类型。
template<typename T>
struct is_integral : integral_constant<bool,
    is_same_v<remove_cv_t<T>, bool> ||
    is_same_v<remove_cv_t<T>, char> ||
    is_same_v<remove_cv_t<T>, signed char> ||
    is_same_v<remove_cv_t<T>, unsigned char> ||
    is_same_v<remove_cv_t<T>, wchar_t> ||
    is_same_v<remove_cv_t<T>, char16_t> ||
    is_same_v<remove_cv_t<T>, char32_t> ||
    is_same_v<remove_cv_t<T>, short> ||
    is_same_v<remove_cv_t<T>, unsigned short> ||
    is_same_v<remove_cv_t<T>, int> ||
    is_same_v<remove_cv_t<T>, unsigned int> ||
    is_same_v<remove_cv_t<T>, long> ||
    is_same_v<remove_cv_t<T>, unsigned long> ||
    is_same_v<remove_cv_t<T>, long long> ||
    is_same_v<remove_cv_t<T>, unsigned long long>> {};

/// @brief is_integral_v - Convenience variable template for is_integral::value
/// @brief is_integral_v - is_integral::value的便捷变量模板
template<typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;

/// @brief is_floating_point - Check if a type is a floating-point type
/// @brief is_floating_point - 检查类型是否为浮点类型
///
/// Determines whether a type is one of the standard floating-point types.
/// This includes float, double, and long double.
///
/// 确定类型是否是标准浮点类型之一。
/// 包括float、double和long double。
template<typename T>
struct is_floating_point : integral_constant<bool,
    is_same_v<remove_cv_t<T>, float> ||
    is_same_v<remove_cv_t<T>, double> ||
    is_same_v<remove_cv_t<T>, long double>> {};

/// @brief is_floating_point_v - Convenience variable template for is_floating_point::value
/// @brief is_floating_point_v - is_floating_point::value的便捷变量模板
template<typename T>
inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

/// @brief is_arithmetic - Check if a type is an arithmetic type
/// @brief is_arithmetic - 检查类型是否为算术类型
///
/// Determines whether a type is either an integral type or a floating-point type.
/// Arithmetic types support basic mathematical operations.
///
/// 确定类型是整数类型还是浮点类型。
/// 算术类型支持基本数学运算。
template<typename T>
struct is_arithmetic : integral_constant<bool,
    is_integral_v<T> || is_floating_point_v<T>> {};

/// @brief is_arithmetic_v - Convenience variable template for is_arithmetic::value
/// @brief is_arithmetic_v - is_arithmetic::value的便捷变量模板
template<typename T>
inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

/// @brief is_signed - Check if a type is a signed arithmetic type
/// @brief is_signed - 检查类型是否为有符号算术类型
///
/// Determines whether an arithmetic type is signed.
/// For non-arithmetic types, returns false.
///
/// 确定算术类型是否为有符号。
/// 对于非算术类型，返回false。
namespace detail {
    template<typename T, bool = is_arithmetic_v<T>>
    struct is_signed_impl : integral_constant<bool, T(-1) < T(0)> {};
    
    template<typename T>
    struct is_signed_impl<T, false> : false_type {};
}

template<typename T>
struct is_signed : detail::is_signed_impl<T> {};

/// @brief is_signed_v - Convenience variable template for is_signed::value
/// @brief is_signed_v - is_signed::value的便捷变量模板
template<typename T>
inline constexpr bool is_signed_v = is_signed<T>::value;

/// @brief is_unsigned - Check if a type is an unsigned arithmetic type
/// @brief is_unsigned - 检查类型是否为无符号算术类型
///
/// Determines whether an arithmetic type is unsigned.
/// For non-arithmetic types, returns false.
///
/// 确定算术类型是否为无符号。
/// 对于非算术类型，返回false。
template<typename T>
struct is_unsigned : integral_constant<bool,
    is_arithmetic_v<T> && !is_signed_v<T>> {};

/// @brief is_unsigned_v - Convenience variable template for is_unsigned::value
/// @brief is_unsigned_v - is_unsigned::value的便捷变量模板
template<typename T>
inline constexpr bool is_unsigned_v = is_unsigned<T>::value;


// ==================== 5. Array and Function Types ====================
// ==================== 5. 数组和函数类型 ====================

/// @brief is_array - Check if a type is an array type
/// @brief is_array - 检查类型是否为数组类型
///
/// Determines whether a type is an array, including both bounded and unbounded arrays.
/// This includes both T[] (unbounded) and T[N] (bounded) array types.
///
/// 确定类型是否为数组，包括有界和无界数组。
/// 包括T[]（无界）和T[N]（有界）数组类型。
template<typename T>
struct is_array : false_type {};

/// @brief Specialization for unbounded arrays
/// @brief 无界数组的特化
template<typename T>
struct is_array<T[]> : true_type {};

/// @brief Specialization for bounded arrays
/// @brief 有界数组的特化
template<typename T, std::size_t N>
struct is_array<T[N]> : true_type {};

/// @brief is_array_v - Convenience variable template for is_array::value
/// @brief is_array_v - is_array::value的便捷变量模板
template<typename T>
inline constexpr bool is_array_v = is_array<T>::value;

// remove_extent - remove one dimension from array
template<typename T>
struct remove_extent {
    using type = T;
};

template<typename T>
struct remove_extent<T[]> {
    using type = T;
};

template<typename T, std::size_t N>
struct remove_extent<T[N]> {
    using type = T;
};

template<typename T>
using remove_extent_t = typename remove_extent<T>::type;

// remove_all_extents - remove all dimensions from array
template<typename T>
struct remove_all_extents {
    using type = T;
};

template<typename T>
struct remove_all_extents<T[]> {
    using type = typename remove_all_extents<T>::type;
};

template<typename T, std::size_t N>
struct remove_all_extents<T[N]> {
    using type = typename remove_all_extents<T>::type;
};

template<typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

/// @brief is_function - Check if a type is a function type
/// @brief is_function - 检查类型是否为函数类型
///
/// Determines whether a type is a function type, including regular functions and C-style variadic functions.
/// This is a simplified implementation that covers most common function signatures.
///
/// 确定类型是否为函数类型，包括常规函数和C风格可变参数函数。
/// 这是一个覆盖大多数常见函数签名的简化实现。
namespace detail {
    template<typename T>
    struct is_function_impl : false_type {};
    
    /// @brief Specialization for regular function types
    /// @brief 常规函数类型的特化
    template<typename Ret, typename... Args>
    struct is_function_impl<Ret(Args...)> : true_type {};
    
    /// @brief Specialization for C-style variadic functions
    /// @brief C风格可变参数函数的特化
    template<typename Ret, typename... Args>
    struct is_function_impl<Ret(Args..., ...)> : true_type {};  // C-style varargs / C风格可变参数
}

template<typename T>
struct is_function : detail::is_function_impl<remove_cv_t<T>> {};

/// @brief is_function_v - Convenience variable template for is_function::value
/// @brief is_function_v - is_function::value的便捷变量模板
template<typename T>
inline constexpr bool is_function_v = is_function<T>::value;


// ==================== 6. Type Transformations (Advanced) ====================
// ==================== 6. 高级类型转换 ====================

// add_const - Add const qualifier to a type
// add_const - 向类型添加const限定符
template<typename T>
struct add_const {
    using type = const T;  // Add const qualifier
                           // 添加const限定符
};

// add_const_t - Convenience alias for add_const::type
// add_const_t - add_const::type的便捷别名
template<typename T>
using add_const_t = typename add_const<T>::type;

// add_volatile - Add volatile qualifier to a type
// add_volatile - 向类型添加volatile限定符
template<typename T>
struct add_volatile {
    using type = volatile T;  // Add volatile qualifier
                              // 添加volatile限定符
};

// add_volatile_t - Convenience alias for add_volatile::type
// add_volatile_t - add_volatile::type的便捷别名
template<typename T>
using add_volatile_t = typename add_volatile<T>::type;

// add_cv - Add both const and volatile qualifiers to a type
// add_cv - 向类型同时添加const和volatile限定符
template<typename T>
struct add_cv {
    using type = const volatile T;  // Add both const and volatile qualifiers
                                    // 同时添加const和volatile限定符
};

// add_cv_t - Convenience alias for add_cv::type
// add_cv_t - add_cv::type的便捷别名
template<typename T>
using add_cv_t = typename add_cv<T>::type;

// add_pointer - Add pointer to a type
// add_pointer - 向类型添加指针
template<typename T>
struct add_pointer {
    using type = remove_reference_t<T>*;  // Add pointer after removing any reference
                                          // 移除任何引用后添加指针
};

// add_pointer_t - Convenience alias for add_pointer::type
// add_pointer_t - add_pointer::type的便捷别名
template<typename T>
using add_pointer_t = typename add_pointer<T>::type;

// add_lvalue_reference - Add lvalue reference to a type
// add_lvalue_reference - 向类型添加左值引用
template<typename T>
struct add_lvalue_reference {
    using type = T&;  // Add lvalue reference
                      // 添加左值引用
};

// specialization for void (void& is not allowed)
// void的特化（void&不允许）
template<>
struct add_lvalue_reference<void> {
    using type = void;
};

template<>
struct add_lvalue_reference<const void> {
    using type = const void;
};

template<>
struct add_lvalue_reference<volatile void> {
    using type = volatile void;
};

template<>
struct add_lvalue_reference<const volatile void> {
    using type = const volatile void;
};

// add_lvalue_reference_t - Convenience alias for add_lvalue_reference::type
// add_lvalue_reference_t - add_lvalue_reference::type的便捷别名
template<typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

// add_rvalue_reference - Add rvalue reference to a type
// add_rvalue_reference - 向类型添加右值引用
template<typename T>
struct add_rvalue_reference {
    using type = T&&;  // Add rvalue reference
                       // 添加右值引用
};

// specialization for void (void&& is not allowed)
// void的特化（void&&不允许）
template<>
struct add_rvalue_reference<void> {
    using type = void;
};

template<>
struct add_rvalue_reference<const void> {
    using type = const void;
};

template<>
struct add_rvalue_reference<volatile void> {
    using type = volatile void;
};

template<>
struct add_rvalue_reference<const volatile void> {
    using type = const volatile void;
};

// add_rvalue_reference_t - Convenience alias for add_rvalue_reference::type
// add_rvalue_reference_t - add_rvalue_reference::type的便捷别名
template<typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;


// ==================== 7. Conditional Selection ====================
// ==================== 7. 条件选择 ====================

/**
 * @brief Compile-time version of the ternary operator for types
 * @brief 编译时的三元运算符版本（用于类型）
 * 
 * @tparam B Boolean condition that determines which type to select
 * @tparam B 决定选择哪个类型的布尔条件
 * @tparam T Type selected when B is true (true branch)
 * @tparam T 当B为真时选择的类型（真分支）
 * @tparam F Type selected when B is false (false branch)
 * @tparam F 当B为假时选择的类型（假分支）
 * 
 * @details Selects one of two types based on a boolean condition at compile time.
 * This is the fundamental building block for compile-time decision making.
 * 
 * @details 根据编译时的布尔条件选择两个类型中的一个。
 * 这是编译时决策的基本构建块。
 */
template<bool B, typename T, typename F>
struct conditional {
    using type = T;  ///< Select T when B is true / 当B为真时选择T
};

/**
 * @brief Specialization for false condition
 * @brief 假条件的特化
 */
template<typename T, typename F>
struct conditional<false, T, F> {
    using type = F;  ///< Select F when B is false / 当B为假时选择F
};

/**
 * @brief Convenience alias for conditional::type
 * @brief conditional::type的便捷别名
 */
template<bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

/**
 * @brief Logical AND for type traits
 * @brief 类型特性的逻辑与
 * 
 * @tparam Args Type traits to combine with logical AND
 * @tparam Args 要用逻辑与组合的类型特性
 * 
 * @details Returns true_type if all type traits evaluate to true, otherwise returns the first false type trait.
 * 
 * @details 如果所有类型特性都评估为真，则返回true_type，否则返回第一个假类型特性。
 */
template<typename...>
struct conjunction : true_type {};

/**
 * @brief Specialization for single type trait
 * @brief 单个类型特性的特化
 */
template<typename T>
struct conjunction<T> : T {};

/**
 * @brief Specialization for multiple type traits
 * @brief 多个类型特性的特化
 */
template<typename T, typename... Args>
struct conjunction<T, Args...> : 
    conditional_t<bool(T::value), conjunction<Args...>, T> {};

/**
 * @brief Convenience variable template for conjunction::value
 * @brief conjunction::value的便捷变量模板
 */
template<typename... Args>
inline constexpr bool conjunction_v = conjunction<Args...>::value;

/**
 * @brief Logical OR for type traits
 * @brief 类型特性的逻辑或
 * 
 * @tparam Args Type traits to combine with logical OR
 * @tparam Args 要用逻辑或组合的类型特性
 * 
 * @details Returns true_type if any type trait evaluates to true, otherwise returns false_type.
 * 
 * @details 如果任何类型特性评估为真，则返回true_type，否则返回false_type。
 */
template<typename...>
struct disjunction : false_type {};

/**
 * @brief Specialization for single type trait
 * @brief 单个类型特性的特化
 */
template<typename T>
struct disjunction<T> : T {};

/**
 * @brief Specialization for multiple type traits
 * @brief 多个类型特性的特化
 */
template<typename T, typename... Args>
struct disjunction<T, Args...> : 
    conditional_t<bool(T::value), T, disjunction<Args...>> {};

/**
 * @brief Convenience variable template for disjunction::value
 * @brief disjunction::value的便捷变量模板
 */
template<typename... Args>
inline constexpr bool disjunction_v = disjunction<Args...>::value;

/**
 * @brief Logical NOT for type traits
 * @brief 类型特性的逻辑非
 * 
 * @tparam T Type trait to negate
 * @tparam T 要取反的类型特性
 * 
 * @details Returns the logical negation of the type trait's value.
 * 
 * @details 返回类型特性值的逻辑非。
 */
template<typename T>
struct negation : integral_constant<bool, !bool(T::value)> {};

/**
 * @brief Convenience variable template for negation::value
 * @brief negation::value的便捷变量模板
 */
template<typename T>
inline constexpr bool negation_v = negation<T>::value;


// ==================== 8. Type Transformations (Complex) ====================

// 8.1 declval - get a reference to a type without constructing an object
// Note: Only declaration, never defined (used in unevaluated contexts only)
template<typename T>
add_rvalue_reference_t<T> declval() noexcept;

// 8.2 decay - apply type transformations as when passed by value
template<typename T>
struct decay {
private:
    using U = remove_reference_t<T>;
public:
    using type = conditional_t<
        is_array_v<U>,
        remove_extent_t<U>*,
        conditional_t<
            is_function_v<U>,
            add_pointer_t<U>,
            remove_cv_t<U>
        >
    >;
};

template<typename T>
using decay_t = typename decay<T>::type;


// ==================== 9. Type Relationships ====================
// ==================== 9. 类型关系 ====================

/// @brief is_base_of - Check if one type is a base class of another
/// @brief is_base_of - 检查一个类型是否是另一个类型的基类
///
/// Determines whether Base is a base class of Derived (or the same class).
/// Both types must be class types. Returns true for the same class.
///
/// 确定Base是否是Derived的基类（或相同类）。
/// 两个类型都必须是类类型。对于相同类返回true。
namespace detail {
    template<typename Base>
    true_type test_base_of(const volatile Base*);
    
    template<typename Base>
    false_type test_base_of(const volatile void*);
    
    template<typename Base, typename Derived>
    auto is_base_of_helper(int) -> decltype(test_base_of<Base>(static_cast<Derived*>(nullptr)));
    
    template<typename Base, typename Derived>
    auto is_base_of_helper(...) -> false_type;
} // namespace detail 

template<typename Base, typename Derived>
struct is_base_of : 
    integral_constant<
        bool,
        is_class_v<Base> && is_class_v<Derived> &&
        decltype(detail::is_base_of_helper<Base, Derived>(0))::value
    > {};

/// @brief Specialization for same type
/// @brief 相同类型的特化
template<typename T>
struct is_base_of<T, T> : integral_constant<bool, is_class_v<T>> {};

/// @brief is_base_of_v - Convenience variable template for is_base_of::value
/// @brief is_base_of_v - is_base_of::value的便捷变量模板
template<typename Base, typename Derived>
inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

/// @brief is_convertible - Check if one type is convertible to another
/// @brief is_convertible - 检查一个类型是否可以转换为另一个类型
///
/// Determines whether an expression of type From can be implicitly converted to type To.
/// Uses SFINAE to detect if the conversion is valid at compile time.
///
/// 确定类型From的表达式是否可以隐式转换为类型To。
/// 使用SFINAE在编译时检测转换是否有效。
namespace detail {
    // Helper function that accepts any type convertible to To
    // 接受任何可转换为To的类型的辅助函数
    template<typename To>
    void test_convertible_helper(To);
    
    template<typename From, typename To>
    auto is_convertible_test(int) -> decltype(
        test_convertible_helper<To>(mystl::declval<From>()), true_type()
    );
    
    template<typename From, typename To>
    auto is_convertible_test(...) -> false_type;
} // namespace detail

template<typename From, typename To>
struct is_convertible : decltype(detail::is_convertible_test<From, To>(0)) {};

// Specialization for void conversions
// void转换的特化
template<typename From>
struct is_convertible<From, void> : true_type {};

template<>
struct is_convertible<void, void> : true_type {};

template<typename To>
struct is_convertible<void, To> : false_type {};

// Specialization for void* to pointer conversions
// void*到指针转换的特化
template<typename To>
struct is_convertible<void*, To*> : true_type {};

template<typename To>
struct is_convertible<const void*, To*> : true_type {};

template<typename To>
struct is_convertible<volatile void*, To*> : true_type {};

template<typename To>
struct is_convertible<const volatile void*, To*> : true_type {};

/// @brief is_convertible_v - Convenience variable template for is_convertible::value
/// @brief is_convertible_v - is_convertible::value的便捷变量模板
template<typename From, typename To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;


// ==================== 10. Helper Tools ====================

// 10.1 void_t - map a list of types to void
template<typename...>
using void_t = void;


// ==================== 11. SFINAE Support ====================
// ==================== 11. SFINAE支持 ====================

/**
 * @brief enable_if - Conditionally enable a template based on a boolean
 * @brief enable_if - 根据布尔值有条件地启用模板
 * 
 * Implements the SFINAE (Substitution Failure Is Not An Error) idiom.
 * When B is true, provides a member typedef 'type' equal to T.
 * When B is false, no member typedef is provided, causing substitution failure.
 * 
 * 实现SFINAE（替换失败不是错误）惯用法。
 * 当B为真时，提供等于T的成员类型定义'type'。
 * 当B为假时，不提供成员类型定义，导致替换失败。
 * 
 * @tparam B Boolean condition that determines whether to provide the type
 * @tparam B 决定是否提供类型的布尔条件
 * @tparam T Type to provide when B is true (defaults to void)
 * @tparam T 当B为真时提供的类型（默认为void）
 * 
 * @note Essential for template metaprogramming and constraint checking
 * @note 对于模板元编程和约束检查至关重要
 * 
 * @see enable_if_t
 * @see enable_if_t
 */
template<bool B, typename T = void>
struct enable_if {};

/**
 * @brief Specialization for true condition
 * @brief 真条件的特化
 * 
 * Provides the member typedef 'type' when the condition is true.
 * This allows the template to be used when the condition is satisfied.
 * 
 * 当条件为真时提供成员类型定义'type'。
 * 这允许在条件满足时使用模板。
 */
template<typename T>
struct enable_if<true, T> {
    using type = T;  // Type provided when condition is true
                     // 条件为真时提供的类型
};

/**
 * @brief enable_if_t - Convenience alias for enable_if::type
 * @brief enable_if_t - enable_if::type的便捷别名
 * 
 * Provides easy access to the type member of enable_if.
 * Commonly used in template parameter lists and return types.
 * 
 * 提供对enable_if类型成员的便捷访问。
 * 常用于模板参数列表和返回类型。
 */
template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;


// ==================== 12. Construction and Assignment Traits ====================
// ==================== 12. 构造和赋值特性 ====================

/// @brief Helper namespace for construction and assignment trait implementations
/// @brief 构造和赋值特性实现的辅助命名空间
namespace detail {
    // Helper for checking if a type can be constructed from arguments
    // 检查类型是否可以从参数构造的辅助函数
    template<typename T, typename... Args>
    auto test_constructible(int) -> decltype(T(mystl::declval<Args>()...), true_type());
    
    template<typename T, typename... Args>
    auto test_constructible(...) -> false_type;
    
    // Helper for checking if a type can be assigned
    // 检查类型是否可以赋值的辅助函数
    template<typename T, typename U>
    auto test_assignable(int) -> decltype(mystl::declval<T>() = mystl::declval<U>(), true_type());
    
    template<typename T, typename U>
    auto test_assignable(...) -> false_type;
    
    // Helper for checking if a type can be destroyed
    // 检查类型是否可以析构的辅助函数
    template<typename T>
    auto test_destructible(int) -> decltype(mystl::declval<T&>().~T(), true_type());
    
    template<typename T>
    auto test_destructible(...) -> false_type;
}

/// @brief is_constructible - Check if a type can be constructed from arguments
/// @brief is_constructible - 检查类型是否可以从参数构造
///
/// Determines whether a type T can be constructed from the specified argument types.
/// Uses SFINAE to detect if the construction expression is valid.
///
/// 确定类型T是否可以从指定的参数类型构造。
/// 使用SFINAE检测构造表达式是否有效。
template<typename T, typename... Args>
struct is_constructible : decltype(detail::test_constructible<T, Args...>(0)) {};

/// @brief is_constructible_v - Convenience variable template for is_constructible::value
/// @brief is_constructible_v - is_constructible::value的便捷变量模板
template<typename T, typename... Args>
inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

/// @brief is_default_constructible - Check if a type can be default constructed
/// @brief is_default_constructible - 检查类型是否可以默认构造
///
/// Determines whether a type can be constructed without any arguments.
/// Equivalent to is_constructible<T>.
///
/// 确定类型是否可以在没有任何参数的情况下构造。
/// 等价于is_constructible<T>。
template<typename T>
struct is_default_constructible : is_constructible<T> {};

/// @brief is_default_constructible_v - Convenience variable template for is_default_constructible::value
/// @brief is_default_constructible_v - is_default_constructible::value的便捷变量模板
template<typename T>
inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;

/// @brief is_copy_constructible - Check if a type can be copy constructed
/// @brief is_copy_constructible - 检查类型是否可以复制构造
///
/// Determines whether a type can be constructed from a const lvalue reference of the same type.
/// Equivalent to is_constructible<T, const T&>.
///
/// 确定类型是否可以从相同类型的const左值引用构造。
/// 等价于is_constructible<T, const T&>。
template<typename T>
struct is_copy_constructible : is_constructible<T, add_lvalue_reference_t<const T>> {};

/// @brief is_copy_constructible_v - Convenience variable template for is_copy_constructible::value
/// @brief is_copy_constructible_v - is_copy_constructible::value的便捷变量模板
template<typename T>
inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

/// @brief is_move_constructible - Check if a type can be move constructed
/// @brief is_move_constructible - 检查类型是否可以移动构造
///
/// Determines whether a type can be constructed from an rvalue reference of the same type.
/// Equivalent to is_constructible<T, T&&>.
///
/// 确定类型是否可以从相同类型的右值引用构造。
/// 等价于is_constructible<T, T&&>。
template<typename T>
struct is_move_constructible : is_constructible<T, add_rvalue_reference_t<T>> {};

/// @brief is_move_constructible_v - Convenience variable template for is_move_constructible::value
/// @brief is_move_constructible_v - is_move_constructible::value的便捷变量模板
template<typename T>
inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

/// @brief is_assignable - Check if a type can be assigned from another type
/// @brief is_assignable - 检查类型是否可以从另一个类型赋值
///
/// Determines whether an expression of type U can be assigned to an lvalue of type T.
/// Uses SFINAE to detect if the assignment expression is valid.
///
/// 确定类型U的表达式是否可以赋值给类型T的左值。
/// 使用SFINAE检测赋值表达式是否有效。
template<typename T, typename U>
struct is_assignable : decltype(detail::test_assignable<T, U>(0)) {};

/// @brief is_assignable_v - Convenience variable template for is_assignable::value
/// @brief is_assignable_v - is_assignable::value的便捷变量模板
template<typename T, typename U>
inline constexpr bool is_assignable_v = is_assignable<T, U>::value;

/// @brief is_copy_assignable - Check if a type can be copy assigned
/// @brief is_copy_assignable - 检查类型是否可以复制赋值
///
/// Determines whether a type can be assigned from a const lvalue reference of the same type.
/// Equivalent to is_assignable<T&, const T&>.
///
/// 确定类型是否可以从相同类型的const左值引用赋值。
/// 等价于is_assignable<T&, const T&>。
template<typename T>
struct is_copy_assignable : is_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};

/// @brief is_copy_assignable_v - Convenience variable template for is_copy_assignable::value
/// @brief is_copy_assignable_v - is_copy_assignable::value的便捷变量模板
template<typename T>
inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;

/// @brief is_move_assignable - Check if a type can be move assigned
/// @brief is_move_assignable - 检查类型是否可以移动赋值
///
/// Determines whether a type can be assigned from an rvalue reference of the same type.
/// Equivalent to is_assignable<T&, T&&>.
///
/// 确定类型是否可以从相同类型的右值引用赋值。
/// 等价于is_assignable<T&, T&&>。
template<typename T>
struct is_move_assignable : is_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};

/// @brief is_move_assignable_v - Convenience variable template for is_move_assignable::value
/// @brief is_move_assignable_v - is_move_assignable::value的便捷变量模板
template<typename T>
inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

/// @brief is_destructible - Check if a type can be destroyed
/// @brief is_destructible - 检查类型是否可以析构
///
/// Determines whether a type can be destroyed (has an accessible destructor).
/// Uses SFINAE to detect if the destructor call is valid.
///
/// 确定类型是否可以析构（具有可访问的析构函数）。
/// 使用SFINAE检测析构函数调用是否有效。
template<typename T>
struct is_destructible : decltype(detail::test_destructible<T>(0)) {};

/// @brief is_destructible_v - Convenience variable template for is_destructible::value
/// @brief is_destructible_v - is_destructible::value的便捷变量模板
template<typename T>
inline constexpr bool is_destructible_v = is_destructible<T>::value;

// Specialization for void (void cannot be constructed, assigned, or destroyed)
template<typename... Args>
struct is_constructible<void, Args...> : false_type {};

template<>
struct is_default_constructible<void> : false_type {};

template<>
struct is_copy_constructible<void> : false_type {};

template<>
struct is_move_constructible<void> : false_type {};

template<typename U>
struct is_assignable<void, U> : false_type {};

template<>
struct is_copy_assignable<void> : false_type {};

template<>
struct is_move_assignable<void> : false_type {};

template<>
struct is_destructible<void> : false_type {};


// ==================== 13. Type Properties ====================

// Compiler intrinsic support detection
#ifdef __GNUC__
    #define MYSTL_HAS_BUILTIN_TRIVIAL 1
    #define MYSTL_HAS_BUILTIN_STANDARD_LAYOUT 1
    #define MYSTL_HAS_BUILTIN_POLYMORPHIC 1
    #define MYSTL_HAS_BUILTIN_ENUM 1
    #define MYSTL_HAS_BUILTIN_FINAL 1
#elif defined(_MSC_VER)
    #define MYSTL_HAS_BUILTIN_TRIVIAL 1
    #define MYSTL_HAS_BUILTIN_STANDARD_LAYOUT 1
    #define MYSTL_HAS_BUILTIN_POLYMORPHIC 1
    #define MYSTL_HAS_BUILTIN_ENUM 1
    #define MYSTL_HAS_BUILTIN_FINAL 1
#else
    #define MYSTL_HAS_BUILTIN_TRIVIAL 0
    #define MYSTL_HAS_BUILTIN_STANDARD_LAYOUT 0
    #define MYSTL_HAS_BUILTIN_POLYMORPHIC 0
    #define MYSTL_HAS_BUILTIN_ENUM 0
    #define MYSTL_HAS_BUILTIN_FINAL 0
#endif

// is_trivial - use compiler intrinsic when available
#ifdef __GNUC__
template<typename T>
struct is_trivial : integral_constant<bool, __is_trivial(T)> {};
#elif defined(_MSC_VER)
template<typename T>
struct is_trivial : integral_constant<bool, __is_trivial(T)> {};
#else
template<typename T>
struct is_trivial : false_type {};
#endif

template<typename T>
inline constexpr bool is_trivial_v = is_trivial<T>::value;

// is_trivially_copyable - use compiler intrinsic when available
#ifdef __GNUC__
template<typename T>
struct is_trivially_copyable : integral_constant<bool, __is_trivially_copyable(T)> {};
#elif defined(_MSC_VER)
template<typename T>
struct is_trivially_copyable : integral_constant<bool, __is_trivially_copyable(T)> {};
#else
template<typename T>
struct is_trivially_copyable : false_type {};
#endif

template<typename T>
inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

// is_trivially_default_constructible - placeholder implementation
template<typename T>
struct is_trivially_default_constructible : false_type {};

template<typename T>
inline constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;

// is_standard_layout - use compiler intrinsic when available
#ifdef __GNUC__
template<typename T>
struct is_standard_layout : integral_constant<bool, __is_standard_layout(T)> {};
#elif defined(_MSC_VER)
template<typename T>
struct is_standard_layout : integral_constant<bool, __is_standard_layout(T)> {};
#else
template<typename T>
struct is_standard_layout : false_type {};
#endif

template<typename T>
inline constexpr bool is_standard_layout_v = is_standard_layout<T>::value;

// is_polymorphic - check if type has virtual functions
namespace detail {
    template<typename T, bool = is_class_v<T>>
    struct polymorphic_test_impl {
        template<typename U>
        static auto test(int) -> decltype(dynamic_cast<const volatile void*>(static_cast<U*>(nullptr)), true_type());
        
        template<typename U>
        static auto test(...) -> false_type;
        
        using type = decltype(test<T>(0));
    };
    
    template<typename T>
    struct polymorphic_test_impl<T, false> {
        using type = false_type;
    };
}

template<typename T>
struct is_polymorphic : integral_constant<bool,
    is_class_v<T> && detail::polymorphic_test_impl<T>::type::value> {};

template<typename T>
inline constexpr bool is_polymorphic_v = is_polymorphic<T>::value;

// is_abstract - check if type is abstract (has pure virtual functions)
// Standard library typically uses compiler intrinsics like __is_abstract(T)
// For portable implementation, we use a SFINAE trick
namespace detail {
    template<typename T, bool = is_class_v<T>>
    struct abstract_test_impl {
        template<typename U>
        static auto test(int) -> decltype(new U, false_type());
        
        template<typename U>
        static auto test(...) -> true_type;
        
        using type = decltype(test<T>(0));
    };
    
    template<typename T>
    struct abstract_test_impl<T, false> {
        using type = false_type;
    };
}

template<typename T>
struct is_abstract : integral_constant<bool,
    is_class_v<T> && detail::abstract_test_impl<T>::type::value> {};

template<typename T>
inline constexpr bool is_abstract_v = is_abstract<T>::value;

// is_final - use compiler intrinsic when available
#ifdef __GNUC__
template<typename T>
struct is_final : integral_constant<bool, __is_final(T)> {};
#elif defined(_MSC_VER)
template<typename T>
struct is_final : integral_constant<bool, __is_final(T)> {};
#else
template<typename T>
struct is_final : false_type {};
#endif

template<typename T>
inline constexpr bool is_final_v = is_final<T>::value;

// is_empty - check if type is an empty class
namespace detail {
    // Helper to check if a class is empty using inheritance technique
    template<typename T>
    struct empty_helper : T {
        char dummy;
    };
    
    template<typename T, bool = is_class_v<T> && !is_final_v<T> && !is_polymorphic_v<T>>
    struct empty_test_impl : false_type {};
    
    template<typename T>
    struct empty_test_impl<T, true> : integral_constant<bool, 
        sizeof(empty_helper<T>) == sizeof(char)> {};
    
    template<typename T>
    struct empty_test_impl<T, false> : false_type {};
}

template<typename T>
struct is_empty : integral_constant<bool, 
    is_class_v<T> && detail::empty_test_impl<T>::value> {};

template<typename T>
inline constexpr bool is_empty_v = is_empty<T>::value;


// ==================== 14. Additional Type Transformations ====================

// is_enum - use compiler intrinsic when available
#ifdef __GNUC__
template<typename T>
struct is_enum : integral_constant<bool, __is_enum(T)> {};
#elif defined(_MSC_VER)
template<typename T>
struct is_enum : integral_constant<bool, __is_enum(T)> {};
#else
template<typename T>
struct is_enum : false_type {};
#endif

template<typename T>
inline constexpr bool is_enum_v = is_enum<T>::value;

/// @brief make_signed - Convert to corresponding signed type
/// @brief make_signed - 转换为对应的有符号类型
///
/// Provides the signed type corresponding to the given integral or enumeration type.
/// For example, make_signed<unsigned int>::type is int.
/// Preserves const/volatile qualifiers.
///
/// 提供与给定整数或枚举类型对应的有符号类型。
/// 例如，make_signed<unsigned int>::type是int。
/// 保留const/volatile限定符。
template<typename T>
struct make_signed {
    static_assert(is_integral_v<T> || is_enum_v<T>, "make_signed requires integral or enum type");
    using type = T; ///< Simplified implementation / 简化实现
};

// Specializations for standard integral types
// 标准整数类型的特化
template<> struct make_signed<bool> { using type = bool; };
template<> struct make_signed<char> { using type = signed char; };
template<> struct make_signed<signed char> { using type = signed char; };
template<> struct make_signed<unsigned char> { using type = signed char; };
template<> struct make_signed<short> { using type = short; };
template<> struct make_signed<unsigned short> { using type = short; };
template<> struct make_signed<int> { using type = int; };
template<> struct make_signed<unsigned int> { using type = int; };
template<> struct make_signed<long> { using type = long; };
template<> struct make_signed<unsigned long> { using type = long; };
template<> struct make_signed<long long> { using type = long long; };
template<> struct make_signed<unsigned long long> { using type = long long; };

// Handle const/volatile qualifiers
// 处理const/volatile限定符
template<typename T> struct make_signed<const T> { using type = const typename make_signed<T>::type; };
template<typename T> struct make_signed<volatile T> { using type = volatile typename make_signed<T>::type; };
template<typename T> struct make_signed<const volatile T> { using type = const volatile typename make_signed<T>::type; };

/// @brief make_signed_t - Convenience alias for make_signed::type
/// @brief make_signed_t - make_signed::type的便捷别名
template<typename T>
using make_signed_t = typename make_signed<T>::type;

/// @brief make_unsigned - Convert to corresponding unsigned type
/// @brief make_unsigned - 转换为对应的无符号类型
///
/// Provides the unsigned type corresponding to the given integral or enumeration type.
/// For example, make_unsigned<int>::type is unsigned int.
/// Preserves const/volatile qualifiers.
///
/// 提供与给定整数或枚举类型对应的无符号类型。
/// 例如，make_unsigned<int>::type是unsigned int。
/// 保留const/volatile限定符。
template<typename T>
struct make_unsigned {
    static_assert(is_integral_v<T> || is_enum_v<T>, "make_unsigned requires integral or enum type");
    using type = T; ///< Simplified implementation / 简化实现
};

// Specializations for standard integral types
// 标准整数类型的特化
template<> struct make_unsigned<bool> { using type = bool; };
template<> struct make_unsigned<char> { using type = unsigned char; };
template<> struct make_unsigned<signed char> { using type = unsigned char; };
template<> struct make_unsigned<unsigned char> { using type = unsigned char; };
template<> struct make_unsigned<short> { using type = unsigned short; };
template<> struct make_unsigned<unsigned short> { using type = unsigned short; };
template<> struct make_unsigned<int> { using type = unsigned int; };
template<> struct make_unsigned<unsigned int> { using type = unsigned int; };
template<> struct make_unsigned<long> { using type = unsigned long; };
template<> struct make_unsigned<unsigned long> { using type = unsigned long; };
template<> struct make_unsigned<long long> { using type = unsigned long long; };
template<> struct make_unsigned<unsigned long long> { using type = unsigned long long; };

// Handle const/volatile qualifiers
// 处理const/volatile限定符
template<typename T> struct make_unsigned<const T> { using type = const typename make_unsigned<T>::type; };
template<typename T> struct make_unsigned<volatile T> { using type = volatile typename make_unsigned<T>::type; };
template<typename T> struct make_unsigned<const volatile T> { using type = const volatile typename make_unsigned<T>::type; };

/// @brief make_unsigned_t - Convenience alias for make_unsigned::type
/// @brief make_unsigned_t - make_unsigned::type的便捷别名
template<typename T>
using make_unsigned_t = typename make_unsigned<T>::type;

/// @brief common_type - Find common type for a set of types
/// @brief common_type - 为一组类型查找公共类型
///
/// Determines the common type to which all specified types can be implicitly converted.
/// This is useful for determining the result type of operations involving multiple types.
///
/// 确定所有指定类型都可以隐式转换到的公共类型。
/// 这对于确定涉及多个类型的操作的结果类型很有用。
template<typename...>
struct common_type {};

/// @brief Specialization for single type
/// @brief 单个类型的特化
template<typename T>
struct common_type<T> {
    using type = decay_t<T>;
};

/// @brief Specialization for two types
/// @brief 两个类型的特化
template<typename T, typename U>
struct common_type<T, U> {
private:
    using decayed_T = decay_t<T>;
    using decayed_U = decay_t<U>;
    using cond_type = decltype(true ? mystl::declval<decayed_T>() : mystl::declval<decayed_U>());
public:
    using type = decay_t<cond_type>;
};

/// @brief Specialization for three or more types
/// @brief 三个或更多类型的特化
template<typename T, typename U, typename... V>
struct common_type<T, U, V...> {
    using type = typename common_type<typename common_type<T, U>::type, V...>::type;
};

/// @brief common_type_t - Convenience alias for common_type::type
/// @brief common_type_t - common_type::type的便捷别名
template<typename... T>
using common_type_t = typename common_type<T...>::type;

/// @brief underlying_type - Get underlying type of enum
/// @brief underlying_type - 获取枚举的基础类型
///
/// Provides the underlying integer type of an enumeration type.
/// Uses compiler intrinsics when available for accurate results.
///
/// 提供枚举类型的基础整数类型。
/// 在可用时使用编译器内部函数以获得准确结果。
template<typename T>
struct underlying_type {
    static_assert(is_enum_v<T>, "underlying_type requires enum type");
    
    // Use compiler intrinsic when available
    // 在可用时使用编译器内部函数
    #ifdef __GNUC__
        using type = __underlying_type(T);
    #elif defined(_MSC_VER)
        using type = __underlying_type(T);
    #else
        // Fallback: try to deduce from enum values (simplified)
        // This is not fully correct but provides a placeholder
        // 后备方案：尝试从枚举值推导（简化）
        // 这不完全正确，但提供了一个占位符
        using type = int;
    #endif
};

/// @brief underlying_type_t - Convenience alias for underlying_type::type
/// @brief underlying_type_t - underlying_type::type的便捷别名
template<typename T>
using underlying_type_t = typename underlying_type<T>::type;


} // namespace mystl
