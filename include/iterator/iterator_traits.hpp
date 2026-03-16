#pragma once

#include "iterator_tags.hpp"
#include "../utility/type_traits.hpp"
#include <cstddef>

/**
 * @file iterator_traits.hpp
 * @brief Iterator traits system for type extraction and category checking
 * @brief 迭代器特性系统，用于类型提取和类别检查
 * 
 * This file defines the iterator_traits template and its specializations,
 * which provide a uniform interface for extracting iterator-related types
 * and checking iterator categories. It supports custom iterators, raw pointers,
 * const/volatile pointers, C arrays, and provides comprehensive iterator
 * concept checking facilities.
 * 
 * 本文件定义了 iterator_traits 模板及其特化，为提取迭代器相关类型
 * 和检查迭代器类别提供统一接口。它支持自定义迭代器、原始指针、
 * const/volatile 指针、C 数组，并提供全面的迭代器概念检查功能。
 * 
 * Key features include:
 * - Main template for custom iterators
 * - Specializations for pointers (T*, const T*, volatile T*, const volatile T*)
 * - Specializations for C arrays (T[N], const T[N])
 * - Type alias helpers (iterator_category_t, value_type_t, etc.)
 * - Safe iterator traits with SFINAE protection
 * - Iterator concept checking (is_iterator, is_input_iterator, etc.)
 * - C++20 style iterator traits (iter_value_t, iter_reference_t, etc.)
 * 
 * 主要功能包括：
 * - 自定义迭代器的主模板
 * - 指针特化 (T*, const T*, volatile T*, const volatile T*)
 * - C 数组特化 (T[N], const T[N])
 * - 类型别名辅助 (iterator_category_t, value_type_t 等)
 * - 具有 SFINAE 保护的安全迭代器特性
 * - 迭代器概念检查 (is_iterator, is_input_iterator 等)
 * - C++20 风格迭代器特性 (iter_value_t, iter_reference_t 等)
 * 
 * @author mySTL shensjt
 * @date 2026-03-17
 * 
 * @see iterator_tags.hpp
 * @see iterator_functions.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/iterator_traits
 */

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

// ==================== Main Template ====================
// ==================== 主模板 ====================

/**
 * @brief Main template for iterator traits
 * @brief 迭代器特性的主模板
 * 
 * @tparam Iterator Iterator type to extract traits from
 * @tparam Iterator 要提取特性的迭代器类型
 * 
 * @details Extracts iterator-related type information from custom iterator types.
 * Custom iterators must define the following member types:
 * - iterator_category: The iterator category tag (e.g., random_access_iterator_tag)
 * - value_type: Type of the elements the iterator points to
 * - difference_type: Type used to represent distances between iterators
 * - pointer: Pointer to value_type
 * - reference: Reference to value_type
 * 
 * @details 从自定义迭代器类型提取迭代器相关的类型信息。
 * 自定义迭代器必须定义以下成员类型：
 * - iterator_category: 迭代器类别标签（例如 random_access_iterator_tag）
 * - value_type: 迭代器指向的元素的类型
 * - difference_type: 用于表示迭代器之间距离的类型
 * - pointer: 指向 value_type 的指针
 * - reference: 指向 value_type 的引用
 * 
 * @note This template is intended for custom iterator types.
 * For pointers and arrays, specializations are provided.
 * @note 此模板适用于自定义迭代器类型。
 * 对于指针和数组，提供了特化版本。
 */
template<typename Iterator>
struct iterator_traits {
    using iterator_category = typename Iterator::iterator_category;  ///< Iterator category / 迭代器类别
    using value_type        = typename Iterator::value_type;         ///< Value type / 值类型
    using difference_type   = typename Iterator::difference_type;    ///< Difference type / 差值类型
    using pointer           = typename Iterator::pointer;            ///< Pointer type / 指针类型
    using reference         = typename Iterator::reference;          ///< Reference type / 引用类型
};

// ==================== Pointer Specializations ====================
// ==================== 指针特化 ====================

/**
 * @brief Internal namespace for implementation details
 * @brief 实现细节的内部命名空间
 */
