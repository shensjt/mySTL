#pragma once

/**
 * @file ostream_iterator.hpp
 * @brief Output iterator that writes values to an output stream
 * @brief 输出迭代器，用于将值写入输出流
 * 
 * This file implements the ostream_iterator class.
 * ostream_iterator is an output iterator that writes successive objects of type T
 * to a std::basic_ostream using operator<<. It's commonly used with algorithms
 * like std::copy to write data to output streams.
 * 
 * 本文件实现了 ostream_iterator 类。
 * ostream_iterator 是一个输出迭代器，使用 operator<< 将连续的 T 类型对象写入 std::basic_ostream。
 * 它通常与 std::copy 等算法一起使用，将数据写入输出流。
 * 
 * @author mySTL shensjt
 * @date 2026-03-19
 * 
 * @see istream_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/ostream_iterator
 */

#include "iterator_tags.hpp"
#include <iostream>

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

/**
 * @brief Output iterator that writes values to an output stream
 * @brief 将值写入输出流的输出迭代器
 * 
 * @tparam T Type of values to write to the stream
 * @tparam T 要写入流的值的类型
 * @tparam CharT Character type (default: char)
 * @tparam CharT 字符类型（默认：char）
 * @tparam Traits Character traits type (default: std::char_traits<CharT>)
 * @tparam Traits 字符特性类型（默认：std::char_traits<CharT>）
 * 
 * @details The ostream_iterator is an output iterator that writes successive objects
 * of type T to a std::basic_ostream using operator<<. When assigned a value through
 * the iterator, the value is written to the output stream, optionally followed by
 * a delimiter string.
 * 
 * @details ostream_iterator 是一个输出迭代器，使用 operator<< 将连续的 T 类型对象写入 std::basic_ostream。
 * 通过迭代器赋值时，值被写入输出流，可以选择后跟分隔符字符串。
 * 
 * @note This iterator category is output_iterator_tag, meaning it only supports
 *       writing operations, not reading.
 * @note 此迭代器类别为 output_iterator_tag，意味着它只支持写操作，不支持读操作。
 * 
 * @note The increment and dereference operators are no-ops that return *this,
 *       allowing the iterator to be used in standard algorithms.
 * @note 递增和解引用运算符是无操作，返回 *this，允许迭代器在标准算法中使用。
 * 
 * @note Useful for writing container contents to output streams with algorithms
 *       like std::copy.
 * @note 用于使用 std::copy 等算法将容器内容写入输出流。
 */
template<typename T, typename CharT = char, typename Traits = std::char_traits<CharT>>
class ostream_iterator {
private:
    std::basic_ostream<CharT, Traits>* stream_;  ///< Pointer to the output stream / 指向输出流的指针
    const CharT* delimiter_;                     ///< Delimiter string (nullptr if none) / 分隔符字符串（如果没有则为nullptr）

public:
    using iterator_category = output_iterator_tag;  ///< Iterator category / 迭代器类别
    using value_type = void;                   ///< Value type (void for output iterators) / 值类型（输出迭代器为void）
    using difference_type = void;              ///< Difference type (void for output iterators) / 差值类型（输出迭代器为void）
    using pointer = void;                      ///< Pointer type (void for output iterators) / 指针类型（输出迭代器为void）
    using reference = void;                    ///< Reference type (void for output iterators) / 引用类型（输出迭代器为void）
    using char_type = CharT;                   ///< Character type / 字符类型
    using traits_type = Traits;                ///< Character traits type / 字符特性类型
    using ostream_type = std::basic_ostream<CharT, Traits>;  ///< Output stream type / 输出流类型

    /**
     * @brief Construct an ostream_iterator for the given stream without delimiter
     * @brief 为给定流构造不带分隔符的 ostream_iterator
     * 
     * @param stream Output stream to write to
     * @param stream 要写入的输出流
     * 
     * @details Creates an ostream_iterator that writes to the specified stream
     * without any delimiter between values.
     * 
     * @details 创建一个写入指定流的 ostream_iterator，值之间没有任何分隔符。
     */
    ostream_iterator(ostream_type& stream)
        : stream_(&stream), delimiter_(nullptr) {}

