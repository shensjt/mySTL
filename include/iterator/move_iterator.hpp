#pragma once

/**
 * @file move_iterator.hpp
 * @brief Iterator adaptor that dereferences to rvalue references
 * @brief 迭代器适配器，解引用为右值引用
 * 
 * This file implements the move_iterator class and make_move_iterator helper function.
 * move_iterator is an iterator adaptor that behaves exactly like the underlying iterator,
 * except that its dereference operator returns an rvalue reference. This enables moving
 * elements from a range rather than copying them.
 * 
 * 本文件实现了 move_iterator 类和 make_move_iterator 辅助函数。
 * move_iterator 是一个迭代器适配器，其行为与底层迭代器完全相同，
 * 但其解引用运算符返回右值引用。这使得可以从范围移动元素而不是复制它们。
 * 
 * @author mySTL shensjt
 * @date 2026-03-19
 * 
 * @see reverse_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/move_iterator
 */

#include "iterator_traits.hpp"
#include "../utility/move.hpp"

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

/**
 * @brief Iterator adaptor that dereferences to rvalue references
 * @brief 解引用为右值引用的迭代器适配器
 * 
 * @tparam Iterator Underlying iterator type
 * @tparam Iterator 底层迭代器类型
 * 
 * @details The move_iterator is an iterator adaptor that behaves exactly like the
 * underlying iterator, except that its dereference operator returns an rvalue reference
 * (value_type&&). This enables algorithms to move elements from a range rather than
 * copying them, which can be more efficient for move-only or expensive-to-copy types.
 * 
 * @details move_iterator 是一个迭代器适配器，其行为与底层迭代器完全相同，
 * 但其解引用运算符返回右值引用（value_type&&）。这使得算法可以从范围移动元素而不是复制它们，
 * 这对于仅移动类型或复制成本高的类型可能更高效。
 * 
 * @note The iterator category is the same as the underlying iterator.
 * @note 迭代器类别与底层迭代器相同。
 * 
 * @note When dereferenced, returns mystl::move(*current_), enabling move semantics.
 * @note 解引用时返回 mystl::move(*current_)，启用移动语义。
 * 
 * @note Useful with algorithms like std::move, std::move_backward, and when
 *       transferring ownership of elements between containers.
 * @note 与 std::move、std::move_backward 等算法以及在容器之间传输元素所有权时很有用。
 * 
 * @see make_move_iterator()
 */
template<typename Iterator>
class move_iterator {
private:
    Iterator current_;  ///< The underlying iterator / 底层迭代器

public:
    using iterator_type = Iterator;  ///< Type of the underlying iterator / 底层迭代器类型
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;  ///< Iterator category / 迭代器类别
    using value_type = typename iterator_traits<Iterator>::value_type;  ///< Value type / 值类型
    using difference_type = typename iterator_traits<Iterator>::difference_type;  ///< Difference type / 差值类型
    using pointer = Iterator;  ///< Pointer type (same as iterator) / 指针类型（与迭代器相同）
    
    // 注意：reference类型是value_type&&（右值引用）
    // 对于const迭代器，我们需要特殊处理
    // Note: reference type is value_type&& (rvalue reference)
    // For const iterators, we need special handling
    using reference = value_type&&;  ///< Reference type (rvalue reference) / 引用类型（右值引用）

    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs a move_iterator with a default-constructed underlying iterator.
     * 
     * @details 使用默认构造的底层迭代器构造 move_iterator。
     */
    move_iterator() = default;
    
    /**
     * @brief Construct from an underlying iterator
     * @brief 从底层迭代器构造
     * 
     * @param i Underlying iterator to adapt
     * @param i 要适配的底层迭代器
     * 
     * @details Constructs a move_iterator that adapts the given iterator.
     * 
     * @details 构造适配给定迭代器的 move_iterator。
     */
    explicit move_iterator(Iterator i) : current_(i) {}
    
