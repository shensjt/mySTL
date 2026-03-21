#pragma once

/**
 * @file deque.hpp
 * @brief Double-ended queue container implementation for mySTL
 * @brief 双端队列容器实现（mySTL）
 * 
 * Provides deque (double-ended queue) container with random access,
 * efficient insertion/removal at both ends, and block-based storage.
 * This implementation uses a segmented array (block-based) approach
 * to provide O(1) insertion/removal at both ends while maintaining
 * random access capability.
 * 
 * 提供具有随机访问、两端高效插入/删除和基于块存储的双端队列容器。
 * 此实现使用分段数组（基于块）方法，在保持随机访问能力的同时，
 * 提供O(1)复杂度的两端插入/删除操作。
 * 
 * @details
 * The deque is implemented as a collection of fixed-size blocks (buffers)
 * managed by a central map array. This design allows:
 * 1. Efficient growth in both directions without reallocation of all elements
 * 2. Random access through block indexing
 * 3. Amortized O(1) push/pop operations at both ends
 * 
 * @details
 * deque被实现为由中央map数组管理的固定大小块（缓冲区）的集合。这种设计允许：
 * 1. 在两个方向上高效增长，无需重新分配所有元素
 * 2. 通过块索引实现随机访问
 * 3. 两端push/pop操作的摊还O(1)复杂度
 * 
 * @author shensjt
 * @date 2026-03-20
 * 
 * @see https://en.cppreference.com/w/cpp/container/deque
 * @see 相关参考：https://zh.cppreference.com/w/cpp/container/deque
 */

#include <cstddef>
#include "../algorithm/algorithm.hpp"
#include "../memory/allocator.hpp"
#include "../memory/allocator_traits.hpp"
#include "../iterator/iterator_traits.hpp"
#include "../iterator/reverse_iterator.hpp"
#include "../utility/move.hpp"
#include "../utility/swap.hpp"

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

/**
 * @brief Forward declaration of deque container class
 * @brief deque容器类的前向声明
 * 
 * @tparam T Type of elements stored in the deque
 * @tparam T 存储在deque中的元素类型
 * @tparam Allocator Allocator type for memory management
 * @tparam Allocator 用于内存管理的分配器类型
 */
template<typename T, typename Allocator>
class deque;

/**
 * @brief Forward declaration of deque iterator class
 * @brief deque迭代器类的前向声明
 * 
 * @tparam T Type of elements in the deque
 * @tparam T deque中元素的类型
 */
template<typename T>
class deque_iterator;

// ==================== deque_iterator Class ====================
// ==================== deque_iterator 类 ====================

/**
 * @brief Random access iterator for deque container
 * @brief deque容器的随机访问迭代器
 * 
 * @tparam T Type of elements in the deque
 * @tparam T deque中元素的类型
 * 
 * @details
 * This iterator supports random access operations for deque containers.
 * It maintains pointers to the current element, current block boundaries,
 * and the block pointer in the central map array. The iterator handles
 * block boundary crossing transparently when moving across elements.
 * 
 * @details
 * 此迭代器支持deque容器的随机访问操作。
 * 它维护指向当前元素、当前块边界以及中央map数组中块指针的指针。
 * 迭代器在跨元素移动时透明地处理块边界跨越。
 * 
 * @note This iterator is a random access iterator and satisfies all
 * requirements of the RandomAccessIterator concept.
 * 
 * @note 此迭代器是随机访问迭代器，满足RandomAccessIterator概念的所有要求。
 */
template<typename T>
class deque_iterator {
public:
    // 迭代器类别
    using iterator_category = mystl::random_access_iterator_tag;  ///< Iterator category / 迭代器类别
    using value_type = T;                                         ///< Type of elements / 元素类型
    using difference_type = std::ptrdiff_t;                       ///< Difference type / 差值类型
    using pointer = T*;                                           ///< Pointer type / 指针类型
    using reference = T&;                                         ///< Reference type / 引用类型
    using map_pointer = T**;                                      ///< Map pointer type / 映射指针类型

private:
    T* cur_;           ///< Pointer to current element / 指向当前元素的指针
    T* first_;         ///< Pointer to start of current block / 指向当前块起始的指针
    T* last_;          ///< Pointer to end of current block (one past last element) / 指向当前块结束的指针（最后一个元素的下一个位置）
    map_pointer node_; ///< Pointer to current block in map array / 指向map数组中当前块的指针

public:
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs an empty iterator pointing to no element.
     * 
     * @details 构造一个不指向任何元素的空迭代器。
     */
    deque_iterator() noexcept : cur_(nullptr), first_(nullptr), last_(nullptr), node_(nullptr) {}
    
    /**
     * @brief Constructor with pointers
     * @brief 带指针的构造函数
     * 
     * @param cur Pointer to current element
     * @param cur 指向当前元素的指针
     * @param first Pointer to start of current block
     * @param first 指向当前块起始的指针
     * @param last Pointer to end of current block (one past last element)
     * @param last 指向当前块结束的指针（最后一个元素的下一个位置）
     * @param node Pointer to current block in map array
     * @param node 指向map数组中当前块的指针
     * 
     * @details Constructs an iterator with the given pointers.
     * 
     * @details 使用给定的指针构造迭代器。
     */
    deque_iterator(T* cur, T* first, T* last, map_pointer node) noexcept
        : cur_(cur), first_(first), last_(last), node_(node) {}
    
    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other Another iterator to copy from
     * @param other 要拷贝的另一个迭代器
     * 
     * @details Constructs an iterator by copying another iterator.
     * 
     * @details 通过拷贝另一个迭代器构造迭代器。
     */
    deque_iterator(const deque_iterator& other) noexcept
        : cur_(other.cur_), first_(other.first_), last_(other.last_), node_(other.node_) {}
    
    /**
     * @brief Constructor from non-const iterator (for const conversion)
     * @brief 从非const迭代器构造（用于const转换）
     * 
     * @tparam U Type of elements in source iterator
     * @tparam U 源迭代器中元素的类型
     * @param other Non-const iterator to convert from
     * @param other 要转换的非const迭代器
     * 
     * @details Constructs a const iterator from a non-const iterator.
     * This constructor is enabled only when T is const U.
     * 
     * @details 从非const迭代器构造const迭代器。
     * 仅当T是const U时启用此构造函数。
     */
    template<typename U>
    deque_iterator(const deque_iterator<U>& other,
                  typename mystl::enable_if<mystl::is_same<T, const U>::value>::type* = nullptr) noexcept
        : cur_(other.current()), first_(other.first()), last_(other.last()), node_(const_cast<map_pointer>(other.node())) {}
    
    /**
     * @brief Constructor from non-const pointers (for const conversion)
     * @brief 从非const指针构造（用于const转换）
     * 
     * @tparam U Type of elements in source pointers
     * @tparam U 源指针中元素的类型
     * @param cur Pointer to current element
     * @param cur 指向当前元素的指针
     * @param first Pointer to start of current block
     * @param first 指向当前块起始的指针
     * @param last Pointer to end of current block
     * @param last 指向当前块结束的指针
     * @param node Pointer to current block in map array
     * @param node 指向map数组中当前块的指针
     * 
     * @details Constructs a const iterator from non-const pointers.
     * Used internally by deque class for begin()/end() implementation.
     * 
     * @details 从非const指针构造const迭代器。
     * 由deque类内部用于begin()/end()实现。
     */
    template<typename U>
    deque_iterator(U* cur, U* first, U* last, U** node,
                  typename mystl::enable_if<mystl::is_same<T, const U>::value>::type* = nullptr) noexcept
        : cur_(cur), first_(first), last_(last), node_(const_cast<map_pointer>(node)) {}
    
    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值操作符
     * 
     * @param other Another iterator to assign from
     * @param other 要赋值的另一个迭代器
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Assigns the contents of another iterator to this iterator.
     * 
     * @details 将另一个迭代器的内容赋值给此迭代器。
     */
    deque_iterator& operator=(const deque_iterator& other) noexcept {
        if (this != &other) {
            cur_ = other.cur_;
            first_ = other.first_;
            last_ = other.last_;
            node_ = other.node_;
        }
        return *this;
    }
    
    /**
     * @brief Dereference operator
     * @brief 解引用操作符
     * 
     * @return Reference to the current element
     * @return 当前元素的引用
     * 
     * @details Returns a reference to the element pointed to by the iterator.
     * 
     * @details 返回迭代器指向的元素的引用。
     */
    reference operator*() const noexcept {
        return *cur_;
    }
    
    /**
     * @brief Member access operator
     * @brief 成员访问操作符
     * 
     * @return Pointer to the current element
     * @return 指向当前元素的指针
     * 
     * @details Returns a pointer to the element pointed to by the iterator.
     * 
     * @details 返回指向迭代器指向的元素的指针。
     */
    pointer operator->() const noexcept {
        return cur_;
    }
    
    /**
     * @brief Prefix increment operator
     * @brief 前缀递增操作符
     * 
     * @return Reference to this iterator after increment
     * @return 递增后此迭代器的引用
     * 
     * @details Advances the iterator to the next element.
     * If the iterator reaches the end of the current block,
     * it automatically moves to the next block.
     * 
     * @details 将迭代器前进到下一个元素。
     * 如果迭代器到达当前块的末尾，它会自动移动到下一个块。
     */
    deque_iterator& operator++() noexcept {
        ++cur_;  // 首先递增当前指针
        if (cur_ == last_) {
            // 如果当前指针到达当前块的末尾，需要移动到下一个块
            // 调用set_node更新节点指针和块边界
            set_node(node_ + 1);
            // 将当前指针设置为新块的起始位置
            cur_ = first_;
        }
        return *this;
    }
    
    /**
     * @brief Postfix increment operator
     * @brief 后缀递增操作符
     * 
     * @param int Dummy parameter to distinguish from prefix increment
     * @param int 用于区分前缀递增的虚拟参数
     * @return Iterator before increment
     * @return 递增前的迭代器
     * 
     * @details Advances the iterator to the next element and returns
     * the previous iterator value.
     * 
     * @details 将迭代器前进到下一个元素并返回先前的迭代器值。
     */
    deque_iterator operator++(int) noexcept {
        deque_iterator temp = *this;  // 保存当前迭代器的副本
        ++(*this);                    // 使用前缀递增操作符前进迭代器
        return temp;                  // 返回保存的副本（递增前的状态）
    }
    
    /**
     * @brief Prefix decrement operator
     * @brief 前缀递减操作符
     * 
     * @return Reference to this iterator after decrement
     * @return 递减后此迭代器的引用
     * 
     * @details Moves the iterator to the previous element.
     * If the iterator reaches the beginning of the current block,
     * it automatically moves to the previous block.
     * 
     * @details 将迭代器移动到前一个元素。
     * 如果迭代器到达当前块的开头，它会自动移动到前一个块。
     */
    deque_iterator& operator--() noexcept {
        if (cur_ == first_) {
            // 如果当前指针在当前块的开头，需要移动到前一个块
            // 调用set_node更新节点指针和块边界
            set_node(node_ - 1);
            // 将当前指针设置为新块的末尾位置（最后一个元素的下一个位置）
            cur_ = last_;
        }
        --cur_;  // 递减当前指针
        return *this;
    }
    
    /**
     * @brief Postfix decrement operator
     * @brief 后缀递减操作符
     * 
     * @param int Dummy parameter to distinguish from prefix decrement
     * @param int 用于区分前缀递减的虚拟参数
     * @return Iterator before decrement
     * @return 递减前的迭代器
     * 
     * @details Moves the iterator to the previous element and returns
     * the previous iterator value.
     * 
     * @details 将迭代器移动到前一个元素并返回先前的迭代器值。
     */
    deque_iterator operator--(int) noexcept {
        deque_iterator temp = *this;  // 保存当前迭代器的副本
        --(*this);                    // 使用前缀递减操作符后退迭代器
        return temp;                  // 返回保存的副本（递减前的状态）
    }
    