namespace detail {
    /**
     * @brief Base template for pointer iterator traits
     * @brief 指针迭代器特性的基础模板
     * 
     * @tparam T Element type
     * @tparam T 元素类型
     * @tparam Qualifiers Pointer qualifiers (e.g., T*, const T*, volatile T*, const volatile T*)
     * @tparam Qualifiers 指针限定符（例如 T*, const T*, volatile T*, const volatile T*）
     * 
     * @details Provides common type definitions for all pointer specializations.
     * All pointers are random access iterators with std::ptrdiff_t as difference_type.
     * 
     * @details 为所有指针特化提供通用的类型定义。
     * 所有指针都是随机访问迭代器，使用 std::ptrdiff_t 作为差值类型。
     */
    template<typename T, typename Qualifiers>
    struct iterator_traits_pointer_base {
        using iterator_category = random_access_iterator_tag;  ///< All pointers are random access iterators / 所有指针都是随机访问迭代器
        using value_type        = T;                           ///< Element type (without qualifiers) / 元素类型（不带限定符）
        using difference_type   = std::ptrdiff_t;              ///< Standard difference type for pointers / 指针的标准差值类型
    };
}

/**
 * @brief Specialization for raw pointers (T*)
 * @brief 原始指针特化 (T*)
 * 
 * @tparam T Element type
 * @tparam T 元素类型
 * 
 * @details Provides iterator traits for raw pointers.
 * All pointers are random access iterators.
 * 
 * @details 为原始指针提供迭代器特性。
 * 所有指针都是随机访问迭代器。
 */
template<typename T>
struct iterator_traits<T*> : detail::iterator_traits_pointer_base<T, T*> {
    using pointer   = T*;   ///< Pointer type (T*) / 指针类型 (T*)
    using reference = T&;   ///< Reference type (T&) / 引用类型 (T&)
};

/**
 * @brief Specialization for const pointers (const T*)
 * @brief const 指针特化 (const T*)
 * 
 * @tparam T Element type
 * @tparam T 元素类型
 * 
 * @details Provides iterator traits for const pointers.
 * The value_type is T (not const T), but pointer and reference are const-qualified.
 * 
 * @details 为 const 指针提供迭代器特性。
 * value_type 是 T（不是 const T），但 pointer 和 reference 是 const 限定的。
 */
template<typename T>
struct iterator_traits<const T*> : detail::iterator_traits_pointer_base<T, const T*> {
    using pointer   = const T*;   ///< Pointer type (const T*) / 指针类型 (const T*)
    using reference = const T&;   ///< Reference type (const T&) / 引用类型 (const T&)
};

/**
 * @brief Specialization for volatile pointers (volatile T*)
 * @brief volatile 指针特化 (volatile T*)
 * 
 * @tparam T Element type
 * @tparam T 元素类型
 * 
 * @details Provides iterator traits for volatile pointers.
 * The value_type is T (not volatile T), but pointer and reference are volatile-qualified.
 * 
 * @details 为 volatile 指针提供迭代器特性。
 * value_type 是 T（不是 volatile T），但 pointer 和 reference 是 volatile 限定的。
 */
template<typename T>
struct iterator_traits<volatile T*> : detail::iterator_traits_pointer_base<T, volatile T*> {
    using pointer   = volatile T*;   ///< Pointer type (volatile T*) / 指针类型 (volatile T*)
    using reference = volatile T&;   ///< Reference type (volatile T&) / 引用类型 (volatile T&)
};

/**
 * @brief Specialization for const volatile pointers (const volatile T*)
 * @brief const volatile 指针特化 (const volatile T*)
 * 
 * @tparam T Element type
 * @tparam T 元素类型
 * 
 * @details Provides iterator traits for const volatile pointers.
 * The value_type is T (not const volatile T), but pointer and reference are const volatile-qualified.
 * 
 * @details 为 const volatile 指针提供迭代器特性。
 * value_type 是 T（不是 const volatile T），但 pointer 和 reference 是 const volatile 限定的。
 */
template<typename T>
struct iterator_traits<const volatile T*> : detail::iterator_traits_pointer_base<T, const volatile T*> {
    using pointer   = const volatile T*;   ///< Pointer type (const volatile T*) / 指针类型 (const volatile T*)
    using reference = const volatile T&;   ///< Reference type (const volatile T&) / 引用类型 (const volatile T&)
};