    /**
     * @brief Converting constructor from compatible move_iterator
     * @brief 从兼容的 move_iterator 进行转换构造
     * 
     * @tparam Iter Compatible iterator type
     * @tparam Iter 兼容的迭代器类型
     * @param other move_iterator to copy from
     * @param other 要拷贝的 move_iterator
     * 
     * @details Constructs a move_iterator by copying the underlying iterator
     * from another move_iterator with a compatible iterator type.
     * 
     * @details 通过从具有兼容迭代器类型的另一个 move_iterator 拷贝底层迭代器来构造 move_iterator。
     */
    template<typename Iter>
    move_iterator(const move_iterator<Iter>& other)
        : current_(other.base()) {}
    
    /**
     * @brief Converting assignment from compatible move_iterator
     * @brief 从兼容的 move_iterator 进行转换赋值
     * 
     * @tparam Iter Compatible iterator type
     * @tparam Iter 兼容的迭代器类型
     * @param other move_iterator to assign from
     * @param other 要赋值的 move_iterator
     * @return Reference to this iterator
     * @return 此迭代器的引用
     */
    template<typename Iter>
    move_iterator& operator=(const move_iterator<Iter>& other) {
        current_ = other.base();
        return *this;
    }

    /**
     * @brief Get the underlying iterator
     * @brief 获取底层迭代器
     * 
     * @return The underlying iterator
     * @return 底层迭代器
     */
    Iterator base() const {
        return current_;
    }

    /**
     * @brief Dereference operator (returns rvalue reference)
     * @brief 解引用运算符（返回右值引用）
     * 
     * @return Rvalue reference to the element
     * @return 元素的右值引用
     * 
     * @details Returns mystl::move(*current_), enabling move semantics.
     * This is the key difference from a regular iterator.
     * 
     * @details 返回 mystl::move(*current_)，启用移动语义。
     * 这是与常规迭代器的关键区别。
     */
    reference operator*() const {
        return mystl::move(*current_);
    }

    /**
     * @brief Member access operator
     * @brief 成员访问运算符
     * 
     * @return Pointer to the element (same as underlying iterator)
     * @return 指向元素的指针（与底层迭代器相同）
     * 
     * @details Returns the underlying iterator, which can be used for member access.
     * 
     * @details 返回底层迭代器，可用于成员访问。
     */
    pointer operator->() const {
        return current_;
    }

    /**
     * @brief Subscript operator (returns rvalue reference)
     * @brief 下标运算符（返回右值引用）
     * 
     * @param n Index offset
     * @param n 索引偏移量
     * @return Rvalue reference to the element at offset n
     * @return 偏移量 n 处元素的右值引用
     * 
     * @details Returns mystl::move(current_[n]), enabling move semantics
     * for random access iterators.
     * 
     * @details 返回 mystl::move(current_[n])，为随机访问迭代器启用移动语义。
     */
    reference operator[](difference_type n) const {
        return mystl::move(current_[n]);
    }

    /**
     * @brief Prefix increment operator
     * @brief 前缀递增运算符
     * 
     * @return Reference to this iterator
     * @return 此迭代器的引用
     */
    move_iterator& operator++() {
        ++current_;
        return *this;
    }

    /**
     * @brief Postfix increment operator
     * @brief 后缀递增运算符
     * 
     * @return Copy of the iterator before incrementing
     * @return 递增前的迭代器副本
     */
    move_iterator operator++(int) {
        move_iterator tmp = *this;
        ++current_;
        return tmp;
    }

    /**
     * @brief Prefix decrement operator
     * @brief 前缀递减运算符
     * 
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @note Only available for bidirectional and random access iterators.
     * @note 仅适用于双向和随机访问迭代器。
     */
    move_iterator& operator--() {
        --current_;
        return *this;
    }

    /**
     * @brief Postfix decrement operator
     * @brief 后缀递减运算符
     * 
     * @return Copy of the iterator before decrementing
     * @return 递减前的迭代器副本
     * 
     * @note Only available for bidirectional and random access iterators.
     * @note 仅适用于双向和随机访问迭代器。
     */
    move_iterator operator--(int) {
        move_iterator tmp = *this;
        --current_;
        return tmp;
    }