    /**
     * @brief Compound addition assignment operator
     * @brief 复合加法赋值操作符
     * 
     * @param n Number of positions to advance
     * @param n 要前进的位置数
     * @return Reference to this iterator after advancement
     * @return 前进后此迭代器的引用
     * 
     * @details
     * Advances the iterator by n positions. This operator handles
     * block boundary crossing when moving across multiple blocks.
     * The algorithm calculates the total offset from the start of
     * the current block, then determines if the movement stays
     * within the current block or requires crossing to other blocks.
     * 
     * @details
     * 将迭代器前进n个位置。此操作符处理跨多个块移动时的块边界跨越。
     * 算法计算从当前块起始的总偏移量，然后确定移动是否保持在当前块内
     * 或需要跨越到其他块。
     */
    deque_iterator& operator+=(difference_type n) noexcept {
        // 计算从当前块起始的总偏移量（当前位置偏移 + n）
        difference_type offset = n + (cur_ - first_);
        
        if (offset >= 0 && offset < difference_type(buffer_size())) {
            // 偏移量在当前块范围内，直接在同一块内移动
            cur_ += n;
        } else {
            // 需要跨块移动，计算需要跨越的块数
            // 对于正向移动：offset / buffer_size()
            // 对于负向移动：使用向上取整除法计算负向块数
            difference_type node_offset = offset > 0 
                ? offset / difference_type(buffer_size())  // 正向移动
                : -difference_type((-offset - 1) / buffer_size()) - 1;  // 负向移动
            
            // 更新到新的块节点
            set_node(node_ + node_offset);
            // 计算在新块内的位置：总偏移量 - 跨越的块数 * 块大小
            cur_ = first_ + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }
    
    /**
     * @brief Addition operator
     * @brief 加法操作符
     * 
     * @param n Number of positions to advance
     * @param n 要前进的位置数
     * @return New iterator advanced by n positions
     * @return 前进n个位置的新迭代器
     * 
     * @details
     * Returns a new iterator that is advanced by n positions from this iterator.
     * This operator does not modify the current iterator.
     * 
     * @details
     * 返回从此迭代器前进n个位置的新迭代器。
     * 此操作符不修改当前迭代器。
     */
    deque_iterator operator+(difference_type n) const noexcept {
        deque_iterator temp = *this;  // 创建当前迭代器的副本
        return temp += n;             // 使用复合加法赋值操作符前进副本
    }
    
    /**
     * @brief Compound subtraction assignment operator
     * @brief 复合减法赋值操作符
     * 
     * @param n Number of positions to move backward
     * @param n 要后退的位置数
     * @return Reference to this iterator after moving backward
     * @return 后退后此迭代器的引用
     * 
     * @details
     * Moves the iterator backward by n positions. This is implemented
     * by adding the negative of n using the += operator.
     * 
     * @details
     * 将迭代器后退n个位置。通过使用+=操作符加上n的负值来实现。
     */
    deque_iterator& operator-=(difference_type n) noexcept {
        return *this += -n;  // 后退n个位置等价于前进-n个位置
    }
    
    /**
     * @brief Subtraction operator
     * @brief 减法操作符
     * 
     * @param n Number of positions to move backward
     * @param n 要后退的位置数
     * @return New iterator moved backward by n positions
     * @return 后退n个位置的新迭代器
     * 
     * @details
     * Returns a new iterator that is moved backward by n positions from this iterator.
     * This operator does not modify the current iterator.
     * 
     * @details
     * 返回从此迭代器后退n个位置的新迭代器。
     * 此操作符不修改当前迭代器。
     */
    deque_iterator operator-(difference_type n) const noexcept {
        deque_iterator temp = *this;  // 创建当前迭代器的副本
        return temp -= n;             // 使用复合减法赋值操作符后退副本
    }
    
    /**
     * @brief Difference operator
     * @brief 差值操作符
     * 
     * @param other Another iterator to subtract from this iterator
     * @param other 从此迭代器减去的另一个迭代器
     * @return Number of elements between this iterator and other
     * @return 此迭代器与另一个迭代器之间的元素数量
     * 
     * @details
     * Calculates the number of elements between this iterator and another iterator.
     * The result is positive if this iterator is after other, negative if before.
     * The calculation accounts for the block-based storage structure:
     * 1. Full blocks between the iterators: (node_ - other.node_ - 1) * buffer_size
     * 2. Elements from this iterator to the end of its block: (cur_ - first_)
     * 3. Elements from the start of other's block to other: (other.last_ - other.cur_)
     * 
     * @details
     * 计算此迭代器与另一个迭代器之间的元素数量。
     * 如果此迭代器在另一个之后，结果为正；如果在之前，结果为负。
     * 计算考虑了基于块的存储结构：
     * 1. 迭代器之间的完整块：(node_ - other.node_ - 1) * buffer_size
     * 2. 从此迭代器到其块末尾的元素：(cur_ - first_)
     * 3. 从另一个迭代器块起始到另一个迭代器的元素：(other.last_ - other.cur_)
     */
    difference_type operator-(const deque_iterator& other) const noexcept {
        // 计算完整块的数量乘以块大小，加上当前块内的偏移量
        return difference_type(buffer_size()) * (node_ - other.node_ - 1) +
               (cur_ - first_) + (other.last_ - other.cur_);
    }
    
    /**
     * @brief Subscript operator
     * @brief 下标访问操作符
     * 
     * @param n Position relative to this iterator
     * @param n 相对于此迭代器的位置
     * @return Reference to the element at position n from this iterator
     * @return 从此迭代器位置n处元素的引用
     * 
     * @details
     * Returns a reference to the element at position n from this iterator.
     * This operator provides random access capability to deque elements.
     * It is implemented by creating a temporary iterator advanced by n
     * positions and then dereferencing it.
     * 
     * @details
     * 返回从此迭代器位置n处元素的引用。
     * 此操作符提供对deque元素的随机访问能力。
     * 通过创建前进n个位置的临时迭代器然后解引用它来实现。
     */
    reference operator[](difference_type n) const noexcept {
        return *(*this + n);  // 创建临时迭代器前进n个位置，然后解引用
    }
    
    /**
     * @brief Equality operator
     * @brief 相等操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if both iterators point to the same element, false otherwise
     * @return 如果两个迭代器指向同一个元素返回true，否则返回false
     * 
     * @details
     * Checks if two iterators point to the same element.
     * For deque iterators, this is determined by comparing the current element pointers.
     * 
     * @details
     * 检查两个迭代器是否指向同一个元素。
     * 对于deque迭代器，通过比较当前元素指针来确定。
     */
    bool operator==(const deque_iterator& other) const noexcept {
        return cur_ == other.cur_;
    }
    
    /**
     * @brief Inequality operator
     * @brief 不等操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if iterators point to different elements, false otherwise
     * @return 如果迭代器指向不同的元素返回true，否则返回false
     * 
     * @details
     * Checks if two iterators point to different elements.
     * This is the negation of the equality operator.
     * 
     * @details
     * 检查两个迭代器是否指向不同的元素。
     * 这是相等操作符的否定。
     */
    bool operator!=(const deque_iterator& other) const noexcept {
        return cur_ != other.cur_;
    }
    
    /**
     * @brief Less-than operator
     * @brief 小于操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if this iterator is before other, false otherwise
     * @return 如果此迭代器在另一个之前返回true，否则返回false
     * 
     * @details
     * Checks if this iterator is before another iterator.
     * For deque iterators, the comparison first checks if both iterators
     * are in the same block. If they are, it compares the current pointers.
     * If they are in different blocks, it compares the block pointers.
     * 
     * @details
     * 检查此迭代器是否在另一个迭代器之前。
     * 对于deque迭代器，比较首先检查两个迭代器是否在同一块中。
     * 如果在同一块中，则比较当前指针。
     * 如果在不同块中，则比较块指针。
     */
    bool operator<(const deque_iterator& other) const noexcept {
        return (node_ == other.node_) ? (cur_ < other.cur_) : (node_ < other.node_);
    }
    
    /**
     * @brief Less-than-or-equal operator
     * @brief 小于等于操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if this iterator is not after other, false otherwise
     * @return 如果此迭代器不在另一个之后返回true，否则返回false
     * 
     * @details
     * Checks if this iterator is not after another iterator.
     * This is equivalent to !(other < *this).
     * 
     * @details
     * 检查此迭代器是否不在另一个迭代器之后。
     * 这等价于 !(other < *this)。
     */
    bool operator<=(const deque_iterator& other) const noexcept {
        return !(other < *this);
    }
    
    /**
     * @brief Greater-than operator
     * @brief 大于操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if this iterator is after other, false otherwise
     * @return 如果此迭代器在另一个之后返回true，否则返回false
     * 
     * @details
     * Checks if this iterator is after another iterator.
     * This is equivalent to other < *this.
     * 
     * @details
     * 检查此迭代器是否在另一个迭代器之后。
     * 这等价于 other < *this。
     */
    bool operator>(const deque_iterator& other) const noexcept {
        return other < *this;
    }
    
    /**
     * @brief Greater-than-or-equal operator
     * @brief 大于等于操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if this iterator is not before other, false otherwise
     * @return 如果此迭代器不在另一个之前返回true，否则返回false
     * 
     * @details
     * Checks if this iterator is not before another iterator.
     * This is equivalent to !(*this < other).
     * 
     * @details
     * 检查此迭代器是否不在另一个迭代器之前。
     * 这等价于 !(*this < other)。
     */
    bool operator>=(const deque_iterator& other) const noexcept {
        return !(*this < other);
    }
    
    /**
     * @brief Set the current block node
     * @brief 设置当前块节点
     * 
     * @param new_node Pointer to the new block in the map array
     * @param new_node 指向map数组中新块的指针
     * 
     * @details Updates the iterator to point to a different block in the map array.
     * This function is called when the iterator crosses a block boundary.
     * It updates the node pointer and recalculates the block boundaries.
     * 
     * @details 更新迭代器以指向map数组中的不同块。
     * 当迭代器跨越块边界时调用此函数。
     * 它更新节点指针并重新计算块边界。
     */
    void set_node(map_pointer new_node) noexcept {
        node_ = new_node;
        first_ = *new_node;
        last_ = first_ + difference_type(buffer_size());
    }
    
    /**
     * @brief Get the current element pointer
     * @brief 获取当前元素指针
     * 
     * @return Pointer to the current element
     * @return 指向当前元素的指针
     * 
     * @details Returns the pointer to the current element.
     * This function is for internal use by the deque class only.
     * 
     * @details 返回指向当前元素的指针。
     * 此函数仅供deque类内部使用。
     */
    T* current() const noexcept { return cur_; }
    
    /**
     * @brief Get the start pointer of the current block
     * @brief 获取当前块的起始指针
     * 
     * @return Pointer to the start of the current block
     * @return 指向当前块起始的指针
     * 
     * @details Returns the pointer to the start of the current block.
     * This function is for internal use by the deque class only.
     * 
     * @details 返回指向当前块起始的指针。
     * 此函数仅供deque类内部使用。
     */
    T* first() const noexcept { return first_; }
    
    /**
     * @brief Get the end pointer of the current block
     * @brief 获取当前块的结束指针
     * 
     * @return Pointer to the end of the current block (one past last element)
     * @return 指向当前块结束的指针（最后一个元素的下一个位置）
     * 
     * @details Returns the pointer to the end of the current block.
     * This function is for internal use by the deque class only.
     * 
     * @details 返回指向当前块结束的指针。
     * 此函数仅供deque类内部使用。
     */
    T* last() const noexcept { return last_; }
    
    /**
     * @brief Get the current block pointer in the map array
     * @brief 获取map数组中的当前块指针
     * 
     * @return Pointer to the current block in the map array
     * @return 指向map数组中当前块的指针
     * 
     * @details Returns the pointer to the current block in the map array.
     * This function is for internal use by the deque class only.
     * 
     * @details 返回指向map数组中当前块的指针。
     * 此函数仅供deque类内部使用。
     */
    map_pointer node() const noexcept { return node_; }
    
private:
    /**
     * @brief Calculate buffer size based on element type size
     * @brief 基于元素类型大小计算缓冲区大小
     * 
     * @return Buffer size in number of elements
     * @return 以元素个数表示的缓冲区大小
     * 
     * @details
     * The buffer size is determined by the size of the element type.
     * Smaller elements use larger buffers to reduce memory overhead
     * from map array management. The sizes are chosen to balance
     * memory efficiency and performance.
     * 
     * @details
     * 缓冲区大小由元素类型的大小决定。
     * 较小的元素使用较大的缓冲区以减少map数组管理的内存开销。
     * 选择这些大小是为了平衡内存效率和性能。
     * 
     * @note Buffer sizes:
     * - sizeof(T) <= 1: 512 elements
     * - sizeof(T) <= 2: 256 elements
     * - sizeof(T) <= 4: 128 elements
     * - sizeof(T) <= 8: 64 elements
     * - sizeof(T) <= 16: 32 elements
     * - sizeof(T) <= 32: 16 elements
     * - sizeof(T) <= 64: 8 elements
     * - sizeof(T) <= 128: 4 elements
     * - sizeof(T) <= 256: 2 elements
     * - otherwise: 1 element
     * 
     * @note 缓冲区大小：
     * - sizeof(T) <= 1: 512个元素
     * - sizeof(T) <= 2: 256个元素
     * - sizeof(T) <= 4: 128个元素
     * - sizeof(T) <= 8: 64个元素
     * - sizeof(T) <= 16: 32个元素
     * - sizeof(T) <= 32: 16个元素
     * - sizeof(T) <= 64: 8个元素
     * - sizeof(T) <= 128: 4个元素
     * - sizeof(T) <= 256: 2个元素
     * - 其他情况: 1个元素
     */
    static size_t buffer_size() noexcept {
        return sizeof(T) <= 1 ? 512 :
               sizeof(T) <= 2 ? 256 :
               sizeof(T) <= 4 ? 128 :
               sizeof(T) <= 8 ? 64 :
               sizeof(T) <= 16 ? 32 :
               sizeof(T) <= 32 ? 16 :
               sizeof(T) <= 64 ? 8 :
               sizeof(T) <= 128 ? 4 :
               sizeof(T) <= 256 ? 2 : 1;
    }
};

/**
 * @brief Non-member addition operator for deque_iterator
 * @brief deque_iterator的非成员加法操作符
 * 
 * @tparam T Type of elements in the deque
 * @tparam T deque中元素的类型
 * @param n Number of positions to advance
 * @param n 要前进的位置数
 * @param it Iterator to advance from
 * @param it 要从此前进的迭代器
 * @return New iterator advanced by n positions from it
 * @return 从it前进n个位置的新迭代器
 * 
 * @details
 * Returns a new iterator that is advanced by n positions from the given iterator.
 * This operator allows writing expressions like `it = 5 + it` in addition to `it = it + 5`.
 * 
 * @details
 * 返回从给定迭代器前进n个位置的新迭代器。
 * 此操作符允许编写像`it = 5 + it`这样的表达式，而不仅仅是`it = it + 5`。
 * 
 * @note Time complexity: O(1) for small n, O(n/buffer_size) for large n
 * @note 时间复杂度：对于小的n为O(1)，对于大的n为O(n/buffer_size)
 */
template<typename T>
deque_iterator<T> operator+(typename deque_iterator<T>::difference_type n,
                           const deque_iterator<T>& it) noexcept {
    return it + n;
}

// ==================== deque Class ====================
// ==================== deque 类 ====================

/**
 * @brief Double-ended queue container
 * @brief 双端队列容器
 * 
 * @tparam T Type of elements stored in the deque
 * @tparam T 存储在deque中的元素类型
 * @tparam Allocator Allocator type for memory management
 * @tparam Allocator 用于内存管理的分配器类型
 * 
 * @details
 * The deque (double-ended queue) is a sequence container that supports
 * efficient insertion and deletion at both ends, as well as random access
 * to elements. Unlike vector, deque does not guarantee that elements are
 * stored contiguously in memory. Instead, it uses a segmented array
 * (block-based) approach to provide O(1) insertion/removal at both ends
 * while maintaining random access capability.
 * 
 * @details
 * deque（双端队列）是一个序列容器，支持在两端高效插入和删除，
 * 以及对元素的随机访问。与vector不同，deque不保证元素在内存中连续存储。
 * 相反，它使用分段数组（基于块）的方法，在保持随机访问能力的同时，
 * 提供O(1)复杂度的两端插入/删除操作。
 * 
 * @note Key features:
 * 1. Random access: O(1) access to any element
 * 2. Efficient insertion/removal at both ends: amortized O(1)
 * 3. Non-contiguous storage: elements are stored in multiple fixed-size blocks
 * 4. Dynamic growth: can grow efficiently in both directions
 * 
 * @note 主要特性：
 * 1. 随机访问：O(1)访问任何元素
 * 2. 两端高效插入/删除：摊还O(1)复杂度
 * 3. 非连续存储：元素存储在多个固定大小的块中
 * 4. 动态增长：可以在两个方向上高效增长
 * 
 * @see https://en.cppreference.com/w/cpp/container/deque
 * @see 相关参考：https://zh.cppreference.com/w/cpp/container/deque
 */
template<typename T, typename Allocator = allocator<T>>
class deque {
public:
    // 类型定义
    using value_type = T;                                   ///< Type of elements stored in the deque / 存储在deque中的元素类型
    using allocator_type = Allocator;                       ///< Type of allocator used for memory management / 用于内存管理的分配器类型
    using size_type = std::size_t;                          ///< Type for size-related operations / 用于大小相关操作的类型
    using difference_type = std::ptrdiff_t;                 ///< Type for difference between iterators / 用于迭代器之间差值的类型
    using reference = value_type&;                          ///< Reference type to elements / 元素的引用类型
    using const_reference = const value_type&;              ///< Const reference type to elements / 元素的常量引用类型
    using pointer = typename allocator_traits<Allocator>::pointer;          ///< Pointer type to elements / 元素的指针类型
    using const_pointer = typename allocator_traits<Allocator>::const_pointer; ///< Const pointer type to elements / 元素的常量指针类型
    
