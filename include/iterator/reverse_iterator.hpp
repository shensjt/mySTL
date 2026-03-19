#pragma once

/**
 * @file reverse_iterator.hpp
 * @brief Iterator adaptor that reverses the direction of traversal
 * @brief 迭代器适配器，反转遍历方向
 * 
 * This file implements the reverse_iterator class.
 * reverse_iterator is an iterator adaptor that reverses the direction of traversal
 * of a bidirectional or random access iterator. It adapts an iterator so that
 * moving forward (++) actually moves backward in the underlying sequence, and
 * moving backward (--) moves forward.
 * 
 * 本文件实现了 reverse_iterator 类。
 * reverse_iterator 是一个迭代器适配器，反转双向或随机访问迭代器的遍历方向。
 * 它适配一个迭代器，使得向前移动（++）实际上在底层序列中向后移动，
 * 而向后移动（--）则向前移动。
 * 
 * @author mySTL shensjt
 * @date 2026-03-19
 * 
 * @see move_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/reverse_iterator
 */

#include "iterator_traits.hpp"
#include "../utility/type_traits.hpp"
#include <cstddef>

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

/**
 * @brief Iterator adaptor that reverses the direction of traversal
 * @brief 反转遍历方向的迭代器适配器
 * 
 * @tparam Iterator Underlying iterator type (must be bidirectional or random access)
 * @tparam Iterator 底层迭代器类型（必须是双向或随机访问迭代器）
 * 
 * @details The reverse_iterator is an iterator adaptor that reverses the direction
 * of traversal of a bidirectional or random access iterator. It adapts an iterator
 * so that moving forward (++) actually moves backward in the underlying sequence,
 * and moving backward (--) moves forward.
 * 
 * @details reverse_iterator 是一个迭代器适配器，反转双向或随机访问迭代器的遍历方向。
 * 它适配一个迭代器，使得向前移动（++）实际上在底层序列中向后移动，
 * 而向后移动（--）则向前移动。
 * 
 * @note The iterator category is the same as the underlying iterator.
 * @note 迭代器类别与底层迭代器相同。
 * 
 * @note The base() function returns the underlying iterator, which points to
 *       the element after the one the reverse_iterator points to.
 * @note base() 函数返回底层迭代器，它指向 reverse_iterator 指向的元素之后的元素。
 * 
 * @note Useful for traversing containers in reverse order with algorithms
 *       and range-based for loops.
 * @note 用于使用算法和基于范围的 for 循环以相反顺序遍历容器。
 */
template<typename Iterator>
class reverse_iterator {
private:
    Iterator current_;  ///< The underlying iterator / 底层迭代器

public:
    using iterator_type = Iterator;  ///< Type of the underlying iterator / 底层迭代器类型
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;  ///< Iterator category / 迭代器类别
    using value_type = typename iterator_traits<Iterator>::value_type;  ///< Value type / 值类型
    using difference_type = typename iterator_traits<Iterator>::difference_type;  ///< Difference type / 差值类型
    using pointer = typename iterator_traits<Iterator>::pointer;  ///< Pointer type / 指针类型
    using reference = typename iterator_traits<Iterator>::reference;  ///< Reference type / 引用类型

    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs a reverse_iterator with a default-constructed underlying iterator.
     * 
     * @details 使用默认构造的底层迭代器构造 reverse_iterator。
     */
    reverse_iterator() = default;
    
    /**
     * @brief Construct from an underlying iterator
     * @brief 从底层迭代器构造
     * 
     * @param x Underlying iterator to adapt
     * @param x 要适配的底层迭代器
     * 
     * @details Constructs a reverse_iterator that adapts the given iterator.
     * The reverse_iterator will point to the element preceding the one pointed to
     * by the underlying iterator.
     * 
     * @details 构造适配给定迭代器的 reverse_iterator。
     * reverse_iterator 将指向底层迭代器指向的元素之前的元素。
     */
    explicit reverse_iterator(Iterator x) : current_(x) {}
    
