#pragma once

/**
 * @file move.hpp
 * @brief Move utility functions
 * 
 * Provides move semantics utilities for efficient value transfer.
 */

#include "type_traits.hpp"  // for remove_reference_t, is_lvalue_reference_v

namespace mystl {

/**
 * @brief Forward an lvalue as either an lvalue or an rvalue
 * 
 * @tparam T Type of value to forward
 * @param t Value to forward
 * @return T&& if t is an rvalue, T& otherwise
 * 
 * @details Perfect forwarding utility. Used to preserve the value category
 * (lvalue/rvalue) of function arguments when passing them through.
 */
template<typename T>
constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

/**
 * @brief Forward an rvalue as an rvalue
 * 
 * @tparam T Type of value to forward
 * @param t Value to forward
 * @return T&&
 * 
 * @details Used when t is known to be an rvalue.
 */
template<typename T>
constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
    static_assert(!is_lvalue_reference_v<T>,
                  "Cannot forward an rvalue as an lvalue");
    return static_cast<T&&>(t);
}

/**
 * @brief Convert a value to an rvalue reference
 * 
 * @tparam T Type of value to move
 * @param t Value to move
 * @return remove_reference_t<T>&&
 * 
 * @details Converts any value to an rvalue reference, enabling move semantics.
 * The value should not be used after being moved from.
 */
template<typename T>
constexpr remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<remove_reference_t<T>&&>(t);
}

/**
 * @brief Exchange values
 * 
 * @tparam T Type of values to exchange
 * @param a First value
 * @param b Second value
 * @return Old value of a
 * 
 * @details Equivalent to: T old = move(a); a = move(b); return old;
 * More efficient than separate swap in some cases.
 */
template<typename T>
constexpr T exchange(T& obj, T&& new_value) noexcept {
    T old = mystl::move(obj);
    obj = mystl::move(new_value);
    return old;
}

/**
 * @brief Exchange values with copy
 * 
 * @tparam T Type of values to exchange
 * @param a First value
 * @param b Second value
 * @return Old value of a
 * 
 * @details Uses copy instead of move for the new value.
 */
template<typename T>
constexpr T exchange(T& obj, const T& new_value) noexcept {
    T old = mystl::move(obj);
    obj = new_value;
    return old;
}

} // namespace mystl
