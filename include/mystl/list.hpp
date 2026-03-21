#pragma once

/**
 * @file list.hpp
 * @brief Doubly-linked list container implementation for mySTL
 * @brief 双向链表容器实现（mySTL）
 * 
 * Provides list (doubly-linked list) container with bidirectional traversal,
 * efficient insertion/removal at any position, and constant time operations.
 * This implementation follows the C++ standard list specification,
 * providing O(1) insertion and deletion operations at any position.
 * 
 * 提供具有双向遍历、在任何位置高效插入/删除和常数时间操作的双向链表容器。
 * 此实现遵循C++标准list规范，在任何位置提供O(1)插入和删除操作。
 * 
 * @details
 * The list is a sequence container that supports constant time insertion
 * and removal of elements from anywhere in the container. It is implemented
 * as a doubly-linked list with a sentinel node to simplify edge cases.
 * 
 * @details
 * list是一个序列容器，支持从容器中任何位置常数时间插入和删除元素。
 * 它被实现为带有哨兵节点的双向链表，以简化边界情况。
 * 
 * @note Key features:
 * 1. Bidirectional traversal: supports both ++ and -- operators
 * 2. Constant time insertion/removal: O(1) at any position
 * 3. No random access: elements cannot be accessed by index
 * 4. Stable iterators: iterators remain valid unless the element is removed
 * 5. Special operations: splice, merge, sort, reverse, unique
 * 
 * @note 主要特性：
 * 1. 双向遍历：支持++和--操作符
 * 2. 常数时间插入/删除：在任何位置O(1)复杂度
 * 3. 无随机访问：不能通过索引访问元素
 * 4. 稳定的迭代器：除非元素被删除，否则迭代器保持有效
 * 5. 特殊操作：拼接、合并、排序、反转、去重
 * 
 * @author shensjt
 * @date 2026-03-20
 * 
 * @see https://en.cppreference.com/w/cpp/container/list
 * @see 相关参考：https://zh.cppreference.com/w/cpp/container/list
 */

#include <cstddef>
#include <initializer_list>
#include "../memory/allocator.hpp"
#include "../memory/allocator_traits.hpp"
#include "../iterator/iterator_traits.hpp"
#include "../iterator/reverse_iterator.hpp"
#include "../utility/move.hpp"
#include "../utility/swap.hpp"
#include "../algorithm/algorithm.hpp"

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

// ==================== list_node Structure ====================
// ==================== list_node 结构体 ====================

/**
 * @brief Node structure for list
 * @brief list的节点结构体
 * 
 * @tparam T Type of element stored in the node
 * @tparam T 节点中存储的元素类型
 * 
 * @details
 * This structure represents a single node in a doubly-linked list.
 * Each node contains a value, a pointer to the previous node,
 * and a pointer to the next node.
 * 
 * @details
 * 此结构体表示双向链表中的单个节点。
 * 每个节点包含一个值、一个指向前一个节点的指针和一个指向下一个节点的指针。
 */
template<typename T>
struct list_node {
    T value;                ///< The stored value / 存储的值
    list_node* prev;        ///< Pointer to the previous node / 指向前一个节点的指针
    list_node* next;        ///< Pointer to the next node / 指向下一个节点的指针
    
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs a node with self-referential pointers.
     * Used for creating sentinel nodes.
     * 
     * @details 构造一个自引用的节点。
     * 用于创建哨兵节点。
     */
    list_node() : prev(this), next(this) {}
    
    /**
     * @brief Constructor with value and pointers
     * @brief 带值和指针的构造函数
     * 
     * @param val Value to store in the node
     * @param val 要存储在节点中的值
     * @param p Pointer to the previous node (default: nullptr)
     * @param p 指向前一个节点的指针（默认：nullptr）
     * @param n Pointer to the next node (default: nullptr)
     * @param n 指向下一个节点的指针（默认：nullptr）
     */
    list_node(const T& val, list_node* p = nullptr, list_node* n = nullptr)
        : value(val), prev(p), next(n) {}
    
    /**
     * @brief Constructor with rvalue reference
     * @brief 带右值引用的构造函数
     * 
     * @param val Value to move into the node
     * @param val 要移动到节点中的值
     * @param p Pointer to the previous node (default: nullptr)
     * @param p 指向前一个节点的指针（默认：nullptr）
     * @param n Pointer to the next node (default: nullptr)
     * @param n 指向下一个节点的指针（默认：nullptr）
     */
    list_node(T&& val, list_node* p = nullptr, list_node* n = nullptr)
        : value(mystl::move(val)), prev(p), next(n) {}
    
    /**
     * @brief In-place constructor with variadic arguments
     * @brief 带可变参数的原位构造函数
     * 
     * @tparam Args Types of arguments for value construction
     * @tparam Args 值构造的参数类型
     * @param p Pointer to the previous node
     * @param p 指向前一个节点的指针
     * @param n Pointer to the next node
     * @param n 指向下一个节点的指针
     * @param args Arguments to forward to value constructor
     * @param args 转发给值构造函数的参数
     */
    template<typename... Args>
    list_node(list_node* p, list_node* n, Args&&... args)
        : value(mystl::forward<Args>(args)...), prev(p), next(n) {}
};

/**
 * @brief Bidirectional iterator for list container
 * @brief list容器的双向迭代器
 * 
 * @tparam T Type of elements in the list
 * @tparam T list中元素的类型
 * 
 * @details
 * This iterator supports bidirectional traversal for list containers.
 * It can move both forward (++) and backward (--) through the list.
 * The iterator maintains a pointer to the current node in the doubly-linked list.
 * 
 * @details
 * 此迭代器支持list容器的双向遍历。
 * 它可以在list中向前（++）和向后（--）移动。
 * 迭代器维护一个指向双向链表中当前节点的指针。
 * 
 * @note This iterator is a bidirectional iterator and satisfies all
 * requirements of the BidirectionalIterator concept.
 * 
 * @note 此迭代器是双向迭代器，满足BidirectionalIterator概念的所有要求。
 * 
 * @note Iterators to list elements remain valid even after insertion
 * and deletion operations, except when the element itself is removed.
 * 
 * @note 即使插入和删除操作后，list元素的迭代器仍然有效，
 * 除非元素本身被删除。
 */
template<typename T>
class list_iterator {
private:
    using node_type = list_node<T>;  ///< Type of node in the list / list中的节点类型
    node_type* node_;                ///< Pointer to the current node / 指向当前节点的指针
    
public:
    // 迭代器类别
    using iterator_category = mystl::bidirectional_iterator_tag;  ///< Iterator category / 迭代器类别
    using value_type = T;                                         ///< Type of elements / 元素类型
    using difference_type = std::ptrdiff_t;                       ///< Difference type / 差值类型
    using pointer = T*;                                           ///< Pointer type / 指针类型
    using reference = T&;                                         ///< Reference type / 引用类型
    
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs an empty iterator pointing to no element.
     * 
     * @details 构造一个不指向任何元素的空迭代器。
     */
    list_iterator() noexcept : node_(nullptr) {}
    
    /**
     * @brief Constructor with node pointer
     * @brief 带节点指针的构造函数
     * 
     * @param node Pointer to the list node
     * @param node 指向list节点的指针
     * 
     * @details Constructs an iterator pointing to the specified node.
     * 
     * @details 构造一个指向指定节点的迭代器。
     */
    explicit list_iterator(node_type* node) noexcept : node_(node) {}
    
    /**
     * @brief Dereference operator
     * @brief 解引用操作符
     * 
     * @return Reference to the element pointed to by the iterator
     * @return 迭代器指向的元素的引用
     * 
     * @details Returns a reference to the element stored in the current node.
     * 
     * @details 返回存储在当前节点中的元素的引用。
     */
    reference operator*() const noexcept {
        return node_->value;
    }
    
    /**
     * @brief Member access operator
     * @brief 成员访问操作符
     * 
     * @return Pointer to the element pointed to by the iterator
     * @return 指向迭代器指向的元素的指针
     * 
     * @details Returns a pointer to the element stored in the current node.
     * This operator allows accessing members of the element using arrow notation.
     * 
     * @details 返回指向存储在当前节点中的元素的指针。
     * 此操作符允许使用箭头表示法访问元素的成员。
     */
    pointer operator->() const noexcept {
        return &node_->value;
    }
    