    /**
     * @brief Converting constructor from compatible reverse_iterator
     * @brief 从兼容的 reverse_iterator 进行转换构造
     * 
     * @tparam Iter Compatible iterator type
     * @tparam Iter 兼容的迭代器类型
     * @param other reverse_iterator to copy from
     * @param other 要拷贝的 reverse_iterator
     * 
     * @details Constructs a reverse_iterator by copying the underlying iterator
     * from another reverse_iterator with a compatible iterator type.
     * 
     * @details 通过从具有兼容迭代器类型的另一个 reverse_iterator 拷贝底层迭代器来构造 reverse_iterator。
     */
    template<typename Iter>
    reverse_iterator(const reverse_iterator<Iter>& other)
        : current_(other.base()) {}
    
    /**
     * @brief Converting assignment from compatible reverse_iterator
     * @brief 从兼容的 reverse_iterator 进行转换赋值
     * 
     * @tparam Iter Compatible iterator type
     * @tparam Iter 兼容的迭代器类型
     * @param other reverse_iterator to assign from
     * @param other 要赋值的 reverse_iterator
     * @return Reference to this iterator
     * @return 此迭代器的引用
     */
    template<typename Iter>
    reverse_iterator& operator=(const reverse_iterator<Iter>& other) {
        current_ = other.base();
        return *this;
    }

    /**
     * @brief Get the underlying iterator
     * @brief 获取底层迭代器
     * 
     * @return The underlying iterator
     * @return 底层迭代器
     * 
     * @details Returns the underlying iterator. Note that the underlying iterator
     * points to the element after the one the reverse_iterator points to.
     * 
     * @details 返回底层迭代器。注意底层迭代器指向 reverse_iterator 指向的元素之后的元素。
     */
    Iterator base() const {
        return current_;
    }

    /**
     * @brief Dereference operator
     * @brief 解引用运算符
     * 
     * @return Reference to the element pointed to by the reverse_iterator
     * @return reverse_iterator 指向的元素的引用
     * 
     * @details Returns a reference to the element preceding the one pointed to
     * by the underlying iterator.
     * 
     * @details 返回到底层迭代器指向的元素之前的元素的引用。
     */
    reference operator*() const {
        Iterator tmp = current_;
        return *--tmp;
    }

    /**
     * @brief Member access operator
     * @brief 成员访问运算符
     * 
     * @return Pointer to the element pointed to by the reverse_iterator
     * @return 指向 reverse_iterator 指向的元素的指针
     * 
     * @details Returns a pointer to the element preceding the one pointed to
     * by the underlying iterator.
     * 
     * @details 返回到底层迭代器指向的元素之前的元素的指针。
     */
    pointer operator->() const {
        Iterator tmp = current_;
        --tmp;
        return &*tmp;
    }

    /**
     * @brief Subscript operator
     * @brief 下标运算符
     * 
     * @param n Index offset
     * @param n 索引偏移量
     * @return Reference to the element at offset n
     * @return 偏移量 n 处元素的引用
     * 
     * @details Returns a reference to the element at offset n from the current position.
     * For reverse_iterator, positive n moves backward in the underlying sequence.
     * 
     * @details 返回到当前位置偏移量 n 处元素的引用。
     * 对于 reverse_iterator，正数 n 在底层序列中向后移动。
     * 
     * @note Only available for random access iterators.
     * @note 仅适用于随机访问迭代器。
     */
    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    /**
     * @brief Prefix increment operator
     * @brief 前缀递增运算符
     * 
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Moves the iterator backward in the underlying sequence
     * (decrements the underlying iterator).
     * 
     * @details 在底层序列中向后移动迭代器（递减底层迭代器）。
     */
    reverse_iterator& operator++() {
        --current_;
        return *this;
    }

    /**
     * @brief Postfix increment operator
     * @brief 后缀递增运算符
     * 
     * @return Copy of the iterator before incrementing
     * @return 递增前的迭代器副本
     * 
     * @details Moves the iterator backward in the underlying sequence and returns
     * a copy of the iterator before the move.
     * 
     * @details 在底层序列中向后移动迭代器，并返回移动前的迭代器副本。
     */
    reverse_iterator operator++(int) {
        reverse_iterator tmp = *this;
        --current_;
        return tmp;
    }

    /**
     * @brief Prefix decrement operator
     * @brief 前缀递减运算符
     * 
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Moves the iterator forward in the underlying sequence
     * (increments the underlying iterator).
     * 
     * @details 在底层序列中向前移动迭代器（递增底层迭代器）。
     * 
     * @note Only available for bidirectional and random access iterators.
     * @note 仅适用于双向和随机访问迭代器。
     */
    reverse_iterator& operator--() {
        ++current_;
        return *this;
    }