// ==================== Type Alias Helpers ====================
// ==================== 类型别名辅助 ====================

/**
 * @brief Type alias for iterator category
 * @brief 迭代器类别的类型别名
 * 
 * @tparam Iterator Iterator type
 * @tparam Iterator 迭代器类型
 * 
 * @details Convenient alias for accessing iterator_category from iterator_traits.
 * 
 * @details 从 iterator_traits 访问 iterator_category 的方便别名。
 */
template<typename Iterator>
using iterator_category_t = typename iterator_traits<Iterator>::iterator_category;

/**
 * @brief Type alias for value type
 * @brief 值类型的类型别名
 * 
 * @tparam Iterator Iterator type
 * @tparam Iterator 迭代器类型
 * 
 * @details Convenient alias for accessing value_type from iterator_traits.
 * 
 * @details 从 iterator_traits 访问 value_type 的方便别名。
 */
template<typename Iterator>
using value_type_t = typename iterator_traits<Iterator>::value_type;

/**
 * @brief Type alias for difference type
 * @brief 差值类型的类型别名
 * 
 * @tparam Iterator Iterator type
 * @tparam Iterator 迭代器类型
 * 
 * @details Convenient alias for accessing difference_type from iterator_traits.
 * 
 * @details 从 iterator_traits 访问 difference_type 的方便别名。
 */
template<typename Iterator>
using difference_type_t = typename iterator_traits<Iterator>::difference_type;

/**
 * @brief Type alias for pointer type
 * @brief 指针类型的类型别名
 * 
 * @tparam Iterator Iterator type
 * @tparam Iterator 迭代器类型
 * 
 * @details Convenient alias for accessing pointer from iterator_traits.
 * 
 * @details 从 iterator_traits 访问 pointer 的方便别名。
 */
template<typename Iterator>
using pointer_t = typename iterator_traits<Iterator>::pointer;

/**
 * @brief Type alias for reference type
 * @brief 引用类型的类型别名
 * 
 * @tparam Iterator Iterator type
 * @tparam Iterator 迭代器类型
 * 
 * @details Convenient alias for accessing reference from iterator_traits.
 * 
 * @details 从 iterator_traits 访问 reference 的方便别名。
 */
template<typename Iterator>
using reference_t = typename iterator_traits<Iterator>::reference;

// ==================== Safe Iterator Traits ====================
// ==================== 安全迭代器特性 ====================

/**
 * @brief Safe iterator traits with SFINAE protection
 * @brief 具有 SFINAE 保护的安全迭代器特性
 * 
 * @tparam Iter Iterator type to extract traits from
 * @tparam Iter 要提取特性的迭代器类型
 * @tparam void SFINAE helper parameter
 * @tparam void SFINAE 辅助参数
 * 
 * @details Provides a safe way to extract iterator traits that won't cause
 * compilation errors for non-iterator types. Uses SFINAE to provide an empty
 * type when iterator_traits cannot be instantiated.
 * 
 * @details 提供一种安全的方式提取迭代器特性，不会对非迭代器类型
 * 导致编译错误。当 iterator_traits 无法实例化时，使用 SFINAE 提供空类型。
 * 
 * @note Useful in template metaprogramming where the type might not be an iterator.
 * @note 在模板元编程中很有用，其中类型可能不是迭代器。
 */
template<typename Iter, typename = void>
struct safe_iterator_traits {};

/**
 * @brief Specialization for valid iterator types
 * @brief 有效迭代器类型的特化
 * 
 * @tparam Iter Valid iterator type
 * @tparam Iter 有效的迭代器类型
 * 
 * @details Inherits from iterator_traits<Iter> when all required member types exist.
 * 
 * @details 当所有必需的成员类型存在时，继承自 iterator_traits<Iter>。
 */
template<typename Iter>
struct safe_iterator_traits<Iter, void_t<
    typename iterator_traits<Iter>::iterator_category,
    typename iterator_traits<Iter>::value_type,
    typename iterator_traits<Iter>::difference_type,
    typename iterator_traits<Iter>::pointer,
    typename iterator_traits<Iter>::reference