    // 迭代器类型
    using iterator = deque_iterator<T>;                     ///< Iterator type for deque / deque的迭代器类型
    using const_iterator = deque_iterator<const T>;         ///< Const iterator type for deque / deque的常量迭代器类型
    using reverse_iterator = mystl::reverse_iterator<iterator>;      ///< Reverse iterator type for deque / deque的反向迭代器类型
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>; ///< Const reverse iterator type for deque / deque的常量反向迭代器类型
    
private:
    using map_pointer = T**;                                    ///< Type of map array pointer / map数组指针类型
    using alloc_traits = allocator_traits<Allocator>;           ///< Allocator traits type / 分配器特性类型
    
    // 核心数据成员
    map_pointer map_;           ///< Pointer array to memory blocks / 内存块指针数组
    size_type map_size_;        ///< Size of the map array / map数组大小
    size_type start_;           ///< Index of the first block containing valid elements / 包含有效元素的第一个块的索引
    size_type finish_;          ///< Index of the block after the last valid element / 最后一个有效元素之后的块的索引
    size_type start_offset_;    ///< Offset within the first block for the first element / 第一个块内第一个元素的偏移
    size_type finish_offset_;   ///< Offset within the last block for the position after the last element / 最后一个块内最后一个元素之后位置的偏移
    size_type size_;            ///< Number of elements in the deque / deque中的元素数量
    Allocator alloc_;           ///< Allocator instance / 分配器实例
    
    // 常量
    /**
     * @brief Buffer size based on element type size
     * @brief 基于元素类型大小的缓冲区大小
     * 
     * @details The buffer size is determined by the size of the element type.
     * Smaller elements use larger buffers to reduce memory overhead.
     * 
     * @details 缓冲区大小由元素类型的大小决定。
     * 较小的元素使用较大的缓冲区以减少内存开销。
     */
    static const size_type buffer_size = sizeof(T) <= 1 ? 512 :
                                         sizeof(T) <= 2 ? 256 :
                                         sizeof(T) <= 4 ? 128 :
                                         sizeof(T) <= 8 ? 64 :
                                         sizeof(T) <= 16 ? 32 :
                                         sizeof(T) <= 32 ? 16 :
                                         sizeof(T) <= 64 ? 8 :
                                         sizeof(T) <= 128 ? 4 :
                                         sizeof(T) <= 256 ? 2 : 1;
    
public:
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs an empty deque with no elements.
     * The deque will have no allocated memory until the first element is inserted.
     * 
     * @details 构造一个没有元素的空deque。
     * 在插入第一个元素之前，deque不会有任何分配的内存。
     */
    deque() noexcept(noexcept(Allocator()))
        : map_(nullptr), map_size_(0), start_(0), finish_(0),
          start_offset_(0), finish_offset_(0), size_(0), alloc_() {}
    
    /**
     * @brief Constructor with allocator
     * @brief 带分配器的构造函数
     * 
     * @param alloc Allocator to use for all memory allocations
     * @param alloc 用于所有内存分配的分配器
     * 
     * @details Constructs an empty deque with the specified allocator.
     * 
     * @details 使用指定的分配器构造一个空deque。
     */
    explicit deque(const Allocator& alloc) noexcept
        : map_(nullptr), map_size_(0), start_(0), finish_(0),
          start_offset_(0), finish_offset_(0), size_(0), alloc_(alloc) {}
    
    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other Another deque to copy from
     * @param other 要拷贝的另一个deque
     * 
     * @details
     * Constructs a deque with a copy of each of the elements in other,
     * in the same order. The allocator is obtained by calling
     * alloc_traits::select_on_container_copy_construction on other's allocator.
     * 
     * @details
     * 构造一个deque，其中包含other中每个元素的副本，顺序相同。
     * 分配器通过调用alloc_traits::select_on_container_copy_construction
     * 从other的分配器获取。
     * 
     * @note Time complexity: O(n) where n is the number of elements in other
     * @note 时间复杂度：O(n)，其中n是other中的元素数量
     */
    deque(const deque& other)
        : alloc_(alloc_traits::select_on_container_copy_construction(other.alloc_)) {
        create_map(other.size_);
        start_ = map_size_ / 2;
        finish_ = start_;
        start_offset_ = 0;
        finish_offset_ = 0;
        size_ = 0;
        
        try {
            for (const_iterator it = other.begin(); it != other.end(); ++it) {
                push_back(*it);
            }
        } catch (...) {
            clear();
            destroy_map();
            throw;
        }
    }
    
    /**
     * @brief Copy constructor with allocator
     * @brief 带分配器的拷贝构造函数
     * 
     * @param other Another deque to copy from
     * @param other 要拷贝的另一个deque
     * @param alloc Allocator to use for all memory allocations
     * @param alloc 用于所有内存分配的分配器
     * 
     * @details
     * Constructs a deque with a copy of each of the elements in other,
     * in the same order, using the specified allocator.
     * 
     * @details
     * 使用指定的分配器构造一个deque，其中包含other中每个元素的副本，顺序相同。
     * 
     * @note Time complexity: O(n) where n is the number of elements in other
     * @note 时间复杂度：O(n)，其中n是other中的元素数量
     */
    deque(const deque& other, const Allocator& alloc)
        : alloc_(alloc) {
        create_map(other.size_);
        start_ = map_size_ / 2;
        finish_ = start_;
        start_offset_ = 0;
        finish_offset_ = 0;
        size_ = 0;
        
        try {
            for (const_iterator it = other.begin(); it != other.end(); ++it) {
                push_back(*it);
            }
        } catch (...) {
            clear();
            destroy_map();
            throw;
        }
    }
    
    /**
     * @brief Move constructor
     * @brief 移动构造函数
     * 
     * @param other Another deque to move from
     * @param other 要移动的另一个deque
     * 
     * @details
     * Constructs a deque by moving elements from another deque.
     * After the move, other is left in a valid but unspecified state.
     * 
     * @details
     * 通过从另一个deque移动元素来构造deque。
     * 移动后，other处于有效但未指定的状态。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    deque(deque&& other) noexcept
        : map_(other.map_), map_size_(other.map_size_),
          start_(other.start_), finish_(other.finish_),
          start_offset_(other.start_offset_), finish_offset_(other.finish_offset_),
          size_(other.size_), alloc_(mystl::move(other.alloc_)) {
        other.map_ = nullptr;
        other.map_size_ = 0;
        other.start_ = 0;
        other.finish_ = 0;
        other.start_offset_ = 0;
        other.finish_offset_ = 0;
        other.size_ = 0;
    }
    
    /**
     * @brief Move constructor with allocator
     * @brief 带分配器的移动构造函数
     * 
     * @param other Another deque to move from
     * @param other 要移动的另一个deque
     * @param alloc Allocator to use for the new deque
     * @param alloc 用于新deque的分配器
     * 
     * @details
     * Constructs a deque by moving elements from another deque.
     * If the provided allocator is equal to other's allocator,
     * the operation is a true move (O(1) complexity).
     * Otherwise, elements are copied one by one.
     * 
     * @details
     * 通过从另一个deque移动元素来构造deque。
     * 如果提供的分配器与other的分配器相等，
     * 则操作是真正的移动（O(1)复杂度）。
     * 否则，元素将被逐个拷贝。
     * 
     * @note Time complexity: O(1) if allocators are equal, O(n) otherwise
     * @note 时间复杂度：如果分配器相等则为O(1)，否则为O(n)
     */
    deque(deque&& other, const Allocator& alloc)
        : alloc_(alloc) {
        if (alloc_ == other.alloc_) {
            // 如果分配器相等，可以移动资源
            map_ = other.map_;
            map_size_ = other.map_size_;
            start_ = other.start_;
            finish_ = other.finish_;
            start_offset_ = other.start_offset_;
            finish_offset_ = other.finish_offset_;
            size_ = other.size_;
            
            other.map_ = nullptr;
            other.map_size_ = 0;
            other.start_ = 0;
            other.finish_ = 0;
            other.start_offset_ = 0;
            other.finish_offset_ = 0;
            other.size_ = 0;
        } else {
            // 分配器不相等，需要拷贝
            create_map(other.size_);
            start_ = map_size_ / 2;
            finish_ = start_;
            start_offset_ = 0;
            finish_offset_ = 0;
            size_ = 0;
            
            try {
                for (const_iterator it = other.begin(); it != other.end(); ++it) {
                    push_back(*it);
                }
            } catch (...) {
                clear();
                destroy_map();
                throw;
            }
        }
    }
    
    /**
     * @brief Constructs a deque with specified number of elements
     * @brief 构造具有指定数量元素的deque
     * 
     * @param count Number of elements to initialize with
     * @param count 要初始化的元素数量
     * @param value Value to initialize each element with (default: T())
     * @param value 用于初始化每个元素的值（默认：T()）
     * @param alloc Allocator to use for all memory allocations (default: Allocator())
     * @param alloc 用于所有内存分配的分配器（默认：Allocator()）
     * 
     * @details
     * Constructs a deque with count copies of value.
     * The deque is initialized with count elements, each initialized as a copy of value.
     * 
     * @details
     * 构造一个deque，其中包含count个value的副本。
     * deque被初始化为count个元素，每个元素初始化为value的副本。
     * 
     * @note Time complexity: O(count)
     * @note 时间复杂度：O(count)
     * 
     * @note Exception safety: Strong guarantee - if an exception is thrown,
     * no memory is leaked and the deque remains in a valid state.
     * 
     * @note 异常安全性：强保证 - 如果抛出异常，
     * 不会发生内存泄漏，deque保持有效状态。
     */
    explicit deque(size_type count, const T& value = T(), const Allocator& alloc = Allocator())
        : alloc_(alloc) {
        create_map(count);
        start_ = map_size_ / 2;
        finish_ = start_;
        start_offset_ = 0;
        finish_offset_ = 0;
        size_ = 0;
        
        try {
            for (size_type i = 0; i < count; ++i) {
                push_back(value);
            }
        } catch (...) {
            clear();
            destroy_map();
            throw;
        }
    }
    
    /**
     * @brief Constructs a deque with elements from a range
     * @brief 使用范围中的元素构造deque
     * 
     * @tparam InputIt Type of input iterator
     * @tparam InputIt 输入迭代器类型
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to the element after the last element in the range
     * @param last 指向范围中最后一个元素之后元素的迭代器
     * @param alloc Allocator to use for all memory allocations (default: Allocator())
     * @param alloc 用于所有内存分配的分配器（默认：Allocator()）
     * 
     * @details
     * Constructs a deque with elements from the range [first, last).
     * For random access iterators, the size is calculated directly for efficiency.
     * For non-random access iterators, the range is traversed twice:
     * once to calculate the size, and once to copy the elements.
     * 
     * @details
     * 使用范围[first, last)中的元素构造deque。
     * 对于随机访问迭代器，为提高效率直接计算大小。
     * 对于非随机访问迭代器，范围被遍历两次：
     * 一次计算大小，一次拷贝元素。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the range
     * @note 时间复杂度：O(n)，其中n是范围中的元素数量
     * 
     * @note Exception safety: Strong guarantee - if an exception is thrown,
     * no memory is leaked and the deque remains in a valid state.
     * 
     * @note 异常安全性：强保证 - 如果抛出异常，
     * 不会发生内存泄漏，deque保持有效状态。
     */
    template<typename InputIt, typename = typename mystl::enable_if<
        !mystl::is_integral<InputIt>::value>::type>
    deque(InputIt first, InputIt last, const Allocator& alloc = Allocator())
        : alloc_(alloc) {
        // 计算距离（如果是随机访问迭代器）
        size_type count = 0;
        if constexpr (mystl::is_random_access_iterator<InputIt>::value) {
            count = last - first;
            create_map(count);
        } else {
            // 对于非随机访问迭代器，先遍历一次计算大小
            InputIt it = first;
            while (it != last) {
                ++count;
                ++it;
            }
            create_map(count);
        }
        
        start_ = map_size_ / 2;
        finish_ = start_;
        start_offset_ = 0;
        finish_offset_ = 0;
        size_ = 0;
        
        try {
            for (; first != last; ++first) {
                push_back(*first);
            }
        } catch (...) {
            clear();
            destroy_map();
            throw;
        }
    }
    
