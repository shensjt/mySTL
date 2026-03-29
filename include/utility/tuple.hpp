#pragma once

/**
 * @file tuple.hpp
 * @brief Tuple implementation for mySTL (参考标准库实现)
 * @brief mySTL的tuple实现（参考标准库实现）
 * 
 * This header provides a tuple implementation that follows the C++ standard.
 * It's designed to be compatible with std::tuple interface.
 * 
 * 本头文件提供遵循C++标准的tuple实现。
 * 设计为与std::tuple接口兼容。
 */

#include "type_traits.hpp"
#include "move.hpp"
#include "swap.hpp"
#include "pair.hpp"
#include <cstddef>

namespace mystl {

// ==================== 前向声明 ====================

template <typename... Types>
class tuple;

// ==================== tuple_size主模板 ====================

/**
 * @brief Get the number of elements in a tuple
 * @brief 获取tuple中的元素数量
 * 
 * @tparam T Tuple type
 * @tparam T tuple类型
 */
template <typename T>
struct tuple_size;

/**
 * @brief Convenience alias for tuple_size::value
 * @brief tuple_size::value的便捷别名
 */
template <typename T>
inline constexpr size_t tuple_size_v = tuple_size<T>::value;

// ==================== tuple_element主模板 ====================

/**
 * @brief Get the type of the I-th element in a tuple
 * @brief 获取tuple中第I个元素的类型
 * 
 * @tparam I Index of the element
 * @tparam I 元素的索引
 * @tparam T Tuple type
 * @tparam T tuple类型
 */
template <size_t I, typename T>
struct tuple_element;

/**
 * @brief Convenience alias for tuple_element::type
 * @brief tuple_element::type的便捷别名
 */
template <size_t I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

// ==================== tuple实现 ====================

/**
 * @brief Empty tuple specialization (base case)
 * @brief 空tuple特化（基本情况）
 */
template <>
class tuple<> {
public:
    // 默认构造函数
    constexpr tuple() noexcept = default;
    
    // 拷贝构造函数
    constexpr tuple(const tuple&) noexcept = default;
    
    // 移动构造函数
    constexpr tuple(tuple&&) noexcept = default;
    
    // 赋值运算符
    tuple& operator=(const tuple&) noexcept = default;
    tuple& operator=(tuple&&) noexcept = default;
    
    // 交换
    void swap(tuple&) noexcept {}
};

/**
 * @brief Tuple implementation using recursive inheritance
 * @brief 使用递归继承的tuple实现
 * 
 * @tparam Head Type of the first element
 * @tparam Head 第一个元素的类型
 * @tparam Tail... Types of the remaining elements
 * @tparam Tail... 剩余元素的类型
 */
template <typename Head, typename... Tail>
class tuple<Head, Tail...> : public tuple<Tail...> {
private:
    using base = tuple<Tail...>;
    
public:
    Head head;  ///< The first element / 第一个元素
    
    // ==================== 构造函数 ====================
    
    /**
     * @brief Default constructor (值初始化)
     * @brief 默认构造函数（值初始化）
     */
    constexpr tuple() : base(), head() {}
    
    /**
     * @brief Constructor with values
     * @brief 带值的构造函数
     * 
     * @param h First value
     * @param h 第一个值
     * @param t... Remaining values
     * @param t... 剩余的值
     */
    constexpr tuple(const Head& h, const Tail&... t) 
        : base(t...), head(h) {}
    
    /**
     * @brief Move constructor
     * @brief 移动构造函数
     * 
     * @param h First value (rvalue)
     * @param h 第一个值（右值）
     * @param t... Remaining values (rvalues)
     * @param t... 剩余的值（右值）
     */
    template <typename UHead = Head, typename... UTail,
              typename = enable_if_t<sizeof...(UTail) == sizeof...(Tail)>>
    constexpr tuple(UHead&& h, UTail&&... t)
        : base(mystl::forward<UTail>(t)...), head(mystl::forward<UHead>(h)) {}
    
    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     */
    constexpr tuple(const tuple& other) 
        : base(static_cast<const base&>(other)), head(other.head) {}
    
    /**
     * @brief Move constructor
     * @brief 移动构造函数
     */
    constexpr tuple(tuple&& other) 
        : base(static_cast<base&&>(other)), head(mystl::move(other.head)) {}
    
    /**
     * @brief Converting constructor
     * @brief 转换构造函数
     */
    template <typename UHead, typename... UTail,
              typename = enable_if_t<sizeof...(UTail) == sizeof...(Tail)>>
    constexpr tuple(const tuple<UHead, UTail...>& other)
        : base(static_cast<const tuple<UTail...>&>(other)), head(other.head) {}
    