>> : iterator_traits<Iter> {};

// ==================== Iterator Concept Checking ====================
// ==================== 迭代器概念检查 ====================

/**
 * @brief Internal namespace for iterator concept checking implementation details
 * @brief 迭代器概念检查实现细节的内部命名空间
 */
namespace detail {
    /**
     * @brief Check if a type has iterator member types
     * @brief 检查类型是否具有迭代器成员类型
     * 
     * @tparam T Type to check
     * @tparam T 要检查的类型
     * @tparam void SFINAE helper parameter
     * @tparam void SFINAE 辅助参数
     */
    template<typename T, typename = void>
    struct has_iterator_members : false_type {};
    
    /**
     * @brief Specialization for types with iterator member types
     * @brief 具有迭代器成员类型的特化
     */
    template<typename T>
    struct has_iterator_members<T, void_t<
        typename T::iterator_category,
        typename T::value_type,
        typename T::difference_type,
        typename T::pointer,
        typename T::reference
    >> : true_type {};
    
    /**
     * @brief Check if a type is a pointer iterator
     * @brief 检查类型是否为指针迭代器
     * 
     * @tparam T Type to check
     * @tparam T 要检查的类型
     */
    template<typename T>
    struct is_pointer_iterator : false_type {};
    
    /**
     * @brief Specialization for raw pointers
     * @brief 原始指针的特化
     */
    template<typename T>
    struct is_pointer_iterator<T*> : true_type {};
    
    /**
     * @brief Specialization for const pointers
     * @brief const 指针的特化
     */
    template<typename T>
    struct is_pointer_iterator<const T*> : true_type {};
    
    /**
     * @brief Specialization for volatile pointers
     * @brief volatile 指针的特化
     */
    template<typename T>
    struct is_pointer_iterator<volatile T*> : true_type {};
    
    /**
     * @brief Specialization for const volatile pointers
     * @brief const volatile 指针的特化
     */
    template<typename T>
    struct is_pointer_iterator<const volatile T*> : true_type {};
}

/**
 * @brief Type trait to check if a type is an iterator
 * @brief 检查类型是否为迭代器的类型特征
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 * 
 * @details Checks if T is either a custom iterator (has iterator member types)
 * or a pointer type (raw, const, volatile, or const volatile).
 * 
 * @details 检查 T 是否是自定义迭代器（具有迭代器成员类型）
 * 或指针类型（原始、const、volatile 或 const volatile）。
 */
template<typename T>
struct is_iterator : integral_constant<bool,
    detail::has_iterator_members<T>::value ||
    detail::is_pointer_iterator<T>::value> {};

/**
 * @brief Variable template version of is_iterator
 * @brief is_iterator 的变量模板版本
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 */
template<typename T>
inline constexpr bool is_iterator_v = is_iterator<T>::value;

/**
 * @brief Internal namespace for iterator category checking implementation
 * @brief 迭代器类别检查实现的内部命名空间
 */
namespace detail {
    /**
     * @brief Generic iterator category check implementation
     * @brief 通用迭代器类别检查实现
     * 
     * @tparam Condition Condition template to apply
     * @tparam Condition 要应用的条件模板
     */
    template<template<typename> class Condition>
    struct iterator_category_check_impl {
        template<typename T, typename = void>
        struct apply : false_type {};
        
        template<typename T>
        struct apply<T, void_t<typename iterator_traits<T>::iterator_category>>
            : Condition<T> {};
    };
    
    // Condition templates for each iterator category
    // 每个迭代器类别的条件模板
    
    template<typename T>
    struct is_input_condition : integral_constant<bool,
        is_base_of_v<input_iterator_tag, iterator_category_t<T>>> {};
    
    template<typename T>
    struct is_output_condition : integral_constant<bool,
        is_same_v<iterator_category_t<T>, output_iterator_tag> || 
        is_base_of_v<output_iterator_tag, iterator_category_t<T>>> {};
    
    template<typename T>
    struct is_forward_condition : integral_constant<bool,
        is_base_of_v<forward_iterator_tag, iterator_category_t<T>>> {};
    
