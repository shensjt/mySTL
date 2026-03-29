#pragma once

/**
 * @file less.hpp
 * @brief Function object for performing comparisons (less than)
 * @brief 用于执行比较的函数对象（小于）
 * 
 * This header provides a function object that invokes operator< on its arguments.
 * It's used as the default comparison function for ordered containers.
 * 
 * 本头文件提供一个函数对象，在其参数上调用operator<。
 * 用作有序容器的默认比较函数。
 * 
 * @author shensjt
 * @date 2026-03-29
 */

namespace mystl {

/**
 * @brief Function object for less-than comparison
 * @brief 小于比较的函数对象
 * 
 * A binary predicate that returns true if the first argument is less than the second.
 * 
 * 一个二元谓词，如果第一个参数小于第二个参数则返回true。
 * 
 * @tparam T Type of the arguments to compare
 * @tparam T 要比较的参数类型
 */
template<typename T>
struct less {
    /**
     * @brief Function call operator
     * @brief 函数调用运算符
     * 
     * @param lhs Left-hand side argument
     * @param lhs 左侧参数
     * @param rhs Right-hand side argument
     * @param rhs 右侧参数
     * @return true if lhs < rhs, false otherwise
     * @return 如果lhs < rhs返回true，否则返回false
     */
    constexpr bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};

/**
 * @brief Function object for greater-than comparison
 * @brief 大于比较的函数对象
 * 
 * A binary predicate that returns true if the first argument is greater than the second.
 * 
 * 一个二元谓词，如果第一个参数大于第二个参数则返回true。
 * 
 * @tparam T Type of the arguments to compare
 * @tparam T 要比较的参数类型
 */
template<typename T>
struct greater {
    /**
     * @brief Function call operator
     * @brief 函数调用运算符
     * 
     * @param lhs Left-hand side argument
     * @param lhs 左侧参数
     * @param rhs Right-hand side argument
     * @param rhs 右侧参数
     * @return true if lhs > rhs, false otherwise
     * @return 如果lhs > rhs返回true，否则返回false
     */
    constexpr bool operator()(const T& lhs, const T& rhs) const {
        return lhs > rhs;
    }
};

/**
 * @brief Function object for less-than-or-equal comparison
 * @brief 小于等于比较的函数对象
 * 
 * A binary predicate that returns true if the first argument is less than or equal to the second.
 * 
 * 一个二元谓词，如果第一个参数小于等于第二个参数则返回true。
 * 
 * @tparam T Type of the arguments to compare
 * @tparam T 要比较的参数类型
 */
template<typename T>
struct less_equal {
    /**
     * @brief Function call operator
     * @brief 函数调用运算符
     * 
     * @param lhs Left-hand side argument
     * @param lhs 左侧参数
     * @param rhs Right-hand side argument
     * @param rhs 右侧参数
     * @return true if lhs <= rhs, false otherwise
     * @return 如果lhs <= rhs返回true，否则返回false
     */
    constexpr bool operator()(const T& lhs, const T& rhs) const {
        return lhs <= rhs;
    }
};

/**
 * @brief Function object for greater-than-or-equal comparison
 * @brief 大于等于比较的函数对象
 * 
 * A binary predicate that returns true if the first argument is greater than or equal to the second.
 * 
 * 一个二元谓词，如果第一个参数大于等于第二个参数则返回true。
 * 
 * @tparam T Type of the arguments to compare
 * @tparam T 要比较的参数类型
 */
template<typename T>
struct greater_equal {
    /**
     * @brief Function call operator
     * @brief 函数调用运算符
     * 
     * @param lhs Left-hand side argument
     * @param lhs 左侧参数
     * @param rhs Right-hand side argument
     * @param rhs 右侧参数
     * @return true if lhs >= rhs, false otherwise
     * @return 如果lhs >= rhs返回true，否则返回false
     */
    constexpr bool operator()(const T& lhs, const T& rhs) const {
        return lhs >= rhs;
    }
};

/**
 * @brief Function object for equality comparison
 * @brief 相等比较的函数对象
 * 
 * A binary predicate that returns true if the first argument is equal to the second.
 * 
 * 一个二元谓词，如果第一个参数等于第二个参数则返回true。
 * 
 * @tparam T Type of the arguments to compare
 * @tparam T 要比较的参数类型
 */
template<typename T>
struct equal_to {
    /**
     * @brief Function call operator
     * @brief 函数调用运算符
     * 
     * @param lhs Left-hand side argument
     * @param lhs 左侧参数
     * @param rhs Right-hand side argument
     * @param rhs 右侧参数
     * @return true if lhs == rhs, false otherwise
     * @return 如果lhs == rhs返回true，否则返回false
     */
    constexpr bool operator()(const T& lhs, const T& rhs) const {
        return lhs == rhs;
    }
};

/**
 * @brief Function object for inequality comparison
 * @brief 不等比较的函数对象
 * 
 * A binary predicate that returns true if the first argument is not equal to the second.
 * 
 * 一个二元谓词，如果第一个参数不等于第二个参数则返回true。
 * 
 * @tparam T Type of the arguments to compare
 * @tparam T 要比较的参数类型
 */
template<typename T>
struct not_equal_to {
    /**
     * @brief Function call operator
     * @brief 函数调用运算符
     * 
     * @param lhs Left-hand side argument
     * @param lhs 左侧参数
     * @param rhs Right-hand side argument
     * @param rhs 右侧参数
     * @return true if lhs != rhs, false otherwise
     * @return 如果lhs != rhs返回true，否则返回false
     */
    constexpr bool operator()(const T& lhs, const T& rhs) const {
        return lhs != rhs;
    }
};

} // namespace mystl