    /**
     * @brief Postfix decrement operator
     * @brief 后缀递减运算符
     * 
     * @return Copy of the iterator before decrementing
     * @return 递减前的迭代器副本
     * 
     * @details Moves the iterator forward in the underlying sequence and returns
     * a copy of the iterator before the move.
     * 
     * @details 在底层序列中向前移动迭代器，并返回移动前的迭代器副本。
     * 
     * @note Only available for bidirectional and random access iterators.
     * @note 仅适用于双向和随机访问迭代器。
     */
    reverse_iterator operator--(int) {
        reverse_iterator tmp = *this;
        ++current_;
        return tmp;
    }

    /**
     * @brief Addition operator
     * @brief 加法运算符
     * 
     * @param n Offset to add
     * @param n 要添加的偏移量
     * @return reverse_iterator moved backward by n positions
     * @return 向后移动 n 个位置的 reverse_iterator
     * 
     * @details Moves the iterator backward by n positions in the underlying sequence.
     * 
     * @details 在底层序列中向后移动迭代器 n 个位置。
     * 
     * @note Only available for random access iterators.
     * @note 仅适用于随机访问迭代器。
     */
    reverse_iterator operator+(difference_type n) const {
        return reverse_iterator(current_ - n);
    }

    /**
     * @brief Compound addition assignment operator
     * @brief 复合加法赋值运算符
     * 
     * @param n Offset to add
     * @param n 要添加的偏移量
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Moves the iterator backward by n positions in the underlying sequence.
     * 
     * @details 在底层序列中向后移动迭代器 n 个位置。
     * 
     * @note Only available for random access iterators.
     * @note 仅适用于随机访问迭代器。
     */
    reverse_iterator& operator+=(difference_type n) {
        current_ -= n;
        return *this;
    }

    /**
     * @brief Subtraction operator
     * @brief 减法运算符
     * 
     * @param n Offset to subtract
     * @param n 要减去的偏移量
     * @return reverse_iterator moved forward by n positions
     * @return 向前移动 n 个位置的 reverse_iterator
     * 
     * @details Moves the iterator forward by n positions in the underlying sequence.
     * 
     * @details 在底层序列中向前移动迭代器 n 个位置。
     * 
     * @note Only available for random access iterators.
     * @note 仅适用于随机访问迭代器。
     */
    reverse_iterator operator-(difference_type n) const {
        return reverse_iterator(current_ + n);
    }

    /**
     * @brief Compound subtraction assignment operator
     * @brief 复合减法赋值运算符
     * 
     * @param n Offset to subtract
     * @param n 要减去的偏移量
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Moves the iterator forward by n positions in the underlying sequence.
     * 
     * @details 在底层序列中向前移动迭代器 n 个位置。
     * 
     * @note Only available for random access iterators.
     * @note 仅适用于随机访问迭代器。
     */
    reverse_iterator& operator-=(difference_type n) {
        current_ += n;
        return *this;
    }

    // Friend declarations for comparison operators / 比较运算符的友元声明
    template<typename Iter1, typename Iter2>
    friend bool operator==(const reverse_iterator<Iter1>& lhs,
                          const reverse_iterator<Iter2>& rhs);

    template<typename Iter1, typename Iter2>
    friend bool operator<(const reverse_iterator<Iter1>& lhs,
                         const reverse_iterator<Iter2>& rhs);
};

/**
 * @brief Equality comparison operator for reverse_iterator
 * @brief reverse_iterator 的相等比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side reverse_iterator
 * @param lhs 左侧 reverse_iterator
 * @param rhs Right-hand side reverse_iterator
 * @param rhs 右侧 reverse_iterator
 * @return true if underlying iterators are equal, false otherwise
 * @return 如果底层迭代器相等则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator==(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
    return lhs.base() == rhs.base();
}

/**
 * @brief Inequality comparison operator for reverse_iterator
 * @brief reverse_iterator 的不等比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side reverse_iterator
 * @param lhs 左侧 reverse_iterator
 * @param rhs Right-hand side reverse_iterator
 * @param rhs 右侧 reverse_iterator
 * @return true if underlying iterators are not equal, false otherwise
 * @return 如果底层迭代器不相等则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator!=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief Less-than comparison operator for reverse_iterator
 * @brief reverse_iterator 的小于比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side reverse_iterator
 * @param lhs 左侧 reverse_iterator
 * @param rhs Right-hand side reverse_iterator
 * @param rhs 右侧 reverse_iterator
 * @return true if lhs.base() > rhs.base(), false otherwise
 * @return 如果 lhs.base() > rhs.base() 则为 true，否则为 false
 * 
 * @note Note the reversed comparison: reverse_iterator comparison is based on
 *       the reverse order of the underlying iterators.
 * @note 注意反转的比较：reverse_iterator 的比较基于底层迭代器的相反顺序。
 * 
 * @note Only meaningful for random access iterators.
 * @note 仅对随机访问迭代器有意义。
 */