    /**
     * @brief Constructs a deque with elements from an initializer list
     * @brief 使用初始化列表中的元素构造deque
     * 
     * @param init Initializer list containing elements to initialize with
     * @param init 包含要初始化元素的初始化列表
     * @param alloc Allocator to use for all memory allocations (default: Allocator())
     * @param alloc 用于所有内存分配的分配器（默认：Allocator()）
     * 
     * @details
     * Constructs a deque with elements from the initializer list.
     * The deque is initialized with the elements from the initializer list,
     * in the same order as they appear in the list.
     * 
     * @details
     * 使用初始化列表中的元素构造deque。
     * deque使用初始化列表中的元素进行初始化，
     * 元素顺序与它们在列表中出现的顺序相同。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the list
     * @note 时间复杂度：O(n)，其中n是列表中的元素数量
     * 
     * @note Exception safety: Strong guarantee - if an exception is thrown,
     * no memory is leaked and the deque remains in a valid state.
     * 
     * @note 异常安全性：强保证 - 如果抛出异常，
     * 不会发生内存泄漏，deque保持有效状态。
     */
    deque(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : alloc_(alloc) {
        size_type count = init.size();
        if (count > 0) {
            create_map(count);
            start_ = map_size_ / 2;
            finish_ = start_;
            start_offset_ = 0;
            finish_offset_ = 0;
            size_ = 0;
            
            try {
                for (const T& value : init) {
                    push_back(value);
                }
            } catch (...) {
                clear();
                destroy_map();
                throw;
            }
        } else {
            map_ = nullptr;
            map_size_ = 0;
            start_ = 0;
            finish_ = 0;
            start_offset_ = 0;
            finish_offset_ = 0;
            size_ = 0;
        }
    }
    
    /**
     * @brief Destructor
     * @brief 析构函数
     * 
     * @details
     * Destroys the deque, deallocating all memory.
     * All elements are destroyed and all memory is deallocated.
     * 
     * @details
     * 销毁deque，释放所有内存。
     * 所有元素被销毁，所有内存被释放。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note 时间复杂度：O(n)，其中n是元素数量
     * 
     * @note Exception safety: noexcept
     * @note 异常安全性：noexcept
     */
    ~deque() {
        clear();
        if (map_) {
            destroy_map();
        }
    }
    
    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值操作符
     * 
     * @param other Another deque to assign from
     * @param other 要赋值的另一个deque
     * @return Reference to this deque
     * @return 此deque的引用
     * 
     * @details
     * Replaces the contents of this deque with a copy of the contents of other.
     * Uses the copy-and-swap idiom for strong exception safety.
     * 
     * @details
     * 用other内容的副本替换此deque的内容。
     * 使用拷贝并交换技术以获得强异常安全性。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is other's size
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是other的大小
     * 
     * @note Exception safety: Strong guarantee
     * @note 异常安全性：强保证
     */
    deque& operator=(const deque& other) {
        if (this != &other) {
            // 使用拷贝并交换技术
            deque temp(other);
            swap(temp);
        }
        return *this;
    }
    
    /**
     * @brief Move assignment operator
     * @brief 移动赋值操作符
     * 
     * @param other Another deque to move from
     * @param other 要移动的另一个deque
     * @return Reference to this deque
     * @return 此deque的引用
     * 
     * @details
     * Replaces the contents of this deque with those of other using move semantics.
     * After the move, other is left in a valid but unspecified state.
     * 
     * @details
     * 使用移动语义用other的内容替换此deque的内容。
     * 移动后，other处于有效但未指定的状态。
     * 
     * @note Time complexity: O(n) where n is the current size
     * @note 时间复杂度：O(n)，其中n是当前大小
     * 
     * @note Exception safety: noexcept
     * @note 异常安全性：noexcept
     */
    deque& operator=(deque&& other) noexcept {
        if (this != &other) {
            // 清理当前资源
            clear();
            if (map_) {
                destroy_map();
            }
            
            // 移动资源
            map_ = other.map_;
            map_size_ = other.map_size_;
            start_ = other.start_;
            finish_ = other.finish_;
            start_offset_ = other.start_offset_;
            finish_offset_ = other.finish_offset_;
            size_ = other.size_;
            
            if (alloc_traits::propagate_on_container_move_assignment::value) {
                alloc_ = mystl::move(other.alloc_);
            }
            
            // 重置other
            other.map_ = nullptr;
            other.map_size_ = 0;
            other.start_ = 0;
            other.finish_ = 0;
            other.start_offset_ = 0;
            other.finish_offset_ = 0;
            other.size_ = 0;
        }
        return *this;
    }
    
    /**
     * @brief Initializer list assignment operator
     * @brief 初始化列表赋值操作符
     * 
     * @param ilist Initializer list containing elements to assign
     * @param ilist 包含要赋值元素的初始化列表
     * @return Reference to this deque
     * @return 此deque的引用
     * 
     * @details
     * Replaces the contents of this deque with elements from the initializer list.
     * This operator is equivalent to assign(ilist.begin(), ilist.end()).
     * 
     * @details
     * 用初始化列表中的元素替换此deque的内容。
     * 此操作符等价于assign(ilist.begin(), ilist.end())。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is the list size
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是列表大小
     * 
     * @note Exception safety: Strong guarantee
     * @note 异常安全性：强保证
     */
    deque& operator=(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
        return *this;
    }
    
    // ==================== Capacity Management ====================
    // ==================== 容量管理 ====================
    
    /**
     * @brief Check if the deque is empty
     * @brief 检查deque是否为空
     * 
     * @return true if the deque is empty, false otherwise
     * @return 如果deque为空返回true，否则返回false
     * 
     * @details
     * Returns whether the deque is empty (i.e., whether its size is 0).
     * This function does not modify the container in any way.
     * 
     * @details
     * 返回deque是否为空（即其大小是否为0）。
     * 此函数不会以任何方式修改容器。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    bool empty() const noexcept { return size_ == 0; }
    
    /**
     * @brief Get the number of elements in the deque
     * @brief 获取deque中的元素数量
     * 
     * @return Number of elements in the deque
     * @return deque中的元素数量
     * 
     * @details
     * Returns the number of elements in the deque.
     * This is the number of actual objects held in the deque,
     * which is not necessarily equal to its storage capacity.
     * 
     * @details
     * 返回deque中的元素数量。
     * 这是deque中实际持有的对象数量，
     * 不一定等于其存储容量。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    size_type size() const noexcept { return size_; }
    
    /**
     * @brief Get the maximum possible number of elements
     * @brief 获取可能的最大元素数量
     * 
     * @return Maximum number of elements the deque can hold
     * @return deque可以容纳的最大元素数量
     * 
     * @details
     * Returns the maximum number of elements that the deque can hold.
     * This value typically reflects the theoretical limit on the size
     * of the container, due to system or library implementation limitations.
     * 
     * @details
     * 返回deque可以容纳的最大元素数量。
     * 此值通常反映了由于系统或库实现限制，
     * 容器大小的理论限制。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    size_type max_size() const noexcept { return alloc_traits::max_size(alloc_); }
    
    /**
     * @brief Reduce memory usage by freeing unused memory
     * @brief 通过释放未使用的内存来减少内存使用
     * 
     * @details
     * Requests the container to reduce its memory usage to fit its size.
     * This function is a non-binding request to reduce memory usage.
     * The container implementation is free to optimize otherwise and
     * leave the deque with a capacity greater than its size.
     * 
     * @details
     * 请求容器减少其内存使用以适应其大小。
     * 此函数是一个非绑定请求，用于减少内存使用。
     * 容器实现可以自由进行其他优化，
     * 并让deque的容量大于其大小。
     * 
     * @note
     * This implementation attempts to reduce the map array size if it's
     * significantly larger than needed, but does not shrink individual
     * buffers. This matches typical standard library behavior.
     * 
     * @note
     * 此实现尝试在map数组比需要的大很多时减少其大小，
     * 但不会缩小单个缓冲区。这符合典型的标准库行为。
     * 
     * @note Time complexity: O(n) where n is the number of blocks
     * @note 时间复杂度：O(n)，其中n是块的数量
     */
    void shrink_to_fit() {
        // 计算实际需要的map大小
        size_type num_nodes = finish_ - start_ + 1;
        if (num_nodes == 0) {
            // 空deque，释放所有内存
            if (map_) {
                destroy_map();
                map_ = nullptr;
                map_size_ = 0;
                start_ = 0;
                finish_ = 0;
                start_offset_ = 0;
                finish_offset_ = 0;
            }
            return;
        }
        
        // 如果map数组比实际需要的大很多，重新分配
        if (map_size_ > 2 * num_nodes + 2) {
            size_type new_map_size = num_nodes + 2;
            map_pointer new_map = allocate_map(new_map_size);
            map_pointer new_start = new_map + (new_map_size - num_nodes) / 2;
            
            // 复制原有指针
            for (size_type i = 0; i < num_nodes; ++i) {
                new_start[i] = map_[start_ + i];
            }
            
            // 释放旧map数组
            deallocate_map(map_, map_size_);
            map_ = new_map;
            map_size_ = new_map_size;
            start_ = new_start - map_;
            finish_ = start_ + num_nodes - 1;
        }
        
        // 对于两端的空块，可以释放
        // 注意：标准库的shrink_to_fit通常不保证释放所有未使用的内存
        // 这里我们只优化map数组，不优化缓冲区本身
    }
    
    // ==================== Iterators ====================
    // ==================== 迭代器 ====================
    
    /**
     * @brief Get iterator to the beginning
     * @brief 获取指向开头的迭代器
     * 
     * @return Iterator pointing to the first element in the deque
     * @return 指向deque中第一个元素的迭代器
     * 
     * @details
     * Returns an iterator pointing to the first element in the deque.
     * If the deque is empty, the returned iterator will be equal to end().
     * 
     * @details
     * 返回指向deque中第一个元素的迭代器。
     * 如果deque为空，返回的迭代器将等于end()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    iterator begin() noexcept {
        if (!map_ || size_ == 0) return iterator();
        return iterator(map_[start_] + start_offset_, map_[start_], map_[start_] + buffer_size, map_ + start_);
    }
    
    /**
     * @brief Get const iterator to the beginning
     * @brief 获取指向开头的const迭代器
     * 
     * @return Const iterator pointing to the first element in the deque
     * @return 指向deque中第一个元素的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the first element in the deque.
     * If the deque is empty, the returned iterator will be equal to end().
     * 
     * @details
     * 返回指向deque中第一个元素的const迭代器。
     * 如果deque为空，返回的迭代器将等于end()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator begin() const noexcept {
        if (!map_ || size_ == 0) return const_iterator();
        return const_iterator(map_[start_] + start_offset_, map_[start_], map_[start_] + buffer_size, map_ + start_);
    }
    
    /**
     * @brief Get iterator to the end
     * @brief 获取指向结尾的迭代器
     * 
     * @return Iterator pointing to the past-the-end element in the deque
     * @return 指向deque中最后一个元素之后位置的迭代器
     * 
     * @details
     * Returns an iterator pointing to the past-the-end element in the deque.
     * The past-the-end element is the theoretical element that would follow
     * the last element in the deque. It does not point to any element, and
     * thus shall not be dereferenced.
     * 
     * @details
     * 返回指向deque中最后一个元素之后位置的迭代器。
     * 最后一个元素之后位置是deque中最后一个元素之后的理论元素。
     * 它不指向任何元素，因此不应被解引用。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    iterator end() noexcept {
        if (!map_ || size_ == 0) return iterator();
        return iterator(map_[finish_] + finish_offset_, map_[finish_], map_[finish_] + buffer_size, map_ + finish_);
    }
    
    /**
     * @brief Get const iterator to the end
     * @brief 获取指向结尾的const迭代器
     * 
     * @return Const iterator pointing to the past-the-end element in the deque
     * @return 指向deque中最后一个元素之后位置的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the past-the-end element in the deque.
     * The past-the-end element is the theoretical element that would follow
     * the last element in the deque. It does not point to any element, and
     * thus shall not be dereferenced.
     * 
     * @details
     * 返回指向deque中最后一个元素之后位置的const迭代器。
     * 最后一个元素之后位置是deque中最后一个元素之后的理论元素。
     * 它不指向任何元素，因此不应被解引用。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator end() const noexcept {
        if (!map_ || size_ == 0) return const_iterator();
        return const_iterator(map_[finish_] + finish_offset_, map_[finish_], map_[finish_] + buffer_size, map_ + finish_);
    }
    
    /**
     * @brief Get const iterator to the beginning
     * @brief 获取指向开头的const迭代器
     * 
     * @return Const iterator pointing to the first element in the deque
     * @return 指向deque中第一个元素的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the first element in the deque.
     * This function is provided for compatibility with C++11's cbegin().
     * 
     * @details
     * 返回指向deque中第一个元素的const迭代器。
     * 此函数为兼容C++11的cbegin()而提供。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator cbegin() const noexcept {
        return begin();
    }
    
    /**
     * @brief Get const iterator to the end
     * @brief 获取指向结尾的const迭代器
     * 
     * @return Const iterator pointing to the past-the-end element in the deque
     * @return 指向deque中最后一个元素之后位置的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the past-the-end element in the deque.
     * This function is provided for compatibility with C++11's cend().
     * 
     * @details
     * 返回指向deque中最后一个元素之后位置的const迭代器。
     * 此函数为兼容C++11的cend()而提供。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator cend() const noexcept {
        return end();
    }
    
    /**
     * @brief Get reverse iterator to the beginning
     * @brief 获取指向开头的反向迭代器
     * 
     * @return Reverse iterator pointing to the last element in the deque
     * @return 指向deque中最后一个元素的反向迭代器
     * 
     * @details
     * Returns a reverse iterator pointing to the last element in the deque.
     * Reverse iterators iterate from the end to the beginning of the deque.
     * 
     * @details
     * 返回指向deque中最后一个元素的反向迭代器。
     * 反向迭代器从deque的末尾向开头迭代。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    
    /**
     * @brief Get const reverse iterator to the beginning
     * @brief 获取指向开头的const反向迭代器
     * 
     * @return Const reverse iterator pointing to the last element in the deque
     * @return 指向deque中最后一个元素的const反向迭代器
     * 
     * @details
     * Returns a const reverse iterator pointing to the last element in the deque.
     * Reverse iterators iterate from the end to the beginning of the deque.
     * 
     * @details
     * 返回指向deque中最后一个元素的const反向迭代器。
     * 反向迭代器从deque的末尾向开头迭代。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    
    /**
     * @brief Get reverse iterator to the end
     * @brief 获取指向结尾的反向迭代器
     * 
     * @return Reverse iterator pointing to the element before the first element
     * @return 指向第一个元素之前元素的反向迭代器
     * 
     * @details
     * Returns a reverse iterator pointing to the element before the first element.
     * This is the past-the-end element for reverse iteration.
     * 
     * @details
     * 返回指向第一个元素之前元素的反向迭代器。
     * 这是反向迭代的最后一个元素之后位置。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    
    /**
     * @brief Get const reverse iterator to the end
     * @brief 获取指向结尾的const反向迭代器
     * 
     * @return Const reverse iterator pointing to the element before the first element
     * @return 指向第一个元素之前元素的const反向迭代器
     * 
     * @details
     * Returns a const reverse iterator pointing to the element before the first element.
     * This is the past-the-end element for reverse iteration.
     * 
     * @details
     * 返回指向第一个元素之前元素的const反向迭代器。
     * 这是反向迭代的最后一个元素之后位置。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    
    /**
     * @brief Get const reverse iterator to the beginning
     * @brief 获取指向开头的const反向迭代器
     * 
     * @return Const reverse iterator pointing to the last element in the deque
     * @return 指向deque中最后一个元素的const反向迭代器
     * 
     * @details
     * Returns a const reverse iterator pointing to the last element in the deque.
     * This function is provided for compatibility with C++11's crbegin().
     * 
     * @details
     * 返回指向deque中最后一个元素的const反向迭代器。
     * 此函数为兼容C++11的crbegin()而提供。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }
    
    /**
     * @brief Get const reverse iterator to the end
     * @brief 获取指向结尾的const反向迭代器
     * 
     * @return Const reverse iterator pointing to the element before the first element
     * @return 指向第一个元素之前元素的const反向迭代器
     * 
     * @details
     * Returns a const reverse iterator pointing to the element before the first element.
     * This function is provided for compatibility with C++11's crend().
     * 
     * @details
     * 返回指向第一个元素之前元素的const反向迭代器。
     * 此函数为兼容C++11的crend()而提供。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_reverse_iterator crend() const noexcept {
        return rend();
    }
    
    // ==================== Element Access ====================
    // ==================== 元素访问 ====================
    
    /**
     * @brief Access specified element without bounds checking
     * @brief 访问指定元素（无边界检查）
     * 
     * @param n Position of the element to access
     * @param n 要访问的元素位置
     * @return Reference to the requested element
     * @return 对请求元素的引用
     * 
     * @details
     * Returns a reference to the element at specified location n.
     * No bounds checking is performed. Accessing an out-of-range
     * element causes undefined behavior.
     * 
     * @details
     * 返回指定位置n处元素的引用。
     * 不执行边界检查。访问超出范围的元素会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    reference operator[](size_type n) noexcept {
        return *(begin() + static_cast<difference_type>(n));
    }
    
    /**
     * @brief Access specified element without bounds checking (const version)
     * @brief 访问指定元素（无边界检查，const版本）
     * 
     * @param n Position of the element to access
     * @param n 要访问的元素位置
     * @return Const reference to the requested element
     * @return 对请求元素的const引用
     * 
     * @details
     * Returns a const reference to the element at specified location n.
     * No bounds checking is performed. Accessing an out-of-range
     * element causes undefined behavior.
     * 
     * @details
     * 返回指定位置n处元素的const引用。
     * 不执行边界检查。访问超出范围的元素会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_reference operator[](size_type n) const noexcept {
        return *(begin() + static_cast<difference_type>(n));
    }
    
    /**
     * @brief Access specified element with bounds checking
     * @brief 访问指定元素（带边界检查）
     * 
     * @param n Position of the element to access
     * @param n 要访问的元素位置
     * @return Reference to the requested element
     * @return 对请求元素的引用
     * 
     * @throws mystl::out_of_range if n is out of range
     * @throws 如果n超出范围，抛出mystl::out_of_range异常
     * 
     * @details
     * Returns a reference to the element at specified location n,
     * with bounds checking. If n is not within the range of the
     * container, an out_of_range exception is thrown.
     * 
     * @details
     * 返回指定位置n处元素的引用，带边界检查。
     * 如果n不在容器范围内，抛出out_of_range异常。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    reference at(size_type n) {
        if (n >= size_) {
            throw mystl::out_of_range("deque::at");
        }
        return (*this)[n];
    }
    
    /**
     * @brief Access specified element with bounds checking (const version)
     * @brief 访问指定元素（带边界检查，const版本）
     * 
     * @param n Position of the element to access
     * @param n 要访问的元素位置
     * @return Const reference to the requested element
     * @return 对请求元素的const引用
     * 
     * @throws mystl::out_of_range if n is out of range
     * @throws 如果n超出范围，抛出mystl::out_of_range异常
     * 
     * @details
     * Returns a const reference to the element at specified location n,
     * with bounds checking. If n is not within the range of the
     * container, an out_of_range exception is thrown.
     * 
     * @details
     * 返回指定位置n处元素的const引用，带边界检查。
     * 如果n不在容器范围内，抛出out_of_range异常。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_reference at(size_type n) const {
        if (n >= size_) {
            throw mystl::out_of_range("deque::at");
        }
        return (*this)[n];
    }
    
    /**
     * @brief Access the first element
     * @brief 访问第一个元素
     * 
     * @return Reference to the first element
     * @return 对第一个元素的引用
     * 
     * @details
     * Returns a reference to the first element in the deque.
     * Calling front() on an empty deque causes undefined behavior.
     * 
     * @details
     * 返回deque中第一个元素的引用。
     * 在空deque上调用front()会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    reference front() noexcept {
        return *begin();
    }
    
    /**
     * @brief Access the first element (const version)
     * @brief 访问第一个元素（const版本）
     * 
     * @return Const reference to the first element
     * @return 对第一个元素的const引用
     * 
     * @details
     * Returns a const reference to the first element in the deque.
     * Calling front() on an empty deque causes undefined behavior.
     * 
     * @details
     * 返回deque中第一个元素的const引用。
     * 在空deque上调用front()会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_reference front() const noexcept {
        return *begin();
    }
    
    /**
     * @brief Access the last element
     * @brief 访问最后一个元素
     * 
     * @return Reference to the last element
     * @return 对最后一个元素的引用
     * 
     * @details
     * Returns a reference to the last element in the deque.
     * Calling back() on an empty deque causes undefined behavior.
     * 
     * @details
     * 返回deque中最后一个元素的引用。
     * 在空deque上调用back()会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    reference back() noexcept {
        iterator tmp = end();
        --tmp;
        return *tmp;
    }
    
    /**
     * @brief Access the last element (const version)
     * @brief 访问最后一个元素（const版本）
     * 
     * @return Const reference to the last element
     * @return 对最后一个元素的const引用
     * 
     * @details
     * Returns a const reference to the last element in the deque.
     * Calling back() on an empty deque causes undefined behavior.
     * 
     * @details
     * 返回deque中最后一个元素的const引用。
     * 在空deque上调用back()会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_reference back() const noexcept {
        const_iterator tmp = end();
        --tmp;
        return *tmp;
    }
    
    // ==================== Modifiers ====================
    // ==================== 修改操作 ====================
    
    /**
     * @brief Add element at the end
     * @brief 在末尾添加元素
     * 
     * @param value Element value to add
     * @param value 要添加的元素值
     * 
     * @details
     * Appends the given element value to the end of the deque.
     * The new element is initialized as a copy of value.
     * 
     * @details
     * 将给定元素值追加到deque的末尾。
     * 新元素初始化为value的副本。
     * 
     * @note
     * If the current buffer is full, a new buffer is allocated.
     * If the map array is full, it is reallocated to accommodate more buffers.
     * 
     * @note
     * 如果当前缓冲区已满，则分配新缓冲区。
     * 如果map数组已满，则重新分配以容纳更多缓冲区。
     * 
     * @note Time complexity: Amortized O(1)
     * @note 时间复杂度：摊还O(1)
     */
    void push_back(const T& value) {
        if (!map_) {
            // 第一次插入，需要初始化map
            create_map(1);
            start_ = finish_ = map_size_ / 2;
            start_offset_ = finish_offset_ = 0;
        }
        
        if (finish_offset_ < buffer_size) {
            // 当前块还有空间
            alloc_traits::construct(alloc_, map_[finish_] + finish_offset_, value);
            ++finish_offset_;
        } else {
            // 需要分配新块
            if (finish_ + 1 >= map_size_) {
                reallocate_map(1, false);
            }
            ++finish_;
            finish_offset_ = 1;
            map_[finish_] = allocate_buffer();
            alloc_traits::construct(alloc_, map_[finish_], value);
        }
        ++size_;
    }
    
    /**
     * @brief Add element at the beginning
     * @brief 在开头添加元素
     * 
     * @param value Element value to add
     * @param value 要添加的元素值
     * 
     * @details
     * Prepends the given element value to the beginning of the deque.
     * The new element is initialized as a copy of value.
     * 
     * @details
     * 将给定元素值前置到deque的开头。
     * 新元素初始化为value的副本。
     * 
     * @note
     * If the current buffer is full at the front, a new buffer is allocated.
     * If the map array is full at the front, it is reallocated to accommodate more buffers.
     * 
     * @note
     * 如果当前缓冲区在前端已满，则分配新缓冲区。
     * 如果map数组在前端已满，则重新分配以容纳更多缓冲区。
     * 
     * @note Time complexity: Amortized O(1)
     * @note 时间复杂度：摊还O(1)
     */
    void push_front(const T& value) {
        if (!map_) {
            // 第一次插入，需要初始化map
            create_map(1);
            start_ = finish_ = map_size_ / 2;
            start_offset_ = finish_offset_ = 0;
        }
        
        if (start_offset_ > 0) {
            // 当前块还有空间
            --start_offset_;
            alloc_traits::construct(alloc_, map_[start_] + start_offset_, value);
        } else {
            // 需要分配新块
            if (start_ == 0) {
                reallocate_map(1, true);
            }
            --start_;
            start_offset_ = buffer_size - 1;
            map_[start_] = allocate_buffer();
            alloc_traits::construct(alloc_, map_[start_] + start_offset_, value);
        }
        ++size_;
    }
    
    /**
     * @brief Remove element from the end
     * @brief 从末尾移除元素
     * 
     * @details
     * Removes the last element of the deque.
     * Calling pop_back() on an empty deque has no effect.
     * 
     * @details
     * 移除deque的最后一个元素。
     * 在空deque上调用pop_back()没有效果。
     * 
     * @note
     * If the last buffer becomes empty after removal, it is deallocated.
     * 
     * @note
     * 如果最后一个缓冲区在移除后变空，则将其释放。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    void pop_back() {
        if (empty()) return;
        
        if (finish_offset_ > 0) {
            --finish_offset_;
            alloc_traits::destroy(alloc_, map_[finish_] + finish_offset_);
        } else {
            deallocate_buffer(map_[finish_]);
            --finish_;
            finish_offset_ = buffer_size - 1;
            alloc_traits::destroy(alloc_, map_[finish_] + finish_offset_);
        }
        --size_;
    }
    
    /**
     * @brief Remove element from the beginning
     * @brief 从开头移除元素
     * 
     * @details
     * Removes the first element of the deque.
     * Calling pop_front() on an empty deque has no effect.
     * 
     * @details
     * 移除deque的第一个元素。
     * 在空deque上调用pop_front()没有效果。
     * 
     * @note
     * If the first buffer becomes empty after removal, it is deallocated.
     * 
     * @note
     * 如果第一个缓冲区在移除后变空，则将其释放。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    void pop_front() {
        if (empty()) return;
        
        alloc_traits::destroy(alloc_, map_[start_] + start_offset_);
        if (start_offset_ < buffer_size - 1) {
            ++start_offset_;
        } else {
            deallocate_buffer(map_[start_]);
            ++start_;
            start_offset_ = 0;
        }
        --size_;
    }
    
    /**
     * @brief Construct element in-place at the end
     * @brief 在末尾原位构造元素
     * 
     * @tparam Args Types of arguments to forward to the constructor
     * @tparam Args 转发给构造函数的参数类型
     * @param args Arguments to forward to the constructor
     * @param args 转发给构造函数的参数
     * @return Reference to the constructed element
     * @return 对构造元素的引用
     * 
     * @details
     * Constructs an element in-place at the end of the deque.
     * The element is constructed using perfect forwarding of the provided arguments.
     * This avoids unnecessary copies or moves that would occur with push_back().
     * Returns a reference to the newly constructed element (C++17 feature).
     * 
     * @details
     * 在deque的末尾原位构造元素。
     * 使用提供的参数的完美转发来构造元素。
     * 这避免了使用push_back()时可能发生的不必要的拷贝或移动。
     * 返回对新构造元素的引用（C++17特性）。
     * 
     * @note
     * If the current buffer is full, a new buffer is allocated.
     * If the map array is full, it is reallocated to accommodate more buffers.
     * 
     * @note
     * 如果当前缓冲区已满，则分配新缓冲区。
     * 如果map数组已满，则重新分配以容纳更多缓冲区。
     * 
     * @note Time complexity: Amortized O(1)
     * @note 时间复杂度：摊还O(1)
     */
    template<typename... Args>
    reference emplace_back(Args&&... args) {
        if (!map_) {
            // 第一次插入，需要初始化map
            create_map(1);
            start_ = finish_ = map_size_ / 2;
            start_offset_ = finish_offset_ = 0;
        }
        
        pointer new_element;
        if (finish_offset_ < buffer_size) {
            // 当前块还有空间，在当前块内构造元素
            new_element = map_[finish_] + finish_offset_;
            alloc_traits::construct(alloc_, new_element, mystl::forward<Args>(args)...);
            ++finish_offset_;
        } else {
            // 需要分配新块
            if (finish_ + 1 >= map_size_) {
                reallocate_map(1, false);  // 在末尾添加新块
            }
            ++finish_;
            finish_offset_ = 1;
            map_[finish_] = allocate_buffer();  // 分配新缓冲区
            new_element = map_[finish_];
            alloc_traits::construct(alloc_, new_element, mystl::forward<Args>(args)...);  // 在新缓冲区构造元素
        }
        ++size_;
        return *new_element;
    }
    
    /**
     * @brief Construct element in-place at the beginning
     * @brief 在开头原位构造元素
     * 
     * @tparam Args Types of arguments to forward to the constructor
     * @tparam Args 转发给构造函数的参数类型
     * @param args Arguments to forward to the constructor
     * @param args 转发给构造函数的参数
     * 
     * @details
     * Constructs an element in-place at the beginning of the deque.
     * The element is constructed using perfect forwarding of the provided arguments.
     * This avoids unnecessary copies or moves that would occur with push_front().
     * 
     * @details
     * 在deque的开头原位构造元素。
     * 使用提供的参数的完美转发来构造元素。
     * 这避免了使用push_front()时可能发生的不必要的拷贝或移动。
     * 
     * @note
     * If the current buffer is full at the front, a new buffer is allocated.
     * If the map array is full at the front, it is reallocated to accommodate more buffers.
     * 
     * @note
     * 如果当前缓冲区在前端已满，则分配新缓冲区。
     * 如果map数组在前端已满，则重新分配以容纳更多缓冲区。
     * 
     * @note Time complexity: Amortized O(1)
     * @note 时间复杂度：摊还O(1)
     */
    template<typename... Args>
    void emplace_front(Args&&... args) {
        if (!map_) {
            // 第一次插入，需要初始化map
            create_map(1);
            start_ = finish_ = map_size_ / 2;
            start_offset_ = finish_offset_ = 0;
        }
        
        if (start_offset_ > 0) {
            // 当前块还有空间，在当前块内构造元素
            --start_offset_;
            alloc_traits::construct(alloc_, map_[start_] + start_offset_, mystl::forward<Args>(args)...);
        } else {
            // 需要分配新块
            if (start_ == 0) {
                reallocate_map(1, true);  // 在开头添加新块
            }
            --start_;
            start_offset_ = buffer_size - 1;
            map_[start_] = allocate_buffer();  // 分配新缓冲区
            alloc_traits::construct(alloc_, map_[start_] + start_offset_, mystl::forward<Args>(args)...);  // 在新缓冲区构造元素
        }
        ++size_;
    }
    
    /**
     * @brief Insert element at specified position
     * @brief 在指定位置插入元素
     * 
     * @param pos Iterator to the position before which the element will be inserted
     * @param pos 元素将插入到此位置之前的迭代器
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return Iterator pointing to the inserted element
     * @return 指向插入元素的迭代器
     * 
     * @details
     * Inserts a copy of the given element at the specified position.
     * The insertion position is determined relative to the beginning of the deque.
     * The implementation chooses the most efficient direction for element movement
     * based on whether the insertion point is closer to the beginning or end.
     * 
     * @details
     * 在指定位置插入给定元素的副本。
     * 插入位置相对于deque的开头确定。
     * 实现根据插入点更靠近开头还是结尾选择最高效的元素移动方向。
     * 
     * @note Time complexity: O(min(n, size-n)) where n is the distance from pos to begin()
     * @note 时间复杂度：O(min(n, size-n))，其中n是pos到begin()的距离
     */
    iterator insert(const_iterator pos, const T& value) {
        // 计算插入位置相对于开头的索引
        difference_type index = pos - begin();
        
        if (index < size_ / 2) {
            // 插入点靠近开头，在开头添加元素然后向右移动
            push_front(value);
            // 将[0, index)范围内的元素向右移动一位
            for (difference_type i = 0; i < index; ++i) {
                (*this)[i] = (*this)[i + 1];
            }
            // 在目标位置设置值
            (*this)[index] = value;
        } else {
            // 插入点靠近结尾，在末尾添加元素然后向左移动
            push_back(value);
            // 将[index, size_-1)范围内的元素向左移动一位
            for (difference_type i = size_ - 1; i > index; --i) {
                (*this)[i] = (*this)[i - 1];
            }
            // 在目标位置设置值
            (*this)[index] = value;
        }
        return begin() + index;
    }
    
    /**
     * @brief Construct element in-place at specified position
     * @brief 在指定位置原位构造元素
     * 
     * @tparam Args Types of arguments to forward to the constructor
     * @tparam Args 转发给构造函数的参数类型
     * @param pos Iterator to the position before which the element will be constructed
     * @param pos 元素将构造到此位置之前的迭代器
     * @param args Arguments to forward to the constructor
     * @param args 转发给构造函数的参数
     * @return Iterator pointing to the constructed element
     * @return 指向构造元素的迭代器
     * 
     * @details
     * Constructs an element in-place at the specified position.
     * The element is constructed using perfect forwarding of the provided arguments.
     * This avoids unnecessary copies or moves that would occur with insert().
     * The implementation chooses the most efficient direction for element movement
     * based on whether the insertion point is closer to the beginning or end.
     * 
     * @details
     * 在指定位置原位构造元素。
     * 使用提供的参数的完美转发来构造元素。
     * 这避免了使用insert()时可能发生的不必要的拷贝或移动。
     * 实现根据插入点更靠近开头还是结尾选择最高效的元素移动方向。
     * 
     * @note Time complexity: O(min(n, size-n)) where n is the distance from pos to begin()
     * @note 时间复杂度：O(min(n, size-n))，其中n是pos到begin()的距离
     */
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        // 计算插入位置相对于开头的索引
        difference_type index = pos - begin();
        
        if (index < size_ / 2) {
            // 插入点靠近开头，在开头原位构造元素
            emplace_front(mystl::forward<Args>(args)...);
            // 将[0, index)范围内的元素向右移动一位（使用移动语义）
            for (difference_type i = 0; i < index; ++i) {
                (*this)[i] = mystl::move((*this)[i + 1]);
            }
            // 销毁目标位置的旧元素，然后重新构造
            alloc_traits::destroy(alloc_, &(*this)[index]);
            alloc_traits::construct(alloc_, &(*this)[index], mystl::forward<Args>(args)...);
        } else {
            // 插入点靠近结尾，在末尾原位构造元素
            emplace_back(mystl::forward<Args>(args)...);
            // 将[index, size_-1)范围内的元素向左移动一位（使用移动语义）
            for (difference_type i = size_ - 1; i > index; --i) {
                (*this)[i] = mystl::move((*this)[i - 1]);
            }
            // 销毁目标位置的旧元素，然后重新构造
            alloc_traits::destroy(alloc_, &(*this)[index]);
            alloc_traits::construct(alloc_, &(*this)[index], mystl::forward<Args>(args)...);
        }
        return begin() + index;
    }
    
