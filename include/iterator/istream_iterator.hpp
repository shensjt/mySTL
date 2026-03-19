#pragma once

/**
 * @file istream_iterator.hpp
 * @brief Input iterator that reads values from an input stream
 * @brief 输入迭代器，用于从输入流读取值
 * 
 * This file implements the istream_iterator class and istream_iterator_sentinel class.
 * istream_iterator is an input iterator that reads successive objects of type T
 * from a std::basic_istream using operator>>. It's commonly used with algorithms
 * like std::copy to read data from input streams.
 * 
 * 本文件实现了 istream_iterator 类和 istream_iterator_sentinel 类。
 * istream_iterator 是一个输入迭代器，使用 operator>> 从 std::basic_istream 读取连续的 T 类型对象。
 * 它通常与 std::copy 等算法一起使用，从输入流读取数据。
 * 
 * @author mySTL shensjt
 * @date 2026-03-19
 * 
 * @see ostream_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/istream_iterator
 */

#include "iterator_tags.hpp"
#include <iostream>
#include <cstddef>

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

// Forward declarations / 前向声明
template<typename T, typename CharT, typename Traits>
class istream_iterator_sentinel;

/**
 * @brief Input iterator that reads values from an input stream
 * @brief 从输入流读取值的输入迭代器
 * 
 * @tparam T Type of values to read from the stream
 * @tparam T 从流中读取的值的类型
 * @tparam CharT Character type (default: char)
 * @tparam CharT 字符类型（默认：char）
 * @tparam Traits Character traits type (default: std::char_traits<CharT>)
 * @tparam Traits 字符特性类型（默认：std::char_traits<CharT>）
 * 
 * @details The istream_iterator is an input iterator that reads successive objects
 * of type T from a std::basic_istream using operator>>. When constructed with a stream,
 * it immediately reads the first value. Subsequent increments read additional values.
 * 
 * @details istream_iterator 是一个输入迭代器，使用 operator>> 从 std::basic_istream 读取连续的 T 类型对象。
 * 当使用流构造时，它会立即读取第一个值。后续的递增操作会读取额外的值。
 * 
 * @note This iterator category is input_iterator_tag, meaning it's a single-pass iterator
 *       that can only be read from and incremented.
 * @note 此迭代器类别为 input_iterator_tag，意味着它是一个单遍迭代器，只能读取和递增。
 * 
 * @note The default-constructed istream_iterator serves as the end-of-stream iterator.
 * @note 默认构造的 istream_iterator 用作流结束迭代器。
 * 
 * @note This iterator supports C++20 sentinel comparisons with istream_iterator_sentinel.
 * @note 此迭代器支持与 istream_iterator_sentinel 的 C++20 哨兵比较。
 * 
 * @see istream_iterator_sentinel
 */
template<typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
class istream_iterator {
private:
    std::basic_istream<CharT, Traits>* stream_;  ///< Pointer to the input stream / 指向输入流的指针
    T value_;                                     ///< Current value read from stream / 从流中读取的当前值
    bool valid_;                                  ///< Whether current value is valid / 当前值是否有效

    /**
     * @brief Read the next value from the stream
     * @brief 从流中读取下一个值
     * 
     * @details Attempts to read a value from the stream using operator>>.
     * If successful, sets valid_ to true and stores the value in value_.
     * If failed (end-of-stream or error), sets valid_ to false and stream_ to nullptr.
     * 
     * @details 尝试使用 operator>> 从流中读取一个值。
     * 如果成功，将 valid_ 设置为 true 并将值存储在 value_ 中。
     * 如果失败（流结束或错误），将 valid_ 设置为 false 并将 stream_ 设置为 nullptr。
     */
    void read() {
        if (stream_ && (*stream_ >> value_)) {
            valid_ = true;
        } else {
            valid_ = false;
            stream_ = nullptr;
        }
    }

public:
    using iterator_category = input_iterator_tag;  ///< Iterator category / 迭代器类别
    using value_type = T;                          ///< Type of values read / 读取的值的类型
    using difference_type = std::ptrdiff_t;        ///< Difference type / 差值类型
    using pointer = const T*;                      ///< Pointer type (const pointer) / 指针类型（常量指针）
    using reference = const T&;                    ///< Reference type (const reference) / 引用类型（常量引用）
    using char_type = CharT;                       ///< Character type / 字符类型
    using traits_type = Traits;                    ///< Character traits type / 字符特性类型
    using istream_type = std::basic_istream<CharT, Traits>;  ///< Input stream type / 输入流类型

    /**
     * @brief Construct an end-of-stream iterator
     * @brief 构造流结束迭代器
     * 
     * @details Creates an istream_iterator that represents the end-of-stream.
     * This iterator compares equal to other end-of-stream iterators.
     * 
     * @details 创建一个表示流结束的 istream_iterator。
     * 此迭代器与其他流结束迭代器相等比较。
     */
    istream_iterator() : stream_(nullptr), valid_(false) {}