    /**
     * @brief Prefix increment operator
     * @brief 前缀递增操作符
     * 
     * @return Reference to this iterator after increment
     * @return 递增后此迭代器的引用
     * 
     * @details Advances the iterator to the next element in the list.
     * 
     * @details 将迭代器前进到list中的下一个元素。
     */
    list_iterator& operator++() noexcept {
        node_ = node_->next;
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
    list_iterator operator++(int) noexcept {
        list_iterator temp = *this;
        ++(*this);
        return temp;
    }
    
    /**
     * @brief Prefix decrement operator
     * @brief 前缀递减操作符
     * 
     * @return Reference to this iterator after decrement
     * @return 递减后此迭代器的引用
     * 
     * @details Moves the iterator to the previous element in the list.
     * 
     * @details 将迭代器移动到list中的前一个元素。
     */
    list_iterator& operator--() noexcept {
        node_ = node_->prev;
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
    list_iterator operator--(int) noexcept {
        list_iterator temp = *this;
        --(*this);
        return temp;
    }
    
    /**
     * @brief Equality comparison operator
     * @brief 相等比较操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if both iterators point to the same node, false otherwise
     * @return 如果两个迭代器指向同一个节点则返回true，否则返回false
     * 
     * @details Checks if two iterators point to the same node in the list.
     * 
     * @details 检查两个迭代器是否指向list中的同一个节点。
     */
    bool operator==(const list_iterator& other) const noexcept {
        return node_ == other.node_;
    }
    
    /**
     * @brief Inequality comparison operator
     * @brief 不等比较操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if iterators point to different nodes, false otherwise
     * @return 如果迭代器指向不同的节点则返回true，否则返回false
     * 
     * @details Checks if two iterators point to different nodes in the list.
     * 
     * @details 检查两个迭代器是否指向list中的不同节点。
     */
    bool operator!=(const list_iterator& other) const noexcept {
        return node_ != other.node_;
    }
    
    /**
     * @brief Get the underlying node pointer
     * @brief 获取底层节点指针
     * 
     * @return Pointer to the current node
     * @return 指向当前节点的指针
     * 
     * @details Returns the underlying node pointer.
     * This function is for internal use by the list class only.
     * 
     * @details 返回底层节点指针。
     * 此函数仅供list类内部使用。
     */
    node_type* node() const noexcept {
        return node_;
    }
};

/**
 * @brief Constant bidirectional iterator for list container
 * @brief list容器的常量双向迭代器
 * 
 * @tparam T Type of elements in the list
 * @tparam T list中元素的类型
 * 
 * @details
 * This iterator provides read-only access to elements in a list container.
 * It supports bidirectional traversal (both forward and backward) and
 * cannot modify the elements it points to.
 * 
 * @details
 * 此迭代器提供对list容器中元素的只读访问。
 * 它支持双向遍历（向前和向后），并且不能修改它指向的元素。
 * 
 * @note This iterator is a bidirectional iterator and satisfies all
 * requirements of the BidirectionalIterator concept.
 * 
 * @note 此迭代器是双向迭代器，满足BidirectionalIterator概念的所有要求。
 * 
 * @note Unlike `list_iterator`, this iterator provides const access to elements,
 * which is useful when you need to traverse a list without modifying its contents.
 * 
 * @note 与`list_iterator`不同，此迭代器提供对元素的常量访问，
 * 当需要遍历list而不修改其内容时很有用。
 * 
 * @note Iterators to list elements remain valid even after insertion
 * and deletion operations, except when the element itself is removed.
 * 
 * @note 即使插入和删除操作后，list元素的迭代器仍然有效，
 * 除非元素本身被删除。
 */
template<typename T>
class list_const_iterator {
private:
    /**
     * @brief Type alias for list node
     * @brief list节点类型别名
     */
    using node_type = list_node<T>;
    
    /**
     * @brief Pointer to the current node (const version)
     * @brief 指向当前节点的指针（常量版本）
     * 
     * @details Points to the current node in the doubly-linked list.
     * This pointer is const, preventing modification of the node through this iterator.
     * 
     * @details 指向双向链表中的当前节点。
     * 此指针是const的，防止通过此迭代器修改节点。
     */
    const node_type* node_;
    
public:
    // ==================== 迭代器类别 ====================
    // ==================== Iterator Categories ====================
    
    /**
     * @brief Iterator category
     * @brief 迭代器类别
     */
    using iterator_category = mystl::bidirectional_iterator_tag;
    
    /**
     * @brief Type of elements in the list
     * @brief list中元素的类型
     */
    using value_type = T;
    
    /**
     * @brief Type for differences between iterators
     * @brief 迭代器之间差值的类型
     */
    using difference_type = std::ptrdiff_t;
    
    /**
     * @brief Pointer type to elements (const version)
     * @brief 元素的指针类型（常量版本）
     */
    using pointer = const T*;
    
    /**
     * @brief Reference type to elements (const version)
     * @brief 元素的引用类型（常量版本）
     */
    using reference = const T&;
    
    // ==================== 构造函数 ====================
    // ==================== Constructors ====================
    
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs an empty iterator pointing to no element.
     * 
     * @details 构造一个不指向任何元素的空迭代器。
     */
    list_const_iterator() noexcept : node_(nullptr) {}
    
    /**
     * @brief Constructor with node pointer
     * @brief 带节点指针的构造函数
     * 
     * @param node Pointer to the list node (const version)
     * @param node 指向list节点的指针（常量版本）
     * 
     * @details Constructs an iterator pointing to the specified node.
     * 
     * @details 构造一个指向指定节点的迭代器。
     */
    explicit list_const_iterator(const node_type* node) noexcept : node_(node) {}
    
    /**
     * @brief Conversion constructor from non-const iterator
     * @brief 从非常量迭代器转换的构造函数
     * 
     * @param it Non-const iterator to convert from
     * @param it 要转换的非常量迭代器
     * 
     * @details Constructs a const iterator from a non-const iterator.
     * This allows implicit conversion from `list_iterator<T>` to `list_const_iterator<T>`.
     * 
     * @details 从非常量迭代器构造一个常量迭代器。
     * 这允许从`list_iterator<T>`隐式转换到`list_const_iterator<T>`。
     */
    list_const_iterator(const list_iterator<T>& it) noexcept : node_(it.node()) {}
    
    // ==================== 解引用操作符 ====================
    // ==================== Dereference Operators ====================
    
    /**
     * @brief Dereference operator
     * @brief 解引用操作符
     * 
     * @return Const reference to the element pointed to by the iterator
     * @return 迭代器指向的元素的常量引用
     * 
     * @details Returns a const reference to the element stored in the current node.
     * This operator provides read-only access to the element.
     * 
     * @details 返回存储在当前节点中的元素的常量引用。
     * 此操作符提供对元素的只读访问。
     */
    reference operator*() const noexcept {
        return node_->value;
    }
    
    /**
     * @brief Member access operator
     * @brief 成员访问操作符
     * 
     * @return Const pointer to the element pointed to by the iterator
     * @return 指向迭代器指向的元素的常量指针
     * 
     * @details Returns a const pointer to the element stored in the current node.
     * This operator allows accessing members of the element using arrow notation,
     * but only for read-only operations.
     * 
     * @details 返回指向存储在当前节点中的元素的常量指针。
     * 此操作符允许使用箭头表示法访问元素的成员，
     * 但仅用于只读操作。
     */
    pointer operator->() const noexcept {
        return &node_->value;
    }
    
    // ==================== 递增操作符 ====================
    // ==================== Increment Operators ====================
    
    /**
     * @brief Prefix increment operator
     * @brief 前缀递增操作符
     * 
     * @return Reference to this iterator after increment
     * @return 递增后此迭代器的引用
     * 
     * @details Advances the iterator to the next element in the list.
     * 
     * @details 将迭代器前进到list中的下一个元素。
     */
    list_const_iterator& operator++() noexcept {
        node_ = node_->next;
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
    list_const_iterator operator++(int) noexcept {
        list_const_iterator temp = *this;
        ++(*this);
        return temp;
    }
    
    // ==================== 递减操作符 ====================
    // ==================== Decrement Operators ====================
    
    /**
     * @brief Prefix decrement operator
     * @brief 前缀递减操作符
     * 
     * @return Reference to this iterator after decrement
     * @return 递减后此迭代器的引用
     * 
     * @details Moves the iterator to the previous element in the list.
     * 
     * @details 将迭代器移动到list中的前一个元素。
     */
    list_const_iterator& operator--() noexcept {
        node_ = node_->prev;
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
    list_const_iterator operator--(int) noexcept {
        list_const_iterator temp = *this;
        --(*this);
        return temp;
    }
    
    // ==================== 比较操作符 ====================
    // ==================== Comparison Operators ====================
    
    /**
     * @brief Equality comparison operator
     * @brief 相等比较操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if both iterators point to the same node, false otherwise
     * @return 如果两个迭代器指向同一个节点则返回true，否则返回false
     * 
     * @details Checks if two iterators point to the same node in the list.
     * 
     * @details 检查两个迭代器是否指向list中的同一个节点。
     */
    bool operator==(const list_const_iterator& other) const noexcept {
        return node_ == other.node_;
    }
    
    /**
     * @brief Inequality comparison operator
     * @brief 不等比较操作符
     * 
     * @param other Another iterator to compare with
     * @param other 要比较的另一个迭代器
     * @return true if iterators point to different nodes, false otherwise
     * @return 如果迭代器指向不同的节点则返回true，否则返回false
     * 
     * @details Checks if two iterators point to different nodes in the list.
     * 
     * @details 检查两个迭代器是否指向list中的不同节点。
     */
    bool operator!=(const list_const_iterator& other) const noexcept {
        return node_ != other.node_;
    }
    
    // ==================== 辅助函数 ====================
    // ==================== Helper Functions ====================
    
    /**
     * @brief Get the underlying node pointer
     * @brief 获取底层节点指针
     * 
     * @return Const pointer to the current node
     * @return 指向当前节点的常量指针
     * 
     * @details Returns the underlying node pointer.
     * This function is for internal use by the list class only.
     * 
     * @details 返回底层节点指针。
     * 此函数仅供list类内部使用。
     */
    const node_type* node() const noexcept {
        return node_;
    }
};

// ==================== list Class ====================
// ==================== list 类 ====================

/**
 * @brief Doubly-linked list container
 * @brief 双向链表容器
 * 
 * @tparam T Type of elements stored in the list
 * @tparam T 存储在list中的元素类型
 * @tparam Allocator Allocator type for memory management (default: allocator<T>)
 * @tparam Allocator 用于内存管理的分配器类型（默认：allocator<T>）
 * 
 * @details
 * The list is a sequence container that supports constant time insertion
 * and removal of elements from anywhere in the container. It is implemented
 * as a doubly-linked list with a sentinel node to simplify edge cases.
 * 
 * @details
 * list是一个序列容器，支持从容器中任何位置常数时间插入和删除元素。
 * 它被实现为带有哨兵节点的双向链表，以简化边界情况。
 * 
 * @note Key characteristics:
 * 1. Bidirectional traversal: supports both ++ and -- operators
 * 2. Constant time insertion/removal: O(1) at any position
 * 3. No random access: elements cannot be accessed by index
 * 4. Stable iterators: iterators remain valid unless the element is removed
 * 5. Special operations: splice, merge, sort, reverse, unique
 * 
 * @note 主要特性：
 * 1. 双向遍历：支持++和--操作符
 * 2. 常数时间插入/删除：在任何位置O(1)复杂度
 * 3. 无随机访问：不能通过索引访问元素
 * 4. 稳定的迭代器：除非元素被删除，否则迭代器保持有效
 * 5. 特殊操作：拼接、合并、排序、反转、去重
 * 
 * @see https://en.cppreference.com/w/cpp/container/list
 * @see 相关参考：https://zh.cppreference.com/w/cpp/container/list
 */
template<typename T, typename Allocator = allocator<T>>
class list {
public:
    // ==================== 类型定义 ====================
    // ==================== Type Definitions ====================
    
    /**
     * @brief Type of elements stored in the list
     * @brief list中存储的元素类型
     */
    using value_type = T;
    
    /**
     * @brief Type of the allocator used for memory management
     * @brief 用于内存管理的分配器类型
     */
    using allocator_type = Allocator;
    
    /**
     * @brief Unsigned integer type used for size and capacity
     * @brief 用于大小和容量的无符号整数类型
     */
    using size_type = std::size_t;
    
    /**
     * @brief Signed integer type used for differences between iterators
     * @brief 用于迭代器之间差值的有符号整数类型
     */
    using difference_type = std::ptrdiff_t;
    
    /**
     * @brief Reference type to elements
     * @brief 元素的引用类型
     */
    using reference = value_type&;
    
    /**
     * @brief Const reference type to elements
     * @brief 元素的常量引用类型
     */
    using const_reference = const value_type&;
    
    /**
     * @brief Pointer type to elements
     * @brief 元素的指针类型
     */
    using pointer = T*;
    
    /**
     * @brief Const pointer type to elements
     * @brief 元素的常量指针类型
     */
    using const_pointer = const T*;
    
    // ==================== 迭代器类型 ====================
    // ==================== Iterator Types ====================
    
    /**
     * @brief Bidirectional iterator type for list
     * @brief list的双向迭代器类型
     * 
     * @details Provides read-write access to elements in the list.
     * Supports bidirectional traversal (both forward and backward).
     * 
     * @details 提供对list中元素的读写访问。
     * 支持双向遍历（向前和向后）。
     */
    using iterator = list_iterator<T>;
    
    /**
     * @brief Const bidirectional iterator type for list
     * @brief list的常量双向迭代器类型
     * 
     * @details Provides read-only access to elements in the list.
     * Supports bidirectional traversal (both forward and backward).
     * 
     * @details 提供对list中元素的只读访问。
     * 支持双向遍历（向前和向后）。
     */
    using const_iterator = list_const_iterator<T>;
    
    /**
     * @brief Reverse iterator type for list
     * @brief list的反向迭代器类型
     * 
     * @details Allows traversal of the list in reverse order.
     * This iterator adapts a bidirectional iterator to traverse in reverse.
     * 
     * @details 允许以相反顺序遍历list。
     * 此迭代器适配双向迭代器以反向遍历。
     */
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    
    /**
     * @brief Const reverse iterator type for list
     * @brief list的常量反向迭代器类型
     * 
     * @details Allows traversal of the list in reverse order with read-only access.
     * This iterator adapts a const bidirectional iterator to traverse in reverse.
     * 
     * @details 允许以相反顺序遍历list，提供只读访问。
     * 此迭代器适配常量双向迭代器以反向遍历。
     */
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;
    
private:
    // 节点类型
    using node_type = list_node<T>;
    
    /**
     * @brief Wrapper allocator for list nodes
     * @brief list节点的包装分配器
     * 
     * @details
     * This wrapper adapts the user-provided allocator (for T) to allocate
     * list_node<T> objects instead. It's necessary because the list needs
     * to allocate nodes (which contain T plus prev/next pointers), not
     * just T objects.
     * 
     * @details
     * 此包装器适配用户提供的分配器（用于T）以分配list_node<T>对象。
     * 这是必要的，因为list需要分配节点（包含T加上prev/next指针），
     * 而不仅仅是T对象。
     * 
     * @note This wrapper uses `::operator new` and `::operator delete`
     * directly for simplicity, but in a production implementation it
     * should use the underlying allocator's rebind mechanism.
     * 
     * @note 此包装器直接使用`::operator new`和`::operator delete`以简化实现，
     * 但在生产实现中应使用底层分配器的rebind机制。
     * 
     * @note The wrapper provides all necessary type aliases required by
     * `allocator_traits` to work correctly with the list container.
     * 
     * @note 包装器提供`allocator_traits`所需的所有必要类型别名，
     * 以便与list容器正确工作。
     */
    class node_allocator_wrapper {
    private:
        /**
         * @brief The underlying allocator for T
         * @brief 用于T的底层分配器
         */
        Allocator alloc_;
        
    public:
        // ==================== 类型别名 ====================
        // ==================== Type Aliases ====================
        
        /**
         * @brief Type of allocated objects (list nodes)
         * @brief 分配对象的类型（list节点）
         */
        using value_type = node_type;
        
        /**
         * @brief Pointer type to allocated objects
         * @brief 指向分配对象的指针类型
         */
        using pointer = node_type*;
        
        /**
         * @brief Const pointer type to allocated objects
         * @brief 指向分配对象的常量指针类型
         */
        using const_pointer = const node_type*;
        
        /**
         * @brief Void pointer type
         * @brief void指针类型
         */
        using void_pointer = void*;
        
        /**
         * @brief Const void pointer type
         * @brief 常量void指针类型
         */
        using const_void_pointer = const void*;
        
        /**
         * @brief Size type
         * @brief 大小类型
         */
        using size_type = std::size_t;
        
        /**
         * @brief Difference type
         * @brief 差值类型
         */
        using difference_type = std::ptrdiff_t;
        
        /**
         * @brief Whether to propagate on copy assignment
         * @brief 是否在拷贝赋值时传播
         */
        using propagate_on_container_copy_assignment = std::false_type;
        
        /**
         * @brief Whether to propagate on move assignment
         * @brief 是否在移动赋值时传播
         */
        using propagate_on_container_move_assignment = std::true_type;
        
        /**
         * @brief Whether to propagate on swap
         * @brief 是否在交换时传播
         */
        using propagate_on_container_swap = std::false_type;
        
        /**
         * @brief Whether allocators are always equal
         * @brief 分配器是否总是相等
         */
        using is_always_equal = std::false_type;
        
        // ==================== 构造函数 ====================
        // ==================== Constructors ====================
        
        /**
         * @brief Constructor with allocator
         * @brief 带分配器的构造函数
         * 
         * @param alloc The underlying allocator for T
         * @param alloc 用于T的底层分配器
         * 
         * @details Constructs a node allocator wrapper with the given allocator.
         * 
         * @details 使用给定的分配器构造节点分配器包装器。
         */
        node_allocator_wrapper(const Allocator& alloc = Allocator()) : alloc_(alloc) {}
        
        // ==================== 分配和释放 ====================
        // ==================== Allocation and Deallocation ====================
        
        /**
         * @brief Allocates memory for n nodes
         * @brief 为n个节点分配内存
         * 
         * @param n Number of nodes to allocate
         * @param n 要分配的节点数量
         * @return Pointer to the allocated memory
         * @return 指向分配内存的指针
         * 
         * @details Allocates memory for n list_node<T> objects.
         * Uses `::operator new` for simplicity.
         * 
         * @details 为n个list_node<T>对象分配内存。
         * 为简化实现，使用`::operator new`。
         */
        node_type* allocate(std::size_t n) {
            return static_cast<node_type*>(::operator new(n * sizeof(node_type)));
        }
        
        /**
         * @brief Deallocates memory for nodes
         * @brief 释放节点的内存
         * 
         * @param p Pointer to the memory to deallocate
         * @param p 指向要释放的内存的指针
         * @param n Number of nodes (unused in this implementation)
         * @param n 节点数量（在此实现中未使用）
         * 
         * @details Deallocates memory previously allocated by allocate().
         * Uses `::operator delete` for simplicity.
         * 
         * @details 释放先前由allocate()分配的内存。
         * 为简化实现，使用`::operator delete`。
         */
        void deallocate(node_type* p, std::size_t n) {
            ::operator delete(p);
        }
        
        // ==================== 构造和销毁 ====================
        // ==================== Construction and Destruction ====================
        
        /**
         * @brief Constructs a node in-place
         * @brief 原位构造节点
         * 
         * @tparam Args Types of arguments for node construction
         * @tparam Args 节点构造的参数类型
         * @param p Pointer to the memory where to construct the node
         * @param p 指向要构造节点的内存的指针
         * @param args Arguments to forward to node constructor
         * @param args 转发给节点构造函数的参数
         * 
         * @details Constructs a list_node<T> object at the specified memory location
         * using placement new and perfect forwarding.
         * 
         * @details 使用placement new和完美转发在指定内存位置构造list_node<T>对象。
         */
        template<typename... Args>
        void construct(node_type* p, Args&&... args) {
            new (p) node_type(mystl::forward<Args>(args)...);
        }
        
        /**
         * @brief Destroys a node
         * @brief 销毁节点
         * 
         * @param p Pointer to the node to destroy
         * @param p 指向要销毁的节点的指针
         * 
         * @details Calls the destructor of the list_node<T> object.
         * 
         * @details 调用list_node<T>对象的析构函数。
         */
        void destroy(node_type* p) {
            p->~node_type();
        }
        
        // ==================== 容量 ====================
        // ==================== Capacity ====================
        
        /**
         * @brief Returns the maximum number of nodes that can be allocated
         * @brief 返回可以分配的最大节点数量
         * 
         * @return Maximum number of nodes
         * @return 最大节点数量
         * 
         * @details Returns the theoretical maximum number of nodes that
         * could be allocated, based on the maximum value of size_type
         * divided by the size of node_type.
         * 
         * @details 返回理论上可以分配的最大节点数量，
         * 基于size_type的最大值除以node_type的大小。
         */
        std::size_t max_size() const noexcept {
            return std::size_t(-1) / sizeof(node_type);
        }
        
        // ==================== 辅助函数 ====================
        // ==================== Helper Functions ====================
        
        /**
         * @brief Returns the underlying allocator
         * @brief 返回底层分配器
         * 
         * @return The underlying allocator for T
         * @return 用于T的底层分配器
         * 
         * @details Returns a copy of the underlying allocator used for T.
         * 
         * @details 返回用于T的底层分配器的副本。
         */
        Allocator get_allocator() const {
            return alloc_;
        }
        
        /**
         * @brief Selects allocator for container copy construction
         * @brief 为容器拷贝构造选择分配器
         * 
         * @return A new node allocator wrapper
         * @return 新的节点分配器包装器
         * 
         * @details Returns a new allocator wrapper to be used when
         * constructing a copy of the container. This is called by
         * `allocator_traits::select_on_container_copy_construction`.
         * 
         * @details 返回构造容器副本时要使用的新分配器包装器。
         * 由`allocator_traits::select_on_container_copy_construction`调用。
         */
        node_allocator_wrapper select_on_container_copy_construction() const {
            return node_allocator_wrapper(alloc_);
        }
    };
    
    /**
     * @brief Type alias for node allocator
     * @brief 节点分配器类型别名
     * 
     * @details
     * Alias for the node allocator wrapper that adapts the user-provided
     * allocator for T to allocate list_node<T> objects.
     * 
     * @details
     * 节点分配器包装器的别名，用于适配用户提供的T分配器以分配list_node<T>对象。
     */
    using node_allocator = node_allocator_wrapper;
    
    /**
     * @brief Type alias for node allocator traits
     * @brief 节点分配器traits类型别名
     * 
     * @details
     * Alias for allocator_traits specialized for the node allocator.
     * This provides a uniform interface to access allocator capabilities
     * such as allocate, deallocate, construct, destroy, max_size, etc.
     * 
     * @details
     * 专门用于节点分配器的allocator_traits的别名。
     * 这提供了一个统一的接口来访问分配器能力，
     * 如allocate、deallocate、construct、destroy、max_size等。
     * 
     * @note This type alias is used throughout the list implementation
     * to access allocator operations in a standardized way.
     * 
     * @note 此类型别名在整个list实现中使用，以标准化的方式访问分配器操作。
     * 
     * @note The allocator_traits template provides a layer of abstraction
     * that allows the list to work with any allocator type that meets
     * the allocator requirements.
     * 
     * @note allocator_traits模板提供了一层抽象，
     * 允许list与任何满足分配器要求的分配器类型一起工作。
     * 
     * @see allocator_traits
     * @see node_allocator
     * @see 参考 allocator_traits
     * @see 参考 node_allocator
     */
    using node_traits = allocator_traits<node_allocator>;
    
    // ==================== 数据成员 ====================
    // ==================== Data Members ====================
    
    /**
     * @brief Sentinel node for the doubly-linked list
     * @brief 双向链表的哨兵节点
     * 
     * @details
     * The sentinel node is a special node that marks both the beginning
     * and end of the list. It simplifies edge cases by ensuring that
     * every node (including the first and last) has valid prev and next
     * pointers. The sentinel's next pointer points to the first element,
     * and its prev pointer points to the last element.
     * 
     * @details
     * 哨兵节点是一个特殊节点，标记链表的开始和结束。
     * 它通过确保每个节点（包括第一个和最后一个）都有有效的prev和next指针来简化边界情况。
     * 哨兵的next指针指向第一个元素，prev指针指向最后一个元素。
     * 
     * @note When the list is empty, sentinel_->next == sentinel_->prev == sentinel_
     * 
     * @note 当链表为空时，sentinel_->next == sentinel_->prev == sentinel_
     */
    node_type* sentinel_;
    
    /**
     * @brief Number of elements in the list
     * @brief 链表中的元素数量
     * 
     * @details Maintains the current size of the list for O(1) size() operation.
     * 
     * @details 维护链表的当前大小，以实现O(1)复杂度的size()操作。
     */
    size_type size_;
    
    /**
     * @brief Allocator for list nodes
     * @brief 链表节点的分配器
     * 
     * @details Allocator used to allocate and deallocate list_node<T> objects.
     * This is a wrapper around the user-provided allocator for T.
     * 
     * @details 用于分配和释放list_node<T>对象的分配器。
     * 这是用户提供的T分配器的包装器。
     */
    node_allocator alloc_;
    
public:
    // ==================== 构造函数和析构函数 ====================
    
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs an empty list with no elements.
     * The list will have a size of 0 and use the default allocator.
     * 
     * @details 构造一个没有元素的空list。
     * list的大小为0，使用默认分配器。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This constructor is noexcept and does not throw exceptions.
     * @note 此构造函数是noexcept的，不会抛出异常。
     */
    list() noexcept : size_(0) {
        init_sentinel();
    }
    
    /**
     * @brief Constructor with allocator
     * @brief 带分配器的构造函数
     * 
     * @param alloc Allocator to use for memory management
     * @param alloc 用于内存管理的分配器
     * 
     * @details Constructs an empty list with no elements,
     * using the specified allocator for memory management.
     * 
     * @details 构造一个没有元素的空list，
     * 使用指定的分配器进行内存管理。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This constructor is noexcept and does not throw exceptions.
     * @note 此构造函数是noexcept的，不会抛出异常。
     * 
     * @see get_allocator()
     * @see 参考 get_allocator()
     */
    explicit list(const Allocator& alloc) noexcept 
        : size_(0), alloc_(alloc) {
        init_sentinel();
    }
    
    /**
     * @brief Constructs a list with specified number of elements
     * @brief 构造具有指定数量元素的list
     * 
     * @param count Number of elements to initialize the list with
     * @param count 初始化list的元素数量
     * @param value Value to initialize each element with (default: T())
     * @param value 用于初始化每个元素的值（默认：T()）
     * @param alloc Allocator to use for memory management (default: Allocator())
     * @param alloc 用于内存管理的分配器（默认：Allocator()）
     * 
     * @details
     * Constructs a list with `count` elements, each initialized to a copy of `value`.
     * The list will have `count` elements, all equal to `value`.
     * 
     * @details
     * 构造一个具有`count`个元素的list，每个元素初始化为`value`的副本。
     * list将有`count`个元素，所有元素都等于`value`。
     * 
     * @note Time complexity: O(count)
     * @note 时间复杂度：O(count)
     * 
     * @note If an exception is thrown during construction (e.g., memory allocation failure),
     * the list will be left in a valid empty state.
     * 
     * @note 如果在构造期间抛出异常（例如，内存分配失败），
     * list将处于有效的空状态。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see push_back()
     * @see 参考 push_back()
     */
    explicit list(size_type count, const T& value = T(), const Allocator& alloc = Allocator())
        : size_(0), alloc_(alloc) {
        init_sentinel();
        try {
            for (size_type i = 0; i < count; ++i) {
                push_back(value);
            }
        } catch (...) {
            clear();
            deallocate_sentinel();
            throw;
        }
    }
    
    /**
     * @brief Constructs a list with elements from a range
     * @brief 使用范围中的元素构造list
     * 
     * @tparam InputIt Type of input iterator that satisfies the InputIterator concept.
     * Must support dereferencing (*it) to get the element value and incrementing (++it)
     * to move to the next element.
     * @tparam InputIt 满足InputIterator概念的输入迭代器类型。
     * 必须支持解引用（*it）以获取元素值和递增（++it）以移动到下一个元素。
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to one past the last element in the range
     * @param last 指向范围中最后一个元素之后位置的迭代器
     * @param alloc Allocator to use for memory management (default: Allocator())
     * @param alloc 用于内存管理的分配器（默认：Allocator()）
     * 
     * @details
     * Constructs a list with elements copied from the range [first, last).
     * The range can be any input iterator range, including arrays, other containers,
     * or input streams.
     * 
     * @details
     * 使用从范围[first, last)复制的元素构造list。
     * 范围可以是任何输入迭代器范围，包括数组、其他容器或输入流。
     * 
     * @note Time complexity: O(n) where n is the distance between first and last
     * @note 时间复杂度：O(n)，其中n是first和last之间的距离
     * 
     * @note This constructor provides strong exception safety guarantee:
     * if an exception is thrown during construction, the list is left in a valid
     * empty state and all allocated resources are properly cleaned up.
     * 
     * @note 此构造函数提供强异常安全保证：
     * 如果在构造期间抛出异常，list将处于有效的空状态，
     * 所有已分配的资源都会被正确清理。
     * 
     * @note Exception safety details:
     * 1. If `init_sentinel()` throws `std::bad_alloc`, the constructor propagates
     *    the exception and no memory is allocated.
     * 2. If `insert_range()` throws any exception, the constructor catches it,
     *    clears the list, deallocates the sentinel, and rethrows the exception.
     * 3. If both operations succeed, the constructor completes normally.
     * 
     * @note 异常安全性详情：
     * 1. 如果`init_sentinel()`抛出`std::bad_alloc`，构造函数传播异常且不分配内存。
     * 2. 如果`insert_range()`抛出任何异常，构造函数捕获它，
     *    清空list，释放哨兵节点，并重新抛出异常。
     * 3. 如果两个操作都成功，构造函数正常完成。
     * 
     * @note The constructor uses a try-catch block to ensure that if element
     * construction fails, the list is properly cleaned up before rethrowing
     * the exception.
     * 
     * @note 构造函数使用try-catch块来确保如果元素构造失败，
     * 在重新抛出异常之前会正确清理list。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * @throws Any exception thrown by T's copy constructor
     * @throws T的拷贝构造函数抛出的任何异常
     * 
     * @see insert_range()
     * @see 参考 insert_range()
     */
    template<typename InputIt>
    list(InputIt first, InputIt last, const Allocator& alloc = Allocator())
        : size_(0), alloc_(alloc) {
        init_sentinel();
        try {
            insert_range(end(), first, last);
        } catch (...) {
            clear();
            deallocate_sentinel();
            throw;
        }
    }
    
    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other Another list to copy from
     * @param other 要从中拷贝的另一个list
     * 
     * @details
     * Constructs a list with a copy of each of the elements in `other`,
     * in the same order. The allocator is obtained by calling
     * `node_traits::select_on_container_copy_construction` on `other.alloc_`.
     * 
     * @details
     * 构造一个list，其中包含`other`中每个元素的副本，顺序相同。
     * 分配器通过对`other.alloc_`调用`node_traits::select_on_container_copy_construction`获得。
     * 
     * @note Time complexity: O(n) where n is the size of other
     * @note 时间复杂度：O(n)，其中n是other的大小
     * 
     * @note If an exception is thrown during construction (e.g., memory allocation failure),
     * the list will be left in a valid empty state.
     * 
     * @note 如果在构造期间抛出异常（例如，内存分配失败），
     * list将处于有效的空状态。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see insert_range()
     * @see 参考 insert_range()
     */
    list(const list& other)
        : size_(0), alloc_(node_traits::select_on_container_copy_construction(other.alloc_)) {
        init_sentinel();
        try {
            insert_range(end(), other.begin(), other.end());
        } catch (...) {
            clear();
            deallocate_sentinel();
            throw;
        }
    }
    
    /**
     * @brief Copy constructor with allocator
     * @brief 带分配器的拷贝构造函数
     * 
     * @param other Another list to copy from
     * @param other 要从中拷贝的另一个list
     * @param alloc Allocator to use for memory management
     * @param alloc 用于内存管理的分配器
     * 
     * @details
     * Constructs a list with a copy of each of the elements in `other`,
     * in the same order, using the specified allocator `alloc`.
     * This constructor allows specifying a different allocator than the one
     * used by `other`.
     * 
     * @details
     * 构造一个list，其中包含`other`中每个元素的副本，顺序相同，
     * 使用指定的分配器`alloc`。
     * 此构造函数允许指定与`other`使用的分配器不同的分配器。
     * 
     * @note Time complexity: O(n) where n is the size of other
     * @note 时间复杂度：O(n)，其中n是other的大小
     * 
     * @note If an exception is thrown during construction (e.g., memory allocation failure),
     * the list will be left in a valid empty state.
     * 
     * @note 如果在构造期间抛出异常（例如，内存分配失败），
     * list将处于有效的空状态。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see insert_range()
     * @see 参考 insert_range()
     */
    list(const list& other, const Allocator& alloc)
        : size_(0), alloc_(alloc) {
        init_sentinel();
        try {
            insert_range(end(), other.begin(), other.end());
        } catch (...) {
            clear();
            deallocate_sentinel();
            throw;
        }
    }
    
    /**
     * @brief Move constructor
     * @brief 移动构造函数
     * 
     * @param other Another list to move from
     * @param other 要从中移动的另一个list
     * 
     * @details
     * Constructs a list by moving resources from `other`. After the move,
     * `other` is left in a valid but unspecified state (empty).
     * This operation is noexcept and does not allocate any memory.
     * 
     * @details
     * 通过从`other`移动资源构造list。移动后，
     * `other`处于有效但未指定的状态（空）。
     * 此操作是noexcept的，不分配任何内存。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This constructor is noexcept, meaning it will never throw exceptions.
     * 
     * @note 此构造函数是noexcept的，意味着它永远不会抛出异常。
     * 
     * @note After the move, `other` will be empty (size() == 0) and
     * its sentinel node pointer will be null.
     * 
     * @note 移动后，`other`将为空（size() == 0），
     * 其哨兵节点指针将为null。
     */
    list(list&& other) noexcept
        : sentinel_(other.sentinel_), size_(other.size_), alloc_(mystl::move(other.alloc_)) {
        other.sentinel_ = nullptr;
        other.size_ = 0;
    }
    
    /**
     * @brief Move constructor with allocator
     * @brief 带分配器的移动构造函数
     * 
     * @param other Another list to move from
     * @param other 要从中移动的另一个list
     * @param alloc Allocator to use for memory management
     * @param alloc 用于内存管理的分配器
     * 
     * @details
     * Constructs a list by moving resources from `other` if the allocators compare equal.
     * If the allocators don't compare equal, the elements are copied instead.
     * After the move, `other` is left in a valid but unspecified state (empty).
     * 
     * @details
     * 如果分配器相等，则通过从`other`移动资源构造list。
     * 如果分配器不相等，则改为拷贝元素。
     * 移动后，`other`处于有效但未指定的状态（空）。
     * 
     * @note Time complexity: O(1) if allocators compare equal, O(n) otherwise
     * @note 时间复杂度：如果分配器相等则为O(1)，否则为O(n)
     * 
     * @note This constructor handles the case where allocators need to be compared.
     * If allocators compare equal, resources can be moved; otherwise, elements must be copied.
     * 
     * @note 此构造函数处理需要比较分配器的情况。
     * 如果分配器相等，可以移动资源；否则，必须拷贝元素。
     * 
     * @note After the move (if allocators compare equal), `other` will be empty
     * (size() == 0) and its sentinel node pointer will be null.
     * 
     * @note 移动后（如果分配器相等），`other`将为空（size() == 0），
     * 其哨兵节点指针将为null。
     * 
     * @throws std::bad_alloc if memory allocation fails during copying
     * @throws std::bad_alloc 如果在拷贝期间内存分配失败
     */
    list(list&& other, const Allocator& alloc)
        : size_(0), alloc_(alloc) {
        if (alloc_ == other.alloc_) {
            // 如果分配器相等，可以移动资源
            sentinel_ = other.sentinel_;
            size_ = other.size_;
            
            other.sentinel_ = nullptr;
            other.size_ = 0;
        } else {
            // 分配器不相等，需要拷贝
            init_sentinel();
            try {
                insert_range(end(), other.begin(), other.end());
            } catch (...) {
                clear();
                deallocate_sentinel();
                throw;
            }
        }
    }
    
    /**
     * @brief Constructs a list from an initializer list
     * @brief 从初始化列表构造list
     * 
     * @param init Initializer list containing elements to initialize the list with
     * @param init 包含用于初始化list的元素的初始化列表
     * @param alloc Allocator to use for memory management (default: Allocator())
     * @param alloc 用于内存管理的分配器（默认：Allocator()）
     * 
     * @details
     * Constructs a list with the elements from the initializer list `init`.
     * The elements are copied in the same order as they appear in the initializer list.
     * 
     * @details
     * 使用初始化列表`init`中的元素构造list。
     * 元素按照它们在初始化列表中出现的相同顺序被复制。
     * 
     * @note Time complexity: O(n) where n is the size of the initializer list
     * @note 时间复杂度：O(n)，其中n是初始化列表的大小
     * 
     * @note This constructor delegates to the range constructor with the
     * initializer list's begin() and end() iterators.
     * 
     * @note 此构造函数委托给带有初始化列表的begin()和end()迭代器的范围构造函数。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see list(InputIt first, InputIt last, const Allocator& alloc)
     * @see 参考 list(InputIt first, InputIt last, const Allocator& alloc)
     */
    list(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : list(init.begin(), init.end(), alloc) {}
    
    /**
     * @brief Destructor
     * @brief 析构函数
     * 
     * @details
     * Destroys the list, deallocating all nodes and the sentinel node.
     * All elements are destroyed and their memory is deallocated.
     * 
     * @details
     * 销毁list，释放所有节点和哨兵节点。
     * 所有元素都被销毁，其内存被释放。
     * 
     * @note Time complexity: O(n) where n is the size of the list
     * @note 时间复杂度：O(n)，其中n是list的大小
     * 
     * @note The destructor calls `clear()` to destroy all elements,
     * then calls `deallocate_sentinel()` to deallocate the sentinel node.
     * 
     * @note 析构函数调用`clear()`销毁所有元素，
     * 然后调用`deallocate_sentinel()`释放哨兵节点。
     * 
     * @note If the list is empty (sentinel_ is null), the destructor does nothing.
     * 
     * @note 如果list为空（sentinel_为null），析构函数不执行任何操作。
     */
    ~list() {
        if (sentinel_) {
            clear();
            deallocate_sentinel();
        }
    }
    
    // ==================== 赋值操作符 ====================
    
    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值操作符
     * 
     * @param other Another list to copy from
     * @param other 要从中拷贝的另一个list
     * @return Reference to this list after assignment
     * @return 赋值后此list的引用
     * 
     * @details
     * Replaces the contents of this list with a copy of the contents of `other`.
     * Uses the copy-and-swap idiom to provide strong exception safety guarantee.
     * 
     * @details
     * 将此list的内容替换为`other`内容的副本。
     * 使用拷贝并交换惯用法提供强异常安全保证。
     * 
     * @note Time complexity: O(n) where n is the size of other
     * @note 时间复杂度：O(n)，其中n是other的大小
     * 
     * @note This operator provides strong exception safety guarantee:
     * if an exception is thrown during the copy, the original list remains unchanged.
     * 
     * @note 此操作符提供强异常安全保证：
     * 如果在拷贝期间抛出异常，原始list保持不变。
     * 
     * @note Self-assignment is handled efficiently (does nothing).
     * 
     * @note 自赋值被高效处理（不执行任何操作）。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see swap()
     * @see 参考 swap()
     */
    list& operator=(const list& other) {
        if (this != &other) {
            list temp(other);
            swap(temp);
        }
        return *this;
    }
    
    /**
     * @brief Move assignment operator
     * @brief 移动赋值操作符
     * 
     * @param other Another list to move from
     * @param other 要从中移动的另一个list
     * @return Reference to this list after assignment
     * @return 赋值后此list的引用
     * 
     * @details
     * Replaces the contents of this list with the contents of `other` using move semantics.
     * After the move, `other` is left in a valid but unspecified state (empty).
     * This operation is noexcept and does not allocate any memory.
     * 
     * @details
     * 使用移动语义将此list的内容替换为`other`的内容。
     * 移动后，`other`处于有效但未指定的状态（空）。
     * 此操作是noexcept的，不分配任何内存。
     * 
     * @note Time complexity: O(n) where n is the size of this list (for cleanup)
     * @note 时间复杂度：O(n)，其中n是此list的大小（用于清理）
     * 
     * @note This operator is noexcept, meaning it will never throw exceptions.
     * 
     * @note 此操作符是noexcept的，意味着它永远不会抛出异常。
     * 
     * @note Self-assignment is handled efficiently (does nothing).
     * 
     * @note 自赋值被高效处理（不执行任何操作）。
     * 
     * @note The operator first clears and deallocates the current list's resources,
     * then takes ownership of `other`'s resources, and finally resets `other` to empty.
     * 
     * @note 操作符首先清理并释放当前list的资源，
     * 然后获取`other`资源的所有权，最后将`other`重置为空。
     */
    list& operator=(list&& other) noexcept {
        if (this != &other) {
            clear();
            deallocate_sentinel();
            
            sentinel_ = other.sentinel_;
            size_ = other.size_;
            alloc_ = mystl::move(other.alloc_);
            
            other.sentinel_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    /**
     * @brief Initializer list assignment operator
     * @brief 初始化列表赋值操作符
     * 
     * @param ilist Initializer list containing elements to assign
     * @param ilist 包含要分配的元素的初始化列表
     * @return Reference to this list after assignment
     * @return 赋值后此list的引用
     * 
     * @details
     * Replaces the contents of this list with the elements from the initializer list `ilist`.
     * Uses the copy-and-swap idiom to provide strong exception safety guarantee.
     * 
     * @details
     * 将此list的内容替换为初始化列表`ilist`中的元素。
     * 使用拷贝并交换惯用法提供强异常安全保证。
     * 
     * @note Time complexity: O(n) where n is the size of the initializer list
     * @note 时间复杂度：O(n)，其中n是初始化列表的大小
     * 
     * @note This operator provides strong exception safety guarantee:
     * if an exception is thrown during the copy, the original list remains unchanged.
     * 
     * @note 此操作符提供强异常安全保证：
     * 如果在拷贝期间抛出异常，原始list保持不变。
     * 
     * @note The operator creates a temporary list from the initializer list,
     * then swaps it with the current list.
     * 
     * @note 操作符从初始化列表创建一个临时list，
     * 然后将其与当前list交换。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see swap()
     * @see 参考 swap()
     */
    list& operator=(std::initializer_list<T> ilist) {
        list temp(ilist);
        swap(temp);
        return *this;
    }
    
    // ==================== 容量相关函数 ====================
    
    /**
     * @brief Checks whether the list is empty
     * @brief 检查list是否为空
     * 
     * @return true if the list is empty, false otherwise
     * @return 如果list为空则返回true，否则返回false
     * 
     * @details
     * Checks if the list contains no elements.
     * This function is equivalent to checking if size() == 0.
     * 
     * @details
     * 检查list是否不包含任何元素。
     * 此函数等价于检查size() == 0。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @see size()
     * @see 参考 size()
     */
    bool empty() const noexcept {
        return size_ == 0;
    }
    
    /**
     * @brief Returns the number of elements in the list
     * @brief 返回list中的元素数量
     * 
     * @return Number of elements in the list
     * @return list中的元素数量
     * 
     * @details
     * Returns the number of elements currently stored in the list.
     * This function has constant time complexity because the size is
     * maintained as a member variable.
     * 
     * @details
     * 返回当前存储在list中的元素数量。
     * 此函数具有常数时间复杂度，因为大小作为成员变量维护。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note Unlike some other containers (like forward_list), list maintains
     * its size as a member variable, so size() is an O(1) operation.
     * 
     * @note 与其他一些容器（如forward_list）不同，list将其大小作为成员变量维护，
     * 因此size()是O(1)操作。
     * 
     * @see empty()
     * @see 参考 empty()
     */
    size_type size() const noexcept {
        return size_;
    }
    
    /**
     * @brief Returns the maximum possible number of elements
     * @brief 返回可能的最大元素数量
     * 
     * @return Maximum number of elements the list can hold
     * @return list可以容纳的最大元素数量
     * 
     * @details
     * Returns the maximum number of elements the list can hold due to system
     * or library implementation limitations. This value typically reflects the
     * maximum value of the container's size_type, reduced by an implementation
     * overhead to account for internal data structures.
     * 
     * @details
     * 返回由于系统或库实现限制，list可以容纳的最大元素数量。
     * 此值通常反映容器size_type的最大值，减去实现开销以考虑内部数据结构。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note The actual limit may be lower than the value returned by max_size()
     * due to memory constraints or other runtime factors.
     * 
     * @note 由于内存约束或其他运行时因素，实际限制可能低于max_size()返回的值。
     * 
     * @note This function delegates to the node allocator's max_size() function
     * to determine the maximum number of nodes that can be allocated.
     * 
     * @note 此函数委托给节点分配器的max_size()函数来确定可以分配的最大节点数量。
     * 
     * @see size()
     * @see 参考 size()
     */
    size_type max_size() const noexcept {
        return node_traits::max_size(alloc_);
    }
    
    // ==================== 元素访问 ====================
    
    /**
     * @brief Accesses the first element
     * @brief 访问第一个元素
     * 
     * @return Reference to the first element in the list
     * @return list中第一个元素的引用
     * 
     * @details
     * Returns a reference to the first element in the list.
     * Calling this function on an empty list results in undefined behavior.
     * 
     * @details
     * 返回list中第一个元素的引用。
     * 在空list上调用此函数会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The function does not check if the list is empty.
     * It is the caller's responsibility to ensure the list is not empty.
     * 
     * @note 此函数不检查list是否为空。
     * 调用者有责任确保list不为空。
     * 
     * @warning Calling front() on an empty list causes undefined behavior.
     * 
     * @warning 在空list上调用front()会导致未定义行为。
     * 
     * @see back()
     * @see empty()
     * @see 参考 back()
     * @see 参考 empty()
     */
    reference front() {
        return sentinel_->next->value;
    }
    
    /**
     * @brief Accesses the first element (const version)
     * @brief 访问第一个元素（常量版本）
     * 
     * @return Const reference to the first element in the list
     * @return list中第一个元素的常量引用
     * 
     * @details
     * Returns a const reference to the first element in the list.
     * Calling this function on an empty list results in undefined behavior.
     * 
     * @details
     * 返回list中第一个元素的常量引用。
     * 在空list上调用此函数会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The function does not check if the list is empty.
     * It is the caller's responsibility to ensure the list is not empty.
     * 
     * @note 此函数不检查list是否为空。
     * 调用者有责任确保list不为空。
     * 
     * @warning Calling front() on an empty list causes undefined behavior.
     * 
     * @warning 在空list上调用front()会导致未定义行为。
     * 
     * @see back()
     * @see empty()
     * @see 参考 back()
     * @see 参考 empty()
     */
    const_reference front() const {
        return sentinel_->next->value;
    }
    
    /**
     * @brief Accesses the last element
     * @brief 访问最后一个元素
     * 
     * @return Reference to the last element in the list
     * @return list中最后一个元素的引用
     * 
     * @details
     * Returns a reference to the last element in the list.
     * Calling this function on an empty list results in undefined behavior.
     * 
     * @details
     * 返回list中最后一个元素的引用。
     * 在空list上调用此函数会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The function does not check if the list is empty.
     * It is the caller's responsibility to ensure the list is not empty.
     * 
     * @note 此函数不检查list是否为空。
     * 调用者有责任确保list不为空。
     * 
     * @warning Calling back() on an empty list causes undefined behavior.
     * 
     * @warning 在空list上调用back()会导致未定义行为。
     * 
     * @see front()
     * @see empty()
     * @see 参考 front()
     * @see 参考 empty()
     */
    reference back() {
        return sentinel_->prev->value;
    }
    
    /**
     * @brief Accesses the last element (const version)
     * @brief 访问最后一个元素（常量版本）
     * 
     * @return Const reference to the last element in the list
     * @return list中最后一个元素的常量引用
     * 
     * @details
     * Returns a const reference to the last element in the list.
     * Calling this function on an empty list results in undefined behavior.
     * 
     * @details
     * 返回list中最后一个元素的常量引用。
     * 在空list上调用此函数会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The function does not check if the list is empty.
     * It is the caller's responsibility to ensure the list is not empty.
     * 
     * @note 此函数不检查list是否为空。
     * 调用者有责任确保list不为空。
     * 
     * @warning Calling back() on an empty list causes undefined behavior.
     * 
     * @warning 在空list上调用back()会导致未定义行为。
     * 
     * @see front()
     * @see empty()
     * @see 参考 front()
     * @see 参考 empty()
     */
    const_reference back() const {
        return sentinel_->prev->value;
    }
    
    // ==================== 迭代器 ====================
    
    /**
     * @brief Returns an iterator to the first element
     * @brief 返回指向第一个元素的迭代器
     * 
     * @return Iterator to the first element in the list
     * @return 指向list中第一个元素的迭代器
     * 
     * @details
     * Returns an iterator pointing to the first element in the list.
     * If the list is empty, the returned iterator will be equal to end().
     * 
     * @details
     * 返回指向list中第一个元素的迭代器。
     * 如果list为空，返回的迭代器将等于end()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @see end()
     * @see cbegin()
     * @see 参考 end()
     * @see 参考 cbegin()
     */
    iterator begin() noexcept {
        return iterator(sentinel_->next);
    }
    
    /**
     * @brief Returns a const iterator to the first element
     * @brief 返回指向第一个元素的常量迭代器
     * 
     * @return Const iterator to the first element in the list
     * @return 指向list中第一个元素的常量迭代器
     * 
     * @details
     * Returns a const iterator pointing to the first element in the list.
     * If the list is empty, the returned iterator will be equal to end().
     * 
     * @details
     * 返回指向list中第一个元素的常量迭代器。
     * 如果list为空，返回的迭代器将等于end()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @see end()
     * @see cbegin()
     * @see 参考 end()
     * @see 参考 cbegin()
     */
    const_iterator begin() const noexcept {
        return const_iterator(sentinel_->next);
    }
    
    /**
     * @brief Returns a const iterator to the first element
     * @brief 返回指向第一个元素的常量迭代器
     * 
     * @return Const iterator to the first element in the list
     * @return 指向list中第一个元素的常量迭代器
     * 
     * @details
     * Returns a const iterator pointing to the first element in the list.
     * This function is provided for consistency with C++ standard containers.
     * 
     * @details
     * 返回指向list中第一个元素的常量迭代器。
     * 提供此函数是为了与C++标准容器保持一致。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note This function is equivalent to begin() const.
     * 
     * @note 此函数等价于begin() const。
     * 
     * @see begin()
     * @see cend()
     * @see 参考 begin()
     * @see 参考 cend()
     */
    const_iterator cbegin() const noexcept {
        return const_iterator(sentinel_->next);
    }
    
    /**
     * @brief Returns an iterator to the element following the last element
     * @brief 返回指向最后一个元素之后位置的迭代器
     * 
     * @return Iterator to the element following the last element
     * @return 指向最后一个元素之后位置的迭代器
     * 
     * @details
     * Returns an iterator to the element following the last element in the list.
     * This iterator acts as a placeholder; attempting to access it results in undefined behavior.
     * 
     * @details
     * 返回指向list中最后一个元素之后位置的迭代器。
     * 此迭代器充当占位符；尝试访问它会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note The end() iterator should not be dereferenced.
     * It is used as a sentinel value to indicate the end of the list.
     * 
     * @note end()迭代器不应被解引用。
     * 它用作哨兵值来指示list的结束。
     * 
     * @see begin()
     * @see cend()
     * @see 参考 begin()
     * @see 参考 cend()
     */
    iterator end() noexcept {
        return iterator(sentinel_);
    }
    
    /**
     * @brief Returns a const iterator to the element following the last element
     * @brief 返回指向最后一个元素之后位置的常量迭代器
     * 
     * @return Const iterator to the element following the last element
     * @return 指向最后一个元素之后位置的常量迭代器
     * 
     * @details
     * Returns a const iterator to the element following the last element in the list.
     * This iterator acts as a placeholder; attempting to access it results in undefined behavior.
     * 
     * @details
     * 返回指向list中最后一个元素之后位置的常量迭代器。
     * 此迭代器充当占位符；尝试访问它会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note The end() iterator should not be dereferenced.
     * It is used as a sentinel value to indicate the end of the list.
     * 
     * @note end()迭代器不应被解引用。
     * 它用作哨兵值来指示list的结束。
     * 
     * @see begin()
     * @see cend()
     * @see 参考 begin()
     * @see 参考 cend()
     */
    const_iterator end() const noexcept {
        return const_iterator(sentinel_);
    }
    
    /**
     * @brief Returns a const iterator to the element following the last element
     * @brief 返回指向最后一个元素之后位置的常量迭代器
     * 
     * @return Const iterator to the element following the last element
     * @return 指向最后一个元素之后位置的常量迭代器
     * 
     * @details
     * Returns a const iterator to the element following the last element in the list.
     * This function is provided for consistency with C++ standard containers.
     * 
     * @details
     * 返回指向list中最后一个元素之后位置的常量迭代器。
     * 提供此函数是为了与C++标准容器保持一致。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note This function is equivalent to end() const.
     * 
     * @note 此函数等价于end() const。
     * 
     * @see end()
     * @see cbegin()
     * @see 参考 end()
     * @see 参考 cbegin()
     */
    const_iterator cend() const noexcept {
        return const_iterator(sentinel_);
    }
    
    /**
     * @brief Returns a reverse iterator to the first element of the reversed list
     * @brief 返回指向反转list第一个元素的反向迭代器
     * 
     * @return Reverse iterator to the first element in the reversed list
     * @return 指向反转list中第一个元素的反向迭代器
     * 
     * @details
     * Returns a reverse iterator pointing to the last element of the non-reversed list.
     * This iterator can be used to traverse the list in reverse order.
     * 
     * @details
     * 返回指向非反转list中最后一个元素的反向迭代器。
     * 此迭代器可用于以相反顺序遍历list。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @see rend()
     * @see crbegin()
     * @see 参考 rend()
     * @see 参考 crbegin()
     */
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    
    /**
     * @brief Returns a const reverse iterator to the first element of the reversed list
     * @brief 返回指向反转list第一个元素的常量反向迭代器
     * 
     * @return Const reverse iterator to the first element in the reversed list
     * @return 指向反转list中第一个元素的常量反向迭代器
     * 
     * @details
     * Returns a const reverse iterator pointing to the last element of the non-reversed list.
     * This iterator provides read-only access for traversing the list in reverse order.
     * 
     * @details
     * 返回指向非反转list中最后一个元素的常量反向迭代器。
     * 此迭代器提供只读访问，用于以相反顺序遍历list。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @see rend()
     * @see crbegin()
     * @see 参考 rend()
     * @see 参考 crbegin()
     */
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    
    /**
     * @brief Returns a const reverse iterator to the first element of the reversed list
     * @brief 返回指向反转list第一个元素的常量反向迭代器
     * 
     * @return Const reverse iterator to the first element in the reversed list
     * @return 指向反转list中第一个元素的常量反向迭代器
     * 
     * @details
     * Returns a const reverse iterator pointing to the last element of the non-reversed list.
     * This function is provided for consistency with C++ standard containers.
     * 
     * @details
     * 返回指向非反转list中最后一个元素的常量反向迭代器。
     * 提供此函数是为了与C++标准容器保持一致。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note This function is equivalent to rbegin() const.
     * @note 此函数等价于rbegin() const。
     * 
     * @see rbegin()
     * @see crend()
     * @see 参考 rbegin()
     * @see 参考 crend()
     */
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }
    
    /**
     * @brief Returns a reverse iterator to the element following the last element of the reversed list
     * @brief 返回指向反转list最后一个元素之后位置的反向迭代器
     * 
     * @return Reverse iterator to the element following the last element in the reversed list
     * @return 指向反转list中最后一个元素之后位置的反向迭代器
     * 
     * @details
     * Returns a reverse iterator pointing to the element before the first element of the non-reversed list.
     * This iterator acts as a sentinel for reverse iteration.
     * 
     * @details
     * 返回指向非反转list第一个元素之前位置的反向迭代器。
     * 此迭代器作为反向迭代的哨兵。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @see rbegin()
     * @see crend()
     * @see 参考 rbegin()
     * @see 参考 crend()
     */
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    
    /**
     * @brief Returns a const reverse iterator to the element following the last element of the reversed list
     * @brief 返回指向反转list最后一个元素之后位置的常量反向迭代器
     * 
     * @return Const reverse iterator to the element following the last element in the reversed list
     * @return 指向反转list中最后一个元素之后位置的常量反向迭代器
     * 
     * @details
     * Returns a const reverse iterator pointing to the element before the first element of the non-reversed list.
     * This iterator provides read-only access as a sentinel for reverse iteration.
     * 
     * @details
     * 返回指向非反转list第一个元素之前位置的常量反向迭代器。
     * 此迭代器提供只读访问，作为反向迭代的哨兵。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @see rbegin()
     * @see crend()
     * @see 参考 rbegin()
     * @see 参考 crend()
     */
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    
    /**
     * @brief Returns a const reverse iterator to the element following the last element of the reversed list
     * @brief 返回指向反转list最后一个元素之后位置的常量反向迭代器
     * 
     * @return Const reverse iterator to the element following the last element in the reversed list
     * @return 指向反转list中最后一个元素之后位置的常量反向迭代器
     * 
     * @details
     * Returns a const reverse iterator pointing to the element before the first element of the non-reversed list.
     * This function is provided for consistency with C++ standard containers.
     * 
     * @details
     * 返回指向非反转list第一个元素之前位置的常量反向迭代器。
     * 提供此函数是为了与C++标准容器保持一致。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note This function is equivalent to rend() const.
     * @note 此函数等价于rend() const。
     * 
     * @see rend()
     * @see crbegin()
     * @see 参考 rend()
     * @see 参考 crbegin()
     */
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }
    
    // ==================== 修改操作 ====================
    
    /**
     * @brief Removes all elements from the list
     * @brief 从list中移除所有元素
     * 
     * @details
     * Removes all elements from the list, leaving it with a size of 0.
     * All elements are destroyed and their memory is deallocated.
     * 
     * @details
     * 从list中移除所有元素，使其大小变为0。
     * 所有元素都被销毁，其内存被释放。
     * 
     * @note Time complexity: O(n) where n is the size of the list
     * @note 时间复杂度：O(n)，其中n是list的大小
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note The function works by repeatedly calling pop_front() until
     * the list is empty. This ensures proper destruction of all elements.
     * 
     * @note 此函数通过重复调用pop_front()直到list为空来工作。
     * 这确保所有元素都被正确销毁。
     * 
     * @see empty()
     * @see pop_front()
     * @see 参考 empty()
     * @see 参考 pop_front()
     */
    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }
    
    /**
     * @brief Inserts a copy of an element at the specified position
     * @brief 在指定位置插入元素的副本
     * 
     * @param pos Iterator to the position before which the element will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param value Value to be copied into the inserted element
     * @param value 要复制到插入元素中的值
     * @return Iterator pointing to the inserted element
     * @return 指向插入元素的迭代器
     * 
     * @details
     * Inserts a copy of `value` before the element pointed to by `pos`.
     * The new element is constructed by copying `value`.
     * 
     * @details
     * 在`pos`指向的元素之前插入`value`的副本。
     * 新元素通过复制`value`构造。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not invalidate iterators or references.
     * The iterator `pos` remains valid and points to the same element.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 迭代器`pos`保持有效并指向相同的元素。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see insert_node()
     * @see create_node()
     * @see 参考 insert_node()
     * @see 参考 create_node()
     */
    iterator insert(const_iterator pos, const T& value) {
        return insert_node(pos, create_node(value));
    }
    
    /**
     * @brief Inserts an element by moving it at the specified position
     * @brief 在指定位置通过移动插入元素
     * 
     * @param pos Iterator to the position before which the element will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param value Value to be moved into the inserted element
     * @param value 要移动到插入元素中的值
     * @return Iterator pointing to the inserted element
     * @return 指向插入元素的迭代器
     * 
     * @details
     * Inserts `value` before the element pointed to by `pos` using move semantics.
     * The new element is constructed by moving `value` into it.
     * 
     * @details
     * 在`pos`指向的元素之前使用移动语义插入`value`。
     * 新元素通过将`value`移动到其中构造。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not invalidate iterators or references.
     * The iterator `pos` remains valid and points to the same element.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 迭代器`pos`保持有效并指向相同的元素。
     * 
     * @note After the move, `value` is left in a valid but unspecified state.
     * 
     * @note 移动后，`value`处于有效但未指定的状态。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see insert_node()
     * @see create_node()
     * @see 参考 insert_node()
     * @see 参考 create_node()
     */
    iterator insert(const_iterator pos, T&& value) {
        return insert_node(pos, create_node(mystl::move(value)));
    }
    
    /**
     * @brief Inserts multiple copies of an element at the specified position
     * @brief 在指定位置插入元素的多个副本
     * 
     * @param pos Iterator to the position before which the elements will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param count Number of elements to insert
     * @param count 要插入的元素数量
     * @param value Value to be copied into each inserted element
     * @param value 要复制到每个插入元素中的值
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details
     * Inserts `count` copies of `value` before the element pointed to by `pos`.
     * The elements are inserted in order, so the first copy appears immediately
     * before `pos`, and subsequent copies appear before the previous ones.
     * 
     * @details
     * 在`pos`指向的元素之前插入`count`个`value`的副本。
     * 元素按顺序插入，因此第一个副本出现在`pos`之前，
     * 后续副本出现在前一个副本之前。
     * 
     * @note Time complexity: O(count)
     * @note 时间复杂度：O(count)
     * 
     * @note If `count` is 0, the function returns an iterator equal to `pos`.
     * 
     * @note 如果`count`为0，函数返回等于`pos`的迭代器。
     * 
     * @note This operation does not invalidate iterators or references.
     * The iterator `pos` remains valid and points to the same element.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 迭代器`pos`保持有效并指向相同的元素。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see insert(const_iterator pos, const T& value)
     * @see 参考 insert(const_iterator pos, const T& value)
     */
    iterator insert(const_iterator pos, size_type count, const T& value) {
        if (count == 0) return iterator(const_cast<node_type*>(pos.node()));
        
        iterator result = iterator(const_cast<node_type*>(pos.node()));
        for (size_type i = 0; i < count; ++i) {
            result = insert(pos, value);
        }
        return result;
    }
    
    /**
     * @brief Inserts elements from a range at the specified position
     * @brief 在指定位置插入范围中的元素
     * 
     * @tparam InputIt Type of input iterator
     * @tparam InputIt 输入迭代器类型
     * @param pos Iterator to the position before which the elements will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to one past the last element in the range
     * @param last 指向范围中最后一个元素之后位置的迭代器
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details
     * Inserts elements from the range [first, last) before the element pointed to by `pos`.
     * The elements are inserted in order, so the first element in the range appears
     * immediately before `pos`, and subsequent elements appear before the previous ones.
     * 
     * @details
     * 在`pos`指向的元素之前插入范围[first, last)中的元素。
     * 元素按顺序插入，因此范围中的第一个元素出现在`pos`之前，
     * 后续元素出现在前一个元素之前。
     * 
     * @note Time complexity: O(n) where n is the distance between first and last
     * @note 时间复杂度：O(n)，其中n是first和last之间的距离
     * 
     * @note If the range is empty (first == last), the function returns an iterator equal to `pos`.
     * 
     * @note 如果范围为空（first == last），函数返回等于`pos`的迭代器。
     * 
     * @note This operation does not invalidate iterators or references.
     * The iterator `pos` remains valid and points to the same element.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 迭代器`pos`保持有效并指向相同的元素。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see insert(const_iterator pos, const T& value)
     * @see 参考 insert(const_iterator pos, const T& value)
     */
    template<typename InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        if (first == last) return iterator(const_cast<node_type*>(pos.node()));
        
        iterator result = iterator(const_cast<node_type*>(pos.node()));
        for (; first != last; ++first) {
            result = insert(pos, *first);
        }
        return result;
    }
    
    /**
     * @brief Inserts elements from an initializer list at the specified position
     * @brief 在指定位置插入初始化列表中的元素
     * 
     * @param pos Iterator to the position before which the elements will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param ilist Initializer list containing elements to insert
     * @param ilist 包含要插入的元素的初始化列表
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details
     * Inserts elements from the initializer list `ilist` before the element pointed to by `pos`.
     * The elements are inserted in the same order as they appear in the initializer list.
     * 
     * @details
     * 在`pos`指向的元素之前插入初始化列表`ilist`中的元素。
     * 元素按照它们在初始化列表中出现的相同顺序插入。
     * 
     * @note Time complexity: O(n) where n is the size of the initializer list
     * @note 时间复杂度：O(n)，其中n是初始化列表的大小
     * 
     * @note If the initializer list is empty, the function returns an iterator equal to `pos`.
     * 
     * @note 如果初始化列表为空，函数返回等于`pos`的迭代器。
     * 
     * @note This operation does not invalidate iterators or references.
     * The iterator `pos` remains valid and points to the same element.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 迭代器`pos`保持有效并指向相同的元素。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see insert(const_iterator pos, InputIt first, InputIt last)
     * @see 参考 insert(const_iterator pos, InputIt first, InputIt last)
     */
    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }
    
    /**
     * @brief Inserts an element at the beginning (copy version)
     * @brief 在开头插入元素（拷贝版本）
     * 
     * @param value Value to be copied into the inserted element
     * @param value 要复制到插入元素中的值
     * 
     * @details
     * Inserts a copy of `value` at the beginning of the list.
     * The new element becomes the first element in the list.
     * 
     * @details
     * 在list的开头插入`value`的副本。
     * 新元素成为list中的第一个元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not invalidate iterators or references.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see push_back()
     * @see pop_front()
     * @see 参考 push_back()
     * @see 参考 pop_front()
     */
    void push_front(const T& value) {
        insert(begin(), value);
    }
    
    /**
     * @brief Inserts an element at the beginning (move version)
     * @brief 在开头插入元素（移动版本）
     * 
     * @param value Value to be moved into the inserted element
     * @param value 要移动到插入元素中的值
     * 
     * @details
     * Inserts `value` at the beginning of the list using move semantics.
     * The new element becomes the first element in the list.
     * 
     * @details
     * 使用移动语义在list的开头插入`value`。
     * 新元素成为list中的第一个元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not invalidate iterators or references.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 
     * @note After the move, `value` is left in a valid but unspecified state.
     * 
     * @note 移动后，`value`处于有效但未指定的状态。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see push_back()
     * @see pop_front()
     * @see 参考 push_back()
     * @see 参考 pop_front()
     */
    void push_front(T&& value) {
        insert(begin(), mystl::move(value));
    }
    
    /**
     * @brief Inserts an element at the end (copy version)
     * @brief 在末尾插入元素（拷贝版本）
     * 
     * @param value Value to be copied into the inserted element
     * @param value 要复制到插入元素中的值
     * 
     * @details
     * Inserts a copy of `value` at the end of the list.
     * The new element becomes the last element in the list.
     * 
     * @details
     * 在list的末尾插入`value`的副本。
     * 新元素成为list中的最后一个元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not invalidate iterators or references.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see push_front()
     * @see pop_back()
     * @see 参考 push_front()
     * @see 参考 pop_back()
     */
    void push_back(const T& value) {
        insert(end(), value);
    }
    
    /**
     * @brief Inserts an element at the end (move version)
     * @brief 在末尾插入元素（移动版本）
     * 
     * @param value Value to be moved into the inserted element
     * @param value 要移动到插入元素中的值
     * 
     * @details
     * Inserts `value` at the end of the list using move semantics.
     * The new element becomes the last element in the list.
     * 
     * @details
     * 使用移动语义在list的末尾插入`value`。
     * 新元素成为list中的最后一个元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not invalidate iterators or references.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 
     * @note After the move, `value` is left in a valid but unspecified state.
     * 
     * @note 移动后，`value`处于有效但未指定的状态。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see push_front()
     * @see pop_back()
     * @see 参考 push_front()
     * @see 参考 pop_back()
     */
    void push_back(T&& value) {
        insert(end(), mystl::move(value));
    }
    
    /**
     * @brief Constructs an element in-place at the beginning
     * @brief 在开头原位构造元素
     * 
     * @tparam Args Types of arguments for element construction
     * @tparam Args 元素构造的参数类型
     * @param args Arguments to forward to element constructor
     * @param args 转发给元素构造函数的参数
     * 
     * @details
     * Constructs an element in-place at the beginning of the list.
     * The element is constructed directly in the list's memory,
     * avoiding unnecessary copies or moves.
     * 
     * @details
     * 在list的开头原位构造元素。
     * 元素直接在list的内存中构造，
     * 避免不必要的拷贝或移动。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not invalidate iterators or references.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 
     * @note This function uses perfect forwarding to pass arguments
     * directly to the element's constructor.
     * 
     * @note 此函数使用完美转发将参数直接传递给元素的构造函数。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see emplace_back()
     * @see push_front()
     * @see 参考 emplace_back()
     * @see 参考 push_front()
     */
    template<typename... Args>
    void emplace_front(Args&&... args) {
        insert(begin(), mystl::forward<Args>(args)...);
    }
    
    /**
     * @brief Constructs an element in-place at the end
     * @brief 在末尾原位构造元素
     * 
     * @tparam Args Types of arguments for element construction
     * @tparam Args 元素构造的参数类型
     * @param args Arguments to forward to element constructor
     * @param args 转发给元素构造函数的参数
     * 
     * @details
     * Constructs an element in-place at the end of the list.
     * The element is constructed directly in the list's memory,
     * avoiding unnecessary copies or moves.
     * 
     * @details
     * 在list的末尾原位构造元素。
     * 元素直接在list的内存中构造，
     * 避免不必要的拷贝或移动。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not invalidate iterators or references.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 
     * @note This function uses perfect forwarding to pass arguments
     * directly to the element's constructor.
     * 
     * @note 此函数使用完美转发将参数直接传递给元素的构造函数。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see emplace_front()
     * @see push_back()
     * @see 参考 emplace_front()
     * @see 参考 push_back()
     */
    template<typename... Args>
    void emplace_back(Args&&... args) {
        insert(end(), mystl::forward<Args>(args)...);
    }
    
    /**
     * @brief Constructs an element in-place at the specified position
     * @brief 在指定位置原位构造元素
     * 
     * @tparam Args Types of arguments for element construction
     * @tparam Args 元素构造的参数类型
     * @param pos Iterator to the position before which the element will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param args Arguments to forward to element constructor
     * @param args 转发给元素构造函数的参数
     * @return Iterator pointing to the inserted element
     * @return 指向插入元素的迭代器
     * 
     * @details
     * Constructs an element in-place before the element pointed to by `pos`.
     * The element is constructed directly in the list's memory,
     * avoiding unnecessary copies or moves.
     * 
     * @details
     * 在`pos`指向的元素之前原位构造元素。
     * 元素直接在list的内存中构造，
     * 避免不必要的拷贝或移动。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not invalidate iterators or references.
     * The iterator `pos` remains valid and points to the same element.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 迭代器`pos`保持有效并指向相同的元素。
     * 
     * @note This function uses perfect forwarding to pass arguments
     * directly to the element's constructor.
     * 
     * @note 此函数使用完美转发将参数直接传递给元素的构造函数。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see emplace_front()
     * @see emplace_back()
     * @see insert()
     * @see 参考 emplace_front()
     * @see 参考 emplace_back()
     * @see 参考 insert()
     */
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        return insert_node(pos, create_node(mystl::forward<Args>(args)...));
    }
    
    /**
     * @brief Removes the first element
     * @brief 移除第一个元素
     * 
     * @details
     * Removes the first element from the list.
     * If the list is empty, the function does nothing.
     * The removed element is destroyed and its memory is deallocated.
     * 
     * @details
     * 从list中移除第一个元素。
     * 如果list为空，函数不执行任何操作。
     * 被移除的元素被销毁，其内存被释放。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function does not throw exceptions.
     * 
     * @note 此函数不会抛出异常。
     * 
     * @note The function checks if the list is empty before attempting to remove.
     * 
     * @note 函数在尝试移除前检查list是否为空。
     * 
     * @see pop_back()
     * @see push_front()
     * @see erase()
     * @see 参考 pop_back()
     * @see 参考 push_front()
     * @see 参考 erase()
     */
    void pop_front() {
        if (!empty()) {
            erase(begin());
        }
    }
    
    /**
     * @brief Removes the last element
     * @brief 移除最后一个元素
     * 
     * @details
     * Removes the last element from the list.
     * If the list is empty, the function does nothing.
     * The removed element is destroyed and its memory is deallocated.
     * 
     * @details
     * 从list中移除最后一个元素。
     * 如果list为空，函数不执行任何操作。
     * 被移除的元素被销毁，其内存被释放。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function does not throw exceptions.
     * 
     * @note 此函数不会抛出异常。
     * 
     * @note The function checks if the list is empty before attempting to remove.
     * 
     * @note 函数在尝试移除前检查list是否为空。
     * 
     * @note This function uses `--end()` to get an iterator to the last element,
     * then calls `erase()` to remove it.
     * 
     * @note 此函数使用`--end()`获取指向最后一个元素的迭代器，
     * 然后调用`erase()`移除它。
     * 
     * @see pop_front()
     * @see push_back()
     * @see erase()
     * @see 参考 pop_front()
     * @see 参考 push_back()
     * @see 参考 erase()
     */
    void pop_back() {
        if (!empty()) {
            erase(--end());
        }
    }
    
    /**
     * @brief Erases the element at the specified position
     * @brief 删除指定位置的元素
     * 
     * @param pos Iterator to the element to remove
     * @param pos 指向要删除元素的迭代器
     * @return Iterator following the last removed element
     * @return 指向最后一个被删除元素之后位置的迭代器
     * 
     * @details
     * Removes the element at the position specified by `pos`.
     * The element is destroyed and its memory is deallocated.
     * 
     * @details
     * 删除`pos`指定位置的元素。
     * 元素被销毁，其内存被释放。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The iterator `pos` must be valid and dereferenceable.
     * 
     * @note 迭代器`pos`必须有效且可解引用。
     * 
     * @note Iterators and references to the erased element are invalidated.
     * Other iterators and references remain valid.
     * 
     * @note 指向被删除元素的迭代器和引用失效。
     * 其他迭代器和引用保持有效。
     * 
     * @note The function returns an iterator pointing to the element
     * that followed the erased element, or end() if the last element was erased.
     * 
     * @note 函数返回指向被删除元素之后元素的迭代器，
     * 如果删除的是最后一个元素，则返回end()。
     * 
     * @warning Calling erase() on an end() iterator results in undefined behavior.
     * 
     * @warning 在end()迭代器上调用erase()会导致未定义行为。
     * 
     * @see pop_front()
     * @see pop_back()
     * @see clear()
     * @see 参考 pop_front()
     * @see 参考 pop_back()
     * @see 参考 clear()
     */
    iterator erase(const_iterator pos) {
        node_type* node = const_cast<node_type*>(pos.node());
        node_type* prev = node->prev;
        node_type* next = node->next;
        
        // 从链表中移除节点
        prev->next = next;
        next->prev = prev;
        
        // 销毁节点
        destroy_node(node);
        --size_;
        
        return iterator(next);
    }
    
    /**
     * @brief Swaps the contents of two lists
     * @brief 交换两个list的内容
     * 
     * @param other Another list to swap with
     * @param other 要与之交换的另一个list
     * 
     * @details
     * Exchanges the contents of this list with those of `other`.
     * This operation is noexcept and does not allocate or deallocate any memory.
     * 
     * @details
     * 将此list的内容与`other`的内容交换。
     * 此操作是noexcept的，不分配或释放任何内存。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note All iterators and references remain valid, but now refer to elements
     * in the other list. The past-the-end iterator is invalidated.
     * 
     * @note 所有迭代器和引用保持有效，但现在指向另一个list中的元素。
     * 尾后迭代器失效。
     * 
     * @note This function swaps the sentinel nodes, sizes, and allocators
     * between the two lists.
     * 
     * @note 此函数交换两个list之间的哨兵节点、大小和分配器。
     * 
     * @see std::swap()
     * @see 参考 std::swap()
     */
    void swap(list& other) noexcept {
        using mystl::swap;
        swap(sentinel_, other.sentinel_);
        swap(size_, other.size_);
        swap(alloc_, other.alloc_);
    }
    
    /**
     * @brief Replaces the contents of the list with specified number of copies of a value
     * @brief 用指定数量的值副本替换list的内容
     * 
     * @param count Number of elements to assign
     * @param count 要分配的元素数量
     * @param value Value to initialize each element with
     * @param value 用于初始化每个元素的值
     * 
     * @details
     * Replaces the current contents of the list with `count` copies of `value`.
     * All existing elements are destroyed and new elements are constructed.
     * 
     * @details
     * 用`count`个`value`的副本替换list的当前内容。
     * 所有现有元素被销毁，新元素被构造。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is count
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是count
     * 
     * @note If an exception is thrown during element construction,
     * the list is left in a valid but unspecified state.
     * 
     * @note 如果在元素构造期间抛出异常，
     * list将处于有效但未指定的状态。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see assign(InputIt first, InputIt last)
     * @see assign(std::initializer_list<T> ilist)
     * @see 参考 assign(InputIt first, InputIt last)
     * @see 参考 assign(std::initializer_list<T> ilist)
     */
    void assign(size_type count, const T& value) {
        clear();
        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
    }
    
    /**
     * @brief Replaces the contents of the list with elements from a range
     * @brief 用范围中的元素替换list的内容
     * 
     * @tparam InputIt Type of input iterator
     * @tparam InputIt 输入迭代器类型
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to one past the last element in the range
     * @param last 指向范围中最后一个元素之后位置的迭代器
     * 
     * @details
     * Replaces the current contents of the list with elements from the range [first, last).
     * All existing elements are destroyed and new elements are constructed from the range.
     * 
     * @details
     * 用范围[first, last)中的元素替换list的当前内容。
     * 所有现有元素被销毁，新元素从范围中构造。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is the distance between first and last
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是first和last之间的距离
     * 
     * @note If an exception is thrown during element construction,
     * the list is left in a valid but unspecified state.
     * 
     * @note 如果在元素构造期间抛出异常，
     * list将处于有效但未指定的状态。
     * 
     * @note The range can be any input iterator range, including arrays,
     * other containers, or input streams.
     * 
     * @note 范围可以是任何输入迭代器范围，包括数组、其他容器或输入流。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see assign(size_type count, const T& value)
     * @see assign(std::initializer_list<T> ilist)
     * @see 参考 assign(size_type count, const T& value)
     * @see 参考 assign(std::initializer_list<T> ilist)
     */
    template<typename InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        for (; first != last; ++first) {
            push_back(*first);
        }
    }
    
