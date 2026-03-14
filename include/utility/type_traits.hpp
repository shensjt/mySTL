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


// ==================== 2. Type Transformations (Basic) ====================

// 2.1 remove_const - remove const
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

// 2.2 remove_volatile - remove volatile
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

// 2.3 remove_cv - remove both const and volatile
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

// 2.4 remove_reference - remove reference
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


// ==================== 3. Type Checks ====================

// 3.1 is_same - check if two types are the same
template<typename T, typename U>
struct is_same : false_type {};

template<typename T>
struct is_same<T, T> : true_type {};

// helper template
template<typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;


// 3.2 is_const - check if const
template<typename T>
struct is_const : false_type {};

template<typename T>
struct is_const<const T> : true_type {};

template<typename T>
inline constexpr bool is_const_v = is_const<T>::value;


// 3.3 is_volatile - check if volatile
template<typename T>
struct is_volatile : false_type {};

template<typename T>
struct is_volatile<volatile T> : true_type {};

template<typename T>
inline constexpr bool is_volatile_v = is_volatile<T>::value;


// 3.4 is_pointer - check if pointer
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


// 3.5 is_reference - check if reference
template<typename T>
struct is_reference : false_type {};

template<typename T>
struct is_reference<T&> : true_type {};

template<typename T>
struct is_reference<T&&> : true_type {};

template<typename T>
inline constexpr bool is_reference_v = is_reference<T>::value;


// 3.6 is_lvalue_reference - lvalue reference
template<typename T>
struct is_lvalue_reference : false_type {};

template<typename T>
struct is_lvalue_reference<T&> : true_type {};

template<typename T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;


// 3.7 is_rvalue_reference - rvalue reference
template<typename T>
struct is_rvalue_reference : false_type {};

template<typename T>
struct is_rvalue_reference<T&&> : true_type {};

template<typename T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;


// 3.8 is_void - check if void (with const and volatile qualifiers)
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


// 3.9 is_null_pointer - check if nullptr_t
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


// 3.10 is_class - check if class (simple implementation)
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


// ==================== 4. Integer and Floating Point Types ====================

// is_integral - check if integral type
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

template<typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;

// is_floating_point - check if floating point type
template<typename T>
struct is_floating_point : integral_constant<bool,
    is_same_v<remove_cv_t<T>, float> ||
    is_same_v<remove_cv_t<T>, double> ||
    is_same_v<remove_cv_t<T>, long double>> {};

template<typename T>
inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

// is_arithmetic - check if arithmetic type (integral or floating point)
template<typename T>
struct is_arithmetic : integral_constant<bool,
    is_integral_v<T> || is_floating_point_v<T>> {};

template<typename T>
inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

// is_signed - check if signed arithmetic type
namespace detail {
    template<typename T, bool = is_arithmetic_v<T>>
    struct is_signed_impl : integral_constant<bool, T(-1) < T(0)> {};
    
    template<typename T>
    struct is_signed_impl<T, false> : false_type {};
}

template<typename T>
struct is_signed : detail::is_signed_impl<T> {};

template<typename T>
inline constexpr bool is_signed_v = is_signed<T>::value;

// is_unsigned - check if unsigned arithmetic type
template<typename T>
struct is_unsigned : integral_constant<bool,
    is_arithmetic_v<T> && !is_signed_v<T>> {};

template<typename T>
inline constexpr bool is_unsigned_v = is_unsigned<T>::value;


// ==================== 5. Array and Function Types ====================

// is_array - check if array type
template<typename T>
struct is_array : false_type {};

template<typename T>
struct is_array<T[]> : true_type {};

template<typename T, std::size_t N>
struct is_array<T[N]> : true_type {};

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

// is_function - check if function type (simplified implementation)
namespace detail {
    template<typename T>
    struct is_function_impl : false_type {};
    
    template<typename Ret, typename... Args>
    struct is_function_impl<Ret(Args...)> : true_type {};
    
    template<typename Ret, typename... Args>
    struct is_function_impl<Ret(Args..., ...)> : true_type {};  // C-style varargs
}