    /**
     * @brief Construct an istream_iterator for the given stream
     * @brief 为给定流构造 istream_iterator
     * 
     * @param stream Input stream to read from
     * @param stream 要读取的输入流
     * 
     * @details Creates an istream_iterator that reads from the specified stream.
     * Immediately reads the first value from the stream.
     * 
     * @details 创建一个从指定流读取的 istream_iterator。
     * 立即从流中读取第一个值。
     */
    istream_iterator(istream_type& stream) : stream_(&stream), valid_(false) {
        read();
    }

    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other istream_iterator to copy from
     * @param other 要拷贝的 istream_iterator
     */
    istream_iterator(const istream_iterator& other)
        : stream_(other.stream_), value_(other.value_), valid_(other.valid_) {}

    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值运算符
     * 
     * @param other istream_iterator to copy from
     * @param other 要拷贝的 istream_iterator
     * @return Reference to this iterator
     * @return 此迭代器的引用
     */
    istream_iterator& operator=(const istream_iterator& other) {
        if (this != &other) {
            stream_ = other.stream_;
            value_ = other.value_;
            valid_ = other.valid_;
        }
        return *this;
    }

    /// @brief Destructor / 析构函数
    ~istream_iterator() = default;

    /**
     * @brief Dereference operator
     * @brief 解引用运算符
     * 
     * @return Const reference to the current value
     * @return 当前值的常量引用
     * 
     * @details Returns a const reference to the current value read from the stream.
     * 
     * @details 返回到从流中读取的当前值的常量引用。
     */
    const T& operator*() const {
        return value_;
    }

    /**
     * @brief Member access operator
     * @brief 成员访问运算符
     * 
     * @return Const pointer to the current value
     * @return 当前值的常量指针
     * 
     * @details Returns a const pointer to the current value read from the stream.
     * 
     * @details 返回到从流中读取的当前值的常量指针。
     */
    const T* operator->() const {
        return &value_;
    }

    /**
     * @brief Prefix increment operator
     * @brief 前缀递增运算符
     * 
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Reads the next value from the stream.
     * If the stream reaches end-of-stream or encounters an error,
     * the iterator becomes an end-of-stream iterator.
     * 
     * @details 从流中读取下一个值。
     * 如果流到达结束或遇到错误，迭代器变为流结束迭代器。
     */
    istream_iterator& operator++() {
        read();
        return *this;
    }

    /**
     * @brief Postfix increment operator
     * @brief 后缀递增运算符
     * 
     * @return Copy of the iterator before incrementing
     * @return 递增前的迭代器副本
     * 
     * @details Reads the next value from the stream and returns a copy of
     * the iterator before the increment.
     * 
     * @details 从流中读取下一个值，并返回递增前的迭代器副本。
     */
    istream_iterator operator++(int) {
        istream_iterator tmp = *this;
        read();
        return tmp;
    }

    /**
     * @brief Equality comparison operator
     * @brief 相等比较运算符
     * 
     * @param lhs Left-hand side iterator
     * @param lhs 左侧迭代器
     * @param rhs Right-hand side iterator
     * @param rhs 右侧迭代器
     * @return true if iterators are equal, false otherwise
     * @return 如果迭代器相等则为 true，否则为 false
     * 
     * @details Two istream_iterators are equal if:
     * 1. Both are end-of-stream iterators (stream_ == nullptr), OR
     * 2. Both point to the same stream and have the same validity state.
     * 
     * @details 两个 istream_iterator 在以下情况下相等：
     * 1. 两者都是流结束迭代器（stream_ == nullptr），或者
     * 2. 两者指向相同的流并且具有相同的有效性状态。
     */
    friend bool operator==(const istream_iterator& lhs, const istream_iterator& rhs) {
        // Both are end iterators or both are valid and point to same stream
        // 两者都是结束迭代器，或者两者都有效且指向相同的流
        return (lhs.stream_ == nullptr && rhs.stream_ == nullptr) ||
               (lhs.stream_ == rhs.stream_ && lhs.valid_ == rhs.valid_);
    }

    /**
     * @brief Inequality comparison operator
     * @brief 不等比较运算符
     * 
     * @param lhs Left-hand side iterator
     * @param lhs 左侧迭代器
     * @param rhs Right-hand side iterator
     * @param rhs 右侧迭代器
     * @return true if iterators are not equal, false otherwise
     * @return 如果迭代器不相等则为 true，否则为 false
     */
    friend bool operator!=(const istream_iterator& lhs, const istream_iterator& rhs) {
        return !(lhs == rhs);
    }

    // Friend declarations for sentinel comparisons / 哨兵比较的友元声明
    template<typename U, typename C, typename Tr>
    friend bool operator==(const istream_iterator<U, C, Tr>& it,
                          const istream_iterator_sentinel<U, C, Tr>&);
    
    template<typename U, typename C, typename Tr>
    friend bool operator==(const istream_iterator_sentinel<U, C, Tr>&,
                          const istream_iterator<U, C, Tr>& it);
};

