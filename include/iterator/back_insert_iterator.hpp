#pragma once

/**
 * @file back_insert_iterator.hpp
 * @brief Output iterator that appends to the back of a container
 * @brief 输出迭代器，用于向容器尾部追加元素
 * 
 * This file implements the back_insert_iterator class and back_inserter helper function.
 * back_insert_iterator is an output iterator that appends elements to the end of a container
 * using the container's push_back() method. It's commonly used with algorithms like std::copy
 * to insert elements at the end of a container.
 * 
 * 本文件实现了 back_insert_iterator 类和 back_inserter 辅助函数。
 * back_insert_iterator 是一个输出迭代器，使用容器的 push_back() 方法将元素追加到容器末尾。
 * 它通常与 std::copy 等算法一起使用，将元素插入到容器末尾。
 * 
 * @author mySTL shensjt
 * @date 2026-03-19
 * 
 * @see front_insert_iterator.hpp
 * @see insert_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/back_insert_iterator
 */

#include "iterator_tags.hpp"
#include "../utility/move.hpp"

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

/**
 * @brief Output iterator that inserts elements at the end of a container
 * @brief 在容器末尾插入元素的输出迭代器
 * 
 * @tparam Container Container type that supports push_back() method
 * @tparam Container 支持 push_back() 方法的容器类型
 * 
 * @details The back_insert_iterator is an output iterator that appends elements to the
 * end of a container using the container's push_back() method. When assigned a value
 * through the iterator, the value is appended to the container.
 * 
 * @details back_insert_iterator 是一个输出迭代器，使用容器的 push_back() 方法将元素追加到容器末尾。
 * 通过迭代器赋值时，值会被追加到容器中。
 * 
 * @note This iterator category is output_iterator_tag, meaning it only supports
 *       writing operations, not reading.
 * @note 此迭代器类别为 output_iterator_tag，意味着它只支持写操作，不支持读操作。
 * 
 * @note The increment and dereference operators are no-ops that return *this,
 *       allowing the iterator to be used in standard algorithms.
 * @note 递增和解引用运算符是无操作，返回 *this，允许迭代器在标准算法中使用。
 * 
 * @see back_inserter()
 */
template<typename Container>
class back_insert_iterator {
protected:
    Container* container_;  ///< Pointer to the container / 指向容器的指针

public:
    using container_type = Container;          ///< Container type / 容器类型
    using iterator_category = output_iterator_tag;  ///< Iterator category / 迭代器类别
    using value_type = void;                   ///< Value type (void for output iterators) / 值类型（输出迭代器为void）
    using difference_type = void;              ///< Difference type (void for output iterators) / 差值类型（输出迭代器为void）
    using pointer = void;                      ///< Pointer type (void for output iterators) / 指针类型（输出迭代器为void）
    using reference = void;                    ///< Reference type (void for output iterators) / 引用类型（输出迭代器为void）

    /**
     * @brief Construct a back_insert_iterator for the given container
     * @brief 为给定容器构造 back_insert_iterator
     * 
     * @param x Reference to the container to insert into
     * @param x 要插入的容器的引用
     * 
     * @details Stores a pointer to the container for later use in assignment operations.
     * 
     * @details 存储指向容器的指针，以便在后续的赋值操作中使用。
     */
    explicit back_insert_iterator(Container& x)
        : container_(&x) {}

    /**
     * @brief Copy-assign a value to the container (appends to the end)
     * @brief 复制赋值一个值到容器（追加到末尾）
     * 
     * @param value Value to append to the container
     * @param value 要追加到容器的值
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Appends the value to the end of the container using push_back().
     * This is the primary operation of the back_insert_iterator.
     * 
     * @details 使用 push_back() 将值追加到容器末尾。
     * 这是 back_insert_iterator 的主要操作。
     */
    back_insert_iterator& operator=(const typename Container::value_type& value) {
        container_->push_back(value);
        return *this;
    }

    /**
     * @brief Move-assign a value to the container (appends to the end)
     * @brief 移动赋值一个值到容器（追加到末尾）
     * 
     * @param value Value to move-append to the container
     * @param value 要移动追加到容器的值
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Move-appends the value to the end of the container using push_back()
     * with move semantics. This avoids unnecessary copies when the value is an rvalue.
     * 
     * @details 使用移动语义通过 push_back() 将值移动追加到容器末尾。
     * 当值是右值时，这避免了不必要的拷贝。
     */
    back_insert_iterator& operator=(typename Container::value_type&& value) {
        container_->push_back(mystl::move(value));
        return *this;
    }

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
    back_insert_iterator& operator*() {
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
    back_insert_iterator& operator++() {
        return *this;
    }

    /**
     * @brief Postfix increment operator (no-op for output iterators)
     * @brief 后缀递增运算符（输出迭代器的无操作）
     * 
     * @return Copy of this iterator
     * @return 此迭代器的副本
     * 
     * @details For output iterators, the postfix increment operator is a no-op that returns
     * a copy of *this. This allows the iterator to be used in algorithms that expect incrementable iterators.
     * 
     * @details 对于输出迭代器，后缀递增运算符是无操作，返回 *this 的副本。
     * 这允许迭代器在需要可递增迭代器的算法中使用。
     */
    back_insert_iterator operator++(int) {
        return *this;
    }
};

/**
 * @brief Convenience function to create a back_insert_iterator
 * @brief 创建 back_insert_iterator 的便捷函数
 * 
 * @tparam Container Container type that supports push_back() method
 * @tparam Container 支持 push_back() 方法的容器类型
 * @param x Reference to the container to insert into
 * @param x 要插入的容器的引用
 * @return back_insert_iterator<Container> for the given container
 * @return 用于给定容器的 back_insert_iterator<Container>
 * 
 * @details Creates a back_insert_iterator for the specified container.
 * This helper function enables cleaner syntax: mystl::back_inserter(container)
 * instead of mystl::back_insert_iterator<Container>(container).
 * 
 * @details 为指定容器创建 back_insert_iterator。
 * 此辅助函数支持更简洁的语法：mystl::back_inserter(container)
 * 而不是 mystl::back_insert_iterator<Container>(container)。
 * 
 * @note Commonly used with standard algorithms:
 *       std::copy(source.begin(), source.end(), mystl::back_inserter(dest));
 * @note 通常与标准算法一起使用：
 *       std::copy(source.begin(), source.end(), mystl::back_inserter(dest));
 */
template<typename Container>
back_insert_iterator<Container> back_inserter(Container& x) {
    return back_insert_iterator<Container>(x);
}

} // namespace mystl
