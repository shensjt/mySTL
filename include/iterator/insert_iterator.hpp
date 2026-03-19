#pragma once

/**
 * @file insert_iterator.hpp
 * @brief Output iterator that inserts at a specific position in a container
 * @brief 输出迭代器，用于在容器的特定位置插入元素
 * 
 * This file implements the insert_iterator class and inserter helper function.
 * insert_iterator is an output iterator that inserts elements at a specified position
 * in a container using the container's insert() method. It's commonly used with algorithms
 * like std::copy to insert elements at arbitrary positions in a container.
 * 
 * 本文件实现了 insert_iterator 类和 inserter 辅助函数。
 * insert_iterator 是一个输出迭代器，使用容器的 insert() 方法在指定位置插入元素。
 * 它通常与 std::copy 等算法一起使用，在容器的任意位置插入元素。
 * 
 * @author mySTL shensjt
 * @date 2026-03-19
 * 
 * @see back_insert_iterator.hpp
 * @see front_insert_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/insert_iterator
 */

#include "iterator_tags.hpp"
#include "../utility/move.hpp"

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

/**
 * @brief Output iterator that inserts elements at a specific position in a container
 * @brief 在容器特定位置插入元素的输出迭代器
 * 
 * @tparam Container Container type that supports insert() method
 * @tparam Container 支持 insert() 方法的容器类型
 * 
 * @details The insert_iterator is an output iterator that inserts elements at a
 * specified position in a container using the container's insert() method.
 * When assigned a value through the iterator, the value is inserted at the
 * current position, and the position is advanced to point after the inserted element.
 * 
 * @details insert_iterator 是一个输出迭代器，使用容器的 insert() 方法在指定位置插入元素。
 * 通过迭代器赋值时，值会被插入到当前位置，然后位置前进到插入元素之后。
 * 
 * @note This iterator category is output_iterator_tag, meaning it only supports
 *       writing operations, not reading.
 * @note 此迭代器类别为 output_iterator_tag，意味着它只支持写操作，不支持读操作。
 * 
 * @note The increment and dereference operators are no-ops that return *this,
 *       allowing the iterator to be used in standard algorithms.
 * @note 递增和解引用运算符是无操作，返回 *this，允许迭代器在标准算法中使用。
 * 
 * @note Unlike back_insert_iterator and front_insert_iterator, insert_iterator
 *       can insert at any position, not just the beginning or end.
 * @note 与 back_insert_iterator 和 front_insert_iterator 不同，insert_iterator
 *       可以在任何位置插入，而不仅仅是开头或结尾。
 * 
 * @see inserter()
 */
template<typename Container>
class insert_iterator {
protected:
    Container* container_;                     ///< Pointer to the container / 指向容器的指针
    typename Container::iterator iter_;        ///< Insertion position iterator / 插入位置迭代器

public:
    using container_type = Container;          ///< Container type / 容器类型
    using iterator_category = output_iterator_tag;  ///< Iterator category / 迭代器类别
    using value_type = void;                   ///< Value type (void for output iterators) / 值类型（输出迭代器为void）
    using difference_type = void;              ///< Difference type (void for output iterators) / 差值类型（输出迭代器为void）
    using pointer = void;                      ///< Pointer type (void for output iterators) / 指针类型（输出迭代器为void）
    using reference = void;                    ///< Reference type (void for output iterators) / 引用类型（输出迭代器为void）

    /**
     * @brief Construct an insert_iterator for the given container and position
     * @brief 为给定容器和位置构造 insert_iterator
     * 
     * @param x Reference to the container to insert into
     * @param x 要插入的容器的引用
     * @param i Iterator specifying the insertion position
     * @param i 指定插入位置的迭代器
     * 
     * @details Stores a pointer to the container and the insertion position iterator
     * for later use in assignment operations.
     * 
     * @details 存储指向容器的指针和插入位置迭代器，以便在后续的赋值操作中使用。
     */
    insert_iterator(Container& x, typename Container::iterator i)
        : container_(&x), iter_(i) {}

    /**
     * @brief Copy-assign a value to the container (inserts at current position)
     * @brief 复制赋值一个值到容器（在当前位置插入）
     * 
     * @param value Value to insert at the current position
     * @param value 要在当前位置插入的值
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Inserts the value at the current position using insert(), then
     * advances the position iterator to point after the inserted element.
     * This ensures subsequent insertions happen after the previously inserted element.
     * 
     * @details 使用 insert() 在当前位置插入值，然后将位置迭代器前进到插入元素之后。
     * 这确保后续插入发生在先前插入的元素之后。
     */
    insert_iterator& operator=(const typename Container::value_type& value) {
        iter_ = container_->insert(iter_, value);
        ++iter_;
        return *this;
    }

    /**
     * @brief Move-assign a value to the container (inserts at current position)
     * @brief 移动赋值一个值到容器（在当前位置插入）
     * 
     * @param value Value to move-insert at the current position
     * @param value 要移动插入到当前位置的值
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Move-inserts the value at the current position using insert()
     * with move semantics, then advances the position iterator.
     * This avoids unnecessary copies when the value is an rvalue.
     * 
     * @details 使用移动语义通过 insert() 在当前位置移动插入值，然后前进位置迭代器。
     * 当值是右值时，这避免了不必要的拷贝。
     */
    insert_iterator& operator=(typename Container::value_type&& value) {
        iter_ = container_->insert(iter_, mystl::move(value));
        ++iter_;
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
    insert_iterator& operator*() {
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
    insert_iterator& operator++() {
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
    insert_iterator operator++(int) {
        return *this;
    }
};

/**
 * @brief Convenience function to create an insert_iterator
 * @brief 创建 insert_iterator 的便捷函数
 * 
 * @tparam Container Container type that supports insert() method
 * @tparam Container 支持 insert() 方法的容器类型
 * @param x Reference to the container to insert into
 * @param x 要插入的容器的引用
 * @param i Iterator specifying the insertion position
 * @param i 指定插入位置的迭代器
 * @return insert_iterator<Container> for the given container and position
 * @return 用于给定容器和位置的 insert_iterator<Container>
 * 
 * @details Creates an insert_iterator for the specified container and position.
 * This helper function enables cleaner syntax: mystl::inserter(container, position)
 * instead of mystl::insert_iterator<Container>(container, position).
 * 
 * @details 为指定容器和位置创建 insert_iterator。
 * 此辅助函数支持更简洁的语法：mystl::inserter(container, position)
 * 而不是 mystl::insert_iterator<Container>(container, position)。
 * 
 * @note Commonly used with standard algorithms:
 *       std::copy(source.begin(), source.end(), mystl::inserter(dest, dest.begin() + 3));
 * @note 通常与标准算法一起使用：
 *       std::copy(source.begin(), source.end(), mystl::inserter(dest, dest.begin() + 3));
 * 
 * @note The insertion position is maintained internally and advanced after each insertion,
 *       so multiple insertions will appear in the original order.
 * @note 插入位置在内部维护并在每次插入后前进，因此多次插入将按原始顺序出现。
 */
template<typename Container>
insert_iterator<Container> inserter(Container& x, typename Container::iterator i) {
    return insert_iterator<Container>(x, i);
}

} // namespace mystl