    template<typename T>
    struct is_bidirectional_condition : integral_constant<bool,
        is_base_of_v<bidirectional_iterator_tag, iterator_category_t<T>>> {};
    
    template<typename T>
    struct is_random_access_condition : integral_constant<bool,
        is_base_of_v<random_access_iterator_tag, iterator_category_t<T>>> {};
    
    template<typename T>
    struct is_contiguous_condition : integral_constant<bool,
        is_base_of_v<contiguous_iterator_tag, iterator_category_t<T>>> {};
}

/**
 * @brief Check if an iterator is an input iterator
 * @brief 检查迭代器是否为输入迭代器
 * 
 * @tparam T Iterator type to check
 * @tparam T 要检查的迭代器类型
 */
template<typename T>
struct is_input_iterator : detail::iterator_category_check_impl<
    detail::is_input_condition>::template apply<T> {};

/**
 * @brief Check if an iterator is an output iterator
 * @brief 检查迭代器是否为输出迭代器
 * 
 * @tparam T Iterator type to check
 * @tparam T 要检查的迭代器类型
 */
template<typename T>
struct is_output_iterator : detail::iterator_category_check_impl<
    detail::is_output_condition>::template apply<T> {};

/**
 * @brief Check if an iterator is a forward iterator
 * @brief 检查迭代器是否为前向迭代器
 * 
 * @tparam T Iterator type to check
 * @tparam T 要检查的迭代器类型
 */
template<typename T>
struct is_forward_iterator : detail::iterator_category_check_impl<
    detail::is_forward_condition>::template apply<T> {};

/**
 * @brief Check if an iterator is a bidirectional iterator
 * @brief 检查迭代器是否为双向迭代器
 * 
 * @tparam T Iterator type to check
 * @tparam T 要检查的迭代器类型
 */
template<typename T>
struct is_bidirectional_iterator : detail::iterator_category_check_impl<
    detail::is_bidirectional_condition>::template apply<T> {};

/**
 * @brief Check if an iterator is a random access iterator
 * @brief 检查迭代器是否为随机访问迭代器
 * 
 * @tparam T Iterator type to check
 * @tparam T 要检查的迭代器类型
 */
template<typename T>
struct is_random_access_iterator : detail::iterator_category_check_impl<
    detail::is_random_access_condition>::template apply<T> {};

/**
 * @brief Check if an iterator is a contiguous iterator (C++20)
 * @brief 检查迭代器是否为连续迭代器 (C++20)
 * 
 * @tparam T Iterator type to check
 * @tparam T 要检查的迭代器类型
 */
template<typename T>
struct is_contiguous_iterator : detail::iterator_category_check_impl<
    detail::is_contiguous_condition>::template apply<T> {};

// Variable template versions
// 变量模板版本

template<typename T>
inline constexpr bool is_input_iterator_v = is_input_iterator<T>::value;

template<typename T>
inline constexpr bool is_output_iterator_v = is_output_iterator<T>::value;

template<typename T>
inline constexpr bool is_forward_iterator_v = is_forward_iterator<T>::value;

template<typename T>
inline constexpr bool is_bidirectional_iterator_v = is_bidirectional_iterator<T>::value;

template<typename T>
inline constexpr bool is_random_access_iterator_v = is_random_access_iterator<T>::value;

template<typename T>
inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<T>::value;

// ==================== C Array Specializations ====================
// ==================== C 数组特化 ====================

/**
 * @brief Specialization for C-style arrays (T[N])
 * @brief C 风格数组特化 (T[N])
 * 
 * @tparam T Element type
 * @tparam T 元素类型
 * @tparam N Array size
 * @tparam N 数组大小
 * 
 * @details Provides iterator traits for C-style arrays.
 * C arrays are random access iterators with contiguous storage.
 * 
 * @details 为 C 风格数组提供迭代器特性。
 * C 数组是具有连续存储的随机访问迭代器。
 */
template<typename T, std::size_t N>
struct iterator_traits<T[N]> {
    using iterator_category = random_access_iterator_tag;  ///< C arrays are random access iterators / C 数组是随机访问迭代器
    using value_type        = T;                           ///< Element type / 元素类型
    using difference_type   = std::ptrdiff_t;              ///< Standard difference type / 标准差值类型
    using pointer           = T*;                          ///< Pointer to element type / 指向元素类型的指针
    using reference         = T&;                          ///< Reference to element type / 指向元素类型的引用
};