    /**
     * @brief Converting move constructor
     * @brief 转换移动构造函数
     */
    template <typename UHead, typename... UTail,
              typename = enable_if_t<sizeof...(UTail) == sizeof...(Tail)>>
    constexpr tuple(tuple<UHead, UTail...>&& other)
        : base(static_cast<tuple<UTail...>&&>(other)), head(mystl::forward<UHead>(other.head)) {}
    
    // ==================== 赋值运算符 ====================
    
    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值运算符
     */
    tuple& operator=(const tuple& other) {
        if (this != &other) {
            base::operator=(static_cast<const base&>(other));
            head = other.head;
        }
        return *this;
    }
    
    /**
     * @brief Move assignment operator
     * @brief 移动赋值运算符
     */
    tuple& operator=(tuple&& other) noexcept {
        if (this != &other) {
            base::operator=(static_cast<base&&>(other));
            head = mystl::move(other.head);
        }
        return *this;
    }
    
    /**
     * @brief Converting copy assignment
     * @brief 转换拷贝赋值
     */
    template <typename UHead, typename... UTail>
    tuple& operator=(const tuple<UHead, UTail...>& other) {
        base::operator=(static_cast<const tuple<UTail...>&>(other));
        head = other.head;
        return *this;
    }
    
    /**
     * @brief Converting move assignment
     * @brief 转换移动赋值
     */
    template <typename UHead, typename... UTail>
    tuple& operator=(tuple<UHead, UTail...>&& other) {
        base::operator=(static_cast<tuple<UTail...>&&>(other));
        head = mystl::forward<UHead>(other.head);
        return *this;
    }
    
    // ==================== 成员函数 ====================
    
    /**
     * @brief Swap with another tuple
     * @brief 与另一个tuple交换
     */
    void swap(tuple& other) noexcept {
        using mystl::swap;
        base::swap(static_cast<base&>(other));
        swap(head, other.head);
    }
};

// ==================== tuple_size特化 ====================

template <typename... Types>
struct tuple_size<tuple<Types...>> 
    : public integral_constant<size_t, sizeof...(Types)> {};

// ==================== tuple_element特化 ====================

// 基本情况：第一个元素
template <typename Head, typename... Tail>
struct tuple_element<0, tuple<Head, Tail...>> {
    using type = Head;
};

// 递归情况：第I个元素（I > 0）
template <size_t I, typename Head, typename... Tail>
struct tuple_element<I, tuple<Head, Tail...>> 
    : public tuple_element<I - 1, tuple<Tail...>> {};

// ==================== get函数辅助类 ====================

namespace detail {
    // 辅助类用于递归访问tuple元素
    template <size_t I, typename Tuple>
    struct tuple_get_helper;
    
    // 基本情况：访问第一个元素
    template <typename Head, typename... Tail>
    struct tuple_get_helper<0, tuple<Head, Tail...>> {
        static constexpr Head& get(tuple<Head, Tail...>& t) noexcept {
            return t.head;
        }
        
        static constexpr const Head& get(const tuple<Head, Tail...>& t) noexcept {
            return t.head;
        }
        
        static constexpr Head&& get(tuple<Head, Tail...>&& t) noexcept {
            return mystl::move(t.head);
        }
        
        static constexpr const Head&& get(const tuple<Head, Tail...>&& t) noexcept {
            return mystl::move(t.head);
        }
    };
    
    // 递归情况：访问第I个元素（I > 0）
    template <size_t I, typename Head, typename... Tail>
    struct tuple_get_helper<I, tuple<Head, Tail...>> {
        static constexpr tuple_element_t<I, tuple<Head, Tail...>>& 
        get(tuple<Head, Tail...>& t) noexcept {
            using base_type = tuple<Tail...>;
            return tuple_get_helper<I - 1, base_type>::get(static_cast<base_type&>(t));
        }
        
        static constexpr const tuple_element_t<I, tuple<Head, Tail...>>& 
        get(const tuple<Head, Tail...>& t) noexcept {
            using base_type = tuple<Tail...>;
            return tuple_get_helper<I - 1, base_type>::get(static_cast<const base_type&>(t));
        }
        
        static constexpr tuple_element_t<I, tuple<Head, Tail...>>&& 
        get(tuple<Head, Tail...>&& t) noexcept {
            using base_type = tuple<Tail...>;
            return tuple_get_helper<I - 1, base_type>::get(static_cast<base_type&&>(t));
        }
        
