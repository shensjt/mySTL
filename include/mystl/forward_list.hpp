#pragma once

/**
 * @file forward_list.hpp
 * @brief Singly-linked list container implementation for mySTL
 * @brief 单向链表容器实现（mySTL）
 * 
 * Provides forward_list (singly-linked list) container with forward-only
 * traversal, minimal memory overhead, and special xxx_after APIs.
 * This implementation follows the C++ standard forward_list specification,
 * providing efficient insertion and deletion operations with O(1) complexity
 * at the front and after specific positions.
 * 
 * 提供具有仅向前遍历、最小内存开销和特殊xxx_after API的单向链表容器。
 * 此实现遵循C++标准forward_list规范，
 * 提供高效的插入和删除操作，在头部和特定位置之后具有O(1)复杂度。
 * 
 * @details
 * The forward_list is a singly-linked list that supports only forward traversal.
 * It uses a sentinel node (head node) to simplify edge cases and provides
 * special APIs like insert_after, erase_after, and splice_after that operate
 * on positions after a given iterator.
 * 
 * @details
 * forward_list是一个仅支持向前遍历的单向链表。
 * 它使用哨兵节点（头节点）来简化边界情况，并提供特殊的API，
 * 如insert_after、erase_after和splice_after，这些操作在给定迭代器之后的位置进行。
 * 
 * @note Key features:
 * 1. Forward-only traversal: supports only ++ operator, not -- operator
 * 2. Minimal memory overhead: only one pointer per node (no prev pointer)
 * 3. Special xxx_after APIs: operations work on positions after iterators
 * 4. No size() member function: to maintain O(1) insertion/deletion
 * 
 * @note 主要特性：
 * 1. 仅向前遍历：仅支持++操作符，不支持--操作符
 * 2. 最小内存开销：每个节点只有一个指针（没有prev指针）
 * 3. 特殊xxx_after API：操作在迭代器之后的位置进行
 * 4. 没有size()成员函数：以保持O(1)插入/删除
 * 
 * @author shensjt
 * @date 2026-03-20
 * 
 * @see https://en.cppreference.com/w/cpp/container/forward_list
 * @see 相关参考：https://zh.cppreference.com/w/cpp/container/forward_list
 */

#include <cstddef>
#include <initializer_list>
#include "../memory/allocator.hpp"
#include "../memory/allocator_traits.hpp"
#include "../iterator/iterator_traits.hpp"
#include "../utility/move.hpp"
#include "../utility/swap.hpp"
#include "../utility/type_traits.hpp"
#include "../algorithm/algorithm.hpp"

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

// ==================== forward_list_node Structure ====================
// ==================== forward_list_node 结构体 ====================

/**
 * @brief Node structure for forward_list
 * @brief forward_list的节点结构体
 * 
 * @tparam T Type of element stored in the node
 * @tparam T 节点中存储的元素类型
 * 
 * @details
 * This structure represents a single node in a singly-linked list.
 * Each node contains a value and a pointer to the next node.
 * 
 * @details
 * 此结构体表示单向链表中的单个节点。
 * 每个节点包含一个值和一个指向下一个节点的指针。
 */
template<typename T>
struct forward_list_node {
    T value;                    ///< The stored value / 存储的值
    forward_list_node* next;    ///< Pointer to the next node / 指向下一个节点的指针
    
    /**
     * @brief Default constructor (for head node)
     * @brief 默认构造函数（用于头节点）
     * 
     * @details Constructs a node with null next pointer.
     * Used for creating the sentinel head node.
     * 
     * @details 构造一个next指针为空的节点。
     * 用于创建哨兵头节点。
     */
    forward_list_node() : next(nullptr) {}
    
    /**
     * @brief Constructor with value and next pointer
     * @brief 带值和next指针的构造函数
     * 
     * @param val Value to store in the node
     * @param val 要存储在节点中的值
     * @param n Pointer to the next node (default: nullptr)
     * @param n 指向下一个节点的指针（默认：nullptr）
     */
    forward_list_node(const T& val, forward_list_node* n = nullptr)
        : value(val), next(n) {}
    
    /**
     * @brief Constructor with rvalue reference
     * @brief 带右值引用的构造函数
     * 
     * @param val Value to move into the node
     * @param val 要移动到节点中的值
     * @param n Pointer to the next node (default: nullptr)
     * @param n 指向下一个节点的指针（默认：nullptr）
     */
    forward_list_node(T&& val, forward_list_node* n = nullptr)
        : value(mystl::move(val)), next(n) {}
    
    /**
     * @brief In-place constructor with variadic arguments
     * @brief 带可变参数的原位构造函数
     * 
     * @tparam Args Types of arguments for value construction
     * @tparam Args 值构造的参数类型
     * @param n Pointer to the next node
     * @param n 指向下一个节点的指针
     * @param args Arguments to forward to value constructor
     * @param args 转发给值构造函数的参数
     */
    template<typename... Args>
    forward_list_node(forward_list_node* n, Args&&... args)
        : value(mystl::forward<Args>(args)...), next(n) {}
};

/**
 * @brief Forward iterator for forward_list container
 * @brief forward_list容器的前向迭代器
 * 
 * @tparam T Type of elements in the forward_list
 * @tparam T forward_list中元素的类型
 * 
 * @details
 * This iterator supports forward-only traversal for forward_list containers.
 * It can only move forward (++) through the list, not backward (--).
 * The iterator maintains a pointer to the current node in the singly-linked list.
 * 
 * @details
 * 此迭代器支持forward_list容器的仅向前遍历。
 * 它只能在list中向前（++）移动，不能向后（--）移动。
 * 迭代器维护一个指向单向链表中当前节点的指针。
 * 
 * @note This iterator is a forward iterator and satisfies all
 * requirements of the ForwardIterator concept.
 * 
 * @note 此迭代器是前向迭代器，满足ForwardIterator概念的所有要求。
 * 
 * @note Unlike bidirectional iterators, forward iterators cannot move backward.
 * This reflects the nature of singly-linked lists which only have forward links.
 * 
 * @note 与双向迭代器不同，前向迭代器不能向后移动。
 * 这反映了单向链表只有向前链接的特性。
 */
template<typename T>
class forward_list_iterator {
public:
    // 迭代器类别
    using iterator_category = mystl::forward_iterator_tag;  ///< Iterator category / 迭代器类别
    using value_type = T;                                   ///< Type of elements / 元素类型
    using difference_type = std::ptrdiff_t;                 ///< Difference type / 差值类型
    using pointer = T*;                                     ///< Pointer type / 指针类型
    using reference = T&;                                   ///< Reference type / 引用类型
    using node_pointer = forward_list_node<T>*;             ///< Node pointer type / 节点指针类型

private:
    node_pointer node_;  ///< Pointer to the current node / 指向当前节点的指针

public:
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs an empty iterator pointing to no element.
     * 
     * @details 构造一个不指向任何元素的空迭代器。
     */
    forward_list_iterator() noexcept : node_(nullptr) {}
    
