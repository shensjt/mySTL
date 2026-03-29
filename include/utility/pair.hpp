#pragma once

/**
 * @file pair.hpp
 * @brief Pair implementation for mySTL (参考标准库实现)
 * @brief mySTL的pair实现（参考标准库实现）
 * 
 * This header provides a pair implementation that follows the C++ standard.
 * It's designed to be compatible with std::pair interface.
 * 
 * 本头文件提供遵循C++标准的pair实现。
 * 设计为与std::pair接口兼容。
 */

#include "type_traits.hpp"
#include "move.hpp"
#include "swap.hpp"

namespace mystl {

// 前向声明
template <typename T1, typename T2>
struct pair;

// 辅助函数声明
template <typename T1, typename T2>
pair<typename decay<T1>::type, typename decay<T2>::type> make_pair(T1&& x, T2&& y);

/**
 * @brief Pair template (参考C++标准库实现)
 * @brief Pair模板（参考C++标准库实现）
 * 
 * @tparam T1 Type of the first element
 * @tparam T1 第一个元素的类型
 * @tparam T2 Type of the second element
 * @tparam T2 第二个元素的类型
 * 
 * 参考标准：ISO/IEC 14882:2017 §23.4.2
 */
template <typename T1, typename T2>
struct pair {
    using first_type = T1;
    using second_type = T2;
    
    T1 first;   ///< The first element / 第一个元素
    T2 second;  ///< The second element / 第二个元素
    
    // ==================== 构造函数 ====================
    
    /**
     * @brief Default constructor (值初始化)
     * @brief 默认构造函数（值初始化）
     * 
     * 效果：first和second被值初始化。
     */
    constexpr pair() : first(), second() {}
    
    /**
     * @brief Constructor with values
     * @brief 带值的构造函数
     * 
     * @param a First value
     * @param a 第一个值
     * @param b Second value
     * @param b 第二个值
     * 
     * 效果：first用a初始化，second用b初始化。
     */
    constexpr pair(const T1& a, const T2& b) : first(a), second(b) {}
    
    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other Another pair to copy from
     * @param other 要拷贝的另一个pair
     * 
     * 效果：first用other.first初始化，second用other.second初始化。
     * 注意：当T1或T2是const类型时，需要显式定义拷贝构造函数。
     */
    constexpr pair(const pair& other) : first(other.first), second(other.second) {}
    
    /**
     * @brief Converting constructor
     * @brief 转换构造函数
     * 
     * @tparam U1 Type convertible to T1
     * @tparam U1 可转换为T1的类型
     * @tparam U2 Type convertible to T2
     * @tparam U2 可转换为T2的类型
     * @param other Another pair with convertible types
     * @param other 具有可转换类型的另一个pair
     * 
     * 效果：first用other.first初始化，second用other.second初始化。
     */
    template <typename U1, typename U2>
    constexpr pair(const pair<U1, U2>& other) 
        : first(other.first), second(other.second) {}
    
    /**
     * @brief Move constructor
     * @brief 移动构造函数
     * 
     * @param a First value (rvalue)
     * @param a 第一个值（右值）
     * @param b Second value (rvalue)
     * @param b 第二个值（右值）
     */
    template <typename U1 = T1, typename U2 = T2>
    constexpr pair(U1&& a, U2&& b) 
        : first(mystl::forward<U1>(a)), second(mystl::forward<U2>(b)) {}
    
    /**
     * @brief Converting move constructor
     * @brief 转换移动构造函数
     * 
     * @tparam U1 Type convertible to T1
     * @tparam U1 可转换为T1的类型
     * @tparam U2 Type convertible to T2
     * @tparam U2 可转换为T2的类型
     * @param other Another pair with convertible types (rvalue)
     * @param other 具有可转换类型的另一个pair（右值）
     */
    template <typename U1, typename U2>
    constexpr pair(pair<U1, U2>&& other) 
        : first(mystl::forward<U1>(other.first)), 
          second(mystl::forward<U2>(other.second)) {}
    