    /**
     * @brief Insert element at specified position (move version)
     * @brief 在指定位置插入元素（移动版本）
     * 
     * @param pos Iterator to the position before which the element will be inserted
     * @param pos 元素将插入到此位置之前的迭代器
     * @param value Element value to move into the deque
     * @param value 要移动到deque中的元素值
     * @return Iterator pointing to the inserted element
     * @return 指向插入元素的迭代器
     * 
     * @details
     * Inserts the given element at the specified position using move semantics.
     * The element is moved into the deque, avoiding unnecessary copies.
     * The implementation chooses the most efficient direction for element movement
     * based on whether the insertion point is closer to the beginning or end.
     * 
     * @details
     * 使用移动语义在指定位置插入给定元素。
     * 元素被移动到deque中，避免不必要的拷贝。
     * 实现根据插入点更靠近开头还是结尾选择最高效的元素移动方向。
     * 
     * @note Time complexity: O(min(n, size-n)) where n is the distance from pos to begin()
     * @note 时间复杂度：O(min(n, size-n))，其中n是pos到begin()的距离
     */
    iterator insert(const_iterator pos, T&& value) {
        // 简化实现：在指定位置插入单个元素（移动语义）
        difference_type index = pos - begin();
        T moved_value = mystl::move(value);
        if (index < size_ / 2) {
            // 靠近开头，向前移动元素
            push_front(moved_value);
            // 将元素向右移动
            for (difference_type i = 0; i < index; ++i) {
                (*this)[i] = mystl::move((*this)[i + 1]);
            }
            (*this)[index] = mystl::move(moved_value);
        } else {
            // 靠近结尾，向后移动元素
            push_back(moved_value);
            // 将元素向左移动
            for (difference_type i = size_ - 1; i > index; --i) {
                (*this)[i] = mystl::move((*this)[i - 1]);
            }
            (*this)[index] = mystl::move(moved_value);
        }
        return begin() + index;
    }
    