/**
 * @brief Specialization for const C-style arrays (const T[N])
 * @brief const C 风格数组特化 (const T[N])
 * 
 * @tparam T Element type
 * @tparam T 元素类型
 * @tparam N Array size
 * @tparam N 数组大小
 * 
 * @details Provides iterator traits for const C-style arrays.
 * The value_type is T (not const T), but pointer and reference are const-qualified.
 * 
 * @details 为 const C 风格数组提供迭代器特性。
 * value_type 是 T（不是 const T），但 pointer 和 reference 是 const 限定的。
 */
template<typename T, std::size_t N>
struct iterator_traits<const T[N]> {
    using iterator_category = random_access_iterator_tag;  ///< Const C arrays are random access iterators / const C 数组是随机访问迭代器
    using value_type        = T;                           ///< Element type (not const T) / 元素类型（不是 const T）
    using difference_type   = std::ptrdiff_t;              ///< Standard difference type / 标准差值类型
    using pointer           = const T*;                    ///< Const pointer to element type / 指向元素类型的 const 指针
    using reference         = const T&;                    ///< Const reference to element type / 指向元素类型的 const 引用
};

// ==================== C++20 Style Iterator Traits ====================
// ==================== C++20 风格迭代器特性 ====================

/**
 * @brief Helper type to remove cvref (const, volatile, reference) qualifiers
 * @brief 移除 cvref（const、volatile、引用）限定符的辅助类型
 * 
 * @tparam T Type to remove qualifiers from
 * @tparam T 要移除限定符的类型
 * 
 * @details Equivalent to std::remove_cvref_t in C++20.
 * 
 * @details 等同于 C++20 中的 std::remove_cvref_t。
 */
template<typename T>
using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;

/**
 * @brief C++20 style iterator value type extraction
 * @brief C++20 风格迭代器值类型提取
 * 
 * @tparam T Iterator type (may have cvref qualifiers)
 * @tparam T 迭代器类型（可能具有 cvref 限定符）
 * 
 * @details Similar to std::iter_value_t in C++20.
 * Removes cvref qualifiers before extracting value_type from iterator_traits.
 * 
 * @details 类似于 C++20 中的 std::iter_value_t。
 * 在从 iterator_traits 提取 value_type 之前移除 cvref 限定符。
 */
template<typename T>
using iter_value_t = typename iterator_traits<remove_cvref_t<T>>::value_type;

/**
 * @brief C++20 style iterator reference type extraction
 * @brief C++20 风格迭代器引用类型提取
 * 
 * @tparam T Iterator type (may have cvref qualifiers)
 * @tparam T 迭代器类型（可能具有 cvref 限定符）
 * 
 * @details Similar to std::iter_reference_t in C++20.
 * 
 * @details 类似于 C++20 中的 std::iter_reference_t。
 */
template<typename T>
using iter_reference_t = typename iterator_traits<remove_cvref_t<T>>::reference;

/**
 * @brief C++20 style iterator difference type extraction
 * @brief C++20 风格迭代器差值类型提取
 * 
 * @tparam T Iterator type (may have cvref qualifiers)
 * @tparam T 迭代器类型（可能具有 cvref 限定符）
 * 
 * @details Similar to std::iter_difference_t in C++20.
 * 
 * @details 类似于 C++20 中的 std::iter_difference_t。
 */
template<typename T>
using iter_difference_t = typename iterator_traits<remove_cvref_t<T>>::difference_type;

/**
 * @brief C++20 style iterator pointer type extraction
 * @brief C++20 风格迭代器指针类型提取
 * 
 * @tparam T Iterator type (may have cvref qualifiers)
 * @tparam T 迭代器类型（可能具有 cvref 限定符）
 * 
 * @details Similar to std::iter_pointer_t in C++20.
 * 
 * @details 类似于 C++20 中的 std::iter_pointer_t。
 */
template<typename T>
using iter_pointer_t = typename iterator_traits<remove_cvref_t<T>>::pointer;

} // namespace mystl