template<typename T>
struct is_function : detail::is_function_impl<remove_cv_t<T>> {};

template<typename T>
inline constexpr bool is_function_v = is_function<T>::value;


// ==================== 6. Type Transformations (Advanced) ====================

// 6.1 add_const - add const
template<typename T>
struct add_const {
    using type = const T;
};

template<typename T>
using add_const_t = typename add_const<T>::type;

// 6.2 add_volatile - add volatile
template<typename T>
struct add_volatile {
    using type = volatile T;
};

template<typename T>
using add_volatile_t = typename add_volatile<T>::type;

// 6.3 add_cv - add const and volatile
template<typename T>
struct add_cv {
    using type = const volatile T;
};

template<typename T>
using add_cv_t = typename add_cv<T>::type;

// 6.4 add_pointer - add pointer
template<typename T>
struct add_pointer {
    using type = remove_reference_t<T>*;
};

template<typename T>
using add_pointer_t = typename add_pointer<T>::type;

// 6.5 add_lvalue_reference - add lvalue reference
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

// 6.6 add_rvalue_reference - add rvalue reference
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


// ==================== 7. Conditional Selection ====================

// 7.1 conditional - compile-time version of ternary operator
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

// 7.2 conjunction - logical AND
template<typename...>
struct conjunction : true_type {};

template<typename T>
struct conjunction<T> : T {};

template<typename T, typename... Args>
struct conjunction<T, Args...> : 
    conditional_t<bool(T::value), conjunction<Args...>, T> {};

template<typename... Args>
inline constexpr bool conjunction_v = conjunction<Args...>::value;

// 7.3 disjunction - logical OR
template<typename...>
struct disjunction : false_type {};

template<typename T>
struct disjunction<T> : T {};

template<typename T, typename... Args>
struct disjunction<T, Args...> : 
    conditional_t<bool(T::value), T, disjunction<Args...>> {};

template<typename... Args>
inline constexpr bool disjunction_v = disjunction<Args...>::value;

// 7.4 negation - logical NOT
template<typename T>
struct negation : integral_constant<bool, !bool(T::value)> {};

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

// 9.1 is_base_of - check if base class
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

template<typename T>
struct is_base_of<T, T> : integral_constant<bool, is_class_v<T>> {};

template<typename Base, typename Derived>
inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

// 9.2 is_convertible - check if types are convertible
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
} // namespace detail

template<typename From, typename To>
struct is_convertible : decltype(detail::is_convertible_test<From, To>(0)) {};

// Specialization for void conversions
template<typename From>
struct is_convertible<From, void> : true_type {};

template<>
struct is_convertible<void, void> : true_type {};

template<typename To>
struct is_convertible<void, To> : false_type {};

// Specialization for void* to pointer conversions
template<typename To>
struct is_convertible<void*, To*> : true_type {};

template<typename To>
struct is_convertible<const void*, To*> : true_type {};

template<typename To>
struct is_convertible<volatile void*, To*> : true_type {};

template<typename To>
struct is_convertible<const volatile void*, To*> : true_type {};

template<typename From, typename To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;


// ==================== 10. Helper Tools ====================

// 10.1 void_t - map a list of types to void
template<typename...>
using void_t = void;


// ==================== 11. SFINAE Support ====================

// enable_if - conditionally enable template based on boolean
template<bool B, typename T = void>
struct enable_if {};

template<typename T>
struct enable_if<true, T> {
    using type = T;
};

template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;


// ==================== 12. Construction and Assignment Traits ====================

namespace detail {
    // Helper for checking if a type can be constructed from arguments
    template<typename T, typename... Args>
    auto test_constructible(int) -> decltype(T(mystl::declval<Args>()...), true_type());
    
    template<typename T, typename... Args>
    auto test_constructible(...) -> false_type;
    
    // Helper for checking if a type can be assigned
    template<typename T, typename U>
    auto test_assignable(int) -> decltype(mystl::declval<T>() = mystl::declval<U>(), true_type());
    
    template<typename T, typename U>
    auto test_assignable(...) -> false_type;
    
    // Helper for checking if a type can be destroyed
    template<typename T>
    auto test_destructible(int) -> decltype(mystl::declval<T&>().~T(), true_type());
    