    /**
     * @brief Constructor with node pointer
     * @brief 带节点指针的构造函数
     * 
     * @param node Pointer to the forward_list node
     * @param node 指向forward_list节点的指针
     * 
     * @details Constructs an iterator pointing to the specified node.
     * 
     * @details 构造一个指向指定节点的迭代器。
     */
    explicit forward_list_iterator(node_pointer node) noexcept : node_(node) {}
    
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
    forward_list_iterator(const forward_list_iterator& other) noexcept
        : node_(other.node_) {}
    
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
    forward_list_iterator& operator=(const forward_list_iterator& other) noexcept {
        if (this != &other) {
            node_ = other.node_;
        }
        return *this;
    }
    
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
     * @details Advances the iterator to the next element in the forward_list.
     * Since forward_list is singly-linked, this is the only direction of movement.
     * 
     * @details 将迭代器前进到forward_list中的下一个元素。
     * 由于forward_list是单向链表，这是唯一的移动方向。
     */
    forward_list_iterator& operator++() noexcept {
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
    forward_list_iterator operator++(int) noexcept {
        forward_list_iterator temp = *this;
        ++(*this);
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
     * @details Checks if two iterators point to the same node in the forward_list.
     * 
     * @details 检查两个迭代器是否指向forward_list中的同一个节点。
     */
    bool operator==(const forward_list_iterator& other) const noexcept {
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
     * @details Checks if two iterators point to different nodes in the forward_list.
     * 
     * @details 检查两个迭代器是否指向forward_list中的不同节点。
     */
    bool operator!=(const forward_list_iterator& other) const noexcept {
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
     * This function is for internal use by the forward_list class only.
     * 
     * @details 返回底层节点指针。
     * 此函数仅供forward_list类内部使用。
     */
    node_pointer node() const noexcept {
        return node_;
    }
};

/**
 * @brief Constant forward iterator for forward_list container
 * @brief forward_list容器的常量前向迭代器
 * 
 * @tparam T Type of elements in the forward_list
 * @tparam T forward_list中元素的类型
 * 
 * @details
 * This iterator provides read-only access to forward_list elements.
 * Like forward_list_iterator, it supports only forward traversal.
 * It cannot modify the elements it points to, making it suitable for
 * const-correct code and read-only operations.
 * 
 * @details
 * 此迭代器提供对forward_list元素的只读访问。
 * 与forward_list_iterator类似，它仅支持向前遍历。
 * 它不能修改其指向的元素，使其适用于const正确的代码和只读操作。
 * 
 * @note This iterator is a constant forward iterator and satisfies all
 * requirements of the ForwardIterator concept for read-only access.
 * 
 * @note 此迭代器是常量前向迭代器，满足只读访问的ForwardIterator概念的所有要求。
 * 
 * @note The main difference from forward_list_iterator is that this iterator
 * returns const references and pointers, preventing modification of elements.
 * 
 * @note 与forward_list_iterator的主要区别是此迭代器返回常量引用和指针，防止元素被修改。
 */
template<typename T>
class forward_list_const_iterator {
public:
    // 迭代器类别
    using iterator_category = mystl::forward_iterator_tag;  ///< Iterator category / 迭代器类别
    using value_type = T;                                   ///< Type of elements / 元素类型
    using difference_type = std::ptrdiff_t;                 ///< Difference type / 差值类型
    using pointer = const T*;                               ///< Pointer type (const) / 指针类型（常量）
    using reference = const T&;                             ///< Reference type (const) / 引用类型（常量）
    using node_pointer = const forward_list_node<T>*;       ///< Node pointer type (const) / 节点指针类型（常量）

private:
    node_pointer node_;  ///< Pointer to the current node / 指向当前节点的指针

public:
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs an empty constant iterator pointing to no element.
     * 
     * @details 构造一个不指向任何元素的空常量迭代器。
     */
    forward_list_const_iterator() noexcept : node_(nullptr) {}
    
    /**
     * @brief Constructor with node pointer
     * @brief 带节点指针的构造函数
     * 
     * @param node Pointer to the forward_list node
     * @param node 指向forward_list节点的指针
     * 
     * @details Constructs a constant iterator pointing to the specified node.
     * 
     * @details 构造一个指向指定节点的常量迭代器。
     */
    explicit forward_list_const_iterator(node_pointer node) noexcept : node_(node) {}
    
    /**
     * @brief Constructor from non-const iterator
     * @brief 从非const迭代器构造
     * 
     * @param other Non-const iterator to convert from
     * @param other 要转换的非const迭代器
     * 
     * @details Constructs a constant iterator from a non-const iterator.
     * This allows implicit conversion from non-const to const iterators.
     * 
     * @details 从非const迭代器构造常量迭代器。
     * 这允许从非const迭代器到const迭代器的隐式转换。
     */
    forward_list_const_iterator(const forward_list_iterator<T>& other) noexcept
        : node_(other.node()) {}
    
    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other Another constant iterator to copy from
     * @param other 要拷贝的另一个常量迭代器
     * 
     * @details Constructs a constant iterator by copying another constant iterator.
     * 
     * @details 通过拷贝另一个常量迭代器构造常量迭代器。
     */
    forward_list_const_iterator(const forward_list_const_iterator& other) noexcept
        : node_(other.node_) {}
    
    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值操作符
     * 
     * @param other Another constant iterator to assign from
     * @param other 要赋值的另一个常量迭代器
     * @return Reference to this iterator
     * @return 此迭代器的引用
     * 
     * @details Assigns the contents of another constant iterator to this iterator.
     * 
     * @details 将另一个常量迭代器的内容赋值给此迭代器。
     */
    forward_list_const_iterator& operator=(const forward_list_const_iterator& other) noexcept {
        if (this != &other) {
            node_ = other.node_;
        }
        return *this;
    }
    
    /**
     * @brief Dereference operator
     * @brief 解引用操作符
     * 
     * @return Const reference to the element pointed to by the iterator
     * @return 迭代器指向的元素的常量引用
     * 
     * @details Returns a const reference to the element stored in the current node.
     * Since this is a constant iterator, the reference cannot be used to modify the element.
     * 
     * @details 返回存储在当前节点中的元素的常量引用。
     * 由于这是常量迭代器，引用不能用于修改元素。
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
     * This operator allows accessing members of the element using arrow notation.
     * Since this is a constant iterator, the pointer cannot be used to modify the element.
     * 
     * @details 返回指向存储在当前节点中的元素的常量指针。
     * 此操作符允许使用箭头表示法访问元素的成员。
     * 由于这是常量迭代器，指针不能用于修改元素。
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
     * @details Advances the iterator to the next element in the forward_list.
     * Since forward_list is singly-linked, this is the only direction of movement.
     * 
     * @details 将迭代器前进到forward_list中的下一个元素。
     * 由于forward_list是单向链表，这是唯一的移动方向。
     */
    forward_list_const_iterator& operator++() noexcept {
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
    forward_list_const_iterator operator++(int) noexcept {
        forward_list_const_iterator temp = *this;
        ++(*this);
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
     * @details Checks if two constant iterators point to the same node in the forward_list.
     * 
     * @details 检查两个常量迭代器是否指向forward_list中的同一个节点。
     */
    bool operator==(const forward_list_const_iterator& other) const noexcept {
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
     * @details Checks if two constant iterators point to different nodes in the forward_list.
     * 
     * @details 检查两个常量迭代器是否指向forward_list中的不同节点。
     */
    bool operator!=(const forward_list_const_iterator& other) const noexcept {
        return node_ != other.node_;
    }
    
    /**
     * @brief Get the underlying node pointer
     * @brief 获取底层节点指针
     * 
     * @return Const pointer to the current node
     * @return 指向当前节点的常量指针
     * 
     * @details Returns the underlying node pointer.
     * This function is for internal use by the forward_list class only.
     * 
     * @details 返回底层节点指针。
     * 此函数仅供forward_list类内部使用。
     */
    node_pointer node() const noexcept {
        return node_;
    }
};

// ==================== forward_list Class ====================
// ==================== forward_list 类 ====================

/**
 * @brief Singly-linked list container
 * @brief 单向链表容器
 * 
 * @tparam T Type of elements stored in the forward_list
 * @tparam T 存储在forward_list中的元素类型
 * @tparam Allocator Allocator type for memory management
 * @tparam Allocator 用于内存管理的分配器类型
 * 
 * @details
 * The forward_list is a container that supports fast insertion and removal
 * of elements from anywhere in the container. It is implemented as a
 * singly-linked list and only supports forward traversal.
 * 
 * @details
 * forward_list是一个支持从容器中任何位置快速插入和删除元素的容器。
 * 它被实现为单向链表，仅支持向前遍历。
 * 
 * @note Key characteristics:
 * 1. Fast random insertion/deletion: O(1) complexity
 * 2. Forward-only iteration: only supports ++ operator
 * 3. No size() member: to maintain O(1) insertion/deletion
 * 4. Special xxx_after APIs: operations work on positions after iterators
 * 5. Minimal memory overhead: only one pointer per node
 * 
 * @note 主要特性：
 * 1. 快速随机插入/删除：O(1)复杂度
 * 2. 仅向前迭代：仅支持++操作符
 * 3. 没有size()成员：以保持O(1)插入/删除
 * 4. 特殊xxx_after API：操作在迭代器之后的位置进行
 * 5. 最小内存开销：每个节点只有一个指针
 * 
 * @see https://en.cppreference.com/w/cpp/container/forward_list
 * @see 相关参考：https://zh.cppreference.com/w/cpp/container/forward_list
 */
template<typename T, typename Allocator = allocator<T>>
class forward_list {
public:
    // 类型定义
    using value_type = T;                                   ///< Type of elements stored in the container / 容器中存储的元素类型
    using allocator_type = Allocator;                       ///< Type of allocator used for memory management / 用于内存管理的分配器类型
    using size_type = std::size_t;                          ///< Type for size-related operations / 用于大小相关操作的类型
    using difference_type = std::ptrdiff_t;                 ///< Type for difference between iterators / 用于迭代器之间差值的类型
    using reference = value_type&;                          ///< Reference type to elements / 元素的引用类型
    using const_reference = const value_type&;              ///< Const reference type to elements / 元素的常量引用类型
    using pointer = typename allocator_traits<Allocator>::pointer;          ///< Pointer type to elements / 元素的指针类型
    using const_pointer = typename allocator_traits<Allocator>::const_pointer; ///< Const pointer type to elements / 元素的常量指针类型
    
    // 迭代器类型
    using iterator = forward_list_iterator<T>;              ///< Iterator type for forward_list / forward_list的迭代器类型
    using const_iterator = forward_list_const_iterator<T>;  ///< Const iterator type for forward_list / forward_list的常量迭代器类型
    
private:
    using node_type = forward_list_node<T>;             ///< Type of node used in the singly-linked list / 单向链表中使用的节点类型
    using node_pointer = node_type*;                    ///< Pointer to node / 节点指针
    using const_node_pointer = const node_type*;        ///< Const pointer to node / 常量节点指针
    using alloc_traits = allocator_traits<Allocator>;   ///< Allocator traits for memory operations / 用于内存操作的分配器特性
    
    /**
     * @brief Wrapper class for node allocation
     * @brief 节点分配包装类
     * 
     * @details
     * This wrapper class adapts the user-provided allocator (for type T)
     * to allocate and deallocate nodes (forward_list_node<T>). It provides
     * a uniform interface for node memory management while maintaining
     * proper exception safety and memory alignment.
     * 
     * @details
     * 此包装类将用户提供的分配器（用于类型T）适配为分配和释放节点（forward_list_node<T>）。
     * 它为节点内存管理提供统一的接口，同时保持适当的异常安全性和内存对齐。
     * 
     * @note This wrapper is necessary because the forward_list needs to allocate
     * nodes (forward_list_node<T>) rather than raw elements (T). The wrapper
     * ensures proper construction and destruction of node objects.
     * 
     * @note 此包装是必要的，因为forward_list需要分配节点（forward_list_node<T>）
     * 而不是原始元素（T）。包装确保节点对象的正确构造和析构。
     */
    class node_allocator_wrapper {
    private:
        Allocator alloc_;  ///< Underlying allocator for type T / 用于类型T的底层分配器
        
    public:
        // 必要的类型别名，用于allocator_traits
        using value_type = node_type;                                   ///< Type of allocated objects / 分配对象的类型
        using pointer = node_type*;                                     ///< Pointer type / 指针类型
        using const_pointer = const node_type*;                         ///< Const pointer type / 常量指针类型
        using void_pointer = void*;                                     ///< Void pointer type / void指针类型
        using const_void_pointer = const void*;                         ///< Const void pointer type / 常量void指针类型
        using size_type = std::size_t;                                  ///< Size type / 大小类型
        using difference_type = std::ptrdiff_t;                         ///< Difference type / 差值类型
        using propagate_on_container_copy_assignment = mystl::false_type; ///< Whether to propagate on copy assignment / 是否在拷贝赋值时传播
        using propagate_on_container_move_assignment = mystl::true_type;  ///< Whether to propagate on move assignment / 是否在移动赋值时传播
        using propagate_on_container_swap = mystl::false_type;            ///< Whether to propagate on swap / 是否在交换时传播
        using is_always_equal = mystl::false_type;                        ///< Whether allocators are always equal / 分配器是否总是相等
        
        /**
         * @brief Default constructor
         * @brief 默认构造函数
         * 
         * @param alloc Allocator to wrap (default: default-constructed Allocator)
         * @param alloc 要包装的分配器（默认：默认构造的Allocator）
         */
        node_allocator_wrapper(const Allocator& alloc = Allocator()) noexcept : alloc_(alloc) {}
        
        /**
         * @brief Allocates memory for a single node
         * @brief 为单个节点分配内存
         * 
         * @return Pointer to the allocated memory
         * @return 指向分配内存的指针
         * 
         * @note Uses global operator new for simplicity. In a production
         * implementation, this would use the underlying allocator.
         * 
         * @note 为简单起见使用全局operator new。在生产实现中，这将使用底层分配器。
         */
        node_pointer allocate() {
            return static_cast<node_pointer>(::operator new(sizeof(node_type)));
        }
        
        /**
         * @brief Deallocates memory for a single node
         * @brief 释放单个节点的内存
         * 
         * @param p Pointer to the memory to deallocate
         * @param p 指向要释放内存的指针
         */
        void deallocate(node_pointer p) noexcept {
            ::operator delete(p);
        }
        
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
         */
        template<typename... Args>
        void construct(node_pointer p, Args&&... args) {
            new (p) node_type(mystl::forward<Args>(args)...);
        }
        
        /**
         * @brief Destroys a node without deallocating memory
         * @brief 销毁节点但不释放内存
         * 
         * @param p Pointer to the node to destroy
         * @param p 指向要销毁节点的指针
         */
        void destroy(node_pointer p) noexcept {
            p->~node_type();
        }
        
        /**
         * @brief Returns the maximum number of nodes that can be allocated
         * @brief 返回可以分配的最大节点数
         * 
         * @return Maximum number of nodes
         * @return 最大节点数
         */
        std::size_t max_size() const noexcept {
            return std::size_t(-1) / sizeof(node_type);
        }
        
        /**
         * @brief Gets the underlying allocator
         * @brief 获取底层分配器
         * 
         * @return The wrapped allocator
         * @return 包装的分配器
         */
        Allocator get_allocator() const {
            return alloc_;
        }
        
        /**
         * @brief Creates a new wrapper for copy construction
         * @brief 为拷贝构造创建新的包装器
         * 
         * @return A new node_allocator_wrapper with the same allocator
         * @return 具有相同分配器的新node_allocator_wrapper
         * 
         * @note This is used by allocator_traits when a container is
         * copy-constructed to ensure proper allocator propagation.
         * 
         * @note 当容器被拷贝构造时，allocator_traits使用此函数以确保正确的分配器传播。
         */
        node_allocator_wrapper select_on_container_copy_construction() const {
            return node_allocator_wrapper(alloc_);
        }
    };
    
    // 核心数据成员
    node_type head_;           ///< Sentinel head node (does not store data) / 哨兵头节点（不存储数据）
    node_allocator_wrapper alloc_wrapper_;  ///< Wrapper for node allocation / 节点分配包装器
    
public:
    // ==================== 构造函数和析构函数 ====================
    
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs an empty forward_list with no elements.
     * The forward_list will have no allocated memory until the first element is inserted.
     * 
     * @details 构造一个没有元素的空forward_list。
     * 在插入第一个元素之前，forward_list不会有任何分配的内存。
     */
    forward_list() noexcept(noexcept(Allocator()))
        : alloc_wrapper_(Allocator()) {
        head_.next = nullptr;
    }
    
    /**
     * @brief Constructor with allocator
     * @brief 带分配器的构造函数
     * 
     * @param alloc Allocator to use for all memory allocations
     * @param alloc 用于所有内存分配的分配器
     * 
     * @details Constructs an empty forward_list with the specified allocator.
     * 
     * @details 使用指定的分配器构造一个空forward_list。
     */
    explicit forward_list(const Allocator& alloc) noexcept
        : alloc_wrapper_(alloc) {
        head_.next = nullptr;
    }
    
    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other Another forward_list to copy from
     * @param other 要拷贝的另一个forward_list
     * 
     * @details
     * Constructs a forward_list with a copy of each of the elements in other,
     * in the same order. The allocator is obtained by calling
     * select_on_container_copy_construction on other's allocator wrapper.
     * 
     * @details
     * 构造一个forward_list，其中包含other中每个元素的副本，顺序相同。
     * 分配器通过调用select_on_container_copy_construction
     * 从other的分配器包装器获取。
     * 
     * @note Time complexity: O(n) where n is the number of elements in other
     * @note 时间复杂度：O(n)，其中n是other中的元素数量
     */
    forward_list(const forward_list& other)
        : alloc_wrapper_(other.alloc_wrapper_.select_on_container_copy_construction()) {
        head_.next = nullptr;
        try {
            iterator it = before_begin();
            const_iterator other_it = other.begin();
            while (other_it != other.end()) {
                it = insert_after(it, *other_it);
                ++other_it;
            }
        } catch (...) {
            clear();
            throw;
        }
    }
    
    /**
     * @brief Copy constructor with allocator
     * @brief 带分配器的拷贝构造函数
     * 
     * @param other Another forward_list to copy from
     * @param other 要拷贝的另一个forward_list
     * @param alloc Allocator to use for all memory allocations
     * @param alloc 用于所有内存分配的分配器
     * 
     * @details
     * Constructs a forward_list with a copy of each of the elements in other,
     * in the same order, using the specified allocator.
     * 
     * @details
     * 使用指定的分配器构造一个forward_list，其中包含other中每个元素的副本，顺序相同。
     * 
     * @note Time complexity: O(n) where n is the number of elements in other
     * @note 时间复杂度：O(n)，其中n是other中的元素数量
     */
    forward_list(const forward_list& other, const Allocator& alloc)
        : alloc_wrapper_(alloc) {
        head_.next = nullptr;
        try {
            iterator it = before_begin();
            const_iterator other_it = other.begin();
            while (other_it != other.end()) {
                it = insert_after(it, *other_it);
                ++other_it;
            }
        } catch (...) {
            clear();
            throw;
        }
    }
    
    /**
     * @brief Move constructor
     * @brief 移动构造函数
     * 
     * @param other Another forward_list to move from
     * @param other 要移动的另一个forward_list
     * 
     * @details
     * Constructs a forward_list by moving elements from another forward_list.
     * After the move, other is left in a valid but unspecified state.
     * 
     * @details
     * 通过从另一个forward_list移动元素来构造forward_list。
     * 移动后，other处于有效但未指定的状态。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    forward_list(forward_list&& other) noexcept
        : alloc_wrapper_(mystl::move(other.alloc_wrapper_)) {
        head_.next = other.head_.next;
        other.head_.next = nullptr;
    }
    
    /**
     * @brief Move constructor with allocator
     * @brief 带分配器的移动构造函数
     * 
     * @param other Another forward_list to move from
     * @param other 要移动的另一个forward_list
     * @param alloc Allocator to use for the new forward_list
     * @param alloc 用于新forward_list的分配器
     * 
     * @details
     * Constructs a forward_list by moving elements from another forward_list.
     * If the provided allocator is equal to other's allocator, the operation
     * is a true move (O(1) complexity). Otherwise, elements are copied one by one.
     * 
     * @details
     * 通过从另一个forward_list移动元素来构造forward_list。
     * 如果提供的分配器与other的分配器相等，则操作是真正的移动（O(1)复杂度）。
     * 否则，元素将被逐个拷贝。
     * 
     * @note Exception safety: Strong guarantee if allocators are equal,
     * basic guarantee otherwise (due to potential element-by-element copy).
     * 
     * @note 异常安全性：如果分配器相等则为强保证，
     * 否则为基本保证（由于可能的逐个元素拷贝）。
     */
    forward_list(forward_list&& other, const Allocator& alloc)
        : alloc_wrapper_(alloc) {
        if (alloc_wrapper_.get_allocator() == other.alloc_wrapper_.get_allocator()) {
            // 如果分配器相等，可以移动资源
            head_.next = other.head_.next;
            other.head_.next = nullptr;
        } else {
            // 分配器不相等，需要拷贝
            head_.next = nullptr;
            try {
                iterator it = before_begin();
                const_iterator other_it = other.begin();
                while (other_it != other.end()) {
                    it = insert_after(it, *other_it);
                    ++other_it;
                }
            } catch (...) {
                clear();
                throw;
            }
        }
    }
    
    /**
     * @brief Constructor with specified size
     * @brief 指定大小的构造函数
     * 
     * @param count Number of elements to initialize with
     * @param count 要初始化的元素数量
     * @param value Value to initialize each element with (default: T())
     * @param value 用于初始化每个元素的值（默认：T()）
     * @param alloc Allocator to use for all memory allocations (default: Allocator())
     * @param alloc 用于所有内存分配的分配器（默认：Allocator()）
     * 
     * @details
     * Constructs a forward_list with count copies of value.
     * 
     * @details
     * 构造一个forward_list，其中包含count个value的副本。
     * 
     * @note Time complexity: O(n) where n is count
     * @note 时间复杂度：O(n)，其中n是count
     */
    explicit forward_list(size_type count, const T& value = T(), const Allocator& alloc = Allocator())
        : alloc_wrapper_(alloc) {
        head_.next = nullptr;
        try {
            iterator it = before_begin();
            for (size_type i = 0; i < count; ++i) {
                it = insert_after(it, value);
            }
        } catch (...) {
            clear();
            throw;
        }
    }
    
    /**
     * @brief Range constructor
     * @brief 范围构造函数
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
     * Constructs a forward_list with elements from the range [first, last).
     * 
     * @details
     * 构造一个forward_list，其中包含范围[first, last)中的元素。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the range
     * @note 时间复杂度：O(n)，其中n是范围中的元素数量
     */
    template<typename InputIt, typename = typename mystl::enable_if<
        !mystl::is_integral<InputIt>::value>::type>
    forward_list(InputIt first, InputIt last, const Allocator& alloc = Allocator())
        : alloc_wrapper_(alloc) {
        head_.next = nullptr;
        try {
            iterator it = before_begin();
            for (; first != last; ++first) {
                it = insert_after(it, *first);
            }
        } catch (...) {
            clear();
            throw;
        }
    }
    
    /**
     * @brief Initializer list constructor
     * @brief 初始化列表构造函数
     * 
     * @param init Initializer list containing elements to initialize with
     * @param init 包含要初始化元素的初始化列表
     * @param alloc Allocator to use for all memory allocations (default: Allocator())
     * @param alloc 用于所有内存分配的分配器（默认：Allocator()）
     * 
     * @details
     * Constructs a forward_list with elements from the initializer list.
     * 
     * @details
     * 构造一个forward_list，其中包含初始化列表中的元素。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the list
     * @note 时间复杂度：O(n)，其中n是列表中的元素数量
     */
    forward_list(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : forward_list(init.begin(), init.end(), alloc) {}
    
    /**
     * @brief Destructor
     * @brief 析构函数
     * 
     * @details
     * Destroys the forward_list, deallocating all memory.
     * 
     * @details
     * 销毁forward_list，释放所有内存。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note 时间复杂度：O(n)，其中n是元素数量
     */
    ~forward_list() {
        clear();
    }
    
    // ==================== 赋值操作符 ====================
    
    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值操作符
     * 
     * @param other Another forward_list to assign from
     * @param other 要赋值的另一个forward_list
     * @return Reference to this forward_list
     * @return 此forward_list的引用
     * 
     * @details
     * Replaces the contents of this forward_list with a copy of the contents of other.
     * Uses the copy-and-swap idiom for strong exception safety.
     * 
     * @details
     * 用other内容的副本替换此forward_list的内容。
     * 使用拷贝并交换技术以获得强异常安全性。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is other's size
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是other的大小
     */
    forward_list& operator=(const forward_list& other) {
        if (this != &other) {
            forward_list temp(other);
            swap(temp);
        }
        return *this;
    }
    
    /**
     * @brief Move assignment operator
     * @brief 移动赋值操作符
     * 
     * @param other Another forward_list to move from
     * @param other 要移动的另一个forward_list
     * @return Reference to this forward_list
     * @return 此forward_list的引用
     * 
     * @details
     * Replaces the contents of this forward_list with those of other using move semantics.
     * After the move, other is left in a valid but unspecified state.
     * 
     * @details
     * 使用移动语义用other的内容替换此forward_list的内容。
     * 移动后，other处于有效但未指定的状态。
     * 
     * @note Time complexity: O(n) where n is the current size
     * @note 时间复杂度：O(n)，其中n是当前大小
     */
    forward_list& operator=(forward_list&& other) noexcept {
        if (this != &other) {
            clear();
            head_.next = other.head_.next;
            alloc_wrapper_ = mystl::move(other.alloc_wrapper_);
            other.head_.next = nullptr;
        }
        return *this;
    }
    
    /**
     * @brief Initializer list assignment operator
     * @brief 初始化列表赋值操作符
     * 
     * @param ilist Initializer list containing elements to assign
     * @param ilist 包含要赋值元素的初始化列表
     * @return Reference to this forward_list
     * @return 此forward_list的引用
     * 
     * @details
     * Replaces the contents of this forward_list with elements from the initializer list.
     * 
     * @details
     * 用初始化列表中的元素替换此forward_list的内容。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is the list size
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是列表大小
     */
    forward_list& operator=(std::initializer_list<T> ilist) {
        forward_list temp(ilist);
        swap(temp);
        return *this;
    }
    
    // ==================== 容量相关函数 ====================
    
    /**
     * @brief Check if the forward_list is empty
     * @brief 检查forward_list是否为空
     * 
     * @return true if the forward_list is empty, false otherwise
     * @return 如果forward_list为空返回true，否则返回false
     * 
     * @details
     * Returns whether the forward_list is empty (i.e., whether it contains no elements).
     * This function does not modify the container in any way.
     * 
     * @details
     * 返回forward_list是否为空（即是否不包含任何元素）。
     * 此函数不会以任何方式修改容器。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    bool empty() const noexcept {
        return head_.next == nullptr;
    }
    
    /**
     * @brief Get the maximum possible number of elements
     * @brief 获取可能的最大元素数量
     * 
     * @return Maximum number of elements the forward_list can hold
     * @return forward_list可以容纳的最大元素数量
     * 
     * @details
     * Returns the maximum number of elements that the forward_list can hold.
     * This value typically reflects the theoretical limit on the size
     * of the container, due to system or library implementation limitations.
     * 
     * @details
     * 返回forward_list可以容纳的最大元素数量。
     * 此值通常反映了由于系统或库实现限制，
     * 容器大小的理论限制。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    size_type max_size() const noexcept {
        return alloc_traits::max_size(alloc_wrapper_.get_allocator());
    }
    
    // 注意：forward_list没有size()成员函数
    
    // ==================== 元素访问 ====================
    
    /**
     * @brief Access the first element
     * @brief 访问第一个元素
     * 
     * @return Reference to the first element
     * @return 对第一个元素的引用
     * 
     * @details
     * Returns a reference to the first element in the forward_list.
     * Calling front() on an empty forward_list causes undefined behavior.
     * 
     * @details
     * 返回forward_list中第一个元素的引用。
     * 在空forward_list上调用front()会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    reference front() noexcept {
        return head_.next->value;
    }
    
    /**
     * @brief Access the first element (const version)
     * @brief 访问第一个元素（const版本）
     * 
     * @return Const reference to the first element
     * @return 对第一个元素的const引用
     * 
     * @details
     * Returns a const reference to the first element in the forward_list.
     * Calling front() on an empty forward_list causes undefined behavior.
     * 
     * @details
     * 返回forward_list中第一个元素的const引用。
     * 在空forward_list上调用front()会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_reference front() const noexcept {
        return head_.next->value;
    }
    
    // 注意：forward_list没有back()成员函数
    
    // ==================== 迭代器 ====================
    
    /**
     * @brief Get iterator to the position before the beginning
     * @brief 获取指向开头之前位置的迭代器
     * 
     * @return Iterator pointing to the position before the first element
     * @return 指向第一个元素之前位置的迭代器
     * 
     * @details
     * Returns an iterator pointing to the position before the first element.
     * This iterator can be used with insert_after() to insert elements at the
     * beginning of the forward_list.
     * 
     * @details
     * 返回指向第一个元素之前位置的迭代器。
     * 此迭代器可与insert_after()一起使用，以在forward_list的开头插入元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    iterator before_begin() noexcept {
        return iterator(&head_);
    }
    
    /**
     * @brief Get const iterator to the position before the beginning
     * @brief 获取指向开头之前位置的const迭代器
     * 
     * @return Const iterator pointing to the position before the first element
     * @return 指向第一个元素之前位置的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the position before the first element.
     * This iterator can be used with insert_after() to insert elements at the
     * beginning of the forward_list.
     * 
     * @details
     * 返回指向第一个元素之前位置的const迭代器。
     * 此迭代器可与insert_after()一起使用，以在forward_list的开头插入元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator before_begin() const noexcept {
        return const_iterator(&head_);
    }
    
    /**
     * @brief Get const iterator to the position before the beginning
     * @brief 获取指向开头之前位置的const迭代器
     * 
     * @return Const iterator pointing to the position before the first element
     * @return 指向第一个元素之前位置的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the position before the first element.
     * This function is provided for compatibility with C++11's cbefore_begin().
     * 
     * @details
     * 返回指向第一个元素之前位置的const迭代器。
     * 此函数为兼容C++11的cbefore_begin()而提供。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator cbefore_begin() const noexcept {
        return const_iterator(&head_);
    }
    
    /**
     * @brief Get iterator to the beginning
     * @brief 获取指向开头的迭代器
     * 
     * @return Iterator pointing to the first element in the forward_list
     * @return 指向forward_list中第一个元素的迭代器
     * 
     * @details
     * Returns an iterator pointing to the first element in the forward_list.
     * If the forward_list is empty, the returned iterator will be equal to end().
     * 
     * @details
     * 返回指向forward_list中第一个元素的迭代器。
     * 如果forward_list为空，返回的迭代器将等于end()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    iterator begin() noexcept {
        return iterator(head_.next);
    }
    
    /**
     * @brief Get const iterator to the beginning
     * @brief 获取指向开头的const迭代器
     * 
     * @return Const iterator pointing to the first element in the forward_list
     * @return 指向forward_list中第一个元素的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the first element in the forward_list.
     * If the forward_list is empty, the returned iterator will be equal to end().
     * 
     * @details
     * 返回指向forward_list中第一个元素的const迭代器。
     * 如果forward_list为空，返回的迭代器将等于end()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator begin() const noexcept {
        return const_iterator(head_.next);
    }
    
    /**
     * @brief Get const iterator to the beginning
     * @brief 获取指向开头的const迭代器
     * 
     * @return Const iterator pointing to the first element in the forward_list
     * @return 指向forward_list中第一个元素的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the first element in the forward_list.
     * This function is provided for compatibility with C++11's cbegin().
     * 
     * @details
     * 返回指向forward_list中第一个元素的const迭代器。
     * 此函数为兼容C++11的cbegin()而提供。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator cbegin() const noexcept {
        return const_iterator(head_.next);
    }
    
    /**
     * @brief Get iterator to the end
     * @brief 获取指向结尾的迭代器
     * 
     * @return Iterator pointing to the past-the-end element in the forward_list
     * @return 指向forward_list中最后一个元素之后位置的迭代器
     * 
     * @details
     * Returns an iterator pointing to the past-the-end element in the forward_list.
     * The past-the-end element is the theoretical element that would follow
     * the last element in the forward_list. It does not point to any element, and
     * thus shall not be dereferenced.
     * 
     * @details
     * 返回指向forward_list中最后一个元素之后位置的迭代器。
     * 最后一个元素之后位置是forward_list中最后一个元素之后的理论元素。
     * 它不指向任何元素，因此不应被解引用。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    iterator end() noexcept {
        return iterator(nullptr);
    }
    
    /**
     * @brief Get const iterator to the end
     * @brief 获取指向结尾的const迭代器
     * 
     * @return Const iterator pointing to the past-the-end element in the forward_list
     * @return 指向forward_list中最后一个元素之后位置的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the past-the-end element in the forward_list.
     * The past-the-end element is the theoretical element that would follow
     * the last element in the forward_list. It does not point to any element, and
     * thus shall not be dereferenced.
     * 
     * @details
     * 返回指向forward_list中最后一个元素之后位置的const迭代器。
     * 最后一个元素之后位置是forward_list中最后一个元素之后的理论元素。
     * 它不指向任何元素，因此不应被解引用。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator end() const noexcept {
        return const_iterator(nullptr);
    }
    
    /**
     * @brief Get const iterator to the end
     * @brief 获取指向结尾的const迭代器
     * 
     * @return Const iterator pointing to the past-the-end element in the forward_list
     * @return 指向forward_list中最后一个元素之后位置的const迭代器
     * 
     * @details
     * Returns a const iterator pointing to the past-the-end element in the forward_list.
     * This function is provided for compatibility with C++11's cend().
     * 
     * @details
     * 返回指向forward_list中最后一个元素之后位置的const迭代器。
     * 此函数为兼容C++11的cend()而提供。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    const_iterator cend() const noexcept {
        return const_iterator(nullptr);
    }
    
    // ==================== 修改操作 ====================
    
    /**
     * @brief Clear all elements from the forward_list
     * @brief 清除forward_list中的所有元素
     * 
     * @details
     * Removes all elements from the forward_list, leaving it with a size of 0.
     * All memory is deallocated, and the forward_list returns to its initial state.
     * 
     * @details
     * 从forward_list中移除所有元素，使其大小变为0。
     * 所有内存都被释放，forward_list返回到其初始状态。
     * 
     * @note
     * Invalidates all iterators, pointers and references to elements of the forward_list.
     * 
     * @note
     * 使所有指向forward_list元素的迭代器、指针和引用失效。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note 时间复杂度：O(n)，其中n是元素数量
     */
    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }
    
    /**
     * @brief Add element at the beginning (copy version)
     * @brief 在开头添加元素（拷贝版本）
     * 
     * @param value Element value to add
     * @param value 要添加的元素值
     * 
     * @details
     * Prepends the given element value to the beginning of the forward_list.
     * The new element is initialized as a copy of value.
     * 
     * @details
     * 将给定元素值前置到forward_list的开头。
     * 新元素初始化为value的副本。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    void push_front(const T& value) {
        head_.next = create_node(value, head_.next);
    }
    
    /**
     * @brief Add element at the beginning (move version)
     * @brief 在开头添加元素（移动版本）
     * 
     * @param value Element value to move into the forward_list
     * @param value 要移动到forward_list中的元素值
     * 
     * @details
     * Prepends the given element value to the beginning of the forward_list.
     * The new element is initialized by moving value.
     * 
     * @details
     * 将给定元素值前置到forward_list的开头。
     * 新元素通过移动value来初始化。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    void push_front(T&& value) {
        head_.next = create_node(mystl::move(value), head_.next);
    }
    
    /**
     * @brief Remove element from the beginning
     * @brief 从开头移除元素
     * 
     * @details
     * Removes the first element of the forward_list.
     * Calling pop_front() on an empty forward_list has no effect.
     * 
     * @details
     * 移除forward_list的第一个元素。
     * 在空forward_list上调用pop_front()没有效果。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    void pop_front() {
        if (!empty()) {
            node_pointer old_head = head_.next;
            head_.next = old_head->next;
            destroy_node(old_head);
        }
    }
    
    /**
     * @brief Insert element after specified position (copy version)
     * @brief 在指定位置之后插入元素（拷贝版本）
     * 
     * @param pos Iterator to the position after which the element will be inserted
     * @param pos 元素将插入到此位置之后的迭代器
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return Iterator pointing to the inserted element
     * @return 指向插入元素的迭代器
     * 
     * @details
     * Inserts a copy of the given element after the specified position.
     * The insertion position is determined relative to the given iterator.
     * 
     * @details
     * 在指定位置之后插入给定元素的副本。
     * 插入位置相对于给定迭代器确定。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    iterator insert_after(const_iterator pos, const T& value) {
        // 获取位置节点的指针（需要移除const限定符以修改next指针）
        node_pointer pos_node = const_cast<node_pointer>(pos.node());
        
        // 创建新节点：新节点的next指向pos_node原来的下一个节点
        node_pointer new_node = create_node(value, pos_node->next);
        
        // 将pos_node的next指针指向新节点，完成插入
        pos_node->next = new_node;
        
        // 返回指向新插入节点的迭代器
        return iterator(new_node);
    }
    
    /**
     * @brief Insert element after specified position (move version)
     * @brief 在指定位置之后插入元素（移动版本）
     * 
     * @param pos Iterator to the position after which the element will be inserted
     * @param pos 元素将插入到此位置之后的迭代器
     * @param value Element value to move into the forward_list
     * @param value 要移动到forward_list中的元素值
     * @return Iterator pointing to the inserted element
     * @return 指向插入元素的迭代器
     * 
     * @details
     * Inserts the given element after the specified position using move semantics.
     * The insertion position is determined relative to the given iterator.
     * 
     * @details
     * 使用移动语义在指定位置之后插入给定元素。
     * 插入位置相对于给定迭代器确定。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    iterator insert_after(const_iterator pos, T&& value) {
        // 获取位置节点的指针（需要移除const限定符以修改next指针）
        node_pointer pos_node = const_cast<node_pointer>(pos.node());
        
        // 创建新节点：使用移动语义避免不必要的拷贝，新节点的next指向pos_node原来的下一个节点
        node_pointer new_node = create_node(mystl::move(value), pos_node->next);
        
        // 将pos_node的next指针指向新节点，完成插入
        pos_node->next = new_node;
        
        // 返回指向新插入节点的迭代器
        return iterator(new_node);
    }
    
    /**
     * @brief Insert multiple copies of value after specified position
     * @brief 在指定位置之后插入多个相同元素
     * 
     * @param pos Iterator to the position after which elements will be inserted
     * @param pos 元素将插入到此位置之后的迭代器
     * @param count Number of elements to insert
     * @param count 要插入的元素数量
     * @param value Value to initialize each element with
     * @param value 用于初始化每个元素的值
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details
     * Inserts count copies of value after the specified position.
     * The elements are inserted sequentially after the given position.
     * 
     * @details
     * 在指定位置之后插入count个value的副本。
     * 元素在给定位置之后顺序插入。
     * 
     * @note Time complexity: O(count)
     * @note 时间复杂度：O(count)
     */
    iterator insert_after(const_iterator pos, size_type count, const T& value) {
        // 获取位置节点的指针（需要移除const限定符以修改next指针）
        node_pointer pos_node = const_cast<node_pointer>(pos.node());
        iterator result = iterator(pos_node);
        
        // 循环插入count个相同元素
        for (size_type i = 0; i < count; ++i) {
            // 每次插入后更新result迭代器，使其指向新插入的节点
            // 这样下一次插入就会在新插入节点之后进行
            result = insert_after(result, value);
        }
        
        // 返回指向第一个新插入节点的迭代器
        return iterator(pos_node->next);
    }
    
    /**
     * @brief Insert range of elements after specified position
     * @brief 在指定位置之后插入元素范围
     * 
     * @tparam InputIt Type of input iterator
     * @tparam InputIt 输入迭代器类型
     * @param pos Iterator to the position after which elements will be inserted
     * @param pos 元素将插入到此位置之后的迭代器
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to the element after the last element in the range
     * @param last 指向范围中最后一个元素之后元素的迭代器
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details
     * Inserts elements from the range [first, last) after the specified position.
     * The elements are inserted in the same order as they appear in the range.
     * 
     * @details
     * 在指定位置之后插入范围[first, last)中的元素。
     * 元素按照它们在范围中出现的顺序插入。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the range
     * @note 时间复杂度：O(n)，其中n是范围中的元素数量
     */
    template<typename InputIt, typename = typename mystl::enable_if<
        !mystl::is_integral<InputIt>::value>::type>
    iterator insert_after(const_iterator pos, InputIt first, InputIt last) {
        node_pointer pos_node = const_cast<node_pointer>(pos.node());
        iterator result = iterator(pos_node);
        
        for (; first != last; ++first) {
            result = insert_after(result, *first);
        }
        
        return iterator(pos_node->next);
    }
    
    /**
     * @brief Insert initializer list after specified position
     * @brief 在指定位置之后插入初始化列表
     * 
     * @param pos Iterator to the position after which elements will be inserted
     * @param pos 元素将插入到此位置之后的迭代器
     * @param ilist Initializer list containing elements to insert
     * @param ilist 包含要插入元素的初始化列表
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details
     * Inserts elements from the initializer list after the specified position.
     * The elements are inserted in the same order as they appear in the list.
     * 
     * @details
     * 在指定位置之后插入初始化列表中的元素。
     * 元素按照它们在列表中出现的顺序插入。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the list
     * @note 时间复杂度：O(n)，其中n是列表中的元素数量
     */
    iterator insert_after(const_iterator pos, std::initializer_list<T> ilist) {
        return insert_after(pos, ilist.begin(), ilist.end());
    }
    
    /**
     * @brief Erase element after specified position
     * @brief 删除指定位置之后的元素
     * 
     * @param pos Iterator to the position before the element to erase
     * @param pos 指向要删除元素之前位置的迭代器
     * @return Iterator pointing to the element after the erased element
     * @return 指向被删除元素之后元素的迭代器
     * 
     * @details
     * Removes the element after the specified position.
     * The element to erase is the one immediately after the given iterator.
     * 
     * @details
     * 删除指定位置之后的元素。
     * 要删除的元素是给定迭代器之后紧邻的元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    iterator erase_after(const_iterator pos) {
        // 获取位置节点的指针（需要移除const限定符以修改next指针）
        node_pointer pos_node = const_cast<node_pointer>(pos.node());
        
        // 检查要删除的节点是否存在
        if (pos_node->next == nullptr) {
            // 如果pos_node是最后一个节点，没有节点可删除，返回end()
            return end();
        }
        
        // 获取要删除的节点
        node_pointer to_delete = pos_node->next;
        
        // 将pos_node的next指针指向要删除节点的下一个节点，跳过要删除的节点
        pos_node->next = to_delete->next;
        
        // 销毁并释放要删除的节点
        destroy_node(to_delete);
        
        // 返回指向被删除节点之后节点的迭代器
        return iterator(pos_node->next);
    }
    
    /**
     * @brief Erase range of elements after specified position
     * @brief 删除指定范围之后的元素
     * 
     * @param first Iterator to the position before the first element to erase
     * @param first 指向要删除的第一个元素之前位置的迭代器
     * @param last Iterator to the position after the last element to erase
     * @param last 指向要删除的最后一个元素之后位置的迭代器
     * @return Iterator pointing to the element after the erased range
     * @return 指向被删除范围之后元素的迭代器
     * 
     * @details
     * Removes the elements in the range (first, last).
     * The range includes all elements after first up to but not including last.
     * 
     * @details
     * 删除范围(first, last)中的元素。
     * 范围包括first之后的所有元素，直到但不包括last。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the range
     * @note 时间复杂度：O(n)，其中n是范围中的元素数量
     */
    iterator erase_after(const_iterator first, const_iterator last) {
        // 获取范围起始和结束节点的指针（需要移除const限定符以修改next指针）
        node_pointer first_node = const_cast<node_pointer>(first.node());
        node_pointer last_node = const_cast<node_pointer>(last.node());
        
        // 检查范围是否有效：如果first和last相同，或者first的下一个节点就是last，则没有节点需要删除
        if (first_node == last_node || first_node->next == last_node) {
            return iterator(last_node);
        }
        
        // 遍历并删除(first_node, last_node)之间的所有节点
        // 注意：范围是开区间(first_node, last_node)，不包括first_node和last_node
        node_pointer current = first_node->next;
        while (current != last_node) {
            // 保存下一个节点的指针，因为当前节点即将被销毁
            node_pointer next = current->next;
            
            // 销毁并释放当前节点
            destroy_node(current);
            
            // 移动到下一个节点
            current = next;
        }
        
        // 将first_node的next指针指向last_node，完成范围删除
        first_node->next = last_node;
        
        // 返回指向last_node的迭代器
        return iterator(last_node);
    }
    
    // ==================== 链表操作 ====================
    
    /**
     * @brief Splice entire other list after specified position
     * @brief 将整个other链表移动到pos位置之后
     * 
     * @param pos Iterator to the position after which other will be spliced
     * @param pos other将拼接到此位置之后的迭代器
     * @param other Another forward_list to splice from
     * @param other 要拼接的另一个forward_list
     * 
     * @details
     * Moves all elements from other into *this after the specified position.
     * After the operation, other becomes empty.
     * 
     * @details
     * 将other中的所有元素移动到*this中指定位置之后。
     * 操作后，other变为空。
     * 
     * @note Time complexity: O(1) if other is empty, O(n) otherwise where n is the size of other
     * @note 时间复杂度：如果other为空则为O(1)，否则为O(n)，其中n是other的大小
     */
    void splice_after(const_iterator pos, forward_list& other) {
        if (this == &other || other.empty()) {
            return;
        }
        
        node_pointer pos_node = const_cast<node_pointer>(pos.node());
        node_pointer other_first = other.head_.next;
        node_pointer other_last = other_first;
        
        // 找到other的最后一个节点
        while (other_last->next != nullptr) {
            other_last = other_last->next;
        }
        
        // 将other的节点插入到当前链表中
        other_last->next = pos_node->next;
        pos_node->next = other_first;
        
        // 清空other
        other.head_.next = nullptr;
    }
    
    /**
     * @brief Splice single node from other list after specified position
     * @brief 将other中的单个节点移动到pos位置之后
     * 
     * @param pos Iterator to the position after which the node will be spliced
     * @param pos 节点将拼接到此位置之后的迭代器
     * @param other Another forward_list to splice from
     * @param other 要拼接的另一个forward_list
     * @param it Iterator to the position before the node to splice
     * @param it 指向要拼接节点之前位置的迭代器
     * 
     * @details
     * Moves the element after it from other into *this after the specified position.
     * The element pointed to by it is not moved, only the element after it.
     * After the operation, the moved element is removed from other.
     * 
     * @details
     * 将it之后的元素从other移动到*this中指定位置之后。
     * it指向的元素不被移动，只移动it之后的元素。
     * 操作后，被移动的元素从other中移除。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    void splice_after(const_iterator pos, forward_list& other, const_iterator it) {
        if (this == &other && pos.node()->next == it.node()) {
            return;
        }

        node_pointer pos_node = const_cast<node_pointer>(pos.node());
        node_pointer it_node = const_cast<node_pointer>(it.node());
        node_pointer node_to_move = it_node->next;

        if (node_to_move == nullptr) {
            return;
        }

        // 从other中移除节点
        it_node->next = node_to_move->next;

        // 将节点插入到当前链表中
        node_to_move->next = pos_node->next;
        pos_node->next = node_to_move;
    }

    /**
     * @brief Splice range of nodes from other list after specified position
     * @brief 将other中的节点范围移动到pos位置之后
     * 
     * @param pos Iterator to the position after which nodes will be spliced
     * @param pos 节点将拼接到此位置之后的迭代器
     * @param other Another forward_list to splice from
     * @param other 要拼接的另一个forward_list
     * @param first Iterator to the position before the first node to splice
     * @param first 指向要拼接的第一个节点之前位置的迭代器
     * @param last Iterator to the position after the last node to splice
     * @param last 指向要拼接的最后一个节点之后位置的迭代器
     * 
     * @details
     * Moves the elements in the range (first, last) from other into *this after the specified position.
     * The range includes all elements after first up to but not including last.
     * After the operation, the moved elements are removed from other.
     * 
     * @details
     * 将范围(first, last)中的元素从other移动到*this中指定位置之后。
     * 范围包括first之后的所有元素，直到但不包括last。
     * 操作后，被移动的元素从other中移除。
     * 
     * @note Time complexity: O(n) where n is the number of elements in the range
     * @note 时间复杂度：O(n)，其中n是范围中的元素数量
     */
    void splice_after(const_iterator pos, forward_list& other, const_iterator first, const_iterator last) {
        if (first == last) {
            return;
        }

        node_pointer pos_node = const_cast<node_pointer>(pos.node());
        node_pointer first_node = const_cast<node_pointer>(first.node());
        node_pointer last_node = const_cast<node_pointer>(last.node());

        // 找到要移动的最后一个节点
        node_pointer node_before_last = first_node;
        while (node_before_last->next != last_node) {
            node_before_last = node_before_last->next;
        }

        // 从other中移除节点范围
        node_pointer range_first = first_node->next;
        node_pointer range_last = node_before_last;

        first_node->next = last_node;

        // 将节点范围插入到当前链表中
        range_last->next = pos_node->next;
        pos_node->next = range_first;
    }
    
    /**
     * @brief Remove all elements equal to the specified value
     * @brief 删除所有等于指定值的元素
     * 
     * @param value Value of elements to remove
     * @param value 要删除元素的值
     * 
     * @details
     * Removes all elements that are equal to the specified value.
     * The comparison is done using operator==.
     * 
     * @details
     * 删除所有等于指定值的元素。
     * 使用operator==进行比较。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note 时间复杂度：O(n)，其中n是元素数量
     */
    void remove(const T& value) {
        node_pointer prev = &head_;
        node_pointer current = head_.next;

        while (current != nullptr) {
            if (current->value == value) {
                // 删除当前节点
                prev->next = current->next;
                destroy_node(current);
                current = prev->next;
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
    
    /**
     * @brief Remove all elements satisfying specific criteria
     * @brief 删除所有满足特定条件的元素
     * 
     * @tparam UnaryPredicate Type of predicate function
     * @tparam UnaryPredicate 谓词函数类型
     * @param p Unary predicate that returns true for elements to remove
     * @param p 一元谓词，对于要删除的元素返回true
     * 
     * @details
     * Removes all elements for which predicate p returns true.
     * The predicate is applied to each element in the forward_list.
     * 
     * @details
     * 删除所有谓词p返回true的元素。
     * 谓词应用于forward_list中的每个元素。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note 时间复杂度：O(n)，其中n是元素数量
     */
    template<typename UnaryPredicate>
    void remove_if(UnaryPredicate p) {
        node_pointer prev = &head_;
        node_pointer current = head_.next;

        while (current != nullptr) {
            if (p(current->value)) {
                // 删除当前节点
                prev->next = current->next;
                destroy_node(current);
                current = prev->next;
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
    
    /**
     * @brief Remove consecutive duplicate elements
     * @brief 删除连续重复元素
     * 
     * @details
     * Removes all but the first element from every consecutive group of
     * equal elements in the forward_list. The comparison is done using
     * operator==.
     * 
     * @details
     * 从forward_list中每个连续相等元素组中删除除第一个元素外的所有元素。
     * 使用operator==进行比较。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note 时间复杂度：O(n)，其中n是元素数量
     */
    void unique() {
        if (head_.next == nullptr || head_.next->next == nullptr) {
            return;
        }

        node_pointer current = head_.next;
        while (current->next != nullptr) {
            if (current->value == current->next->value) {
                // 删除下一个节点
                node_pointer to_delete = current->next;
                current->next = to_delete->next;
                destroy_node(to_delete);
            } else {
                current = current->next;
            }
        }
    }

    /**
     * @brief Remove consecutive duplicate elements using custom predicate
     * @brief 使用自定义谓词删除连续重复元素
     * 
     * @tparam BinaryPredicate Type of binary predicate function
     * @tparam BinaryPredicate 二元谓词函数类型
     * @param p Binary predicate that returns true if two elements are considered equivalent
     * @param p 二元谓词，如果两个元素被认为是等价的则返回true
     * 
     * @details
     * Removes all but the first element from every consecutive group of
     * equivalent elements in the forward_list. The equivalence is determined
     * by the binary predicate p.
     * 
     * @details
     * 从forward_list中每个连续等价元素组中删除除第一个元素外的所有元素。
     * 等价性由二元谓词p确定。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note 时间复杂度：O(n)，其中n是元素数量
     */
    template<typename BinaryPredicate>
    void unique(BinaryPredicate p) {
        if (head_.next == nullptr || head_.next->next == nullptr) {
            return;
        }
        
        node_pointer current = head_.next;
        while (current->next != nullptr) {
            if (p(current->value, current->next->value)) {
                // 删除下一个节点
                node_pointer to_delete = current->next;
                current->next = to_delete->next;
                destroy_node(to_delete);
            } else {
                current = current->next;
            }
        }
    }
    
    /**
     * @brief Merge two sorted forward_lists
     * @brief 合并两个已排序的forward_list
     * 
     * @param other Another sorted forward_list to merge
     * @param other 要合并的另一个已排序forward_list
     * 
     * @details
     * Merges two sorted forward_lists into one sorted forward_list.
     * The elements are compared using operator<.
     * After the merge, other becomes empty.
     * 
     * @details
     * 将两个已排序的forward_list合并为一个已排序的forward_list。
     * 使用operator<比较元素。
     * 合并后，other变为空。
     * 
     * @note Time complexity: O(n + m) where n and m are the sizes of the lists
     * @note 时间复杂度：O(n + m)，其中n和m是列表的大小
     */
    void merge(forward_list& other) {
        merge(other, [](const T& a, const T& b) { return a < b; });
    }

    /**
     * @brief Merge two sorted forward_lists with custom comparator
     * @brief 使用自定义比较器合并两个已排序的forward_list
     * 
     * @tparam Compare Type of comparison function
     * @tparam Compare 比较函数的类型
     * @param other Another sorted forward_list to merge
     * @param other 要合并的另一个已排序forward_list
     * @param comp Comparison function that returns true if the first argument
     * should be ordered before the second
     * @param comp 比较函数，如果第一个参数应排在第二个参数之前则返回true
     * 
     * @details
     * Merges two sorted forward_lists into one sorted forward_list.
     * The elements are compared using the provided comparator.
     * After the merge, other becomes empty.
     * 
     * @details
     * 将两个已排序的forward_list合并为一个已排序的forward_list。
     * 使用提供的比较器比较元素。
     * 合并后，other变为空。
     * 
     * @note Time complexity: O(n + m) where n and m are the sizes of the lists
     * @note 时间复杂度：O(n + m)，其中n和m是列表的大小
     */
    template<typename Compare>
    void merge(forward_list& other, Compare comp) {
        if (this == &other || other.empty()) {
            return;
        }

        if (empty()) {
            // 如果当前链表为空，直接交换
            swap(other);
            return;
        }

        node_pointer this_prev = &head_;
        node_pointer this_current = head_.next;
        node_pointer other_current = other.head_.next;

        while (this_current != nullptr && other_current != nullptr) {
            if (comp(other_current->value, this_current->value)) {
                // other_current应该插入到this_current之前
                node_pointer next_other = other_current->next;

                // 插入节点
                this_prev->next = other_current;
                other_current->next = this_current;

                // 更新指针
                this_prev = other_current;
                other_current = next_other;
                other.head_.next = other_current;
            } else {
                this_prev = this_current;
                this_current = this_current->next;
            }
        }

        // 如果other还有剩余节点，全部追加到末尾
        if (other_current != nullptr) {
            this_prev->next = other_current;
            other.head_.next = nullptr;
        }
    }
    
    /**
     * @brief Sorts the elements in ascending order using merge sort
     * @brief 使用归并排序按升序对元素进行排序
     * 
     * @details
     * Implements the merge sort algorithm for singly-linked lists.
     * The algorithm recursively splits the list into halves, sorts each half,
     * and then merges the sorted halves back together.
     * 
     * @details
     * 为单向链表实现归并排序算法。
     * 该算法递归地将列表分成两半，对每一半进行排序，
     * 然后将排序后的两半合并回一起。
     * 
     * @note Time complexity: O(n log n)
     * @note Space complexity: O(log n) for recursion stack
     * 
     * @note 时间复杂度：O(n log n)
     * @note 空间复杂度：递归栈的O(log n)
     */
    void sort() {
        sort([](const T& a, const T& b) { return a < b; });
    }
    
    /**
     * @brief Sorts the elements using merge sort with custom comparator
     * @brief 使用自定义比较器的归并排序对元素进行排序
     * 
     * @tparam Compare Type of comparison function
     * @tparam Compare 比较函数的类型
     * @param comp Comparison function that returns true if the first argument
     * should be ordered before the second
     * @param comp 比较函数，如果第一个参数应排在第二个参数之前则返回true
     * 
     * @details
     * Implements the merge sort algorithm with a custom comparator.
     * The algorithm follows the divide-and-conquer approach:
     * 1. Split the list into two halves using the fast-slow pointer technique
     * 2. Recursively sort each half
     * 3. Merge the sorted halves back together
     * 
     * @details
     * 使用自定义比较器实现归并排序算法。
     * 该算法遵循分治方法：
     * 1. 使用快慢指针技术将列表分成两半
     * 2. 递归地对每一半进行排序
     * 3. 将排序后的两半合并回一起
     * 
     * @note Time complexity: O(n log n) in all cases
     * @note Space complexity: O(log n) for recursion stack
     * 
     * @note 时间复杂度：所有情况下都是O(n log n)
     * @note 空间复杂度：递归栈的O(log n)
     */
    template<typename Compare>
    void sort(Compare comp) {
        if (head_.next == nullptr || head_.next->next == nullptr) {
            return;
        }
        
        // 使用归并排序
        forward_list left, right;
        split(*this, left, right);
        
        left.sort(comp);
        right.sort(comp);
        
        // 合并两个已排序的链表
        merge_sorted(left, right, comp);
    }
    
    /**
     * @brief Reverses the order of elements in the list
     * @brief 反转列表中元素的顺序
     * 
     * @details
     * Reverses the singly-linked list in-place by iteratively reversing
     * the direction of each node's next pointer. The algorithm uses
     * three pointers (prev, current, next) to traverse the list and
     * reverse links one by one.
     * 
     * @details
     * 通过迭代地反转每个节点的next指针方向，在原地反转单向链表。
     * 该算法使用三个指针（prev、current、next）遍历列表并逐个反转链接。
     * 
     * @note Time complexity: O(n) where n is the number of elements
     * @note Space complexity: O(1) - uses only a few pointers
     * 
     * @note 时间复杂度：O(n)，其中n是元素数量
     * @note 空间复杂度：O(1) - 仅使用几个指针
     */
    void reverse() noexcept {
        // 空链表或只有一个元素，无需反转
        if (head_.next == nullptr || head_.next->next == nullptr) {
            return;
        }
        
        node_pointer prev = nullptr;    // 前一个节点，初始为nullptr
        node_pointer current = head_.next;  // 当前节点，从第一个元素开始
        node_pointer next = nullptr;    // 下一个节点，用于保存current->next
        
        // 遍历链表，反转每个节点的next指针
        while (current != nullptr) {
            // 保存下一个节点，因为即将修改current->next
            next = current->next;
            
            // 反转当前节点的指针：指向prev而不是next
            current->next = prev;
            
            // 移动prev到当前节点，为下一次迭代做准备
            prev = current;
            
            // 移动到下一个节点
            current = next;
        }
        
        // 更新头节点的next指针，指向新的第一个节点（原来的最后一个节点）
        head_.next = prev;
    }
    
    // ==================== 其他操作 ====================
    
    /**
     * @brief Swap contents with another forward_list
     * @brief 与另一个forward_list交换内容
     * 
     * @param other Another forward_list to swap with
     * @param other 要交换的另一个forward_list
     * 
     * @details
     * Exchanges the contents of this forward_list with those of other.
     * Does not invoke any move, copy, or swap operations on individual elements.
     * All iterators and references remain valid. The past-the-end iterator is invalidated.
     * 
     * @details
     * 将此forward_list的内容与other交换。
     * 不会对单个元素调用任何移动、拷贝或交换操作。
     * 所有迭代器和引用保持有效。最后一个元素之后位置的迭代器失效。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    void swap(forward_list& other) noexcept {
        using mystl::swap;
        swap(head_.next, other.head_.next);
        swap(alloc_wrapper_, other.alloc_wrapper_);
    }
    
    /**
     * @brief Resize the forward_list
     * @brief 调整forward_list的大小
     * 
     * @param count New size of the forward_list
     * @param count forward_list的新大小
     * 
     * @details
     * Resizes the forward_list to contain count elements.
     * If the current size is greater than count, the forward_list is reduced to its first count elements.
     * If the current size is less than count, additional default-inserted elements are appended.
     * 
     * @details
     * 调整forward_list的大小以包含count个元素。
     * 如果当前大小大于count，forward_list被缩减为其前count个元素。
     * 如果当前大小小于count，则追加额外的默认插入元素。
     * 
     * @note Time complexity: O(|count - size|)
     * @note 时间复杂度：O(|count - size|)
     */
    void resize(size_type count) {
        resize(count, T());
    }
    
    /**
     * @brief Resize the forward_list with specified value
     * @brief 使用指定值调整forward_list的大小
     * 
     * @param count New size of the forward_list
     * @param count forward_list的新大小
     * @param value The value to initialize new elements with
     * @param value 用于初始化新元素的值
     * 
     * @details
     * Resizes the forward_list to contain count elements.
     * If the current size is greater than count, the forward_list is reduced to its first count elements.
     * If the current size is less than count, additional copies of value are appended.
     * 
     * @details
     * 调整forward_list的大小以包含count个元素。
     * 如果当前大小大于count，forward_list被缩减为其前count个元素。
     * 如果当前大小小于count，则追加value的额外副本。
     * 
     * @note Time complexity: O(|count - size|)
     * @note 时间复杂度：O(|count - size|)
     */
    void resize(size_type count, const T& value) {
        iterator it = before_begin();
        size_type current_size = 0;
        
        // 计算当前大小
        while (it.node()->next != nullptr) {
            ++current_size;
            ++it;
        }
        
        if (count > current_size) {
            // 需要添加元素
            size_type to_add = count - current_size;
            for (size_type i = 0; i < to_add; ++i) {
                insert_after(it, value);
                ++it;
            }
        } else if (count < current_size) {
            // 需要删除元素
            it = before_begin();
            for (size_type i = 0; i < count; ++i) {
                ++it;
            }
            erase_after(it, end());
        }
    }
    
    /**
     * @brief Assign values to the forward_list
     * @brief 为forward_list赋值
     * 
     * @param count Number of elements to assign
     * @param count 要分配的元素数量
     * @param value Value to assign to each element
     * @param value 分配给每个元素的值
     * 
     * @details
     * Replaces the contents of the forward_list with count copies of value.
     * All existing elements are destroyed and new elements are constructed.
     * 
     * @details
     * 用count个value的副本替换forward_list的内容。
     * 所有现有元素被销毁，新元素被构造。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is count
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是count
     */
    void assign(size_type count, const T& value) {
        clear();
        iterator it = before_begin();
        for (size_type i = 0; i < count; ++i) {
            it = insert_after(it, value);
        }
    }
    
    /**
     * @brief Assign range of values to the forward_list
     * @brief 为forward_list分配值范围
     * 
     * @tparam InputIt Type of input iterator
     * @tparam InputIt 输入迭代器类型
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to the element after the last element in the range
     * @param last 指向范围中最后一个元素之后元素的迭代器
     * 
     * @details
     * Replaces the contents of the forward_list with elements from the range [first, last).
     * All existing elements are destroyed and new elements are constructed.
     * 
     * @details
     * 用范围[first, last)中的元素替换forward_list的内容。
     * 所有现有元素被销毁，新元素被构造。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is the number of elements in the range
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是范围中的元素数量
     */
    template<typename InputIt, typename = typename mystl::enable_if<
        !mystl::is_integral<InputIt>::value>::type>
    void assign(InputIt first, InputIt last) {
        clear();
        iterator it = before_begin();
        for (; first != last; ++first) {
            it = insert_after(it, *first);
        }
    }
    
    /**
     * @brief Assign initializer list to the forward_list
     * @brief 为forward_list分配初始化列表
     * 
     * @param ilist Initializer list containing elements to assign
     * @param ilist 包含要分配元素的初始化列表
     * 
     * @details
     * Replaces the contents of the forward_list with elements from the initializer list.
     * All existing elements are destroyed and new elements are constructed.
     * 
     * @details
     * 用初始化列表中的元素替换forward_list的内容。
     * 所有现有元素被销毁，新元素被构造。
     * 
     * @note Time complexity: O(n + m) where n is the current size and m is the number of elements in the list
     * @note 时间复杂度：O(n + m)，其中n是当前大小，m是列表中的元素数量
     */
    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }
    
    /**
     * @brief Get the allocator associated with the forward_list
     * @brief 获取与forward_list关联的分配器
     * 
     * @return The allocator associated with the forward_list
     * @return 与forward_list关联的分配器
     * 
     * @details
     * Returns a copy of the allocator associated with the forward_list.
     * 
     * @details
     * 返回与forward_list关联的分配器的副本。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    allocator_type get_allocator() const noexcept {
        return alloc_wrapper_.get_allocator();
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
     * Constructs an element in-place at the beginning of the forward_list.
     * The element is constructed using perfect forwarding of the provided arguments.
     * This avoids unnecessary copies or moves that would occur with push_front().
     * 
     * @details
     * 在forward_list的开头原位构造元素。
     * 使用提供的参数的完美转发来构造元素。
     * 这避免了使用push_front()时可能发生的不必要的拷贝或移动。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    template<typename... Args>
    void emplace_front(Args&&... args) {
        head_.next = create_node(head_.next, mystl::forward<Args>(args)...);
    }
    
    /**
     * @brief Construct element in-place after specified position
     * @brief 在指定位置之后原位构造元素
     * 
     * @tparam Args Types of arguments to forward to the constructor
     * @tparam Args 转发给构造函数的参数类型
     * @param pos Iterator to the position after which the element will be constructed
     * @param pos 元素将构造到此位置之后的迭代器
     * @param args Arguments to forward to the constructor
     * @param args 转发给构造函数的参数
     * @return Iterator pointing to the constructed element
     * @return 指向构造元素的迭代器
     * 
     * @details
     * Constructs an element in-place after the specified position.
     * The element is constructed using perfect forwarding of the provided arguments.
     * This avoids unnecessary copies or moves that would occur with insert_after().
     * 
     * @details
     * 在指定位置之后原位构造元素。
     * 使用提供的参数的完美转发来构造元素。
     * 这避免了使用insert_after()时可能发生的不必要的拷贝或移动。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     */
    template<typename... Args>
    iterator emplace_after(const_iterator pos, Args&&... args) {
        node_pointer pos_node = const_cast<node_pointer>(pos.node());
        node_pointer new_node = create_node(pos_node->next, mystl::forward<Args>(args)...);
        pos_node->next = new_node;
        return iterator(new_node);
    }
    
    // ==================== 比较操作符 ====================
    
    /**
     * @brief Equality comparison operator
     * @brief 相等比较操作符
     * 
     * @param other Another forward_list to compare with
     * @param other 要比较的另一个forward_list
     * @return true if both forward_lists have the same size and elements are equal,
     * false otherwise
     * @return 如果两个forward_list大小相同且元素相等则返回true，否则返回false
     * 
     * @details
     * Compares two forward_lists for equality. Two forward_lists are equal if they
     * have the same number of elements and each element in one forward_list compares
     * equal with the element in the same position in the other forward_list.
     * 
     * @details
     * 比较两个forward_list是否相等。如果两个forward_list具有相同数量的元素，
     * 并且一个forward_list中的每个元素与另一个forward_list中相同位置的元素相等，
     * 则它们相等。
     * 
     * @note Time complexity: O(min(n, m)) where n and m are the sizes of the lists
     * @note 时间复杂度：O(min(n, m))，其中n和m是列表的大小
     */
    bool operator==(const forward_list& other) const {
        const_iterator it1 = begin();
        const_iterator it2 = other.begin();
        
        while (it1 != end() && it2 != other.end()) {
            if (*it1 != *it2) {
                return false;
            }
            ++it1;
            ++it2;
        }
        
        return it1 == end() && it2 == other.end();
    }
    
    /**
     * @brief Inequality comparison operator
     * @brief 不等比较操作符
     * 
     * @param other Another forward_list to compare with
     * @param other 要比较的另一个forward_list
     * @return true if forward_lists are not equal, false otherwise
     * @return 如果forward_list不相等则返回true，否则返回false
     * 
     * @details
     * Compares two forward_lists for inequality. Returns true if the forward_lists
     * are not equal, i.e., if they have different sizes or at least one pair of
     * corresponding elements are not equal.
     * 
     * @details
     * 比较两个forward_list是否不相等。如果forward_list不相等则返回true，
     * 即如果它们大小不同或至少有一对对应元素不相等。
     * 
     * @note This operator is implemented in terms of operator==
     * @note 此操作符基于operator==实现
     */
    bool operator!=(const forward_list& other) const {
        return !(*this == other);
    }
    
    /**
     * @brief Less-than comparison operator
     * @brief 小于比较操作符
     * 
     * @param other Another forward_list to compare with
     * @param other 要比较的另一个forward_list
     * @return true if this forward_list is lexicographically less than other,
     * false otherwise
     * @return 如果此forward_list在字典序上小于other则返回true，否则返回false
     * 
     * @details
     * Performs lexicographical comparison between two forward_lists.
     * Compares the elements sequentially using operator< for the element type.
     * The comparison stops at the first mismatching element or when one list ends.
     * 
     * @details
     * 对两个forward_list执行字典序比较。
     * 使用元素类型的operator<依次比较元素。
     * 比较在第一个不匹配的元素处停止，或当一个列表结束时停止。
     * 
     * @note Time complexity: O(min(n, m)) where n and m are the sizes of the lists
     * @note 时间复杂度：O(min(n, m))，其中n和m是列表的大小
     */
    bool operator<(const forward_list& other) const {
        return mystl::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }
    
    /**
     * @brief Less-than-or-equal comparison operator
     * @brief 小于等于比较操作符
     * 
     * @param other Another forward_list to compare with
     * @param other 要比较的另一个forward_list
     * @return true if this forward_list is lexicographically less than or equal to other,
     * false otherwise
     * @return 如果此forward_list在字典序上小于或等于other则返回true，否则返回false
     * 
     * @details
     * Checks if this forward_list is less than or equal to another forward_list.
     * Implemented as !(other < *this), which is equivalent to *this <= other.
     * 
     * @details
     * 检查此forward_list是否小于或等于另一个forward_list。
     * 实现为!(other < *this)，等价于*this <= other。
     */
    bool operator<=(const forward_list& other) const {
        return !(other < *this);
    }
    
    /**
     * @brief Greater-than comparison operator
     * @brief 大于比较操作符
     * 
     * @param other Another forward_list to compare with
     * @param other 要比较的另一个forward_list
     * @return true if this forward_list is lexicographically greater than other,
     * false otherwise
     * @return 如果此forward_list在字典序上大于other则返回true，否则返回false
     * 
     * @details
     * Checks if this forward_list is greater than another forward_list.
     * Implemented as other < *this, which is equivalent to *this > other.
     * 
     * @details
     * 检查此forward_list是否大于另一个forward_list。
     * 实现为other < *this，等价于*this > other。
     */
    bool operator>(const forward_list& other) const {
        return other < *this;
    }
    
    /**
     * @brief Greater-than-or-equal comparison operator
     * @brief 大于等于比较操作符
     * 
     * @param other Another forward_list to compare with
     * @param other 要比较的另一个forward_list
     * @return true if this forward_list is lexicographically greater than or equal to other,
     * false otherwise
     * @return 如果此forward_list在字典序上大于或等于other则返回true，否则返回false
     * 
     * @details
     * Checks if this forward_list is greater than or equal to another forward_list.
     * Implemented as !(*this < other), which is equivalent to *this >= other.
     * 
     * @details
     * 检查此forward_list是否大于或等于另一个forward_list。
     * 实现为!(*this < other)，等价于*this >= other。
     */
    bool operator>=(const forward_list& other) const {
        return !(*this < other);
    }
    
private:
    // 节点管理辅助函数
    
    /**
     * @brief Creates a new node with the given value
     * @brief 创建具有给定值的新节点
     * 
     * @param value Value to store in the node
     * @param value 要存储在节点中的值
     * @param next Pointer to the next node (default: nullptr)
     * @param next 指向下一个节点的指针（默认：nullptr）
     * @return Pointer to the newly created node
     * @return 指向新创建节点的指针
     * 
     * @note Exception safety: Strong guarantee - if node construction fails,
     * memory is properly deallocated.
     * 
     * @note 异常安全性：强保证 - 如果节点构造失败，内存会被正确释放。
     */
    node_pointer create_node(const T& value, node_pointer next = nullptr) {
        node_pointer new_node = alloc_wrapper_.allocate();
        try {
            alloc_wrapper_.construct(new_node, next, value);
        } catch (...) {
            alloc_wrapper_.deallocate(new_node);
            throw;
        }
        return new_node;
    }
    
    /**
     * @brief Creates a new node with the given rvalue reference
     * @brief 创建具有给定右值引用的新节点
     * 
     * @param value Value to move into the node
     * @param value 要移动到节点中的值
     * @param next Pointer to the next node (default: nullptr)
     * @param next 指向下一个节点的指针（默认：nullptr）
     * @return Pointer to the newly created node
     * @return 指向新创建节点的指针
     * 
     * @note Exception safety: Strong guarantee - if node construction fails,
     * memory is properly deallocated.
     * 
     * @note 异常安全性：强保证 - 如果节点构造失败，内存会被正确释放。
     */
    node_pointer create_node(T&& value, node_pointer next = nullptr) {
        node_pointer new_node = alloc_wrapper_.allocate();
        try {
            alloc_wrapper_.construct(new_node, next, mystl::move(value));
        } catch (...) {
            alloc_wrapper_.deallocate(new_node);
            throw;
        }
        return new_node;
    }
    
    /**
     * @brief Creates a new node with variadic arguments for in-place construction
     * @brief 使用可变参数原位构造新节点
     * 
     * @tparam Args Types of arguments for node construction
     * @tparam Args 节点构造的参数类型
     * @param next Pointer to the next node
     * @param next 指向下一个节点的指针
     * @param args Arguments to forward to node constructor
     * @param args 转发给节点构造函数的参数
     * @return Pointer to the newly created node
     * @return 指向新创建节点的指针
     * 
     * @note Exception safety: Strong guarantee - if node construction fails,
     * memory is properly deallocated.
     * 
     * @note 异常安全性：强保证 - 如果节点构造失败，内存会被正确释放。
     */
    template<typename... Args>
    node_pointer create_node(node_pointer next, Args&&... args) {
        node_pointer new_node = alloc_wrapper_.allocate();
        try {
            alloc_wrapper_.construct(new_node, next, mystl::forward<Args>(args)...);
        } catch (...) {
            alloc_wrapper_.deallocate(new_node);
            throw;
        }
        return new_node;
    }
    
    /**
     * @brief Destroys a node and deallocates its memory
     * @brief 销毁节点并释放其内存
     * 
     * @param node Pointer to the node to destroy
     * @param node 指向要销毁节点的指针
     * 
     * @note This function is noexcept, ensuring no exceptions are thrown
     * during node destruction.
     * 
     * @note 此函数是noexcept的，确保在节点销毁期间不会抛出异常。
     */
    void destroy_node(node_pointer node) noexcept {
        if (node) {
            alloc_wrapper_.destroy(node);
            alloc_wrapper_.deallocate(node);
        }
    }
    
    /**
     * @brief Splits a linked list into two halves (for merge sort)
     * @brief 将链表分割为两半（用于归并排序）
     * 
     * @param source Source list to split
     * @param source 要分割的源链表
     * @param left First half of the split
     * @param left 分割的第一半
     * @param right Second half of the split
     * @param right 分割的第二半
     * 
     * @details
     * Uses the fast-slow pointer technique to find the middle node.
     * The source list is split into two halves: left contains nodes
     * from the beginning to the middle, right contains nodes from
     * the middle to the end.
     * 
     * @details
     * 使用快慢指针技术找到中间节点。
     * 源链表被分割为两半：left包含从开始到中间的节点，
     * right包含从中间到结束的节点。
     * 
     * @note Time complexity: O(n) where n is the length of the source list.
     * 
     * @note 时间复杂度：O(n)，其中n是源链表的长度。
     */
    void split(forward_list& source, forward_list& left, forward_list& right) {
        if (source.empty()) {
            return;
        }
        
        // 使用快慢指针找到中间节点
        node_pointer slow = source.head_.next;
        node_pointer fast = source.head_.next;
        
        while (fast != nullptr && fast->next != nullptr && fast->next->next != nullptr) {
            slow = slow->next;
            fast = fast->next->next;
        }
        
        // 分割链表
        node_pointer mid = slow;
        
        // left包含[head_.next, mid]
        left.head_.next = source.head_.next;
        node_pointer left_last = mid;
        
        // right包含[mid->next, end)
        right.head_.next = mid->next;
        
        // 断开连接
        if (left_last != nullptr) {
            left_last->next = nullptr;
        }
        
        // 清空source
        source.head_.next = nullptr;
    }
    
    /**
     * @brief Merges two sorted linked lists (for merge sort)
     * @brief 合并两个已排序的链表（用于归并排序）
     * 
     * @tparam Compare Comparison function type
     * @tparam Compare 比较函数类型
     * @param left First sorted list to merge
     * @param left 要合并的第一个已排序链表
     * @param right Second sorted list to merge
     * @param right 要合并的第二个已排序链表
     * @param comp Comparison function for ordering elements
     * @param comp 用于元素排序的比较函数
     * 
     * @details
     * Merges two sorted linked lists into a single sorted list.
     * The merged result is stored in the current forward_list object.
     * This function is used by the merge sort algorithm.
     * 
     * @details
     * 将两个已排序的链表合并为一个已排序的链表。
     * 合并结果存储在当前forward_list对象中。
     * 此函数由归并排序算法使用。
     * 
     * @note Time complexity: O(n + m) where n and m are the lengths
     * of the left and right lists respectively.
     * 
     * @note 时间复杂度：O(n + m)，其中n和m分别是left和right链表的长度。
     */
    template<typename Compare>
    void merge_sorted(forward_list& left, forward_list& right, Compare comp) {
        node_pointer left_current = left.head_.next;
        node_pointer right_current = right.head_.next;
        
        head_.next = nullptr;
        node_pointer tail = &head_;
        
        while (left_current != nullptr && right_current != nullptr) {
            if (comp(left_current->value, right_current->value)) {
                // 取left的节点
                tail->next = left_current;
                left_current = left_current->next;
            } else {
                // 取right的节点
                tail->next = right_current;
                right_current = right_current->next;
            }
            tail = tail->next;
        }
        
        // 追加剩余节点
        if (left_current != nullptr) {
            tail->next = left_current;
        } else if (right_current != nullptr) {
            tail->next = right_current;
        }
        
        // 重置left和right
        left.head_.next = nullptr;
        right.head_.next = nullptr;
    }
};

/**
 * @brief Swaps the contents of two forward_list containers
 * @brief 交换两个forward_list容器的内容
 * 
 * @tparam T Type of elements stored in the forward_list
 * @tparam T 存储在forward_list中的元素类型
 * @tparam Alloc Allocator type for memory management
 * @tparam Alloc 用于内存管理的分配器类型
 * @param lhs First forward_list to swap
 * @param lhs 要交换的第一个forward_list
 * @param rhs Second forward_list to swap
 * @param rhs 要交换的第二个forward_list
 * 
 * @details
 * Exchanges the contents of lhs and rhs. Calls lhs.swap(rhs).
 * This function provides a non-member swap function for forward_list
 * that can be found via argument-dependent lookup (ADL).
 * 
 * @details
 * 交换lhs和rhs的内容。调用lhs.swap(rhs)。
 * 此函数为forward_list提供非成员swap函数，
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
void swap(forward_list<T, Alloc>& lhs, forward_list<T, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace mystl
