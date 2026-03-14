#pragma once
#include <cstddef>

namespace mystl {
// ==================== 1. Basic Utilities ====================

// 1.1 integral_constant - The Base of All Traits
template<typename T, T v>
struct integral_constant {
    static constexpr T value = v;             // the value
    using value_type = T;                     // value type
    using type = integral_constant<T, v>;     // self type

    // convert to value
    constexpr operator value_type() const noexcept {
        return value;
    }

    // function call
    constexpr value_type operator()() const noexcept {
        return value;
    }
};

// Two basic type aliases
using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;


// ==================== 2. Type Checks ====================

// 2.1 is_same - check if two types are the same
template<typename T, typename U>
struct is_same : false_type {};

template<typename T>
struct is_same<T, T> : true_type {};

// helper template
template<typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;


// 2.2 is_const - check if const
template<typename T>
struct is_const : false_type {};

template<typename T>
struct is_const<const T> : true_type {};

template<typename T>
inline constexpr bool is_const_v = is_const<T>::value;


// 2.3 is_volatile - check if volatile
template<typename T>
struct is_volatile : false_type {};

template<typename T>
struct is_volatile<volatile T> : true_type {};

template<typename T>
inline constexpr bool is_volatile_v = is_volatile<T>::value;


// 2.4 is_pointer - check if pointer
template<typename T>
struct is_pointer : false_type {};

template<typename T>
struct is_pointer<T*> : true_type {};

template<typename T>
struct is_pointer<T* const> : true_type {};

template<typename T>
struct is_pointer<T* volatile> : true_type {};

template<typename T>
struct is_pointer<T* const volatile> : true_type {};

template<typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;


// 2.5 is_reference - check if reference
template<typename T>
struct is_reference : false_type {};

template<typename T>
struct is_reference<T&> : true_type {};

template<typename T>
struct is_reference<T&&> : true_type {};

template<typename T>
inline constexpr bool is_reference_v = is_reference<T>::value;


// 2.6 is_lvalue_reference - lvalue reference
template<typename T>
struct is_lvalue_reference : false_type {};

template<typename T>
struct is_lvalue_reference<T&> : true_type {};

template<typename T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;


// 2.7 is_rvalue_reference - rvalue reference
template<typename T>
struct is_rvalue_reference : false_type {};

template<typename T>
struct is_rvalue_reference<T&&> : true_type {};

template<typename T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;


// 2.8 is_void - check if void (with const and volatile qualifiers)
template<typename T>
struct is_void : false_type {};

template<>
struct is_void<void> : true_type {};

template<>
struct is_void<const void> : true_type {};

template<>
struct is_void<volatile void> : true_type {};

template<>
struct is_void<const volatile void> : true_type {};

template<typename T>
inline constexpr bool is_void_v = is_void<T>::value;


// 2.9 is_null_pointer - check if nullptr_t
template<typename T>
struct is_null_pointer : false_type {};

template<>
struct is_null_pointer<std::nullptr_t> : true_type {};

template<>
struct is_null_pointer<const std::nullptr_t> : true_type {};

template<>
struct is_null_pointer<volatile std::nullptr_t> : true_type {};

template<>
struct is_null_pointer<const volatile std::nullptr_t> : true_type {};

template<typename T>
inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;


// 2.10 is_class - check if class (simple implementation)
namespace detail {
    template<typename T>
    char test_is_class(int T::*);  // if T is a class, this overload is viable
    