template<typename Iterator1, typename Iterator2>
bool operator<(const reverse_iterator<Iterator1>& lhs,
               const reverse_iterator<Iterator2>& rhs) {
    return lhs.base() > rhs.base();
}

/**
 * @brief Less-than-or-equal comparison operator for reverse_iterator
 * @brief reverse_iterator 的小于等于比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side reverse_iterator
 * @param lhs 左侧 reverse_iterator
 * @param rhs Right-hand side reverse_iterator
 * @param rhs 右侧 reverse_iterator
 * @return true if lhs.base() >= rhs.base(), false otherwise
 * @return 如果 lhs.base() >= rhs.base() 则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator<=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief Greater-than comparison operator for reverse_iterator
 * @brief reverse_iterator 的大于比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side reverse_iterator
 * @param lhs 左侧 reverse_iterator
 * @param rhs Right-hand side reverse_iterator
 * @param rhs 右侧 reverse_iterator
 * @return true if lhs.base() < rhs.base(), false otherwise
 * @return 如果 lhs.base() < rhs.base() 则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator>(const reverse_iterator<Iterator1>& lhs,
               const reverse_iterator<Iterator2>& rhs) {
    return rhs < lhs;
}

/**
 * @brief Greater-than-or-equal comparison operator for reverse_iterator
 * @brief reverse_iterator 的大于等于比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side reverse_iterator
 * @param lhs 左侧 reverse_iterator
 * @param rhs Right-hand side reverse_iterator
 * @param rhs 右侧 reverse_iterator
 * @return true if lhs.base() <= rhs.base(), false otherwise
 * @return 如果 lhs.base() <= rhs.base() 则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator>=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
    return !(lhs < rhs);
}

/**
 * @brief Addition operator (commutative version)
 * @brief 加法运算符（交换律版本）
 * 
 * @tparam Iterator Iterator type
 * @tparam Iterator 迭代器类型
 * @param n Offset to add
 * @param n 要添加的偏移量
 * @param it reverse_iterator to add to
 * @param it 要添加的 reverse_iterator
 * @return reverse_iterator moved backward by n positions
 * @return 向后移动 n 个位置的 reverse_iterator
 * 
 * @note Enables n + it syntax in addition to it + n.
 * @note 除了 it + n 语法外，还支持 n + it 语法。
 */
template<typename Iterator>
reverse_iterator<Iterator> operator+(
    typename reverse_iterator<Iterator>::difference_type n,
    const reverse_iterator<Iterator>& it) {
    return it + n;
}

/**
 * @brief Subtraction operator between two reverse_iterators
 * @brief 两个 reverse_iterator 之间的减法运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side reverse_iterator
 * @param lhs 左侧 reverse_iterator
 * @param rhs Right-hand side reverse_iterator
 * @param rhs 右侧 reverse_iterator
 * @return Distance between the underlying iterators (with sign reversed)
 * @return 底层迭代器之间的距离（符号反转）
 * 
 * @details Returns rhs.base() - lhs.base(), the distance between the two iterators
 * with the sign reversed to account for the reverse traversal direction.
 * 
 * @details 返回 rhs.base() - lhs.base()，即两个迭代器之间的距离，符号反转以考虑反向遍历方向。
 */
template<typename Iterator1, typename Iterator2>
auto operator-(const reverse_iterator<Iterator1>& lhs,
               const reverse_iterator<Iterator2>& rhs)
    -> decltype(rhs.base() - lhs.base()) {
    return rhs.base() - lhs.base();
}

} // namespace mystl