    /**
     * @brief Replaces the contents of the list with elements from an initializer list
     * @brief 用初始化列表中的元素替换list的内容
     * 
     * @param ilist Initializer list containing elements to assign
     * @param ilist 包含要分配的元素的初始化列表
     * 
     * @details
     * Replaces the current contents of the list with elements from the initializer list `ilist`.
     * All existing elements are destroyed and new elements are constructed from the initializer list.
     * 
     * @details
     * 用初始化列表`ilist`中的元素替换list的当前内容。
     * 所有现有元素被销毁，新元素从初始化列表中构造。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is the size of the initializer list
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是初始化列表的大小
     * 
     * @note If an exception is thrown during element construction,
     * the list is left in a valid but unspecified state.
     * 
     * @note 如果在元素构造期间抛出异常，
     * list将处于有效但未指定的状态。
     * 
     * @note This function delegates to the range version of assign() with
     * the initializer list's begin() and end() iterators.
     * 
     * @note 此函数委托给assign()的范围版本，使用初始化列表的begin()和end()迭代器。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see assign(size_type count, const T& value)
     * @see assign(InputIt first, InputIt last)
     * @see 参考 assign(size_type count, const T& value)
     * @see 参考 assign(InputIt first, InputIt last)
     */
    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }
    
    /**
     * @brief Transfers all elements from another list into this list
     * @brief 将另一个list的所有元素转移到此list中
     * 
     * @param pos Iterator to the position before which the elements will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param other Another list to transfer elements from
     * @param other 要从中转移元素的另一个list
     * 
     * @details
     * Transfers all elements from `other` into this list before the element pointed to by `pos`.
     * The elements are inserted before `pos` in the same order as they were in `other`.
     * After this operation, `other` becomes empty.
     * 
     * @details
     * 在`pos`指向的元素之前将`other`的所有元素转移到此list中。
     * 元素按照它们在`other`中的相同顺序插入到`pos`之前。
     * 此操作后，`other`变为空。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not copy or move elements; it only relinks nodes.
     * No elements are constructed or destroyed.
     * 
     * @note 此操作不复制或移动元素；它只重新链接节点。
     * 没有元素被构造或销毁。
     * 
     * @note All iterators and references to the transferred elements remain valid,
     * but now refer to elements in this list.
     * 
     * @note 所有指向被转移元素的迭代器和引用保持有效，
     * 但现在指向此list中的元素。
     * 
     * @note If `this == &other` or `other` is empty, the function does nothing.
     * 
     * @note 如果`this == &other`或`other`为空，函数不执行任何操作。
     * 
     * @see splice(const_iterator pos, list& other, const_iterator it)
     * @see splice(const_iterator pos, list& other, const_iterator first, const_iterator last)
     * @see 参考 splice(const_iterator pos, list& other, const_iterator it)
     * @see 参考 splice(const_iterator pos, list& other, const_iterator first, const_iterator last)
     */
    void splice(const_iterator pos, list& other) {
        if (this == &other || other.empty()) {
            return;
        }
        
        node_type* pos_node = const_cast<node_type*>(pos.node());
        node_type* other_first = other.sentinel_->next;
        node_type* other_last = other.sentinel_->prev;
        
        // 从other中移除节点
        other.sentinel_->next = other.sentinel_;
        other.sentinel_->prev = other.sentinel_;
        
        // 将节点插入到当前链表中
        node_type* prev_node = pos_node->prev;
        
        prev_node->next = other_first;
        other_first->prev = prev_node;
        
        other_last->next = pos_node;
        pos_node->prev = other_last;
        
        // 更新大小
        size_ += other.size_;
        other.size_ = 0;
    }
    
    /**
     * @brief Transfers a single element from another list into this list
     * @brief 将另一个list中的单个元素转移到此list中
     * 
     * @param pos Iterator to the position before which the element will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param other Another list to transfer the element from
     * @param other 要从中转移元素的另一个list
     * @param it Iterator to the element to transfer from `other`
     * @param it 指向要从`other`转移的元素的迭代器
     * 
     * @details
     * Transfers the element pointed to by `it` from `other` into this list
     * before the element pointed to by `pos`. The element is moved, not copied.
     * After this operation, the element is removed from `other` and inserted
     * into this list.
     * 
     * @details
     * 将`it`指向的元素从`other`转移到此list中，在`pos`指向的元素之前。
     * 元素被移动，而不是复制。
     * 此操作后，元素从`other`中移除并插入到此list中。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operation does not copy or move the element value;
     * it only relinks the node between the two lists.
     * No element is constructed or destroyed.
     * 
     * @note 此操作不复制或移动元素值；
     * 它只在两个链表之间重新链接节点。
     * 没有元素被构造或销毁。
     * 
     * @note All iterators and references to the transferred element remain valid,
     * but now refer to the element in this list.
     * 
     * @note 所有指向被转移元素的迭代器和引用保持有效，
     * 但现在指向此list中的元素。
     * 
     * @note If `this == &other` and `pos == it`, the function does nothing.
     * 
     * @note 如果`this == &other`且`pos == it`，函数不执行任何操作。
     * 
     * @note The iterator `it` must be valid and dereferenceable in `other`.
     * 
     * @note 迭代器`it`必须在`other`中有效且可解引用。
     * 
     * @see splice(const_iterator pos, list& other)
     * @see splice(const_iterator pos, list& other, const_iterator first, const_iterator last)
     * @see 参考 splice(const_iterator pos, list& other)
     * @see 参考 splice(const_iterator pos, list& other, const_iterator first, const_iterator last)
     */
    void splice(const_iterator pos, list& other, const_iterator it) {
        if (this == &other && pos == it) {
            return;
        }
        
        node_type* pos_node = const_cast<node_type*>(pos.node());
        node_type* it_node = const_cast<node_type*>(it.node());
        
        // 从other中移除节点
        it_node->prev->next = it_node->next;
        it_node->next->prev = it_node->prev;
        
        // 将节点插入到当前链表中
        node_type* prev_node = pos_node->prev;
        
        prev_node->next = it_node;
        it_node->prev = prev_node;
        
        it_node->next = pos_node;
        pos_node->prev = it_node;
        
        // 更新大小
        ++size_;
        --other.size_;
    }
    
    /**
     * @brief Transfers a range of elements from another list into this list
     * @brief 将另一个list中的元素范围转移到此list中
     * 
     * @param pos Iterator to the position before which the elements will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param other Another list to transfer elements from
     * @param other 要从中转移元素的另一个list
     * @param first Iterator to the first element in the range to transfer
     * @param first 指向要转移范围中第一个元素的迭代器
     * @param last Iterator to one past the last element in the range to transfer
     * @param last 指向要转移范围中最后一个元素之后位置的迭代器
     * 
     * @details
     * Transfers elements in the range [first, last) from `other` into this list
     * before the element pointed to by `pos`. The elements are moved, not copied.
     * After this operation, the elements are removed from `other` and inserted
     * into this list in the same order.
     * 
     * @details
     * 将范围[first, last)中的元素从`other`转移到此list中，在`pos`指向的元素之前。
     * 元素被移动，而不是复制。
     * 此操作后，元素从`other`中移除并按照相同顺序插入到此list中。
     * 
     * @note Time complexity: O(1) for the operation itself, O(n) to count elements
     * @note 时间复杂度：操作本身O(1)，计数元素O(n)
     * 
     * @note This operation does not copy or move element values;
     * it only relinks nodes between the two lists.
     * No elements are constructed or destroyed.
     * 
     * @note 此操作不复制或移动元素值；
     * 它只在两个链表之间重新链接节点。
     * 没有元素被构造或销毁。
     * 
     * @note All iterators and references to the transferred elements remain valid,
     * but now refer to elements in this list.
     * 
     * @note 所有指向被转移元素的迭代器和引用保持有效，
     * 但现在指向此list中的元素。
     * 
     * @note If the range is empty (first == last), the function does nothing.
     * 
     * @note 如果范围为空（first == last），函数不执行任何操作。
     * 
     * @note The range [first, last) must be a valid range in `other`.
     * 
     * @note 范围[first, last)必须是`other`中的有效范围。
     * 
     * @note The function needs to count the number of elements in the range
     * to update the size counters correctly.
     * 
     * @note 函数需要计算范围中的元素数量以正确更新大小计数器。
     * 
     * @see splice(const_iterator pos, list& other)
     * @see splice(const_iterator pos, list& other, const_iterator it)
     * @see 参考 splice(const_iterator pos, list& other)
     * @see 参考 splice(const_iterator pos, list& other, const_iterator it)
     */
    void splice(const_iterator pos, list& other, const_iterator first, const_iterator last) {
        if (first == last) {
            return;
        }
        
        node_type* pos_node = const_cast<node_type*>(pos.node());
        node_type* first_node = const_cast<node_type*>(first.node());
        node_type* last_node = const_cast<node_type*>(last.node());
        node_type* before_last = last_node->prev;
        
        // 计算移动的节点数量
        size_type count = 0;
        node_type* current = first_node;
        while (current != last_node) {
            ++count;
            current = current->next;
        }
        
        // 从other中移除节点范围
        first_node->prev->next = last_node;
        last_node->prev = first_node->prev;
        
        // 将节点范围插入到当前链表中
        node_type* prev_node = pos_node->prev;
        
        prev_node->next = first_node;
        first_node->prev = prev_node;
        
        before_last->next = pos_node;
        pos_node->prev = before_last;
        
        // 更新大小
        size_ += count;
        other.size_ -= count;
    }
    
    /**
     * @brief Reverses the order of elements in the list
     * @brief 反转list中元素的顺序
     * 
     * @details
     * Reverses the order of elements in the list.
     * This operation is noexcept and does not allocate or deallocate any memory.
     * 
     * @details
     * 反转list中元素的顺序。
     * 此操作是noexcept的，不分配或释放任何内存。
     * 
     * @note Time complexity: O(n) where n is the size of the list
     * @note 时间复杂度：O(n)，其中n是list的大小
     * 
     * @note This function is noexcept and does not throw exceptions.
     * 
     * @note 此函数是noexcept的，不会抛出异常。
     * 
     * @note The function works by swapping the prev and next pointers
     * of each node, effectively reversing the direction of the list.
     * 
     * @note 此函数通过交换每个节点的prev和next指针来工作，
     * 有效地反转了链表的方向。
     * 
     * @note If the list has 0 or 1 elements, the function does nothing.
     * 
     * @note 如果list有0或1个元素，函数不执行任何操作。
     * 
     * @see sort()
     * @see 参考 sort()
     */
    void reverse() noexcept {
        if (size_ <= 1) {
            return;
        }
        
        // 遍历链表，交换每个节点的prev和next指针
        // 算法原理：对于双向链表，反转操作只需交换每个节点的前后指针
        // 时间复杂度：O(n)，空间复杂度：O(1)
        node_type* current = sentinel_->next;
        while (current != sentinel_) {
            // 交换当前节点的前后指针
            // 使用临时变量保存next指针，因为交换后需要继续遍历
            node_type* temp = current->next;
            current->next = current->prev;
            current->prev = temp;
            current = temp;  // 移动到下一个节点（原next节点）
        }
        
        // 交换哨兵节点的前后指针，使其指向新的首尾节点
        node_type* temp = sentinel_->next;
        sentinel_->next = sentinel_->prev;
        sentinel_->prev = temp;
    }
    
    /**
     * @brief Removes all elements equal to the specified value
     * @brief 删除所有等于指定值的元素
     * 
     * @param value Value to compare elements against
     * @param value 用于比较元素的值
     * 
     * @details
     * Removes all elements from the list that compare equal to `value`.
     * The elements are removed using the equality operator (==).
     * 
     * @details
     * 从list中删除所有等于`value`的元素。
     * 使用相等操作符（==）比较元素。
     * 
     * @note Time complexity: O(n) where n is the size of the list
     * @note 时间复杂度：O(n)，其中n是list的大小
     * 
     * @note This function does not throw exceptions unless the equality
     * comparison or element destruction throws.
     * 
     * @note 此函数不会抛出异常，除非相等比较或元素销毁抛出异常。
     * 
     * @note The function traverses the list once, removing matching elements
     * as they are encountered.
     * 
     * @note 函数遍历list一次，遇到匹配的元素时立即删除。
     * 
     * @note Iterators and references to removed elements are invalidated.
     * Other iterators and references remain valid.
     * 
     * @note 指向被删除元素的迭代器和引用失效。
     * 其他迭代器和引用保持有效。
     * 
     * @see remove_if()
     * @see unique()
     * @see 参考 remove_if()
     * @see 参考 unique()
     */
    void remove(const T& value) {
        node_type* current = sentinel_->next;
        while (current != sentinel_) {
            node_type* next = current->next;
            if (current->value == value) {
                // 从链表中移除节点
                current->prev->next = current->next;
                current->next->prev = current->prev;
                
                // 销毁节点
                destroy_node(current);
                --size_;
            }
            current = next;
        }
    }
    
    /**
     * @brief Removes all elements satisfying a predicate
     * @brief 删除所有满足谓词条件的元素
     * 
     * @tparam UnaryPredicate Type of the predicate function
     * @tparam UnaryPredicate 谓词函数的类型
     * @param p Unary predicate that returns true for elements to remove
     * @param p 一元谓词，对于要删除的元素返回true
     * 
     * @details
     * Removes all elements from the list for which predicate `p` returns true.
     * The predicate is applied to each element in the list.
     * 
     * @details
     * 从list中删除所有谓词`p`返回true的元素。
     * 谓词应用于list中的每个元素。
     * 
     * @note Time complexity: O(n) where n is the size of the list
     * @note 时间复杂度：O(n)，其中n是list的大小
     * 
     * @note This function does not throw exceptions unless the predicate
     * or element destruction throws.
     * 
     * @note 此函数不会抛出异常，除非谓词或元素销毁抛出异常。
     * 
     * @note The predicate must be callable with `const T&` and return
     * a value convertible to bool.
     * 
     * @note 谓词必须可以使用`const T&`调用，并返回可转换为bool的值。
     * 
     * @note The function traverses the list once, removing elements
     * for which the predicate returns true.
     * 
     * @note 函数遍历list一次，删除谓词返回true的元素。
     * 
     * @note Iterators and references to removed elements are invalidated.
     * Other iterators and references remain valid.
     * 
     * @note 指向被删除元素的迭代器和引用失效。
     * 其他迭代器和引用保持有效。
     * 
     * @see remove()
     * @see unique()
     * @see 参考 remove()
     * @see 参考 unique()
     */
    template<typename UnaryPredicate>
    void remove_if(UnaryPredicate p) {
        node_type* current = sentinel_->next;
        while (current != sentinel_) {
            node_type* next = current->next;
            if (p(current->value)) {
                // 从链表中移除节点
                current->prev->next = current->next;
                current->next->prev = current->prev;
                
                // 销毁节点
                destroy_node(current);
                --size_;
            }
            current = next;
        }
    }
    
    /**
     * @brief Removes consecutive duplicate elements
     * @brief 删除连续重复元素
     * 
     * @details
     * Removes all but the first element from every consecutive group of
     * equal elements in the list. Only consecutive duplicates are removed;
     * non-consecutive duplicates are not affected.
     * 
     * @details
     * 从list中每个连续相等元素组中删除除第一个元素外的所有元素。
     * 仅删除连续重复元素；非连续重复元素不受影响。
     * 
     * @note Time complexity: O(n) where n is the size of the list
     * @note 时间复杂度：O(n)，其中n是list的大小
     * 
     * @note This function uses the equality operator (==) to compare elements.
     * 
     * @note 此函数使用相等操作符（==）比较元素。
     * 
     * @note If the list has 0 or 1 elements, the function does nothing.
     * 
     * @note 如果list有0或1个元素，函数不执行任何操作。
     * 
     * @note The function traverses the list once, removing consecutive duplicates
     * as they are encountered.
     * 
     * @note 函数遍历list一次，遇到连续重复元素时立即删除。
     * 
     * @note Iterators and references to removed elements are invalidated.
     * Other iterators and references remain valid.
     * 
     * @note 指向被删除元素的迭代器和引用失效。
     * 其他迭代器和引用保持有效。
     * 
     * @see unique(BinaryPredicate p)
     * @see remove()
     * @see 参考 unique(BinaryPredicate p)
     * @see 参考 remove()
     */
    void unique() {
        if (size_ <= 1) {
            return;
        }
        
        node_type* current = sentinel_->next;
        while (current != sentinel_ && current->next != sentinel_) {
            node_type* next = current->next;
            if (current->value == next->value) {
                // 从链表中移除下一个节点
                next->prev->next = next->next;
                next->next->prev = next->prev;
                
                // 销毁节点
                destroy_node(next);
                --size_;
            } else {
                current = next;
            }
        }
    }
    
    /**
     * @brief Removes consecutive duplicate elements using a binary predicate
     * @brief 使用二元谓词删除连续重复元素
     * 
     * @tparam BinaryPredicate Type of the binary predicate function
     * @tparam BinaryPredicate 二元谓词函数的类型
     * @param p Binary predicate that returns true if two elements should be considered equal
     * @param p 二元谓词，如果两个元素应被视为相等则返回true
     * 
     * @details
     * Removes all but the first element from every consecutive group of
     * elements for which the predicate `p` returns true. Only consecutive
     * elements satisfying the predicate are removed.
     * 
     * @details
     * 从list中每个连续满足谓词`p`的元素组中删除除第一个元素外的所有元素。
     * 仅删除连续满足谓词的元素。
     * 
     * @note Time complexity: O(n) where n is the size of the list
     * @note 时间复杂度：O(n)，其中n是list的大小
     * 
     * @note The predicate must be callable with two `const T&` arguments
     * and return a value convertible to bool.
     * 
     * @note 谓词必须可以使用两个`const T&`参数调用，并返回可转换为bool的值。
     * 
     * @note If the list has 0 or 1 elements, the function does nothing.
     * 
     * @note 如果list有0或1个元素，函数不执行任何操作。
     * 
     * @note The function traverses the list once, removing consecutive elements
     * for which the predicate returns true.
     * 
     * @note 函数遍历list一次，删除连续满足谓词的元素。
     * 
     * @note Iterators and references to removed elements are invalidated.
     * Other iterators and references remain valid.
     * 
     * @note 指向被删除元素的迭代器和引用失效。
     * 其他迭代器和引用保持有效。
     * 
     * @see unique()
     * @see remove_if()
     * @see 参考 unique()
     * @see 参考 remove_if()
     */
    template<typename BinaryPredicate>
    void unique(BinaryPredicate p) {
        if (size_ <= 1) {
            return;
        }
        
        node_type* current = sentinel_->next;
        while (current != sentinel_ && current->next != sentinel_) {
            node_type* next = current->next;
            if (p(current->value, next->value)) {
                // 从链表中移除下一个节点
                next->prev->next = next->next;
                next->next->prev = next->prev;
                
                // 销毁节点
                destroy_node(next);
                --size_;
            } else {
                current = next;
            }
        }
    }
    
    /**
     * @brief Merges two sorted lists
     * @brief 合并两个已排序的链表
     * 
     * @param other Another sorted list to merge
     * @param other 要合并的另一个已排序链表
     * 
     * @details
     * Merges the sorted list `other` into this sorted list.
     * Both lists must be sorted in ascending order according to the default
     * less-than operator (<). After the merge, `other` becomes empty.
     * 
     * @details
     * 将已排序链表`other`合并到此已排序链表中。
     * 两个链表必须根据默认的小于操作符（<）按升序排序。
     * 合并后，`other`变为空。
     * 
     * @note Time complexity: O(n + m) where n and m are the sizes of the lists
     * @note 时间复杂度：O(n + m)，其中n和m是链表的大小
     * 
     * @note This function assumes both lists are already sorted.
     * If they are not sorted, the result is undefined.
     * 
     * @note 此函数假设两个链表都已排序。
     * 如果它们未排序，结果是未定义的。
     * 
     * @note The merge is stable: for equivalent elements, elements from `this`
     * list come before elements from `other`.
     * 
     * @note 合并是稳定的：对于等价元素，来自`this`链表的元素
     * 在来自`other`链表的元素之前。
     * 
     * @note This operation does not allocate or deallocate any memory;
     * it only relinks nodes between the two lists.
     * 
     * @note 此操作不分配或释放任何内存；
     * 它只在两个链表之间重新链接节点。
     * 
     * @see merge(list& other, Compare comp)
     * @see sort()
     * @see 参考 merge(list& other, Compare comp)
     * @see 参考 sort()
     */
    void merge(list& other) {
        merge(other, [](const T& a, const T& b) { return a < b; });
    }
    
    /**
     * @brief Merges two sorted lists using a custom comparison function
     * @brief 使用自定义比较函数合并两个已排序的链表
     * 
     * @tparam Compare Type of the comparison function
     * @tparam Compare 比较函数的类型
     * @param other Another sorted list to merge
     * @param other 要合并的另一个已排序链表
     * @param comp Comparison function that returns true if the first argument
     *             should come before the second in the sorted order
     * @param comp 比较函数，如果第一个参数在排序顺序中应该在第二个参数之前则返回true
     * 
     * @details
     * Merges the sorted list `other` into this sorted list using the comparison
     * function `comp`. Both lists must be sorted according to `comp`.
     * After the merge, `other` becomes empty.
     * 
     * @details
     * 使用比较函数`comp`将已排序链表`other`合并到此已排序链表中。
     * 两个链表必须根据`comp`排序。
     * 合并后，`other`变为空。
     * 
     * @note Time complexity: O(n + m) where n and m are the sizes of the lists
     * @note 时间复杂度：O(n + m)，其中n和m是链表的大小
     * 
     * @note This function assumes both lists are already sorted according to `comp`.
     * If they are not sorted, the result is undefined.
     * 
     * @note 此函数假设两个链表都已根据`comp`排序。
     * 如果它们未排序，结果是未定义的。
     * 
     * @note The merge is stable: for equivalent elements (where `comp` returns false
     * for both orderings), elements from `this` list come before elements from `other`.
     * 
     * @note 合并是稳定的：对于等价元素（`comp`对两种顺序都返回false），
     * 来自`this`链表的元素在来自`other`链表的元素之前。
     * 
     * @note The comparison function must provide a strict weak ordering.
     * 
     * @note 比较函数必须提供严格弱序。
     * 
     * @note This operation does not allocate or deallocate any memory;
     * it only relinks nodes between the two lists.
     * 
     * @note 此操作不分配或释放任何内存；
     * 它只在两个链表之间重新链接节点。
     * 
     * @see merge(list& other)
     * @see sort()
     * @see 参考 merge(list& other)
     * @see 参考 sort()
     */
    template<typename Compare>
    void merge(list& other, Compare comp) {
        if (this == &other || other.empty()) {
            return;
        }
        
        if (empty()) {
            // 如果当前链表为空，直接交换
            swap(other);
            return;
        }
        
        iterator this_it = begin();
        iterator other_it = other.begin();
        
        while (this_it != end() && other_it != other.end()) {
            if (comp(*other_it, *this_it)) {
                // other_it应该插入到this_it之前
                iterator next_other = other_it;
                ++next_other;
                
                // 使用splice移动单个节点
                splice(this_it, other, other_it);
                
                other_it = next_other;
            } else {
                ++this_it;
            }
        }
        
        // 如果other还有剩余节点，全部追加到末尾
        if (other_it != other.end()) {
            splice(end(), other, other_it, other.end());
        }
    }
    
    /**
     * @brief Sorts the elements in ascending order
     * @brief 按升序排序元素
     * 
     * @details
     * Sorts the elements in the list in ascending order according to the default
     * less-than operator (<). The sort is stable: the relative order of equivalent
     * elements is preserved.
     * 
     * @details
     * 根据默认的小于操作符（<）按升序对list中的元素进行排序。
     * 排序是稳定的：等价元素的相对顺序被保留。
     * 
     * @note Time complexity: O(n log n) where n is the size of the list
     * @note 时间复杂度：O(n log n)，其中n是list的大小
     * 
     * @note This function uses the merge sort algorithm, which is well-suited
     * for linked lists because it doesn't require random access.
     * 
     * @note 此函数使用归并排序算法，该算法非常适合链表，因为它不需要随机访问。
     * 
     * @note The sort is stable, meaning that elements that compare equal
     * maintain their original relative order.
     * 
     * @note 排序是稳定的，意味着比较相等的元素保持其原始相对顺序。
     * 
     * @note If the list has 0 or 1 elements, the function does nothing.
     * 
     * @note 如果list有0或1个元素，函数不执行任何操作。
     * 
     * @note This function delegates to the templated version with a default
     * comparison function that uses operator<.
     * 
     * @note 此函数委托给使用operator<的默认比较函数的模板版本。
     * 
     * @see sort(Compare comp)
     * @see merge()
     * @see 参考 sort(Compare comp)
     * @see 参考 merge()
     */
    void sort() {
        sort([](const T& a, const T& b) { return a < b; });
    }
    
    /**
     * @brief Sorts the elements using a custom comparison function
     * @brief 使用自定义比较函数排序元素
     * 
     * @tparam Compare Type of the comparison function
     * @tparam Compare 比较函数的类型
     * @param comp Comparison function that returns true if the first argument
     *             should come before the second in the sorted order
     * @param comp 比较函数，如果第一个参数在排序顺序中应该在第二个参数之前则返回true
     * 
     * @details
     * Sorts the elements in the list according to the comparison function `comp`.
     * The sort is stable: the relative order of elements for which `comp` returns
     * false for both orderings is preserved.
     * 
     * @details
     * 根据比较函数`comp`对list中的元素进行排序。
     * 排序是稳定的：对于`comp`对两种顺序都返回false的元素，其相对顺序被保留。
     * 
     * @note Time complexity: O(n log n) where n is the size of the list
     * @note 时间复杂度：O(n log n)，其中n是list的大小
     * 
     * @note This function uses the merge sort algorithm, which is well-suited
     * for linked lists because it doesn't require random access.
     * 
     * @note 此函数使用归并排序算法，该算法非常适合链表，因为它不需要随机访问。
     * 
     * @note The comparison function must provide a strict weak ordering.
     * 
     * @note 比较函数必须提供严格弱序。
     * 
     * @note If the list has 0 or 1 elements, the function does nothing.
     * 
     * @note 如果list有0或1个元素，函数不执行任何操作。
     * 
     * @note The algorithm works by recursively splitting the list into halves,
     * sorting each half, and then merging the sorted halves.
     * 
     * @note 算法通过递归地将list分成两半，
     * 对每一半进行排序，然后合并已排序的两半来工作。
     * 
     * @see sort()
     * @see merge(list& other, Compare comp)
     * @see 参考 sort()
     * @see 参考 merge(list& other, Compare comp)
     */
    template<typename Compare>
    void sort(Compare comp) {
        if (size_ <= 1) {
            return;
        }
        
        // 使用归并排序
        list left, right;
        split(*this, left, right);
        
        left.sort(comp);
        right.sort(comp);
        
        // 合并两个已排序的链表
        merge_sorted(left, right, comp);
    }
    
    // 获取分配器
    allocator_type get_allocator() const noexcept {
        return alloc_.get_allocator();
    }
    
