#pragma once

/**
 * @file swap.hpp
 * @brief Swap utility functions
 * 
 * Provides swap functionality for various types, supporting ADL (Argument Dependent Lookup).
 */

#include <cstddef>  // for std::size_t

// 前向声明move函数，避免循环包含
namespace mystl {
template<typename T>
constexpr remove_reference_t<T>&& move(T&& t) noexcept;
}

namespace mystl {

/**
 * @brief Swap two values
 * 
 * @tparam T Type of values to swap
 * @param a First value
 * @param b Second value
 * 
 * @details Uses move semantics for efficient swapping. Supports ADL, so user-defined
 * types can provide their own swap overloads that will be found via ADL.
 */
template<typename T>
void swap(T& a, T& b) noexcept {
    T temp = mystl::move(a);
    a = mystl::move(b);
    b = mystl::move(temp);
}

/**
 * @brief Swap two arrays
 * 
 * @tparam T Type of array elements
 * @tparam N Size of arrays
 * @param a First array
 * @param b Second array
 */
template<typename T, std::size_t N>
void swap(T (&a)[N], T (&b)[N]) noexcept(noexcept(swap(*a, *b))) {
    for (std::size_t i = 0; i < N; ++i) {
        swap(a[i], b[i]);
    }
}

/**
 * @brief Enable ADL for swap
 * 
 * This function template enables ADL for swap operations. It will find
 * the best swap function via ADL, falling back to mystl::swap if no
 * better match is found.
 * 
 * @tparam T Type of values to swap
 * @param a First value
 * @param b Second value
 */
template<typename T>
void adl_swap(T& a, T& b) noexcept(noexcept(swap(a, b))) {
    using mystl::swap;
    swap(a, b);
}

} // namespace mystl