    /**
     * @brief Insert multiple copies of value at specified position
     * @brief 在指定位置插入多个相同元素
     * 
     * @param pos Iterator to the position before which elements will be inserted
     * @param pos 元素将插入到此位置之前的迭代器
     * @param count Number of elements to insert
     * @param count 要插入的元素数量
     * @param value Value to initialize each element with
     * @param value 用于初始化每个元素的值
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details
     * Inserts count copies of value at the specified position.
     * The implementation first adds the elements at the end,
     * then moves existing elements to make space, and finally
     * fills the inserted positions with the specified value.
     * 
     * @details
     * 在指定位置插入count个value的副本。
     * 实现首先在末尾添加元素，
     * 然后移动现有元素以腾出空间，最后
     * 用指定值填充插入位置。
     * 
     * @note Time complexity: O(count + min(n, size-n)) where n is the distance from pos to begin()
     * @note 时间复杂度：O(count + min(n, size-n))，其中n是pos到begin()的距离
     */
    iterator insert(const_iterator pos, size_type count, const T& value) {
        difference_type index = pos - begin();
        if (count == 0) return begin() + index;
        
        // 保存原始大小
        size_type old_size = size_;
        
        // 首先在末尾添加count个元素
        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
        
        // 然后将元素移动到正确位置
        if (index < old_size) {
            // 将[index, old_size)的元素向右移动count个位置
            for (difference_type i = old_size - 1; i >= index; --i) {
                (*this)[i + count] = mystl::move((*this)[i]);
            }
            
            // 填充新插入的元素
            for (size_type i = 0; i < count; ++i) {
                (*this)[index + i] = value;
            }
        }
        
        return begin() + index;
    }
    
    /**
     * @brief Insert range of elements at specified position
     * @brief 在指定位置插入元素范围
     * 
     * @tparam InputIt Type of input iterator
     * @tparam InputIt 输入迭代器类型
     * @param pos Iterator to the position before which elements will be inserted
     * @param pos 元素将插入到此位置之前的迭代器
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to the element after the last element in the range
     * @param last 指向范围中最后一个元素之后元素的迭代器
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details
     * Inserts elements from the range [first, last) at the specified position.
     * For random access iterators, the size is calculated directly for efficiency.
     * For non-random access iterators, the range is traversed twice:
     * once to calculate the size, and once to copy the elements.
     * 
     * @details
     * 在指定位置插入范围[first, last)中的元素。
     * 对于随机访问迭代器，为提高效率直接计算大小。
     * 对于非随机访问迭代器，范围被遍历两次：
     * 一次计算大小，一次拷贝元素。
     * 
     * @note Time complexity: O(count + min(n, size-n)) where n is the distance from pos to begin()
     *       and count is the number of elements in the range
     * @note 时间复杂度：O(count + min(n, size-n))，其中n是pos到begin()的距离，
     *       count是范围中的元素数量
     */
    template<typename InputIt, typename = typename mystl::enable_if<
        !mystl::is_integral<InputIt>::value>::type>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        difference_type index = pos - begin();
        if (first == last) return begin() + index;
        
        // 计算要插入的元素数量
        size_type count = 0;
        if constexpr (mystl::is_random_access_iterator<InputIt>::value) {
            count = last - first;
        } else {
            // 对于非随机访问迭代器，先遍历计算数量
            InputIt it = first;
            while (it != last) {
                ++count;
                ++it;
            }
        }
        
        if (count == 0) return begin() + index;
        
        // 保存原始大小
        size_type old_size = size_;
        
        // 首先在末尾添加元素
        InputIt it = first;
        for (size_type i = 0; i < count; ++i) {
            push_back(*it);
            ++it;
        }
        
        // 然后将元素移动到正确位置
        if (index < old_size) {
            // 将[index, old_size)的元素向右移动count个位置
            for (difference_type i = old_size - 1; i >= index; --i) {
                (*this)[i + count] = mystl::move((*this)[i]);
            }
            
            // 填充新插入的元素
            it = first;
            for (size_type i = 0; i < count; ++i) {
                (*this)[index + i] = *it;
                ++it;
            }
        }
        
        return begin() + index;
    }
    