    template<typename T>
    auto test_destructible(...) -> false_type;
}

// is_constructible - check if type can be constructed from arguments
template<typename T, typename... Args>
struct is_constructible : decltype(detail::test_constructible<T, Args...>(0)) {};

template<typename T, typename... Args>
inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

// is_default_constructible - check if type can be default constructed
template<typename T>
struct is_default_constructible : is_constructible<T> {};

template<typename T>
inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;

// is_copy_constructible - check if type can be copy constructed
template<typename T>
struct is_copy_constructible : is_constructible<T, add_lvalue_reference_t<const T>> {};

template<typename T>
inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

// is_move_constructible - check if type can be move constructed
template<typename T>
struct is_move_constructible : is_constructible<T, add_rvalue_reference_t<T>> {};

template<typename T>
inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

// is_assignable - check if type can be assigned from another type
template<typename T, typename U>
struct is_assignable : decltype(detail::test_assignable<T, U>(0)) {};

template<typename T, typename U>
inline constexpr bool is_assignable_v = is_assignable<T, U>::value;

// is_copy_assignable - check if type can be copy assigned
template<typename T>
struct is_copy_assignable : is_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};

template<typename T>
inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;

// is_move_assignable - check if type can be move assigned
template<typename T>
struct is_move_assignable : is_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>> {};

template<typename T>
inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

// is_destructible - check if type can be destroyed
template<typename T>
struct is_destructible : decltype(detail::test_destructible<T>(0)) {};

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

// make_signed - convert to signed type
template<typename T>
struct make_signed {
    static_assert(is_integral_v<T> || is_enum_v<T>, "make_signed requires integral or enum type");
    using type = T; // Simplified implementation
};

// Specializations for standard integral types
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
template<typename T> struct make_signed<const T> { using type = const typename make_signed<T>::type; };
template<typename T> struct make_signed<volatile T> { using type = volatile typename make_signed<T>::type; };
template<typename T> struct make_signed<const volatile T> { using type = const volatile typename make_signed<T>::type; };

template<typename T>
using make_signed_t = typename make_signed<T>::type;

// make_unsigned - convert to unsigned type
template<typename T>
struct make_unsigned {
    static_assert(is_integral_v<T> || is_enum_v<T>, "make_unsigned requires integral or enum type");
    using type = T; // Simplified implementation
};

// Specializations for standard integral types
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
template<typename T> struct make_unsigned<const T> { using type = const typename make_unsigned<T>::type; };
template<typename T> struct make_unsigned<volatile T> { using type = volatile typename make_unsigned<T>::type; };
template<typename T> struct make_unsigned<const volatile T> { using type = const volatile typename make_unsigned<T>::type; };

template<typename T>
using make_unsigned_t = typename make_unsigned<T>::type;

// common_type - find common type for a set of types
template<typename...>
struct common_type {};

template<typename T>
struct common_type<T> {
    using type = decay_t<T>;
};

template<typename T, typename U>
struct common_type<T, U> {
private:
    using decayed_T = decay_t<T>;
    using decayed_U = decay_t<U>;
    using cond_type = decltype(true ? mystl::declval<decayed_T>() : mystl::declval<decayed_U>());
public:
    using type = decay_t<cond_type>;
};

template<typename T, typename U, typename... V>
struct common_type<T, U, V...> {
    using type = typename common_type<typename common_type<T, U>::type, V...>::type;
};

template<typename... T>
using common_type_t = typename common_type<T...>::type;

// underlying_type - get underlying type of enum
template<typename T>
struct underlying_type {
    static_assert(is_enum_v<T>, "underlying_type requires enum type");
    
    // Use compiler intrinsic when available
    #ifdef __GNUC__
        using type = __underlying_type(T);
    #elif defined(_MSC_VER)
        using type = __underlying_type(T);
    #else
        // Fallback: try to deduce from enum values (simplified)
        // This is not fully correct but provides a placeholder
        using type = int;
    #endif
};

template<typename T>
using underlying_type_t = typename underlying_type<T>::type;


} // namespace mystl