private:
    /**
     * @brief Splits a list into two halves for merge sort
     * @brief 将链表分割为两半用于归并排序
     * 
     * @param source Source list to split
     * @param source 要分割的源链表
     * @param left Left half of the split (first half)
     * @param left 分割的左半部分（前半部分）
     * @param right Right half of the split (second half)
     * @param right 分割的右半部分（后半部分）
     * 
     * @details
     * Splits the source list into two approximately equal halves.
     * This function is used by the merge sort algorithm to divide
     * the list into smaller sublists for sorting.
     * 
     * @details
     * 将源链表分割为两个近似相等的两半。
     * 此函数由归并排序算法使用，将链表分割为更小的子链表进行排序。
     * 
     * @note Time complexity: O(n) where n is the size of the source list
     * @note 时间复杂度：O(n)，其中n是源链表的大小
     * 
     * @note This function uses the slow/fast pointer technique to find
     * the middle of the list efficiently. The fast pointer moves twice
     * as fast as the slow pointer, so when the fast pointer reaches
     * the end, the slow pointer is at the middle.
     * 
     * @note 此函数使用慢/快指针技术高效地找到链表的中间位置。
     * 快指针移动速度是慢指针的两倍，因此当快指针到达末尾时，
     * 慢指针位于中间位置。
     * 
     * @note After the split, the source list becomes empty.
     * 
     * @note 分割后，源链表变为空。
     * 
     * @note The split is performed by relinking nodes, not by copying elements.
     * This makes the operation O(1) in terms of element operations.
     * 
     * @note 分割通过重新链接节点执行，而不是通过复制元素。
     * 这使得操作在元素操作方面是O(1)的。
     * 
     * @note The algorithm works as follows:
     * 1. Use slow and fast pointers to find the middle node
     * 2. Split the list at the middle node
     * 3. Update the size counters for left and right halves
     * 4. Reset the source list to empty state
     * 
     * @note 算法工作流程如下：
     * 1. 使用慢指针和快指针找到中间节点
     * 2. 在中间节点处分割链表
     * 3. 更新左半部分和右半部分的大小计数器
     * 4. 将源链表重置为空状态
     * 
     * @see sort(Compare comp)
     * @see merge_sorted()
     * @see 参考 sort(Compare comp)
     * @see 参考 merge_sorted()
     */
    void split(list& source, list& left, list& right) {
        if (source.empty()) {
            return;
        }
        
        // 找到中间节点
        node_type* slow = source.sentinel_->next;
        node_type* fast = source.sentinel_->next;
        
        while (fast != source.sentinel_ && fast->next != source.sentinel_) {
            slow = slow->next;
            fast = fast->next->next;
        }
        
        // 分割链表
        node_type* mid = slow;
        
        // left包含[begin, mid)
        left.sentinel_->next = source.sentinel_->next;
        left.sentinel_->prev = mid->prev;
        mid->prev->next = left.sentinel_;
        left.sentinel_->next->prev = left.sentinel_;
        
        // right包含[mid, end)
        right.sentinel_->next = mid;
        right.sentinel_->prev = source.sentinel_->prev;
        source.sentinel_->prev->next = right.sentinel_;
        mid->prev = right.sentinel_;
        
        // 更新大小
        size_type left_size = 0;
        node_type* current = left.sentinel_->next;
        while (current != left.sentinel_) {
            ++left_size;
            current = current->next;
        }
        
        left.size_ = left_size;
        right.size_ = source.size_ - left_size;
        source.size_ = 0;
        
        // 重置source
        source.sentinel_->next = source.sentinel_;
        source.sentinel_->prev = source.sentinel_;
    }
    
    /**
     * @brief Merges two sorted lists into this list
     * @brief 将两个已排序的链表合并到此链表中
     * 
     * @tparam Compare Type of the comparison function that defines a strict weak ordering.
     * Must be callable with two arguments of type `const T&` and return a value convertible to `bool`.
     * The function should return `true` if the first argument should come before the second
     * in the sorted order.
     * @tparam Compare 定义严格弱序的比较函数类型。
     * 必须可以使用两个`const T&`类型的参数调用，并返回可转换为`bool`的值。
     * 如果第一个参数在排序顺序中应该在第二个参数之前，函数应返回`true`。
     * @param left First sorted list to merge
     * @param left 要合并的第一个已排序链表
     * @param right Second sorted list to merge
     * @param right 要合并的第二个已排序链表
     * @param comp Comparison function for sorting
     * @param comp 用于排序的比较函数
     * 
     * @details
     * Merges two sorted lists (`left` and `right`) into this list using the
     * comparison function `comp`. This function is used by the merge sort
     * algorithm to combine sorted sublists.
     * 
     * @details
     * 使用比较函数`comp`将两个已排序的链表（`left`和`right`）合并到此链表中。
     * 此函数由归并排序算法使用，用于合并已排序的子链表。
     * 
     * @note Time complexity: O(n + m) where n and m are the sizes of left and right
     * @note 时间复杂度：O(n + m)，其中n和m是left和right的大小
     * 
     * @note This function assumes both input lists are already sorted according to `comp`.
     * If they are not sorted, the result is undefined.
     * 
     * @note 此函数假设两个输入链表都已根据`comp`排序。
     * 如果它们未排序，结果是未定义的。
     * 
     * @note The merge is stable: for equivalent elements (where `comp` returns false
     * for both orderings), elements from `left` come before elements from `right`.
     * 
     * @note 合并是稳定的：对于等价元素（`comp`对两种顺序都返回false），
     * 来自`left`链表的元素在来自`right`链表的元素之前。
     * 
     * @note After the merge, both `left` and `right` lists become empty.
     * 
     * @note 合并后，`left`和`right`链表都变为空。
     * 
     * @note This operation does not allocate or deallocate any memory;
     * it only relinks nodes between the lists. This makes it O(1) in terms
     * of element operations.
     * 
     * @note 此操作不分配或释放任何内存；
     * 它只在链表之间重新链接节点。这使得它在元素操作方面是O(1)的。
     * 
     * @note The algorithm works by comparing the front elements of both lists
     * and moving the smaller one to the result list, repeating until one list
     * is empty, then appending the remaining elements.
     * 
     * @note 算法通过比较两个链表的前端元素，将较小的移动到结果链表，
     * 重复直到一个链表为空，然后追加剩余元素。
     * 
     * @see split()
     * @see sort(Compare comp)
     * @see 参考 split()
     * @see 参考 sort(Compare comp)
     */
    template<typename Compare>
    void merge_sorted(list& left, list& right, Compare comp) {
        node_type* left_current = left.sentinel_->next;
        node_type* right_current = right.sentinel_->next;
        
        sentinel_->next = sentinel_;
        sentinel_->prev = sentinel_;
        size_ = 0;
        
        while (left_current != left.sentinel_ && right_current != right.sentinel_) {
            if (comp(left_current->value, right_current->value)) {
                // 取left的节点
                node_type* next_left = left_current->next;
                append_node(left_current);
                left_current = next_left;
            } else {
                // 取right的节点
                node_type* next_right = right_current->next;
                append_node(right_current);
                right_current = next_right;
            }
        }
        
        // 追加剩余节点
        while (left_current != left.sentinel_) {
            node_type* next_left = left_current->next;
            append_node(left_current);
            left_current = next_left;
        }
        
        while (right_current != right.sentinel_) {
            node_type* next_right = right_current->next;
            append_node(right_current);
            right_current = next_right;
        }
        
        // 重置left和right
        left.sentinel_->next = left.sentinel_;
        left.sentinel_->prev = left.sentinel_;
        left.size_ = 0;
        
        right.sentinel_->next = right.sentinel_;
        right.sentinel_->prev = right.sentinel_;
        right.size_ = 0;
    }
    
    /**
     * @brief Appends a node to the end of the list
     * @brief 将节点追加到链表的末尾
     * 
     * @param node Node to append to the list
     * @param node 要追加到链表的节点
     * 
     * @details
     * Appends a node to the end of the list. This function is used internally
     * by other list operations to add nodes to the list efficiently.
     * 
     * @details
     * 将节点追加到链表的末尾。此函数由其他list操作内部使用，
     * 以高效地向链表添加节点。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function assumes the node is not already part of any list.
     * 
     * @note 此函数假设节点尚未属于任何链表。
     * 
     * @note The function updates the sentinel node's prev pointer and the
     * previous last node's next pointer to link the new node into the list.
     * 
     * @note 此函数更新哨兵节点的prev指针和先前最后一个节点的next指针，
     * 以将新节点链接到链表中。
     * 
     * @note The list size is incremented after the node is appended.
     * 
     * @note 节点追加后，链表大小递增。
     * 
     * @see insert_node()
     * @see create_node()
     * @see 参考 insert_node()
     * @see 参考 create_node()
     */
    void append_node(node_type* node) {
        node->prev = sentinel_->prev;
        node->next = sentinel_;
        
        sentinel_->prev->next = node;
        sentinel_->prev = node;
        
        ++size_;
    }
    