    template<typename T>
    long test_is_class(...);       // fallback overload
}

template<typename T>
struct is_class : integral_constant<bool, sizeof(detail::test_is_class<T>(0)) == 1> {};

template<typename T>
inline constexpr bool is_class_v = is_class<T>::value;


// ==================== 3. Type Transformations ====================

// 3.1 remove_const - remove const
template<typename T>
struct remove_const {
    using type = T;
};

template<typename T>
struct remove_const<const T> {
    using type = T;
};

template<typename T>
using remove_const_t = typename remove_const<T>::type;

// 3.2 remove_volatile - remove volatile
template<typename T>
struct remove_volatile {
    using type = T;
};

template<typename T>
struct remove_volatile<volatile T> {
    using type = T;
};

template<typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

// 3.3 remove_cv - remove both const and volatile
template<typename T>
struct remove_cv {
    using type = T;
};

template<typename T>
struct remove_cv<const T> {
    using type = T;
};

template<typename T>
struct remove_cv<volatile T> {
    using type = T;
};

template<typename T>
struct remove_cv<const volatile T> {
    using type = T;
};

template<typename T>
using remove_cv_t = typename remove_cv<T>::type;

// 3.4 remove_reference - remove reference
template<typename T>
struct remove_reference {
    using type = T;
};

template<typename T>
struct remove_reference<T&> {
    using type = T;
};

template<typename T>
struct remove_reference<T&&> {
    using type = T;
};

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;

// 3.5 add_const - add const
template<typename T>
struct add_const {
    using type = const T;
};

template<typename T>
using add_const_t = typename add_const<T>::type;

// 3.6 add_volatile - add volatile
template<typename T>
struct add_volatile {
    using type = volatile T;
};

template<typename T>
using add_volatile_t = typename add_volatile<T>::type;

// 3.7 add_cv - add const and volatile
template<typename T>
struct add_cv {
    using type = const volatile T;
};

template<typename T>
using add_cv_t = typename add_cv<T>::type;

// 3.8 add_pointer - add pointer
template<typename T>
struct add_pointer {
    using type = remove_reference_t<T>*;
};

template<typename T>
using add_pointer_t = typename add_pointer<T>::type;

// 3.9 add_lvalue_reference - add lvalue reference
template<typename T>
struct add_lvalue_reference {
    using type = T&;
};

// specialization for void (void& is not allowed)
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

template<typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

// 3.10 add_rvalue_reference - add rvalue reference
template<typename T>
struct add_rvalue_reference {
    using type = T&&;
};

// specialization for void (void&& is not allowed)
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

template<typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

// 3.11 declval - get a reference to a type without constructing an object
// Note: Only declaration, never defined (used in unevaluated contexts only)
template<typename T>
add_rvalue_reference_t<T> declval() noexcept;


// ==================== 5. Type Relationships ====================

// 5.1 is_same (already implemented)

// 5.2 is_base_of - check if base class
namespace detail {
    template<typename Base>
    true_type test_base_of(const volatile Base*);
    
    template<typename Base>
    false_type test_base_of(const volatile void*);
    
    template<typename Base, typename Derived>
    auto is_base_of_helper(int) -> decltype(test_base_of<Base>(static_cast<Derived*>(nullptr)));
    
    template<typename Base, typename Derived>
    auto is_base_of_helper(...) -> true_type;
}

template<typename Base, typename Derived>
struct is_base_of : 
    integral_constant<
        bool,
        is_class_v<Base> && is_class_v<Derived> &&
        decltype(detail::is_base_of_helper<Base, Derived>(0))::value
    > {};

// 5.3 is_convertible - check if types are convertible
namespace detail {
    // Helper function that accepts any type convertible to To
    template<typename To>
    void test_convertible_helper(To);
    
    template<typename From, typename To>
    auto is_convertible_test(int) -> decltype(
        test_convertible_helper<To>(mystl::declval<From>()), true_type()
    );
    
    template<typename From, typename To>
    auto is_convertible_test(...) -> false_type;
}

template<typename From, typename To>
struct is_convertible : decltype(detail::is_convertible_test<From, To>(0)) {};


// ==================== 6. Conditional Selection ====================

// 6.1 conditional - compile-time version of ternary operator
template<bool B, typename T, typename F>
struct conditional {
    using type = T;
};

template<typename T, typename F>
struct conditional<false, T, F> {
    using type = F;
};

template<bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

// 6.2 conjunction - logical AND
template<typename...>
struct conjunction : true_type {};

template<typename T>
struct conjunction<T> : T {};

template<typename T, typename... Args>
struct conjunction<T, Args...> : 
    conditional_t<bool(T::value), conjunction<Args...>, T> {};

template<typename... Args>
inline constexpr bool conjunction_v = conjunction<Args...>::value;

// 6.3 disjunction - logical OR
template<typename...>
struct disjunction : false_type {};

template<typename T>
struct disjunction<T> : T {};

template<typename T, typename... Args>
struct disjunction<T, Args...> : 
    conditional_t<bool(T::value), T, disjunction<Args...>> {};

template<typename... Args>
inline constexpr bool disjunction_v = disjunction<Args...>::value;

// 6.4 negation - logical NOT
template<typename T>
struct negation : integral_constant<bool, !bool(T::value)> {};

template<typename T>
inline constexpr bool negation_v = negation<T>::value;


// ==================== 7. Helper Tools ====================

// 7.1 void_t - map a list of types to void
template<typename...>
using void_t = void;


} // namespace mystl
