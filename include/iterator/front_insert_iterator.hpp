#pragma once

/**
 * @file front_insert_iterator.hpp
 * @brief Output iterator that inserts to the front of a container
 * @brief 输出迭代器，用于向容器头部插入元素
 * 
 * This file implements the front_insert_iterator class and front_inserter helper function.
 * front_insert_iterator is an output iterator that inserts elements at the beginning of a container
 * using the container's push_front() method. It's commonly used with algorithms like std::copy
 * to insert elements at the front of a container.
 * 
 * 本文件实现了 front_insert_iterator 类和 front_inserter 辅助函数。
 * front_insert_iterator 是一个输出迭代器，使用容器的 push_front() 方法将元素插入到容器头部。
 * 它通常与 std::copy 等算法一起使用，将元素插入到容器头部。
 * 
 * @author mySTL shensjt
 * @date 2026-03-19
 * 
 * @see back_insert_iterator.hpp
 * @see insert_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/front_insert_iterator
 */

#include "iterator_tags.hpp"
#include "../utility/move.hpp"

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

/**
 * @brief Output iterator that inserts elements at the front of a container
 * @brief 在容器头部插入元素的输出迭代器
 * 
 * @tparam Container Container type that supports push_front() method
 * @tparam Container 支持 push_front() 方法的容器类型
 * 
 * @details The front_insert_iterator is an output iterator that inserts elements at the
 * beginning of a container using the container's push_front() method. When assigned a value
 * through the iterator, the value is inserted at the front of the container.
 * 
 * @details front_insert_iterator 是一个输出迭代器，使用容器的 push_front() 方法将元素插入到容器头部。
 * 通过迭代器赋值时，值会被插入到容器头部。
 * 
 * @note This iterator category is output_iterator_tag, meaning it only supports
 *       writing operations, not reading.
 * @note 此迭代器类别为 output_iterator_tag，意味着它只支持写操作，不支持读操作。
 * 
 * @note The increment and dereference operators are no-ops that return *this,
 *       allowing the iterator to be used in standard algorithms.
 * @note 递增和解引用运算符是无操作，返回 *this，允许迭代器在标准算法中使用。
 * 
 * @note Unlike back_insert_iterator, front_insert_iterator requires containers
 *       that support push_front() (e.g., std::list, std::deque, but not std::vector).
 * @note 与 back_insert_iterator 不同，front_insert_iterator 需要支持 push_front() 的容器
 *       （例如 std::list、std::deque，但不包括 std::vector）。
 * 
 * @see front_inserter()
 */
template<typename Container>
class front_insert_iterator {
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
     * @brief Construct a front_insert_iterator for the given container
     * @brief 为给定容器构造 front_insert_iterator
     * 
     * @param x Reference to the container to insert into
     * @param x 要插入的容器的引用
     * 
     * @details Stores a pointer to the container for later use in assignment operations.
     * 
     * @details 存储指向容器的指针，以便在后续的赋值操作中使用。
     */
    explicit front_insert_iterator(Container& x)
        : container_(&x) {}

    /**
     * @brief Copy-assign a value to the container (inserts at the front)
     * @brief 复制赋值一个值到容器（插入到头部）
     * 
     * @param value Value to insert at the front of the container
     * @param value 要插入到容器头部的值
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Inserts the value at the front of the container using push_front().
     * This is the primary operation of the front_insert_iterator.
     * 
     * @details 使用 push_front() 将值插入到容器头部。
     * 这是 front_insert_iterator 的主要操作。
     */
    front_insert_iterator& operator=(const typename Container::value_type& value) {
        container_->push_front(value);
        return *this;
    }

    /**
     * @brief Move-assign a value to the container (inserts at the front)
     * @brief 移动赋值一个值到容器（插入到头部）
     * 
     * @param value Value to move-insert at the front of the container
     * @param value 要移动插入到容器头部的值
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Move-inserts the value at the front of the container using push_front()
     * with move semantics. This avoids unnecessary copies when the value is an rvalue.
     * 
     * @details 使用移动语义通过 push_front() 将值移动插入到容器头部。
     * 当值是右值时，这避免了不必要的拷贝。
     */
    front_insert_iterator& operator=(typename Container::value_type&& value) {
        container_->push_front(mystl::move(value));
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
    front_insert_iterator& operator*() {
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
    front_insert_iterator& operator++() {
        return *this;
    }

    /**
     * @brief Postfix increment operator (no-op for output iterators)
     * @brief 后缀递增运算符（输出迭代器的无操作）
     * 
     * @return Copy of this iterator
     * @return 此迭代器的副本
     * 
     * @details For output迭代器，后缀递增运算符是无操作，返回 *this 的副本。
     * 这允许迭代器在需要可递增迭代器的算法中使用。
     */
    front_insert_iterator operator++(int) {
        return *this;
    }
};

/**
 * @brief Convenience function to create a front_insert_iterator
 * @brief 创建 front_insert_iterator 的便捷函数
 * 
 * @tparam Container Container type that supports push_front() method
 * @tparam Container 支持 push_front() 方法的容器类型
 * @param x Reference to the container to insert into
 * @param x 要插入的容器的引用
 * @return front_insert_iterator<Container> for the given container
 * @return 用于给定容器的 front_insert_iterator<Container>
 * 
 * @details Creates a front_insert_iterator for the specified container.
 * This helper function enables cleaner syntax: mystl::front_inserter(container)
 * instead of mystl::front_insert_iterator<Container>(container).
 * 
 * @details 为指定容器创建 front_insert_iterator。
 * 此辅助函数支持更简洁的语法：mystl::front_inserter(container)
 * 而不是 mystl::front_insert_iterator<Container>(container)。
 * 
 * @note Commonly used with standard algorithms:
 *       std::copy(source.begin(), source.end(), mystl::front_inserter(dest));
 * @note 通常与标准算法一起使用：
 *       std::copy(source.begin(), source.end(), mystl::front_inserter(dest));
 * 
 * @note The elements will appear in reverse order in the destination container
 *       because each new element is inserted at the front.
 * @note 元素在目标容器中将按相反顺序出现，因为每个新元素都插入在头部。
 */
template<typename Container>
front_insert_iterator<Container> front_inserter(Container& x) {
    return front_insert_iterator<Container>(x);
}

} // namespace mystl