    // 注意：标准库有piecewise_construct_t构造函数
    // 这里简化实现，不提供完整版本
    
    // ==================== 赋值运算符 ====================
    
    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值运算符
     */
    pair& operator=(const pair& other) {
        if (this != &other) {
            first = other.first;
            second = other.second;
        }
        return *this;
    }
    
    /**
     * @brief Move assignment operator
     * @brief 移动赋值运算符
     */
    pair& operator=(pair&& other) noexcept {
        if (this != &other) {
            first = mystl::move(other.first);
            second = mystl::move(other.second);
        }
        return *this;
    }
    
    /**
     * @brief Converting copy assignment
     * @brief 转换拷贝赋值
     */
    template <typename U1, typename U2>
    pair& operator=(const pair<U1, U2>& other) {
        first = other.first;
        second = other.second;
        return *this;
    }
    
    /**
     * @brief Converting move assignment
     * @brief 转换移动赋值
     */
    template <typename U1, typename U2>
    pair& operator=(pair<U1, U2>&& other) {
        first = mystl::forward<U1>(other.first);
        second = mystl::forward<U2>(other.second);
        return *this;
    }
    
    // ==================== 成员函数 ====================
    
    /**
     * @brief Swap with another pair
     * @brief 与另一个pair交换
     * 
     * @param other Another pair to swap with
     * @param other 要交换的另一个pair
     */
    void swap(pair& other) noexcept {
        using mystl::swap;
        swap(first, other.first);
        swap(second, other.second);
    }
};

// ==================== 非成员函数 ====================

/**
 * @brief Create a pair from two values (参考std::make_pair)
 * @brief 从两个值创建pair（参考std::make_pair）
 * 
 * @tparam T1 Type of the first value (decayed)
 * @tparam T1 第一个值的类型（decayed）
 * @tparam T2 Type of the second value (decayed)
 * @tparam T2 第二个值的类型（decayed）
 * @param x First value
 * @param x 第一个值
 * @param y Second value
 * @param y 第二个值
 * @return A pair containing the two values
 * @return 包含两个值的pair
 * 
 * 效果：返回pair<decay_t<T1>, decay_t<T2>>(forward<T1>(x), forward<T2>(y))
 */
template <typename T1, typename T2>
inline pair<typename decay<T1>::type, typename decay<T2>::type> 
make_pair(T1&& x, T2&& y) {
    using U1 = typename decay<T1>::type;
    using U2 = typename decay<T2>::type;
    return pair<U1, U2>(mystl::forward<T1>(x), mystl::forward<T2>(y));
}

/**
 * @brief Compare two pairs for equality
 * @brief 比较两个pair是否相等
 */
template <typename T1, typename T2>
inline bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

/**
 * @brief Compare two pairs for inequality
 * @brief 比较两个pair是否不相等
 */
template <typename T1, typename T2>
inline bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief Compare two pairs (less than)
 * @brief 比较两个pair（小于）
 */
template <typename T1, typename T2>
inline bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return lhs.first < rhs.first || 
           (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

/**
 * @brief Compare two pairs (greater than)
 * @brief 比较两个pair（大于）
 */
template <typename T1, typename T2>
inline bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return rhs < lhs;
}

/**
 * @brief Compare two pairs (less than or equal)
 * @brief 比较两个pair（小于等于）
 */
template <typename T1, typename T2>
inline bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief Compare two pairs (greater than or equal)
 * @brief 比较两个pair（大于等于）
 */
template <typename T1, typename T2>
inline bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs < rhs);
}

/**
 * @brief Swap two pairs
 * @brief 交换两个pair
 */
template <typename T1, typename T2>
inline void swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs) noexcept {
    lhs.swap(rhs);
}

// ==================== 特化 ====================

// 为pair特化is_trivially_copyable等类型特性
// 这里简化处理，实际标准库有更复杂的特化

} // namespace mystl