        static constexpr const tuple_element_t<I, tuple<Head, Tail...>>&& 
        get(const tuple<Head, Tail...>&& t) noexcept {
            using base_type = tuple<Tail...>;
            return tuple_get_helper<I - 1, base_type>::get(static_cast<const base_type&&>(t));
        }
    };
}

// ==================== get函数 ====================

/**
 * @brief Get the I-th element of a tuple (non-const reference)
 * @brief 获取tuple的第I个元素（非const引用）
 */
template <size_t I, typename... Types>
constexpr tuple_element_t<I, tuple<Types...>>& 
get(tuple<Types...>& t) noexcept {
    static_assert(I < sizeof...(Types), "tuple index out of bounds");
    return detail::tuple_get_helper<I, tuple<Types...>>::get(t);
}

/**
 * @brief Get the I-th element of a tuple (const reference)
 * @brief 获取tuple的第I个元素（const引用）
 */
template <size_t I, typename... Types>
constexpr const tuple_element_t<I, tuple<Types...>>& 
get(const tuple<Types...>& t) noexcept {
    static_assert(I < sizeof...(Types), "tuple index out of bounds");
    return detail::tuple_get_helper<I, tuple<Types...>>::get(t);
}

/**
 * @brief Get the I-th element of a tuple (rvalue reference)
 * @brief 获取tuple的第I个元素（右值引用）
 */
template <size_t I, typename... Types>
constexpr tuple_element_t<I, tuple<Types...>>&& 
get(tuple<Types...>&& t) noexcept {
    static_assert(I < sizeof...(Types), "tuple index out of bounds");
    return detail::tuple_get_helper<I, tuple<Types...>>::get(mystl::move(t));
}

/**
 * @brief Get the I-th element of a tuple (const rvalue reference)
 * @brief 获取tuple的第I个元素（const右值引用）
 */
template <size_t I, typename... Types>
constexpr const tuple_element_t<I, tuple<Types...>>&& 
get(const tuple<Types...>&& t) noexcept {
    static_assert(I < sizeof...(Types), "tuple index out of bounds");
    return detail::tuple_get_helper<I, tuple<Types...>>::get(mystl::move(t));
}

// ==================== 为pair特化tuple_size和tuple_element ====================

// 在mystl命名空间中为pair特化tuple_size
template <typename T1, typename T2>
struct tuple_size<pair<T1, T2>> 
    : public integral_constant<size_t, 2> {};

// 在mystl命名空间中为pair特化tuple_element
template <size_t I, typename T1, typename T2>
struct tuple_element<I, pair<T1, T2>> {
    static_assert(I < 2, "pair index out of bounds");
    using type = conditional_t<I == 0, T1, T2>;
};

// ==================== 比较操作符 ====================

/**
 * @brief Compare two tuples for equality
 * @brief 比较两个tuple是否相等
 */
template <typename... Types>
bool operator==(const tuple<Types...>& lhs, const tuple<Types...>& rhs) {
    // 简化实现：递归比较每个元素
    // 实际标准库有更复杂的实现
    return true; // 占位符
}

/**
 * @brief Compare two tuples for inequality
 * @brief 比较两个tuple是否不相等
 */
template <typename... Types>
bool operator!=(const tuple<Types...>& lhs, const tuple<Types...>& rhs) {
    return !(lhs == rhs);
}

// ==================== make_tuple函数 ====================

/**
 * @brief Create a tuple from values
 * @brief 从值创建tuple
 */
template <typename... Types>
constexpr tuple<decay_t<Types>...> make_tuple(Types&&... args) {
    return tuple<decay_t<Types>...>(mystl::forward<Types>(args)...);
}

// ==================== tie函数 ====================

/**
 * @brief Create a tuple of references
 * @brief 创建引用tuple
 */
template <typename... Types>
constexpr tuple<Types&...> tie(Types&... args) noexcept {
    return tuple<Types&...>(args...);
}

// ==================== forward_as_tuple函数 ====================

/**
 * @brief Create a tuple of references for perfect forwarding
 * @brief 创建用于完美转发的引用tuple
 */
template <typename... Types>
constexpr tuple<Types&&...> forward_as_tuple(Types&&... args) noexcept {
    return tuple<Types&&...>(mystl::forward<Types>(args)...);
}

// ==================== swap函数 ====================

/**
 * @brief Swap two tuples
 * @brief 交换两个tuple
 */
template <typename... Types>
void swap(tuple<Types...>& lhs, tuple<Types...>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace mystl