    /**
     * @brief Construct an ostream_iterator for the given stream with delimiter
     * @brief 为给定流构造带分隔符的 ostream_iterator
     * 
     * @param stream Output stream to write to
     * @param stream 要写入的输出流
     * @param delimiter Delimiter string to write between values (null-terminated)
     * @param delimiter 值之间写入的分隔符字符串（以null结尾）
     * 
     * @details Creates an ostream_iterator that writes to the specified stream,
     * writing the delimiter string after each value (except possibly the last).
     * 
     * @details 创建一个写入指定流的 ostream_iterator，在每个值之后写入分隔符字符串（可能最后一个除外）。
     */
    ostream_iterator(ostream_type& stream, const CharT* delimiter)
        : stream_(&stream), delimiter_(delimiter) {}

    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other ostream_iterator to copy from
     * @param other 要拷贝的 ostream_iterator
     */
    ostream_iterator(const ostream_iterator& other)
        : stream_(other.stream_), delimiter_(other.delimiter_) {}

    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值运算符
     * 
     * @param other ostream_iterator to copy from
     * @param other 要拷贝的 ostream_iterator
     * @return Reference to this iterator
     * @return 此迭代器的引用
     */
    ostream_iterator& operator=(const ostream_iterator& other) {
        if (this != &other) {
            stream_ = other.stream_;
            delimiter_ = other.delimiter_;
        }
        return *this;
    }

    /// @brief Destructor / 析构函数
    ~ostream_iterator() = default;

    /**
     * @brief Dereference operator (no-op for output iterators)
     * @brief 解引用运算符（输出迭代器的无操作）
     * 
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details For output iterators, the dereference operator is a no-op that returns
     * *this. This allows syntax like *it = value in algorithms.
     * 
     * @details 对于输出迭代器，解引用运算符是无操作，返回 *this。
     * 这允许在算法中使用 *it = value 这样的语法。
     */
    ostream_iterator& operator*() {
        return *this;
    }

    /**
     * @brief Prefix increment operator (no-op for output iterators)
     * @brief 前缀递增运算符（输出迭代器的无操作）
     * 
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details For output iterators, the increment operator is a no-op that returns
     * *this. This allows the iterator to be used in algorithms that expect incrementable iterators.
     * 
     * @details 对于输出迭代器，递增运算符是无操作，返回 *this。
     * 这允许迭代器在需要可递增迭代器的算法中使用。
     */
    ostream_iterator& operator++() {
        return *this;
    }

    /**
     * @brief Postfix increment operator (no-op for output iterators)
     * @brief 后缀递增运算符（输出迭代器的无操作）
     * 
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details For output iterators, the postfix increment operator is a no-op that returns
     * *this. This allows the iterator to be used in algorithms that expect incrementable iterators.
     * 
     * @details 对于输出迭代器，后缀递增运算符是无操作，返回 *this。
     * 这允许迭代器在需要可递增迭代器的算法中使用。
     */
    ostream_iterator& operator++(int) {
        return *this;
    }

    /**
     * @brief Assignment operator that writes a value to the stream
     * @brief 将值写入流的赋值运算符
     * 
     * @param value Value to write to the stream
     * @param value 要写入流的值
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Writes the value to the output stream using operator<<.
     * If a delimiter was specified during construction, writes the delimiter
     * after the value.
     * 
     * @details 使用 operator<< 将值写入输出流。
     * 如果在构造期间指定了分隔符，则在值之后写入分隔符。
     * 
     * @note This is the primary operation of the ostream_iterator.
     * @note 这是 ostream_iterator 的主要操作。
     */
    ostream_iterator& operator=(const T& value) {
        if (stream_) {
            *stream_ << value;
            if (delimiter_) {
                *stream_ << delimiter_;
            }
        }
        return *this;
    }
};

} // namespace mystl