    /**
     * @brief Insert initializer list at specified position
     * @brief 在指定位置插入初始化列表
     * 
     * @param pos Iterator to the position before which elements will be inserted
     * @param pos 元素将插入到此位置之前的迭代器
     * @param ilist Initializer list containing elements to insert
     * @param ilist 包含要插入元素的初始化列表
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details
     * Inserts elements from the initializer list at the specified position.
     * This function is a convenience wrapper for insert(pos, ilist.begin(), ilist.end()).
     * 
     * @details
     * 在指定位置插入初始化列表中的元素。
     * 此函数是insert(pos, ilist.begin(), ilist.end())的便捷包装。
     * 
     * @note Time complexity: O(count + min(n, size-n)) where n is the distance from pos to begin()
     *       and count is the number of elements in the list
     * @note 时间复杂度：O(count + min(n, size-n))，其中n是pos到begin()的距离，
     *       count是列表中的元素数量
     */
    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }
    
    /**
     * @brief Erase element at specified position
     * @brief 删除指定位置的元素
     * 
     * @param pos Iterator to the element to remove
     * @param pos 指向要移除元素的迭代器
     * @return Iterator following the last removed element
     * @return 指向最后一个被移除元素之后的迭代器
     * 
     * @details
     * Removes the element at the specified position.
     * The implementation chooses the most efficient direction for element movement
     * based on whether the deletion point is closer to the beginning or end.
     * 
     * @details
     * 删除指定位置的元素。
     * 实现根据删除点更靠近开头还是结尾选择最高效的元素移动方向。
     * 
     * @note
     * Invalidates iterators and references at or after the point of the erase.
     * 
     * @note
     * 使删除点处或之后的迭代器和引用失效。
     * 
     * @note Time complexity: O(min(n, size-n)) where n is the distance from pos to begin()
     * @note 时间复杂度：O(min(n, size-n))，其中n是pos到begin()的距离
     */
    iterator erase(const_iterator pos) {
        if (pos == end()) return end();
        
        difference_type index = pos - begin();
        if (index < size_ / 2) {
            // 删除点靠近开头，将[0, index)范围内的元素向左移动一位
            for (difference_type i = index; i > 0; --i) {
                (*this)[i] = (*this)[i - 1];
            }
            pop_front();  // 移除第一个元素
        } else {
            // 删除点靠近结尾，将[index+1, size_)范围内的元素向右移动一位
            for (difference_type i = index; i < static_cast<difference_type>(size_) - 1; ++i) {
                (*this)[i] = (*this)[i + 1];
            }
            pop_back();  // 移除最后一个元素
        }
        return begin() + index;
    }
    
    /**
     * @brief Erase elements that satisfy a predicate
     * @brief 删除满足谓词的元素
     * 
     * @tparam Predicate Type of the predicate function
     * @tparam Predicate 谓词函数类型
     * @param pred Unary predicate which returns true for elements to be removed
     * @param pred 一元谓词，对于要删除的元素返回true
     * @return Number of elements erased
     * @return 删除的元素数量
     * 
     * @details
     * Erases all elements that satisfy the predicate pred from the deque.
     * This function is equivalent to the C++20 erase_if member function.
     * The algorithm preserves the relative order of the remaining elements.
     * 
     * @details
     * 从deque中删除所有满足谓词pred的元素。
     * 此函数等价于C++20的erase_if成员函数。
     * 算法保留剩余元素的相对顺序。
     * 
     * @note
     * Invalidates iterators and references to the erased elements.
     * 
     * @note
     * 使指向被删除元素的迭代器和引用失效。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note 时间复杂度：O(n)，其中n是元素数量
     */
    template<typename Predicate>
    size_type erase_if(Predicate pred) {
        size_type old_size = size_;
        iterator write = begin();
        iterator read = begin();
        
        while (read != end()) {
            if (!pred(*read)) {
                if (write != read) {
                    *write = mystl::move(*read);
                }
                ++write;
            }
            ++read;
        }
        
        // 删除末尾多余的元素
        size_type erased = old_size - size_type(write - begin());
        while (write != end()) {
            pop_back();
        }
        
        return erased;
    }
    
    /**
     * @brief Erase range of elements
     * @brief 删除元素范围
     * 
     * @param first Iterator to the first element to remove
     * @param first 指向要移除的第一个元素的迭代器
     * @param last Iterator to the element after the last element to remove
     * @param last 指向要移除的最后一个元素之后元素的迭代器
     * @return Iterator following the last removed element
     * @return 指向最后一个被移除元素之后的迭代器
     * 
     * @details
     * Removes the elements in the range [first, last).
     * This implementation uses a simplified approach that erases elements
     * one by one. A more efficient implementation would move elements
     * in bulk to reduce the number of element moves.
     * 
     * @details
     * 删除范围[first, last)中的元素。
     * 此实现使用简化方法逐个删除元素。
     * 更高效的实现会批量移动元素以减少元素移动次数。
     * 
     * @note
     * Invalidates iterators and references at or after the point of the erase.
     * 
     * @note
     * 使删除点处或之后的迭代器和引用失效。
     * 
     * @note Time complexity: O(n + m) where n is the number of elements erased
     *       and m is the number of elements after the erased range
     * @note 时间复杂度：O(n + m)，其中n是被删除的元素数量，
     *       m是被删除范围之后的元素数量
     */
    iterator erase(const_iterator first, const_iterator last) {
        if (first == last) {
            // 如果范围为空，返回first对应的迭代器
            return begin() + (first - begin());
        }
        
        difference_type n = last - first;
        difference_type first_index = first - begin();
        
        // 简化实现：逐个删除元素
        // 注意：这不是最高效的实现，但对于简化版本足够
        for (difference_type i = 0; i < n; ++i) {
            erase(begin() + first_index);
        }
        
        return begin() + first_index;
    }
    
    /**
     * @brief Clear all elements from the deque
     * @brief 清除deque中的所有元素
     * 
     * @details
     * Removes all elements from the deque, leaving it with a size of 0.
     * All memory is deallocated, and the deque returns to its initial state.
     * 
     * @details
     * 从deque中移除所有元素，使其大小变为0。
     * 所有内存都被释放，deque返回到其初始状态。
     * 
     * @note
     * Invalidates all iterators, pointers and references to elements of the deque.
     * 
     * @note
     * 使所有指向deque元素的迭代器、指针和引用失效。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note 时间复杂度：O(n)，其中n是元素数量
     */
    void clear() {
        // 销毁所有元素
        for (iterator it = begin(); it != end(); ++it) {
            alloc_traits::destroy(alloc_, &*it);
        }
        
        // 释放所有缓冲区
        if (map_) {
            for (size_type i = start_; i <= finish_; ++i) {
                deallocate_buffer(map_[i]);
            }
            deallocate_map(map_, map_size_);
            map_ = nullptr;
        }
        
        // 重置状态
        map_size_ = 0;
        start_ = 0;
        finish_ = 0;
        start_offset_ = 0;
        finish_offset_ = 0;
        size_ = 0;
    }
    
    /**
     * @brief Swap contents with another deque
     * @brief 与另一个deque交换内容
     * 
     * @param other Another deque to swap with
     * @param other 要交换的另一个deque
     * 
     * @details
     * Exchanges the contents of this deque with those of other.
     * Does not invoke any move, copy, or swap operations on individual elements.
     * All iterators and references remain valid. The past-the-end iterator is invalidated.
     * 
     * @details
     * 将此deque的内容与other交换。
     * 不会对单个元素调用任何移动、拷贝或交换操作。
     * 所有迭代器和引用保持有效。最后一个元素之后位置的迭代器失效。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note Exception safety: noexcept
     * @note 异常安全性：noexcept
     */
    void swap(deque& other) noexcept {
        using mystl::swap;
        
        swap(map_, other.map_);
        swap(map_size_, other.map_size_);
        swap(start_, other.start_);
        swap(finish_, other.finish_);
        swap(start_offset_, other.start_offset_);
        swap(finish_offset_, other.finish_offset_);
        swap(size_, other.size_);
        
        if (alloc_traits::propagate_on_container_swap::value) {
            swap(alloc_, other.alloc_);
        }
    }
    
    /**
     * @brief Resize the deque (default value version)
     * @brief 调整deque的大小（默认值版本）
     * 
     * @param new_size New size of the deque
     * @param new_size deque的新大小
     * 
     * @details
     * Resizes the deque to contain new_size elements.
     * If the current size is greater than new_size, the deque is reduced to its first new_size elements.
     * If the current size is less than new_size, additional default-inserted elements are appended.
     * 
     * @details
     * 调整deque的大小以包含new_size个元素。
     * 如果当前大小大于new_size，deque被缩减为其前new_size个元素。
     * 如果当前大小小于new_size，则追加额外的默认插入元素。
     * 
     * @note Time complexity: O(|new_size - size|)
     * @note 时间复杂度：O(|new_size - size|)
     */
    void resize(size_type new_size) {
        resize(new_size, T());
    }
    
    /**
     * @brief Resize the deque with specified value
     * @brief 使用指定值调整deque的大小
     * 
     * @param new_size New size of the deque
     * @param new_size deque的新大小
     * @param value The value to initialize new elements with
     * @param value 用于初始化新元素的值
     * 
     * @details
     * Resizes the deque to contain new_size elements.
     * If the current size is greater than new_size, the deque is reduced to its first new_size elements.
     * If the current size is less than new_size, additional copies of value are appended.
     * 
     * @details
     * 调整deque的大小以包含new_size个元素。
     * 如果当前大小大于new_size，deque被缩减为其前new_size个元素。
     * 如果当前大小小于new_size，则追加value的额外副本。
     * 
     * @note Time complexity: O(|new_size - size|)
     * @note 时间复杂度：O(|new_size - size|)
     */
    void resize(size_type new_size, const T& value) {
        if (new_size < size_) {
            // 缩小容器
            size_type to_erase = size_ - new_size;
            for (size_type i = 0; i < to_erase; ++i) {
                pop_back();
            }
        } else if (new_size > size_) {
            // 扩大容器
            size_type to_add = new_size - size_;
            for (size_type i = 0; i < to_add; ++i) {
                push_back(value);
            }
        }
    }
    
    /**
     * @brief Assign multiple copies of value to the deque
     * @brief 将多个相同元素赋值给deque
     * 
     * @param count Number of elements to assign
     * @param count 要分配的元素数量
     * @param value Value to assign to each element
     * @param value 分配给每个元素的值
     * 
     * @details
     * Replaces the contents of the deque with count copies of value.
     * The function first clears the deque, then creates a new map array
     * and inserts count copies of value.
     * 
     * @details
     * 用count个value的副本替换deque的内容。
     * 函数首先清空deque，然后创建新的map数组
     * 并插入count个value的副本。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is count
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是count
     * 
     * @note Exception safety: Strong guarantee - if an exception is thrown,
     * the deque is left in a valid state (empty).
     * 
     * @note 异常安全性：强保证 - 如果抛出异常，
     * deque将处于有效状态（空）。
     */
    void assign(size_type count, const T& value) {
        clear();
        if (count > 0) {
            create_map(count);
            start_ = map_size_ / 2;
            finish_ = start_;
            start_offset_ = 0;
            finish_offset_ = 0;
            size_ = 0;
            
            try {
                for (size_type i = 0; i < count; ++i) {
                    push_back(value);
                }
            } catch (...) {
                clear();
                destroy_map();
                throw;
            }
        }
    }
    
    /**
     * @brief Assign elements from iterator range to the deque
     * @brief 将迭代器范围的元素赋值给deque
     * 
     * @tparam InputIt Type of input iterator
     * @tparam InputIt 输入迭代器类型
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to the element after the last element in the range
     * @param last 指向范围中最后一个元素之后元素的迭代器
     * 
     * @details
     * Replaces the contents of the deque with elements from the specified range.
     * The function first clears the deque, then inserts elements from the range.
     * For random access iterators, the size is calculated directly for efficiency.
     * For non-random access iterators, a temporary deque is created and swapped.
     * 
     * @details
     * 用指定范围的元素替换deque的内容。
     * 函数首先清空deque，然后从范围插入元素。
     * 对于随机访问迭代器，为提高效率直接计算大小。
     * 对于非随机访问迭代器，创建临时deque并进行交换。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the range
     * @note 时间复杂度：O(n)，其中n是范围中的元素数量
     * 
     * @note Exception safety: Strong guarantee - if an exception is thrown,
     * the deque is left in a valid state (empty).
     * 
     * @note 异常安全性：强保证 - 如果抛出异常，
     * deque将处于有效状态（空）。
     */
    template<typename InputIt, typename = typename mystl::enable_if<
        !mystl::is_integral<InputIt>::value>::type>
    void assign(InputIt first, InputIt last) {
        clear();
        
        // 对于随机访问迭代器，可以直接计算大小
        if constexpr (mystl::is_random_access_iterator<InputIt>::value) {
            size_type count = last - first;
            if (count > 0) {
                create_map(count);
                start_ = map_size_ / 2;
                finish_ = start_;
                start_offset_ = 0;
                finish_offset_ = 0;
                size_ = 0;
                
                try {
                    for (; first != last; ++first) {
                        push_back(*first);
                    }
                } catch (...) {
                    clear();
                    destroy_map();
                    throw;
                }
            }
        } else {
            // 对于非随机访问迭代器，使用迭代器范围构造函数
            // 创建一个临时deque，然后交换
            deque temp(first, last, alloc_);
            swap(temp);
        }
    }
    
    /**
     * @brief Assign elements from initializer list to the deque
     * @brief 将初始化列表的元素赋值给deque
     * 
     * @param ilist Initializer list containing elements to assign
     * @param ilist 包含要赋值元素的初始化列表
     * 
     * @details
     * Replaces the contents of the deque with elements from the initializer list.
     * This function is a convenience wrapper for assign(ilist.begin(), ilist.end()).
     * 
     * @details
     * 用初始化列表的元素替换deque的内容。
     * 此函数是assign(ilist.begin(), ilist.end())的便捷包装。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the list
     * @note 时间复杂度：O(n)，其中n是列表中的元素数量
     */
    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }
    
    /**
     * @brief Get the allocator associated with the deque
     * @brief 获取与deque关联的分配器
     * 
     * @return The allocator associated with the deque
     * @return 与deque关联的分配器
     * 
     * @details
     * Returns a copy of the allocator associated with the deque.
     * 
     * @details
     * 返回与deque关联的分配器的副本。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    allocator_type get_allocator() const noexcept {
        return alloc_;
    }
    
    /**
     * @brief Equality comparison operator
     * @brief 相等比较操作符
     * 
     * @param other Another deque to compare with
     * @param other 要比较的另一个deque
     * @return true if both deques have the same size and elements are equal,
     * false otherwise
     * @return 如果两个deque大小相同且元素相等则返回true，否则返回false
     * 
     * @details
     * Compares two deques for equality. Two deques are equal if they
     * have the same number of elements and each element in one deque compares
     * equal with the element in the same position in the other deque.
     * 
     * @details
     * 比较两个deque是否相等。如果两个deque具有相同数量的元素，
     * 并且一个deque中的每个元素与另一个deque中相同位置的元素相等，
     * 则它们相等。
     * 
     * @note Time complexity: O(min(n, m)) where n and m are the sizes of the deques
     * @note 时间复杂度：O(min(n, m))，其中n和m是deque的大小
     */
    bool operator==(const deque& other) const {
        if (size_ != other.size_) return false;
        return mystl::equal(begin(), end(), other.begin());
    }
    
    /**
     * @brief Inequality comparison operator
     * @brief 不等比较操作符
     * 
     * @param other Another deque to compare with
     * @param other 要比较的另一个deque
     * @return true if deques are not equal, false otherwise
     * @return 如果deque不相等则返回true，否则返回false
     * 
     * @details
     * Compares two deques for inequality. Returns true if the deques
     * are not equal, i.e., if they have different sizes or at least one pair of
     * corresponding elements are not equal.
     * 
     * @details
     * 比较两个deque是否不相等。如果deque不相等则返回true，
     * 即如果它们大小不同或至少有一对对应元素不相等。
     * 
     * @note This operator is implemented in terms of operator==
     * @note 此操作符基于operator==实现
     */
    bool operator!=(const deque& other) const {
        return !(*this == other);
    }
    
    /**
     * @brief Less-than comparison operator
     * @brief 小于比较操作符
     * 
     * @param other Another deque to compare with
     * @param other 要比较的另一个deque
     * @return true if this deque is lexicographically less than other,
     * false otherwise
     * @return 如果此deque在字典序上小于other则返回true，否则返回false
     * 
     * @details
     * Performs lexicographical comparison between two deques.
     * Compares the elements sequentially using operator< for the element type.
     * The comparison stops at the first mismatching element or when one deque ends.
     * 
     * @details
     * 对两个deque执行字典序比较。
     * 使用元素类型的operator<依次比较元素。
     * 比较在第一个不匹配的元素处停止，或当一个deque结束时停止。
     * 
     * @note Time complexity: O(min(n, m)) where n and m are the sizes of the deques
     * @note 时间复杂度：O(min(n, m))，其中n和m是deque的大小
     */
    bool operator<(const deque& other) const {
        return mystl::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }
    
    /**
     * @brief Less-than-or-equal comparison operator
     * @brief 小于等于比较操作符
     * 
     * @param other Another deque to compare with
     * @param other 要比较的另一个deque
     * @return true if this deque is lexicographically less than or equal to other,
     * false otherwise
     * @return 如果此deque在字典序上小于或等于other则返回true，否则返回false
     * 
     * @details
     * Checks if this deque is less than or equal to another deque.
     * Implemented as !(other < *this), which is equivalent to *this <= other.
     * 
     * @details
     * 检查此deque是否小于或等于另一个deque。
     * 实现为!(other < *this)，等价于*this <= other。
     */
    bool operator<=(const deque& other) const {
        return !(other < *this);
    }
    
    /**
     * @brief Greater-than comparison operator
     * @brief 大于比较操作符
     * 
     * @param other Another deque to compare with
     * @param other 要比较的另一个deque
     * @return true if this deque is lexicographically greater than other,
     * false otherwise
     * @return 如果此deque在字典序上大于other则返回true，否则返回false
     * 
     * @details
     * Checks if this deque is greater than another deque.
     * Implemented as other < *this, which is equivalent to *this > other.
     * 
     * @details
     * 检查此deque是否大于另一个deque。
     * 实现为other < *this，等价于*this > other。
     */
    bool operator>(const deque& other) const {
        return other < *this;
    }
    
    /**
     * @brief Greater-than-or-equal comparison operator
     * @brief 大于等于比较操作符
     * 
     * @param other Another deque to compare with
     * @param other 要比较的另一个deque
     * @return true if this deque is lexicographically greater than or equal to other,
     * false otherwise
     * @return 如果此deque在字典序上大于或等于other则返回true，否则返回false
     * 
     * @details
     * Checks if this deque is greater than or equal to another deque.
     * Implemented as !(*this < other), which is equivalent to *this >= other.
     * 
     * @details
     * 检查此deque是否大于或等于另一个deque。
     * 实现为!(*this < other)，等价于*this >= other。
     */
    bool operator>=(const deque& other) const {
        return !(*this < other);
    }

    /**
     * @brief Erases elements equal to the specified value
     * @brief 删除等于指定值的元素
     * 
     * @param value Value to compare elements against
     * @param value 用于比较元素的值
     * @return Number of elements erased
     * @return 删除的元素数量
     * 
     * @details
     * Erases all elements from the deque that compare equal to `value`.
     * The elements are removed using the equality operator (==).
     * Returns the number of elements that were erased.
     * 
     * @details
     * 从deque中删除所有等于`value`的元素。
     * 使用相等操作符（==）比较元素。
     * 返回被删除的元素数量。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the deque
     * @note 时间复杂度：O(n)，其中n是deque中的元素数量
     * 
     * @note This function does not throw exceptions unless the equality
     * comparison or element destruction throws.
     * 
     * @note 此函数不会抛出异常，除非相等比较或元素销毁抛出异常。
     * 
     * @note The function traverses the deque once, erasing matching elements
     * as they are encountered.
     * 
     * @note 函数遍历deque一次，遇到匹配的元素时立即删除。
     * 
     * @note Iterators and references to erased elements are invalidated.
     * Other iterators and references remain valid.
     * 
     * @note 指向被删除元素的迭代器和引用失效。
     * 其他迭代器和引用保持有效。
     * 
     * @see erase_if()
     * @see 参考 erase_if()
     */
    size_type erase(const T& value) {
        size_type erased_count = 0;
        iterator it = begin();
        while (it != end()) {
            if (*it == value) {
                it = erase(it);
                ++erased_count;
            } else {
                ++it;
            }
        }
        return erased_count;
    }