/**
 * @brief Sentinel type for end-of-stream comparisons (C++20 style)
 * @brief 用于流结束比较的哨兵类型（C++20 风格）
 * 
 * @tparam T Type of values read by the iterator
 * @tparam T 迭代器读取的值的类型
 * @tparam CharT Character type (default: char)
 * @tparam CharT 字符类型（默认：char）
 * @tparam Traits Character traits type (default: std::char_traits<CharT>)
 * @tparam Traits 字符特性类型（默认：std::char_traits<CharT>）
 * 
 * @details Empty class used as a sentinel for C++20-style range-based comparisons.
 * Allows comparing istream_iterator with a sentinel to check for end-of-stream
 * without requiring a default-constructed istream_iterator.
 * 
 * @details 用作 C++20 风格基于范围的比较的哨兵的空类。
 * 允许将 istream_iterator 与哨兵进行比较以检查流结束，而不需要默认构造的 istream_iterator。
 * 
 * @note This enables cleaner syntax in C++20 range-based for loops and algorithms.
 * @note 这在 C++20 基于范围的 for 循环和算法中支持更简洁的语法。
 */
template<typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
class istream_iterator_sentinel {};

/**
 * @brief Compare istream_iterator with sentinel
 * @brief 比较 istream_iterator 与哨兵
 * 
 * @tparam T Type of values read by the iterator
 * @tparam T 迭代器读取的值的类型
 * @tparam CharT Character type
 * @tparam CharT 字符类型
 * @tparam Traits Character traits type
 * @tparam Traits 字符特性类型
 * @param it istream_iterator to compare
 * @param it 要比较的 istream_iterator
 * @param sentinel Sentinel (unused parameter)
 * @param sentinel 哨兵（未使用的参数）
 * @return true if iterator is at end-of-stream, false otherwise
 * @return 如果迭代器在流结束则为 true，否则为 false
 * 
 * @details An istream_iterator equals the sentinel when it has reached end-of-stream
 * (i.e., when valid_ is false).
 * 
 * @details 当 istream_iterator 到达流结束（即 valid_ 为 false）时，它等于哨兵。
 */
template<typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
bool operator==(const istream_iterator<T, CharT, Traits>& it,
                const istream_iterator_sentinel<T, CharT, Traits>&) {
    return !it.valid_;
}

/**
 * @brief Compare sentinel with istream_iterator (symmetric version)
 * @brief 比较哨兵与 istream_iterator（对称版本）
 * 
 * @tparam T Type of values read by the iterator
 * @tparam T 迭代器读取的值的类型
 * @tparam CharT Character type
 * @tparam CharT 字符类型
 * @tparam Traits Character traits type
 * @tparam Traits 字符特性类型
 * @param sentinel Sentinel (unused parameter)
 * @param sentinel 哨兵（未使用的参数）
 * @param it istream_iterator to compare
 * @param it 要比较的 istream_iterator
 * @return true if iterator is at end-of-stream, false otherwise
 * @return 如果迭代器在流结束则为 true，否则为 false
 */
template<typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
bool operator==(const istream_iterator_sentinel<T, CharT, Traits>&,
                const istream_iterator<T, CharT, Traits>& it) {
    return !it.valid_;
}

/**
 * @brief Inequality comparison between istream_iterator and sentinel
 * @brief istream_iterator 与哨兵的不等比较
 * 
 * @tparam T Type of values read by the iterator
 * @tparam T 迭代器读取的值的类型
 * @tparam CharT Character type
 * @tparam CharT 字符类型
 * @tparam Traits Character traits type
 * @tparam Traits 字符特性类型
 * @param it istream_iterator to compare
 * @param it 要比较的 istream_iterator
 * @param sentinel Sentinel
 * @param sentinel 哨兵
 * @return true if iterator is not at end-of-stream, false otherwise
 * @return 如果迭代器不在流结束则为 true，否则为 false
 */
template<typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
bool operator!=(const istream_iterator<T, CharT, Traits>& it,
                const istream_iterator_sentinel<T, CharT, Traits>& sentinel) {
    return !(it == sentinel);
}

/**
 * @brief Inequality comparison between sentinel and istream_iterator (symmetric version)
 * @brief 哨兵与 istream_iterator 的不等比较（对称版本）
 * 
 * @tparam T Type of values read by the iterator
 * @tparam T 迭代器读取的值的类型
 * @tparam CharT Character type
 * @tparam CharT 字符类型
 * @tparam Traits Character traits type
 * @tparam Traits 字符特性类型
 * @param sentinel Sentinel
 * @param sentinel 哨兵
 * @param it istream_iterator to compare
 * @param it 要比较的 istream_iterator
 * @return true if iterator is not at end-of-stream, false otherwise
 * @return 如果迭代器不在流结束则为 true，否则为 false
 */
template<typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
bool operator!=(const istream_iterator_sentinel<T, CharT, Traits>& sentinel,
                const istream_iterator<T, CharT, Traits>& it) {
    return !(it == sentinel);
}

} // namespace mystl