    /**
     * @brief Addition operator
     * @brief 加法运算符
     * 
     * @param n Offset to add
     * @param n 要添加的偏移量
     * @return move_iterator advanced by n positions
     * @return 前进 n 个位置的 move_iterator
     * 
     * @note Only available for random access iterators.
     * @note 仅适用于随机访问迭代器。
     */
    move_iterator operator+(difference_type n) const {
        return move_iterator(current_ + n);
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
     * @note Only available for random access iterators.
     * @note 仅适用于随机访问迭代器。
     */
    move_iterator& operator+=(difference_type n) {
        current_ += n;
        return *this;
    }

    /**
     * @brief Subtraction operator
     * @brief 减法运算符
     * 
     * @param n Offset to subtract
     * @param n 要减去的偏移量
     * @return move_iterator moved back by n positions
     * @return 后退 n 个位置的 move_iterator
     * 
     * @note Only available for random access iterators.
     * @note 仅适用于随机访问迭代器。
     */
    move_iterator operator-(difference_type n) const {
        return move_iterator(current_ - n);
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
     * @note Only available for random access iterators.
     * @note 仅适用于随机访问迭代器。
     */
    move_iterator& operator-=(difference_type n) {
        current_ -= n;
        return *this;
    }

    // Friend declarations for comparison operators / 比较运算符的友元声明
    template<typename Iter1, typename Iter2>
    friend bool operator==(const move_iterator<Iter1>& lhs,
                          const move_iterator<Iter2>& rhs);

    template<typename Iter1, typename Iter2>
    friend bool operator<(const move_iterator<Iter1>& lhs,
                         const move_iterator<Iter2>& rhs);
};

/**
 * @brief Equality comparison operator for move_iterator
 * @brief move_iterator 的相等比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side move_iterator
 * @param lhs 左侧 move_iterator
 * @param rhs Right-hand side move_iterator
 * @param rhs 右侧 move_iterator
 * @return true if underlying iterators are equal, false otherwise
 * @return 如果底层迭代器相等则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator==(const move_iterator<Iterator1>& lhs,
                const move_iterator<Iterator2>& rhs) {
    return lhs.base() == rhs.base();
}

/**
 * @brief Inequality comparison operator for move_iterator
 * @brief move_iterator 的不等比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side move_iterator
 * @param lhs 左侧 move_iterator
 * @param rhs Right-hand side move_iterator
 * @param rhs 右侧 move_iterator
 * @return true if underlying iterators are not equal, false otherwise
 * @return 如果底层迭代器不相等则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator!=(const move_iterator<Iterator1>& lhs,
                const move_iterator<Iterator2>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief Less-than comparison operator for move_iterator
 * @brief move_iterator 的小于比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side move_iterator
 * @param lhs 左侧 move_iterator
 * @param rhs Right-hand side move_iterator
 * @param rhs 右侧 move_iterator
 * @return true if lhs.base() < rhs.base(), false otherwise
 * @return 如果 lhs.base() < rhs.base() 则为 true，否则为 false
 * 
 * @note Only meaningful for random access iterators.
 * @note 仅对随机访问迭代器有意义。
 */
template<typename Iterator1, typename Iterator2>
bool operator<(const move_iterator<Iterator1>& lhs,
               const move_iterator<Iterator2>& rhs) {
    return lhs.base() < rhs.base();
}

/**
 * @brief Less-than-or-equal comparison operator for move_iterator
 * @brief move_iterator 的小于等于比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side move_iterator
 * @param lhs 左侧 move_iterator
 * @param rhs Right-hand side move_iterator
 * @param rhs 右侧 move_iterator
 * @return true if lhs.base() <= rhs.base(), false otherwise
 * @return 如果 lhs.base() <= rhs.base() 则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator<=(const move_iterator<Iterator1>& lhs,
                const move_iterator<Iterator2>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief Greater-than comparison operator for move_iterator
 * @brief move_iterator 的大于比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side move_iterator
 * @param lhs 左侧 move_iterator
 * @param rhs Right-hand side move_iterator
 * @param rhs 右侧 move_iterator
 * @return true if lhs.base() > rhs.base(), false otherwise
 * @return 如果 lhs.base() > rhs.base() 则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator>(const move_iterator<Iterator1>& lhs,
               const move_iterator<Iterator2>& rhs) {
    return rhs < lhs;
}

/**
 * @brief Greater-than-or-equal comparison operator for move_iterator
 * @brief move_iterator 的大于等于比较运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side move_iterator
 * @param lhs 左侧 move_iterator
 * @param rhs Right-hand side move_iterator
 * @param rhs 右侧 move_iterator
 * @return true if lhs.base() >= rhs.base(), false otherwise
 * @return 如果 lhs.base() >= rhs.base() 则为 true，否则为 false
 */
template<typename Iterator1, typename Iterator2>
bool operator>=(const move_iterator<Iterator1>& lhs,
                const move_iterator<Iterator2>& rhs) {
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
 * @param it move_iterator to add to
 * @param it 要添加的 move_iterator
 * @return move_iterator advanced by n positions
 * @return 前进 n 个位置的 move_iterator
 * 
 * @note Enables n + it syntax in addition to it + n.
 * @note 除了 it + n 语法外，还支持 n + it 语法。
 */
template<typename Iterator>
move_iterator<Iterator> operator+(
    typename move_iterator<Iterator>::difference_type n,
    const move_iterator<Iterator>& it) {
    return it + n;
}

/**
 * @brief Subtraction operator between two move_iterators
 * @brief 两个 move_iterator 之间的减法运算符
 * 
 * @tparam Iterator1 First iterator type
 * @tparam Iterator1 第一个迭代器类型
 * @tparam Iterator2 Second iterator type
 * @tparam Iterator2 第二个迭代器类型
 * @param lhs Left-hand side move_iterator
 * @param lhs 左侧 move_iterator
 * @param rhs Right-hand side move_iterator
 * @param rhs 右侧 move_iterator
 * @return Distance between the underlying iterators
 * @return 底层迭代器之间的距离
 * 
 * @details Returns lhs.base() - rhs.base(), the distance between the two iterators.
 * 
 * @details 返回 lhs.base() - rhs.base()，即两个迭代器之间的距离。
 */
template<typename Iterator1, typename Iterator2>
auto operator-(const move_iterator<Iterator1>& lhs,
               const move_iterator<Iterator2>& rhs)
    -> decltype(lhs.base() - rhs.base()) {
    return lhs.base() - rhs.base();
}

/**
 * @brief Helper function to create a move_iterator
 * @brief 创建 move_iterator 的辅助函数
 * 
 * @tparam Iterator Iterator type
 * @tparam Iterator 迭代器类型
 * @param i Iterator to adapt
 * @param i 要适配的迭代器
 * @return move_iterator<Iterator> adapting the given iterator
 * @return 适配给定迭代器的 move_iterator<Iterator>
 * 
 * @details Creates a move_iterator from the given iterator.
 * This helper function enables cleaner syntax: mystl::make_move_iterator(it)
 * instead of mystl::move_iterator<Iterator>(it).
 * 
 * @details 从给定迭代器创建 move_iterator。
 * 此辅助函数支持更简洁的语法：mystl::make_move_iterator(it)
 * 而不是 mystl::move_iterator<Iterator>(it)。
 * 
 * @note Commonly used with algorithms like std::move:
 *       std::move(source.begin(), source.end(), mystl::make_move_iterator(dest.begin()));
 * @note 通常与 std::move 等算法一起使用：
 *       std::move(source.begin(), source.end(), mystl::make_move_iterator(dest.begin()));
 */
template<typename Iterator>
move_iterator<Iterator> make_move_iterator(Iterator i) {
    return move_iterator<Iterator>(i);
}

} // namespace mystl