private:
    /**
     * @brief Initializes the sentinel node for the list
     * @brief 初始化list的哨兵节点
     * 
     * @details
     * Allocates and constructs the sentinel node for the doubly-linked list.
     * The sentinel node is a special node that marks both the beginning
     * and end of the list, simplifying edge cases in list operations.
     * 
     * @details
     * 为双向链表分配并构造哨兵节点。
     * 哨兵节点是一个特殊节点，标记链表的开始和结束，
     * 简化list操作中的边界情况。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is called by list constructors to set up the
     * initial state of the list.
     * 
     * @note 此函数由list构造函数调用，以设置list的初始状态。
     * 
     * @note The sentinel node is self-referential: its prev and next
     * pointers both point to itself when the list is empty.
     * 
     * @note 哨兵节点是自引用的：当链表为空时，其prev和next指针都指向自身。
     * 
     * @note If memory allocation or construction fails, the function
     * properly cleans up any allocated resources before rethrowing the exception.
     * 
     * @note 如果内存分配或构造失败，函数在重新抛出异常之前
     * 会正确清理任何已分配的资源。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see deallocate_sentinel()
     * @see 参考 deallocate_sentinel()
     */
    void init_sentinel() {
        sentinel_ = node_traits::allocate(alloc_, 1);
        try {
            node_traits::construct(alloc_, sentinel_);
            sentinel_->prev = sentinel_;
            sentinel_->next = sentinel_;
        } catch (...) {
            node_traits::deallocate(alloc_, sentinel_, 1);
            throw;
        }
    }
    
    /**
     * @brief Deallocates the sentinel node of the list
     * @brief 释放list的哨兵节点
     * 
     * @details
     * Destroys and deallocates the sentinel node of the doubly-linked list.
     * This function is called by the destructor and other operations that
     * need to clean up the list's internal structure.
     * 
     * @details
     * 销毁并释放双向链表的哨兵节点。
     * 此函数由析构函数和其他需要清理list内部结构的操作调用。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function should only be called when the list is empty
     * (size() == 0) or when the list is being destroyed.
     * 
     * @note 此函数应仅在list为空（size() == 0）或list正在被销毁时调用。
     * 
     * @note The function first destroys the sentinel node object,
     * then deallocates its memory using the node allocator.
     * 
     * @note 函数首先销毁哨兵节点对象，
     * 然后使用节点分配器释放其内存。
     * 
     * @note This function does not throw exceptions.
     * 
     * @note 此函数不会抛出异常。
     * 
     * @see init_sentinel()
     * @see ~list()
     * @see 参考 init_sentinel()
     * @see 参考 ~list()
     */
    void deallocate_sentinel() {
        node_traits::destroy(alloc_, sentinel_);
        node_traits::deallocate(alloc_, sentinel_, 1);
    }
    
    /**
     * @brief Creates a node with a copy of the given value
     * @brief 创建包含给定值副本的节点
     * 
     * @param value Value to copy into the node
     * @param value 要复制到节点中的值
     * @return Pointer to the newly created node
     * @return 指向新创建节点的指针
     * 
     * @details
     * Allocates memory for a new node and constructs it with a copy of `value`.
     * This function handles memory allocation and construction with proper
     * exception safety: if construction fails, allocated memory is deallocated.
     * 
     * @details
     * 为新节点分配内存，并使用`value`的副本构造它。
     * 此函数以适当的异常安全性处理内存分配和构造：
     * 如果构造失败，已分配的内存将被释放。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is used internally by list operations that need to
     * create new nodes with specific values.
     * 
     * @note 此函数由需要创建具有特定值的新节点的list操作内部使用。
     * 
     * @note The function provides strong exception safety guarantee:
     * if an exception is thrown during allocation or construction,
     * no memory is leaked and the list remains unchanged.
     * 
     * @note 此函数提供强异常安全保证：
     * 如果在分配或构造期间抛出异常，
     * 不会发生内存泄漏，list保持不变。
     * 
     * @note Exception safety details:
     * 1. If `node_traits::allocate` throws `std::bad_alloc`, the function propagates
     *    the exception and no memory is allocated.
     * 2. If `node_traits::construct` throws any exception, the allocated memory
     *    is deallocated before rethrowing the exception.
     * 3. If both operations succeed, the function returns a valid node pointer.
     * 
     * @note 异常安全性详情：
     * 1. 如果`node_traits::allocate`抛出`std::bad_alloc`，函数传播异常且不分配内存。
     * 2. 如果`node_traits::construct`抛出任何异常，在重新抛出异常之前会释放已分配的内存。
     * 3. 如果两个操作都成功，函数返回有效的节点指针。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * @throws Any exception thrown by T's copy constructor
     * @throws T的拷贝构造函数抛出的任何异常
     * 
     * @see create_node(T&& value)
     * @see create_node(Args&&... args)
     * @see destroy_node()
     * @see 参考 create_node(T&& value)
     * @see 参考 create_node(Args&&... args)
     * @see 参考 destroy_node()
     */
    node_type* create_node(const T& value) {
        node_type* node = node_traits::allocate(alloc_, 1);
        try {
            node_traits::construct(alloc_, node, value);
            return node;
        } catch (...) {
            node_traits::deallocate(alloc_, node, 1);
            throw;
        }
    }
    
    /**
     * @brief Creates a node by moving the given value
     * @brief 通过移动给定值创建节点
     * 
     * @param value Value to move into the node
     * @param value 要移动到节点中的值
     * @return Pointer to the newly created node
     * @return 指向新创建节点的指针
     * 
     * @details
     * Allocates memory for a new node and constructs it by moving `value` into it.
     * This function handles memory allocation and construction with proper
     * exception safety: if construction fails, allocated memory is deallocated.
     * 
     * @details
     * 为新节点分配内存，并通过将`value`移动到其中来构造它。
     * 此函数以适当的异常安全性处理内存分配和构造：
     * 如果构造失败，已分配的内存将被释放。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is used internally by list operations that need to
     * create new nodes with move semantics.
     * 
     * @note 此函数由需要以移动语义创建新节点的list操作内部使用。
     * 
     * @note After the move, `value` is left in a valid but unspecified state.
     * 
     * @note 移动后，`value`处于有效但未指定的状态。
     * 
     * @note The function provides strong exception safety guarantee:
     * if an exception is thrown, no memory is leaked.
     * 
     * @note 此函数提供强异常安全保证：
     * 如果抛出异常，不会发生内存泄漏。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see create_node(const T& value)
     * @see create_node(Args&&... args)
     * @see destroy_node()
     * @see 参考 create_node(const T& value)
     * @see 参考 create_node(Args&&... args)
     * @see 参考 destroy_node()
     */
    node_type* create_node(T&& value) {
        node_type* node = node_traits::allocate(alloc_, 1);
        try {
            node_traits::construct(alloc_, node, mystl::move(value));
            return node;
        } catch (...) {
            node_traits::deallocate(alloc_, node, 1);
            throw;
        }
    }
    
    /**
     * @brief Creates a node in-place with variadic arguments
     * @brief 使用可变参数原位创建节点
     * 
     * @tparam Args Types of arguments for node construction
     * @tparam Args 节点构造的参数类型
     * @param args Arguments to forward to node constructor
     * @param args 转发给节点构造函数的参数
     * @return Pointer to the newly created node
     * @return 指向新创建节点的指针
     * 
     * @details
     * Allocates memory for a new node and constructs it in-place using
     * perfect forwarding of arguments. This function is used for emplace
     * operations to avoid unnecessary copies or moves.
     * 
     * @details
     * 为新节点分配内存，并使用参数的完美转发原位构造它。
     * 此函数用于emplace操作，以避免不必要的拷贝或移动。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is used internally by emplace operations to
     * construct elements directly in the list's memory.
     * 
     * @note 此函数由emplace操作内部使用，以直接在list的内存中构造元素。
     * 
     * @note The function uses perfect forwarding to pass arguments directly
     * to the element's constructor.
     * 
     * @note 此函数使用完美转发将参数直接传递给元素的构造函数。
     * 
     * @note The function provides strong exception safety guarantee:
     * if an exception is thrown, no memory is leaked.
     * 
     * @note 此函数提供强异常安全保证：
     * 如果抛出异常，不会发生内存泄漏。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see create_node(const T& value)
     * @see create_node(T&& value)
     * @see destroy_node()
     * @see emplace()
     * @see 参考 create_node(const T& value)
     * @see 参考 create_node(T&& value)
     * @see 参考 destroy_node()
     * @see 参考 emplace()
     */
    template<typename... Args>
    node_type* create_node(Args&&... args) {
        node_type* node = node_traits::allocate(alloc_, 1);
        try {
            node_traits::construct(alloc_, node, nullptr, nullptr, mystl::forward<Args>(args)...);
            return node;
        } catch (...) {
            node_traits::deallocate(alloc_, node, 1);
            throw;
        }
    }
    
    /**
     * @brief Inserts a node at the specified position
     * @brief 在指定位置插入节点
     * 
     * @param pos Iterator to the position before which the node will be inserted
     * @param pos 指向要插入节点之前位置的迭代器
     * @param new_node Node to insert
     * @param new_node 要插入的节点
     * @return Iterator pointing to the inserted node
     * @return 指向插入节点的迭代器
     * 
     * @details
     * Inserts a node before the element pointed to by `pos`.
     * This function is used internally by other list operations to insert
     * nodes efficiently without allocating or constructing new elements.
     * 
     * @details
     * 在`pos`指向的元素之前插入节点。
     * 此函数由其他list操作内部使用，以高效地插入节点，
     * 而无需分配或构造新元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function assumes `new_node` is not already part of any list.
     * 
     * @note 此函数假设`new_node`尚未属于任何链表。
     * 
     * @note The function updates the prev and next pointers of the surrounding
     * nodes to link the new node into the list.
     * 
     * @note 此函数更新周围节点的prev和next指针，以将新节点链接到链表中。
     * 
     * @note The list size is incremented after the node is inserted.
     * 
     * @note 节点插入后，链表大小递增。
     * 
     * @note This operation does not invalidate iterators or references.
     * The iterator `pos` remains valid and points to the same element.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 迭代器`pos`保持有效并指向相同的元素。
     * 
     * @see create_node()
     * @see insert()
     * @see 参考 create_node()
     * @see 参考 insert()
     */
    iterator insert_node(const_iterator pos, node_type* new_node) {
        node_type* pos_node = const_cast<node_type*>(pos.node());
        node_type* prev_node = pos_node->prev;
        
        // 连接新节点
        new_node->prev = prev_node;
        new_node->next = pos_node;
        
        // 更新前后节点的连接
        prev_node->next = new_node;
        pos_node->prev = new_node;
        
        ++size_;
        return iterator(new_node);
    }
    
    /**
     * @brief Destroys and deallocates a node
     * @brief 销毁并释放节点
     * 
     * @param node Node to destroy and deallocate
     * @param node 要销毁并释放的节点
     * 
     * @details
     * Destroys the node object and deallocates its memory.
     * This function is used internally by list operations to clean up
     * nodes that are no longer needed.
     * 
     * @details
     * 销毁节点对象并释放其内存。
     * 此函数由list操作内部使用，以清理不再需要的节点。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function does not throw exceptions.
     * 
     * @note 此函数不会抛出异常。
     * 
     * @note The function first calls the node's destructor to destroy
     * the stored element, then deallocates the node's memory.
     * 
     * @note 函数首先调用节点的析构函数来销毁存储的元素，
     * 然后释放节点的内存。
     * 
     * @note This function is the counterpart to `create_node()` and should
     * be used to clean up nodes created by `create_node()`.
     * 
     * @note 此函数是`create_node()`的对应函数，应用于清理由`create_node()`创建的节点。
     * 
     * @see create_node()
     * @see erase()
     * @see clear()
     * @see 参考 create_node()
     * @see 参考 erase()
     * @see 参考 clear()
     */
    void destroy_node(node_type* node) {
        node_traits::destroy(alloc_, node);
        node_traits::deallocate(alloc_, node, 1);
    }
    
    /**
     * @brief Inserts elements from a range at the specified position
     * @brief 在指定位置插入范围中的元素
     * 
     * @tparam InputIt Type of input iterator
     * @tparam InputIt 输入迭代器类型
     * @param pos Iterator to the position before which the elements will be inserted
     * @param pos 指向要插入元素之前位置的迭代器
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to one past the last element in the range
     * @param last 指向范围中最后一个元素之后位置的迭代器
     * 
     * @details
     * Inserts elements from the range [first, last) before the element pointed to by `pos`.
     * This function is used internally by list constructors to efficiently insert
     * multiple elements from a range.
     * 
     * @details
     * 在`pos`指向的元素之前插入范围[first, last)中的元素。
     * 此函数由list构造函数内部使用，以高效地从范围插入多个元素。
     * 
     * @note Time complexity: O(n) where n is the distance between first and last
     * @note 时间复杂度：O(n)，其中n是first和last之间的距离
     * 
     * @note This function delegates to the public `insert()` function for each element.
     * 
     * @note 此函数为每个元素委托给公共`insert()`函数。
     * 
     * @note If the range is empty (first == last), the function does nothing.
     * 
     * @note 如果范围为空（first == last），函数不执行任何操作。
     * 
     * @note This operation does not invalidate iterators or references.
     * The iterator `pos` remains valid and points to the same element.
     * 
     * @note 此操作不会使迭代器或引用失效。
     * 迭代器`pos`保持有效并指向相同的元素。
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::bad_alloc 如果内存分配失败
     * 
     * @see insert(const_iterator pos, InputIt first, InputIt last)
     * @see list(InputIt first, InputIt last, const Allocator& alloc)
     * @see 参考 insert(const_iterator pos, InputIt first, InputIt last)
     * @see 参考 list(InputIt first, InputIt last, const Allocator& alloc)
     */
    template<typename InputIt>
    void insert_range(const_iterator pos, InputIt first, InputIt last) {
        for (; first != last; ++first) {
            insert(pos, *first);
        }
    }
};