private:
    /**
     * @brief Allocate a new buffer
     * @brief 分配新缓冲区
     * 
     * @return Pointer to the allocated buffer
     * @return 指向分配的缓冲区的指针
     * 
     * @details
     * Allocates a new buffer of size buffer_size using the allocator.
     * The buffer size is determined by the element type size.
     * 
     * @details
     * 使用分配器分配大小为buffer_size的新缓冲区。
     * 缓冲区大小由元素类型大小决定。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    pointer allocate_buffer() {
        return alloc_traits::allocate(alloc_, buffer_size);
    }
    
    /**
     * @brief Deallocate a buffer
     * @brief 释放缓冲区
     * 
     * @param p Pointer to the buffer to deallocate
     * @param p 指向要释放的缓冲区的指针
     * 
     * @details
     * Deallocates a previously allocated buffer.
     * The buffer must have been allocated with allocate_buffer().
     * 
     * @details
     * 释放先前分配的缓冲区。
     * 缓冲区必须已使用allocate_buffer()分配。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    void deallocate_buffer(pointer p) noexcept {
        alloc_traits::deallocate(alloc_, p, buffer_size);
    }
    
    /**
     * @brief Allocate map array
     * @brief 分配map数组
     * 
     * @param n Size of the map array to allocate
     * @param n 要分配的map数组大小
     * @return Pointer to the allocated map array
     * @return 指向分配的map数组的指针
     * 
     * @details
     * Allocates a map array of size n using raw memory allocation.
     * The map array stores pointers to individual buffers.
     * 
     * @details
     * 使用原始内存分配分配大小为n的map数组。
     * map数组存储指向各个缓冲区的指针。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    map_pointer allocate_map(size_type n) {
        // 使用原始内存分配来分配指针数组
        return static_cast<map_pointer>(::operator new(n * sizeof(T*)));
    }
    
    /**
     * @brief Deallocate map array
     * @brief 释放map数组
     * 
     * @param p Pointer to the map array to deallocate
     * @param p 指向要释放的map数组的指针
     * @param n Size of the map array (unused, for interface consistency)
     * @param n map数组大小（未使用，用于接口一致性）
     * 
     * @details
     * Deallocates a previously allocated map array.
     * The map array must have been allocated with allocate_map().
     * 
     * @details
     * 释放先前分配的map数组。
     * map数组必须已使用allocate_map()分配。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    void deallocate_map(map_pointer p, size_type n) noexcept {
        ::operator delete(p);
    }
    
    /**
     * @brief Create map array for specified number of elements
     * @brief 为指定数量的元素创建map数组
     * 
     * @param num_elements Number of elements the deque will hold
     * @param num_elements deque将容纳的元素数量
     * 
     * @details
     * Creates the initial map array and allocates buffers for the deque.
     * The map size is calculated based on the number of elements,
     * with a minimum size of 8 to allow growth in both directions.
     * 
     * @details
     * 创建初始map数组并为deque分配缓冲区。
     * map大小基于元素数量计算，
     * 最小大小为8以允许在两个方向上增长。
     * 
     * @note
     * The map array is centered to allow equal growth in both directions.
     * 
     * @note
     * map数组居中放置以允许在两个方向上均衡增长。
     * 
     * @note Time complexity: O(num_elements / buffer_size)
     * @note 时间复杂度：O(num_elements / buffer_size)
     */
    void create_map(size_type num_elements) {
        // 计算需要的块数：元素数量 / 块大小 + 1（向上取整）
        size_type num_nodes = num_elements / buffer_size + 1;
        // map大小至少为8，或num_nodes + 2（为两端增长留出空间）
        map_size_ = mystl::max(static_cast<size_type>(8), num_nodes + 2);
        map_ = allocate_map(map_size_);
        
        // 初始化map数组中间部分，为两端增长留出空间
        start_ = map_size_ / 2 - num_nodes / 2;
        finish_ = start_ + num_nodes - 1;
        
        // 为每个块分配缓冲区
        for (size_type i = start_; i <= finish_; ++i) {
            map_[i] = allocate_buffer();
        }
    }
    
    /**
     * @brief Destroy map array and all buffers
     * @brief 销毁map数组和所有缓冲区
     * 
     * @details
     * Deallocates all buffers and the map array.
     * This function is called by the destructor and clear().
     * 
     * @details
     * 释放所有缓冲区和map数组。
     * 此函数由析构函数和clear()调用。
     * 
     * @note Time complexity: O(num_nodes) where num_nodes is the number of blocks
     * @note 时间复杂度：O(num_nodes)，其中num_nodes是块的数量
     */
    void destroy_map() noexcept {
        if (map_) {
            // 释放所有缓冲区
            for (size_type i = start_; i <= finish_; ++i) {
                deallocate_buffer(map_[i]);
            }
            // 释放map数组
            deallocate_map(map_, map_size_);
            map_ = nullptr;
        }
    }
    
    /**
     * @brief Reallocate map array to accommodate more nodes
     * @brief 重新分配map数组以容纳更多节点
     * 
     * @param nodes_to_add Number of nodes to add
     * @param nodes_to_add 要添加的节点数
     * @param add_at_front Whether to add nodes at the front (true) or back (false)
     * @param add_at_front 是否在前端添加节点（true）或在后端添加节点（false）
     * 
     * @details
     * Reallocates the map array when it becomes full.
     * The implementation either shifts existing pointers within the current map
     * or allocates a new larger map and copies the pointers.
     * 
     * @details
     * 当map数组变满时重新分配它。
     * 实现要么在当前map内移动现有指针，
     * 要么分配新的更大的map并复制指针。
     * 
     * @note
     * The new map size is increased by at least the current size or nodes_to_add,
     * whichever is larger, plus 2 for additional growth space.
     * 
     * @note
     * 新map大小至少增加当前大小或nodes_to_add中的较大值，
     * 再加上2以提供额外的增长空间。
     * 
     * @note Time complexity: O(num_nodes) where num_nodes is the number of blocks
     * @note 时间复杂度：O(num_nodes)，其中num_nodes是块的数量
     */
    void reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes = finish_ - start_ + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;
        
        map_pointer new_start;
        if (map_size_ > 2 * new_num_nodes) {
            // map数组有足够空间，只需调整start位置
            new_start = map_ + (map_size_ - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            if (new_start < map_ + start_) {
                // 向前移动：从start_开始向后复制
                for (size_type i = 0; i < old_num_nodes; ++i) {
                    new_start[i] = map_[start_ + i];
                }
            } else {
                // 向后移动：从start_开始向前复制
                for (size_type i = old_num_nodes; i > 0; --i) {
                    new_start[i - 1] = map_[start_ + i - 1];
                }
            }
        } else {
            // 需要重新分配map数组
            size_type new_map_size = map_size_ + mystl::max(map_size_, nodes_to_add) + 2;
            map_pointer new_map = allocate_map(new_map_size);
            new_start = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            
            // 复制原有指针
            for (size_type i = 0; i < old_num_nodes; ++i) {
                new_start[i] = map_[start_ + i];
            }
            
            // 释放旧map数组
            deallocate_map(map_, map_size_);
            map_ = new_map;
            map_size_ = new_map_size;
        }
        
        // 更新start和finish索引
        start_ = new_start - map_;
        finish_ = start_ + old_num_nodes - 1;
    }
};

/**
 * @brief Swaps the contents of two deque containers
 * @brief 交换两个deque容器的内容
 * 
 * @tparam T Type of elements stored in the deque
 * @tparam T 存储在deque中的元素类型
 * @tparam Alloc Allocator type for memory management
 * @tparam Alloc 用于内存管理的分配器类型
 * @param lhs First deque to swap
 * @param lhs 要交换的第一个deque
 * @param rhs Second deque to swap
 * @param rhs 要交换的第二个deque
 * 
 * @details
 * Exchanges the contents of lhs and rhs. Calls lhs.swap(rhs).
 * This function provides a non-member swap function for deque
 * that can be found via argument-dependent lookup (ADL).
 * 
 * @details
 * 交换lhs和rhs的内容。调用lhs.swap(rhs)。
 * 此函数为deque提供非成员swap函数，
 * 可以通过参数依赖查找（ADL）找到。
 * 
 * @note
 * This function does not invoke any move, copy, or swap operations
 * on individual elements. All iterators and references remain valid.
 * 
 * @note
 * 此函数不会对单个元素调用任何移动、拷贝或交换操作。
 * 所有迭代器和引用保持有效。
 * 
 * @note Time complexity: O(1)
 * @note 时间复杂度：O(1)
 * 
 * @note Exception safety: noexcept
 * @note 异常安全性：noexcept
 */
template<typename T, typename Alloc>
void swap(deque<T, Alloc>& lhs, deque<T, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace mystl