/**
 * @brief Swaps the contents of two lists
 * @brief 交换两个list的内容
 * 
 * @tparam T Type of elements in the list
 * @tparam T list中元素的类型
 * @tparam Alloc Allocator type for memory management
 * @tparam Alloc 用于内存管理的分配器类型
 * @param lhs First list to swap
 * @param lhs 要交换的第一个list
 * @param rhs Second list to swap
 * @param rhs 要交换的第二个list
 * 
 * @details
 * Swaps the contents of `lhs` and `rhs` by calling `lhs.swap(rhs)`.
 * This function is provided to enable ADL (Argument-Dependent Lookup)
 * for list swap operations.
 * 
 * @details
 * 通过调用`lhs.swap(rhs)`交换`lhs`和`rhs`的内容。
 * 提供此函数以启用list交换操作的ADL（参数依赖查找）。
 * 
 * @note Time complexity: O(1)
 * @note 时间复杂度：O(1)
 * 
 * @note This function is noexcept and does not throw exceptions.
 * 
 * @note 此函数是noexcept的，不会抛出异常。
 * 
 * @note This function delegates to the member function `swap()` of the list class.
 * 
 * @note 此函数委托给list类的成员函数`swap()`。
 * 
 * @see list::swap()
 * @see 参考 list::swap()
 */
template<typename T, typename Alloc>
void swap(list<T, Alloc>& lhs, list<T, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

/**
 * @brief Equality comparison operator for lists
 * @brief list的相等比较操作符
 * 
 * @tparam T Type of elements in the list
 * @tparam T list中元素的类型
 * @tparam Alloc Allocator type for memory management
 * @tparam Alloc 用于内存管理的分配器类型
 * @param lhs First list to compare
 * @param lhs 要比较的第一个list
 * @param rhs Second list to compare
 * @param rhs 要比较的第二个list
 * @return true if the lists are equal, false otherwise
 * @return 如果list相等则返回true，否则返回false
 * 
 * @details
 * Checks if two lists are equal by comparing their sizes first,
 * then comparing their elements pairwise using `mystl::equal`.
 * Two lists are equal if they have the same size and all corresponding
 * elements compare equal.
 * 
 * @details
 * 通过首先比较它们的大小，然后使用`mystl::equal`逐对比较它们的元素来检查两个list是否相等。
 * 如果两个list具有相同的大小且所有对应元素比较相等，则它们相等。
 * 
 * @note Time complexity: O(n) where n is the size of the lists
 * @note 时间复杂度：O(n)，其中n是list的大小
 * 
 * @note The comparison uses the equality operator (==) for the element type T.
 * 
 * @note 比较使用元素类型T的相等操作符（==）。
 * 
 * @note If the lists have different sizes, the function returns false immediately
 * without comparing elements. This optimization reduces the worst-case time
 * complexity when lists have different sizes.
 * 
 * @note 如果list具有不同的大小，函数立即返回false而不比较元素。
 * 此优化减少了当list具有不同大小时的最坏情况时间复杂度。
 * 
 * @note The algorithm uses `mystl::equal` which stops at the first mismatch,
 * providing early exit for unequal lists.
 * 
 * @note 算法使用`mystl::equal`，它在第一个不匹配处停止，
 * 为不相等的list提供早期退出。
 * 
 * @see operator!=()
 * @see mystl::equal()
 * @see 参考 operator!=()
 * @see 参考 mystl::equal()
 */
template<typename T, typename Alloc>
bool operator==(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/**
 * @brief Inequality comparison operator for lists
 * @brief list的不等比较操作符
 * 
 * @tparam T Type of elements in the list
 * @tparam T list中元素的类型
 * @tparam Alloc Allocator type for memory management
 * @tparam Alloc 用于内存管理的分配器类型
 * @param lhs First list to compare
 * @param lhs 要比较的第一个list
 * @param rhs Second list to compare
 * @param rhs 要比较的第二个list
 * @return true if the lists are not equal, false otherwise
 * @return 如果list不相等则返回true，否则返回false
 * 
 * @details
 * Checks if two lists are not equal by delegating to the equality operator.
 * This operator is the logical negation of the equality operator.
 * 
 * @details
 * 通过委托给相等操作符来检查两个list是否不相等。
 * 此操作符是相等操作符的逻辑否定。
 * 
 * @note Time complexity: O(n) where n is the size of the lists
 * @note 时间复杂度：O(n)，其中n是list的大小
 * 
 * @note This operator is equivalent to `!(lhs == rhs)`.
 * 
 * @note 此操作符等价于`!(lhs == rhs)`。
 * 
 * @note The operator delegates to `operator==` and negates its result,
 * so it has the same time complexity as the equality operator.
 * 
 * @note 此操作符委托给`operator==`并否定其结果，
 * 因此它具有与相等操作符相同的时间复杂度。
 * 
 * @see operator==()
 * @see 参考 operator==()
 */
template<typename T, typename Alloc>
bool operator!=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief Less-than comparison operator for lists
 * @brief list的小于比较操作符
 * 
 * @tparam T Type of elements in the list
 * @tparam T list中元素的类型
 * @tparam Alloc Allocator type for memory management
 * @tparam Alloc 用于内存管理的分配器类型
 * @param lhs First list to compare
 * @param lhs 要比较的第一个list
 * @param rhs Second list to compare
 * @param rhs 要比较的第二个list
 * @return true if lhs is lexicographically less than rhs, false otherwise
 * @return 如果lhs在字典序上小于rhs则返回true，否则返回false
 * 
 * @details
 * Performs lexicographical comparison of two lists using `mystl::lexicographical_compare`.
 * The comparison is done element by element, and the first mismatching element
 * determines the result. If one list is a prefix of the other, the shorter list
 * is considered less.
 * 
 * @details
 * 使用`mystl::lexicographical_compare`对两个list执行字典序比较。
 * 比较是逐元素进行的，第一个不匹配的元素决定结果。
 * 如果一个list是另一个的前缀，则较短的list被认为较小。
 * 
 * @note Time complexity: O(min(n, m)) where n and m are the sizes of the lists
 * @note 时间复杂度：O(min(n, m))，其中n和m是list的大小
 * 
 * @note The comparison uses the less-than operator (<) for the element type T.
 * 
 * @note 比较使用元素类型T的小于操作符（<）。
 * 
 * @note Lexicographical comparison is the standard way to compare sequences
 * in C++ and is used by all standard sequence containers.
 * 
 * @note 字典序比较是C++中比较序列的标准方式，被所有标准序列容器使用。
 * 
 * @note The algorithm uses `mystl::lexicographical_compare` which stops at the first
 * mismatch, providing early exit for lists that differ early.
 * 
 * @note 算法使用`mystl::lexicographical_compare`，它在第一个不匹配处停止，
 * 为早期不同的list提供早期退出。
 * 
 * @note If the lists have different sizes and the shorter list is a prefix of the
 * longer list, the shorter list is considered less.
 * 
 * @note 如果list具有不同的大小且较短的list是较长list的前缀，
 * 则较短的list被认为较小。
 * 
 * @see operator<=()
 * @see operator>()
 * @see operator>=()
 * @see mystl::lexicographical_compare()
 * @see 参考 operator<=()
 * @see 参考 operator>()
 * @see 参考 operator>=()
 * @see 参考 mystl::lexicographical_compare()
 */
template<typename T, typename Alloc>
bool operator<(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
    return mystl::lexicographical_compare(lhs.begin(), lhs.end(),
                                         rhs.begin(), rhs.end());
}

/**
 * @brief Less-than-or-equal-to comparison operator for lists
 * @brief list的小于等于比较操作符
 * 
 * @tparam T Type of elements in the list
 * @tparam T list中元素的类型
 * @tparam Alloc Allocator type for memory management
 * @tparam Alloc 用于内存管理的分配器类型
 * @param lhs First list to compare
 * @param lhs 要比较的第一个list
 * @param rhs Second list to compare
 * @param rhs 要比较的第二个list
 * @return true if lhs is lexicographically less than or equal to rhs, false otherwise
 * @return 如果lhs在字典序上小于等于rhs则返回true，否则返回false
 * 
 * @details
 * Checks if `lhs` is lexicographically less than or equal to `rhs`.
 * This operator is equivalent to `!(rhs < lhs)`.
 * 
 * @details
 * 检查`lhs`在字典序上是否小于等于`rhs`。
 * 此操作符等价于`!(rhs < lhs)`。
 * 
 * @note Time complexity: O(min(n, m)) where n and m are the sizes of the lists
 * @note 时间复杂度：O(min(n, m))，其中n和m是list的大小
 * 
 * @note This operator delegates to the less-than operator and negates the result
 * with swapped arguments. It has the same time complexity as the less-than operator.
 * 
 * @note 此操作符委托给小于操作符，并使用交换的参数否定结果。
 * 它具有与小于操作符相同的时间复杂度。
 * 
 * @note The operator uses the same lexicographical comparison algorithm as `operator<`,
 * but with the arguments swapped and the result negated.
 * 
 * @note 此操作符使用与`operator<`相同的字典序比较算法，
 * 但交换了参数并否定了结果。
 * 
 * @see operator<()
 * @see operator>()
 * @see operator>=()
 * @see 参考 operator<()
 * @see 参考 operator>()
 * @see 参考 operator>=()
 */
template<typename T, typename Alloc>
bool operator<=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief Greater-than comparison operator for lists
 * @brief list的大于比较操作符
 * 
 * @tparam T Type of elements in the list
 * @tparam T list中元素的类型
 * @tparam Alloc Allocator type for memory management
 * @tparam Alloc 用于内存管理的分配器类型
 * @param lhs First list to compare
 * @param lhs 要比较的第一个list
 * @param rhs Second list to compare
 * @param rhs 要比较的第二个list
 * @return true if lhs is lexicographically greater than rhs, false otherwise
 * @return 如果lhs在字典序上大于rhs则返回true，否则返回false
 * 
 * @details
 * Checks if `lhs` is lexicographically greater than `rhs`.
 * This operator is equivalent to `rhs < lhs`.
 * 
 * @details
 * 检查`lhs`在字典序上是否大于`rhs`。
 * 此操作符等价于`rhs < lhs`。
 * 
 * @note Time complexity: O(min(n, m)) where n and m are the sizes of the lists
 * @note 时间复杂度：O(min(n, m))，其中n和m是list的大小
 * 
 * @note This operator delegates to the less-than operator with swapped arguments.
 * It has the same time complexity as the less-than operator.
 * 
 * @note 此操作符委托给小于操作符，使用交换的参数。
 * 它具有与小于操作符相同的时间复杂度。
 * 
 * @note The operator uses the same lexicographical comparison algorithm as `operator<`,
 * but with the arguments swapped.
 * 
 * @note 此操作符使用与`operator<`相同的字典序比较算法，
 * 但交换了参数。
 * 
 * @see operator<()
 * @see operator<=()
 * @see operator>=()
 * @see 参考 operator<()
 * @see 参考 operator<=()
 * @see 参考 operator>=()
 */
template<typename T, typename Alloc>
bool operator>(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
    return rhs < lhs;
}

/**
 * @brief Greater-than-or-equal-to comparison operator for lists
 * @brief list的大于等于比较操作符
 * 
 * @tparam T Type of elements in the list
 * @tparam T list中元素的类型
 * @tparam Alloc Allocator type for memory management
 * @tparam Alloc 用于内存管理的分配器类型
 * @param lhs First list to compare
 * @param lhs 要比较的第一个list
 * @param rhs Second list to compare
 * @param rhs 要比较的第二个list
 * @return true if lhs is lexicographically greater than or equal to rhs, false otherwise
 * @return 如果lhs在字典序上大于等于rhs则返回true，否则返回false
 * 
 * @details
 * Checks if `lhs` is lexicographically greater than or equal to `rhs`.
 * This operator is equivalent to `!(lhs < rhs)`.
 * 
 * @details
 * 检查`lhs`在字典序上是否大于等于`rhs`。
 * 此操作符等价于`!(lhs < rhs)`。
 * 
 * @note Time complexity: O(min(n, m)) where n and m are the sizes of the lists
 * @note 时间复杂度：O(min(n, m))，其中n和m是list的大小
 * 
 * @note This operator delegates to the less-than operator and negates the result.
 * It has the same time complexity as the less-than operator.
 * 
 * @note 此操作符委托给小于操作符并否定结果。
 * 它具有与小于操作符相同的时间复杂度。
 * 
 * @note The operator uses the same lexicographical comparison algorithm as `operator<`,
 * but negates the result.
 * 
 * @note 此操作符使用与`operator<`相同的字典序比较算法，
 * 但否定了结果。
 * 
 * @see operator<()
 * @see operator<=()
 * @see operator>()
 * @see 参考 operator<()
 * @see 参考 operator<=()
 * @see 参考 operator>()
 */
template<typename T, typename Alloc>
bool operator>=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
    return !(lhs < rhs);
}

} // namespace mystl
