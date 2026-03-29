#pragma once

/**
 * @file rb_tree.hpp
 * @brief Complete red-black tree implementation for mySTL
 * @brief mySTL完整红黑树实现
 * 
 * This file implements a complete red-black tree based on the working simple version.
 * It provides full functionality for associative containers (set, map, etc.).
 * 
 * 本文件基于工作正常的简化版本实现完整的红黑树。
 * 为关联容器（set、map等）提供完整功能。
 * 
 * @author shensjt
 * @date 2026-03-29
 */

#include <cstddef>
#include "../memory/allocator.hpp"
#include "../memory/allocator_traits.hpp"
#include "../utility/less.hpp"
#include "../utility/pair.hpp"
#include "../algorithm/algorithm.hpp"
#include "detail/rb_tree_node.hpp"

namespace mystl {

/**
 * @brief Identity function object for key extraction
 * @brief 用于键提取的恒等函数对象
 * 
 * @tparam T Type of the value
 * @tparam T 值的类型
 */
template<typename T>
struct identity {
    /**
     * @brief Returns the value unchanged
     * @brief 返回不变的值
     * 
     * @param x The value to return
     * @param x 要返回的值
     * @return const T& The same value
     * @return const T& 相同的值
     */
    const T& operator()(const T& x) const { return x; }
};

/**
 * @brief Red-black tree class
 * @brief 红黑树类
 * 
 * Implements self-balancing binary search tree with efficient search, insert and delete operations.
 * Provides full STL-compatible interface.
 * 
 * 实现自平衡二叉搜索树，支持高效的查找、插入和删除操作。
 * 提供完整的STL兼容接口。
 * 
 * @tparam Key Key type
 * @tparam Key 键类型
 * @tparam Value Value type (for set, Value=Key; for map, Value=pair<const Key, T>)
 * @tparam Value 值类型（对于set，Value=Key；对于map，Value=pair<const Key, T>）
 * @tparam KeyOfValue Function object type for extracting key from value
 * @tparam KeyOfValue 从值中提取键的函数对象类型
 * @tparam Compare Comparison function object type
 * @tparam Compare 比较函数对象类型
 * @tparam Alloc Allocator type
 * @tparam Alloc 分配器类型
 */
template<typename Key, typename Value, typename KeyOfValue = identity<Value>,
         typename Compare = less<Key>, typename Alloc = allocator<Value>,
         bool AllowDuplicates = false>
class rb_tree {
private:
    using node_type = detail::rb_tree_node<Value>;      ///< Type of tree node / 树节点类型
    using node_ptr = node_type*;                        ///< Pointer to tree node / 树节点指针
    using base_ptr = detail::rb_tree_node_base*;        ///< Pointer to base node / 基础节点指针
    using color_type = detail::rb_tree_color;           ///< Type for node color / 节点颜色类型
    
    using allocator_type = Alloc;                       ///< Type of value allocator / 值分配器类型
    using node_allocator = typename allocator_type::template rebind<node_type>::other;  ///< Type of node allocator / 节点分配器类型
    
    /**
     * @brief Forward iterator for red-black tree
     * @brief 红黑树正向迭代器
     * 
     * Implements in-order traversal of the red-black tree.
     * Provides standard iterator interface with ++, --, *, -> operators.
     * 
     * 实现红黑树的中序遍历。
     * 提供标准的迭代器接口，包括++、--、*、->操作符。
     */
    struct iterator {
        node_ptr node_;  ///< Current node pointer / 当前节点指针
        
        /**
         * @brief Default constructor
         * @brief 默认构造函数
         */
        iterator() noexcept : node_(nullptr) {}
        
        /**
         * @brief Constructor from node pointer
         * @brief 从节点指针构造
         * 
         * @param node Pointer to the node
         * @param node 指向节点的指针
         */
        explicit iterator(node_ptr node) noexcept : node_(node) {}
        
        /**
         * @brief Dereference operator
         * @brief 解引用操作符
         * 
         * @return Value& Reference to the value stored in the node
         * @return Value& 节点中存储的值的引用
         */
        Value& operator*() const noexcept { return node_->value; }
        
        /**
         * @brief Arrow operator
         * @brief 箭头操作符
         * 
         * @return Value* Pointer to the value stored in the node
         * @return Value* 指向节点中存储的值的指针
         */
        Value* operator->() const noexcept { return &node_->value; }
        
        /**
         * @brief Pre-increment operator (move to next node in in-order traversal)
         * @brief 前自增操作符（移动到中序遍历的下一个节点）
         * 
         * @return iterator& Reference to this iterator
         * @return iterator& 此迭代器的引用
         * 
         * @note Time complexity: O(log n) in worst case, O(1) amortized
         * @note 时间复杂度：最坏情况O(log n)，摊还O(1)
         */
        iterator& operator++() noexcept {
            if (node_ != nullptr) {
                // 使用节点的successor方法
                base_ptr next = node_->successor();
                if (next == nullptr || detail::rb_tree_nil_node::is_nil(next)) {
                    node_ = nullptr;  // 到达结束
                } else {
                    node_ = static_cast<node_ptr>(next);
                }
            }
            return *this;
        }
        
        iterator operator++(int) noexcept {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        iterator& operator--() noexcept {
            if (node_ != nullptr) {
                node_ = node_type::from_base(node_->predecessor());
            }
            return *this;
        }
        
        iterator operator--(int) noexcept {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const noexcept {
            return node_ == other.node_;
        }
        
        bool operator!=(const iterator& other) const noexcept {
            return node_ != other.node_;
        }
    };
    
    struct const_iterator {
        const node_ptr node_;
        
        const_iterator() noexcept : node_(nullptr) {}
        explicit const_iterator(node_ptr node) noexcept : node_(node) {}
        const_iterator(const iterator& it) noexcept : node_(it.node_) {}
        
        const Value& operator*() const noexcept { return node_->value; }
        const Value* operator->() const noexcept { return &node_->value; }
        
        const_iterator& operator++() noexcept {
            if (node_ != nullptr) {
                // 使用节点的successor方法
                base_ptr next = node_->successor();
                if (next == nullptr || detail::rb_tree_nil_node::is_nil(next)) {
                    const_cast<node_ptr&>(node_) = nullptr;  // 到达结束
                } else {
                    const_cast<node_ptr&>(node_) = static_cast<node_ptr>(next);
                }
            }
            return *this;
        }
        
        const_iterator operator++(int) noexcept {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        const_iterator& operator--() noexcept {
            if (node_ != nullptr) {
                const_cast<node_ptr&>(node_) = node_type::from_base(node_->predecessor());
            }
            return *this;
        }
        
        const_iterator operator--(int) noexcept {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const const_iterator& other) const noexcept {
            return node_ == other.node_;
        }
        
        bool operator!=(const const_iterator& other) const noexcept {
            return node_ != other.node_;
        }
    };
    
    /**
     * @brief Reverse iterator for red-black tree
     * @brief 红黑树反向迭代器
     * 
     * Implements reverse in-order traversal of the red-black tree.
     * Provides standard reverse iterator interface with ++, --, *, -> operators.
     * 
     * 实现红黑树的逆中序遍历。
     * 提供标准的反向迭代器接口，包括++、--、*、->操作符。
     */
    struct reverse_iterator {
        node_ptr node_;  ///< Current node pointer / 当前节点指针
        
        /**
         * @brief Default constructor
         * @brief 默认构造函数
         */
        reverse_iterator() noexcept : node_(nullptr) {}
        
        /**
         * @brief Constructor from node pointer
         * @brief 从节点指针构造
         * 
         * @param node Pointer to the node
         * @param node 指向节点的指针
         */
        explicit reverse_iterator(node_ptr node) noexcept : node_(node) {}
        
        /**
         * @brief Constructor from forward iterator
         * @brief 从正向迭代器构造
         * 
         * @param it Forward iterator
         * @param it 正向迭代器
         */
        explicit reverse_iterator(const iterator& it) noexcept : node_(it.node_) {}
        
        /**
         * @brief Dereference operator
         * @brief 解引用操作符
         * 
         * @return Value& Reference to the value stored in the node
         * @return Value& 节点中存储的值的引用
         */
        Value& operator*() const noexcept { return node_->value; }
        
        /**
         * @brief Arrow operator
         * @brief 箭头操作符
         * 
         * @return Value* Pointer to the value stored in the node
         * @return Value* 指向节点中存储的值的指针
         */
        Value* operator->() const noexcept { return &node_->value; }
        
        /**
         * @brief Pre-increment operator (move to previous node in in-order traversal)
         * @brief 前自增操作符（移动到中序遍历的前一个节点）
         * 
         * @return reverse_iterator& Reference to this iterator
         * @return reverse_iterator& 此迭代器的引用
         * 
         * @note Time complexity: O(log n) in worst case, O(1) amortized
         * @note 时间复杂度：最坏情况O(log n)，摊还O(1)
         */
        reverse_iterator& operator++() noexcept {
            if (node_ != nullptr) {
                base_ptr next = node_->predecessor();
                if (next == nullptr || detail::rb_tree_nil_node::is_nil(next)) {
                    node_ = nullptr;  // 到达结束
                } else {
                    node_ = static_cast<node_ptr>(next);
                }
            }
            return *this;
        }
        
        reverse_iterator operator++(int) noexcept {
            reverse_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        /**
         * @brief Pre-decrement operator (move to next node in reverse in-order traversal)
         * @brief 前自减操作符（移动到逆中序遍历的下一个节点）
         * 
         * @return reverse_iterator& Reference to this iterator
         * @return reverse_iterator& 此迭代器的引用
         */
        reverse_iterator& operator--() noexcept {
            if (node_ != nullptr) {
                // 使用节点的successor方法
                base_ptr next = node_->successor();
                if (next == nullptr || detail::rb_tree_nil_node::is_nil(next)) {
                    node_ = nullptr;  // 到达结束
                } else {
                    node_ = static_cast<node_ptr>(next);
                }
            }
            return *this;
        }
        
        reverse_iterator operator--(int) noexcept {
            reverse_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const reverse_iterator& other) const noexcept {
            return node_ == other.node_;
        }
        
        bool operator!=(const reverse_iterator& other) const noexcept {
            return node_ != other.node_;
        }
        
        /**
         * @brief Convert to forward iterator
         * @brief 转换为正向迭代器
         * 
         * @return iterator Forward iterator
         * @return iterator 正向迭代器
         */
        iterator base() const noexcept {
            return iterator(node_);
        }
    };
    
    struct const_reverse_iterator {
        const node_ptr node_;
        
        const_reverse_iterator() noexcept : node_(nullptr) {}
        explicit const_reverse_iterator(node_ptr node) noexcept : node_(node) {}
        const_reverse_iterator(const reverse_iterator& it) noexcept : node_(it.node_) {}
        const_reverse_iterator(const const_iterator& it) noexcept : node_(it.node_) {}
        
        const Value& operator*() const noexcept { return node_->value; }
        const Value* operator->() const noexcept { return &node_->value; }
        
        const_reverse_iterator& operator++() noexcept {
            if (node_ != nullptr) {
                base_ptr next = node_->predecessor();
                if (next == nullptr || detail::rb_tree_nil_node::is_nil(next)) {
                    const_cast<node_ptr&>(node_) = nullptr;  // 到达结束
                } else {
                    const_cast<node_ptr&>(node_) = static_cast<node_ptr>(next);
                }
            }
            return *this;
        }
        
        const_reverse_iterator operator++(int) noexcept {
            const_reverse_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        const_reverse_iterator& operator--() noexcept {
            if (node_ != nullptr) {
                // 使用节点的successor方法
                base_ptr next = node_->successor();
                if (next == nullptr || detail::rb_tree_nil_node::is_nil(next)) {
                    const_cast<node_ptr&>(node_) = nullptr;  // 到达结束
                } else {
                    const_cast<node_ptr&>(node_) = static_cast<node_ptr>(next);
                }
            }
            return *this;
        }
        
        const_reverse_iterator operator--(int) noexcept {
            const_reverse_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const const_reverse_iterator& other) const noexcept {
            return node_ == other.node_;
        }
        
        bool operator!=(const const_reverse_iterator& other) const noexcept {
            return node_ != other.node_;
        }
        
        /**
         * @brief Convert to const forward iterator
         * @brief 转换为常量正向迭代器
         * 
         * @return const_iterator Const forward iterator
         * @return const_iterator 常量正向迭代器
         */
        const_iterator base() const noexcept {
            return const_iterator(node_);
        }
    };
    
private:
    node_allocator alloc_;
    Compare comp_;
    KeyOfValue key_of_;
    base_ptr root_;
    base_ptr nil_;
    std::size_t size_;
    
    base_ptr get_nil() const noexcept {
        return detail::rb_tree_nil_node::instance();
    }
    
    node_ptr create_node(const Value& value, color_type color = detail::RED) {
        node_ptr node = node_type::allocate(alloc_);
        try {
            node_type::construct(alloc_, node, color, value);
        } catch (...) {
            node_type::deallocate(alloc_, node);
            throw;
        }
        return node;
    }
    
    node_ptr create_node(Value&& value, color_type color = detail::RED) {
        node_ptr node = node_type::allocate(alloc_);
        try {
            node_type::construct(alloc_, node, color, mystl::move(value));
        } catch (...) {
            node_type::deallocate(alloc_, node);
            throw;
        }
        return node;
    }
    
    void destroy_node(node_ptr node) {
        node_type::destroy(alloc_, node);
        node_type::deallocate(alloc_, node);
    }
    
    void rotate_left(base_ptr x) {
        base_ptr y = x->right;
        x->right = y->left;
        
        if (y->left != nil_) {
            y->left->parent = x;
        }
        
        y->parent = x->parent;
        
        if (x == root_) {
            root_ = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
    }
    
    void rotate_right(base_ptr x) {
        base_ptr y = x->left;
        x->left = y->right;
        
        if (y->right != nil_) {
            y->right->parent = x;
        }
        
        y->parent = x->parent;
        
        if (x == root_) {
            root_ = y;
        } else if (x == x->parent->right) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }
        
        y->right = x;
        x->parent = y;
    }
    
    void insert_fixup(base_ptr z) {
        while (z != root_ && z->parent->color == detail::RED) {
            if (z->parent == z->parent->parent->left) {
                base_ptr y = z->parent->parent->right;
                if (y->color == detail::RED) {
                    z->parent->color = detail::BLACK;
                    y->color = detail::BLACK;
                    z->parent->parent->color = detail::RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotate_left(z);
                    }
                    z->parent->color = detail::BLACK;
                    z->parent->parent->color = detail::RED;
                    rotate_right(z->parent->parent);
                }
            } else {
                base_ptr y = z->parent->parent->left;
                if (y->color == detail::RED) {
                    z->parent->color = detail::BLACK;
                    y->color = detail::BLACK;
                    z->parent->parent->color = detail::RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotate_right(z);
                    }
                    z->parent->color = detail::BLACK;
                    z->parent->parent->color = detail::RED;
                    rotate_left(z->parent->parent);
                }
            }
        }
        root_->color = detail::BLACK;
    }
    
    void transplant(base_ptr u, base_ptr v) {
        if (u->parent == nil_) {
            root_ = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }
    
    base_ptr minimum(base_ptr x) const {
        while (x->left != nil_) {
            x = x->left;
        }
        return x;
    }
    
    base_ptr maximum(base_ptr x) const {
        while (x->right != nil_) {
            x = x->right;
        }
        return x;
    }
    
    void erase_fixup(base_ptr x) {
        while (x != root_ && x->color == detail::BLACK) {
            if (x == x->parent->left) {
                base_ptr w = x->parent->right;
                if (w->color == detail::RED) {
                    w->color = detail::BLACK;
                    x->parent->color = detail::RED;
                    rotate_left(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == detail::BLACK && w->right->color == detail::BLACK) {
                    w->color = detail::RED;
                    x = x->parent;
                } else {
                    if (w->right->color == detail::BLACK) {
                        w->left->color = detail::BLACK;
                        w->color = detail::RED;
                        rotate_right(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = detail::BLACK;
                    w->right->color = detail::BLACK;
                    rotate_left(x->parent);
                    x = root_;
                }
            } else {
                base_ptr w = x->parent->left;
                if (w->color == detail::RED) {
                    w->color = detail::BLACK;
                    x->parent->color = detail::RED;
                    rotate_right(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == detail::BLACK && w->left->color == detail::BLACK) {
                    w->color = detail::RED;
                    x = x->parent;
                } else {
                    if (w->left->color == detail::BLACK) {
                        w->right->color = detail::BLACK;
                        w->color = detail::RED;
                        rotate_left(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = detail::BLACK;
                    w->left->color = detail::BLACK;
                    rotate_right(x->parent);
                    x = root_;
                }
            }
        }
        x->color = detail::BLACK;
    }
    
    void clear_subtree(base_ptr node) {
        if (node != nil_) {
            clear_subtree(node->left);
            clear_subtree(node->right);
            destroy_node(static_cast<node_ptr>(node));
        }
    }
    
public:
    using key_type = Key;
    using value_type = Value;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename allocator_traits<allocator_type>::pointer;
    using const_pointer = typename allocator_traits<allocator_type>::const_pointer;
    using iterator = iterator;
    using const_iterator = const_iterator;
    using reverse_iterator = reverse_iterator;
    using const_reverse_iterator = const_reverse_iterator;
    
    rb_tree() : rb_tree(Compare()) {}
    
    explicit rb_tree(const Compare& comp, const Alloc& alloc = Alloc())
        : alloc_(alloc), comp_(comp), key_of_(), root_(get_nil()), nil_(get_nil()), size_(0) {}
    
    ~rb_tree() {
        clear();
    }
    
    rb_tree(const rb_tree& other)
        : alloc_(other.alloc_), comp_(other.comp_), key_of_(other.key_of_),
          root_(get_nil()), nil_(get_nil()), size_(0) {
        // 深拷贝所有节点
        for (auto it = other.begin(); it != other.end(); ++it) {
            insert(*it);
        }
    }
    
    rb_tree& operator=(const rb_tree& other) {
        if (this != &other) {
            clear();
            alloc_ = other.alloc_;
            comp_ = other.comp_;
            key_of_ = other.key_of_;
            // 深拷贝所有节点
            for (auto it = other.begin(); it != other.end(); ++it) {
                insert(*it);
            }
        }
        return *this;
    }
    
    rb_tree(rb_tree&& other) noexcept
        : alloc_(mystl::move(other.alloc_)),
          comp_(mystl::move(other.comp_)),
          key_of_(mystl::move(other.key_of_)),
          root_(other.root_),
          nil_(other.nil_),
          size_(other.size_) {
        other.root_ = nil_;
        other.size_ = 0;
    }
    
    rb_tree& operator=(rb_tree&& other) noexcept {
        if (this != &other) {
            clear();
            alloc_ = mystl::move(other.alloc_);
            comp_ = mystl::move(other.comp_);
            key_of_ = mystl::move(other.key_of_);
            root_ = other.root_;
            nil_ = other.nil_;
            size_ = other.size_;
            other.root_ = nil_;
            other.size_ = 0;
        }
        return *this;
    }
    
    bool empty() const noexcept { return size_ == 0; }
    size_type size() const noexcept { return size_; }
    
    iterator begin() noexcept {
        if (root_ == nil_) return end();
        return iterator(static_cast<node_ptr>(minimum(root_)));
    }
    
    const_iterator begin() const noexcept {
        if (root_ == nil_) return end();
        return const_iterator(static_cast<node_ptr>(minimum(root_)));
    }
    
    iterator end() noexcept {
        return iterator(nullptr);  // 使用nullptr表示结束
    }
    
    const_iterator end() const noexcept {
        return const_iterator(nullptr);  // 使用nullptr表示结束
    }
    
    reverse_iterator rbegin() noexcept {
        if (root_ == nil_) return rend();
        return reverse_iterator(static_cast<node_ptr>(maximum(root_)));
    }
    
    const_reverse_iterator rbegin() const noexcept {
        if (root_ == nil_) return rend();
        return const_reverse_iterator(static_cast<node_ptr>(maximum(root_)));
    }
    
    reverse_iterator rend() noexcept {
        return reverse_iterator(nullptr);  // 使用nullptr表示结束
    }
    
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(nullptr);  // 使用nullptr表示结束
    }
    
    pair<iterator, bool> insert(const value_type& value) {
        base_ptr y = nil_;
        base_ptr x = root_;
        
        while (x != nil_) {
            y = x;
            if (comp_(key_of_(value), key_of_(static_cast<node_ptr>(x)->value))) {
                x = x->left;
            } else if (comp_(key_of_(static_cast<node_ptr>(x)->value), key_of_(value))) {
                x = x->right;
            } else {
                // 如果允许重复，继续向右子树查找
                if (AllowDuplicates) {
                    x = x->right;
                } else {
                    return {iterator(static_cast<node_ptr>(x)), false};
                }
            }
        }
        
        node_ptr z = create_node(value);
        z->parent = y;
        
        if (y == nil_) {
            root_ = z;
        } else if (comp_(key_of_(value), key_of_(static_cast<node_ptr>(y)->value))) {
            y->left = z;
        } else {
            y->right = z;
        }
        
        z->left = nil_;
        z->right = nil_;
        z->color = detail::RED;
        
        insert_fixup(z);
        ++size_;
        return {iterator(z), true};
    }
    
    pair<iterator, bool> insert(value_type&& value) {
        base_ptr y = nil_;
        base_ptr x = root_;
        
        while (x != nil_) {
            y = x;
            if (comp_(key_of_(value), key_of_(static_cast<node_ptr>(x)->value))) {
                x = x->left;
            } else if (comp_(key_of_(static_cast<node_ptr>(x)->value), key_of_(value))) {
                x = x->right;
            } else {
                // 如果允许重复，继续向右子树查找
                if (AllowDuplicates) {
                    x = x->right;
                } else {
                    return {iterator(static_cast<node_ptr>(x)), false};
                }
            }
        }
        
        node_ptr z = create_node(mystl::move(value));
        z->parent = y;
        
        if (y == nil_) {
            root_ = z;
        } else if (comp_(key_of_(z->value), key_of_(static_cast<node_ptr>(y)->value))) {
            y->left = z;
        } else {
            y->right = z;
        }
        
        z->left = nil_;
        z->right = nil_;
        z->color = detail::RED;
        
        insert_fixup(z);
        ++size_;
        return {iterator(z), true};
    }
    
    /**
     * @brief Insert element with hint
     * @brief 带提示插入元素
     * 
     * @param hint Hint for the position where the element can be inserted
     * @param hint 元素可以插入位置的提示
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return iterator Iterator to the inserted element or to the element that prevented insertion
     * @return iterator 指向插入元素的迭代器，或指向阻止插入的元素的迭代器
     * 
     * @note The hint is used as a starting point for the search, which can improve
     *       performance if the hint is correct. If the hint is wrong, the performance
     *       is the same as the regular insert.
     * @note 提示用作搜索的起点，如果提示正确可以提高性能。如果提示错误，性能与常规插入相同。
     */
    iterator insert(const_iterator hint, const value_type& value) {
        // 如果提示是end()，从根开始搜索
        if (hint.node_ == nullptr) {
            return insert(value).first;
        }
        
        // 获取提示节点的键
        const key_type& hint_key = key_of_(*hint);
        const key_type& new_key = key_of_(value);
        
        // 检查提示是否有效
        // 情况1：新键小于提示键，检查提示是否是第一个不小于新键的元素
        if (comp_(new_key, hint_key)) {
            // 获取提示的前驱
            auto prev = const_iterator(static_cast<node_ptr>(hint.node_->predecessor()));
            if (prev.node_ == nullptr || comp_(key_of_(*prev), new_key)) {
                // 提示是第一个不小于新键的元素，从提示开始搜索
                return insert_impl(hint.node_, value).first;
            }
        }
        // 情况2：新键大于提示键，检查提示是否是最后一个不大于新键的元素
        else if (comp_(hint_key, new_key)) {
            // 获取提示的后继
            auto next = const_iterator(static_cast<node_ptr>(hint.node_->successor()));
            if (next.node_ == nullptr || comp_(new_key, key_of_(*next))) {
                // 提示是最后一个不大于新键的元素，从提示开始搜索
                return insert_impl(hint.node_, value).first;
            }
        }
        // 情况3：键相等
        else {
            // 如果允许重复，从提示开始搜索
            if (AllowDuplicates) {
                return insert_impl(hint.node_, value).first;
            } else {
                // 不允许重复，返回现有元素
                return iterator(const_cast<node_ptr>(hint.node_));
            }
        }
        
        // 提示无效，使用常规插入
        return insert(value).first;
    }
    
    /**
     * @brief Insert element with hint (move version)
     * @brief 带提示插入元素（移动版本）
     * 
     * @param hint Hint for the position where the element can be inserted
     * @param hint 元素可以插入位置的提示
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return iterator Iterator to the inserted element or to the element that prevented insertion
     * @return iterator 指向插入元素的迭代器，或指向阻止插入的元素的迭代器
     */
    iterator insert(const_iterator hint, value_type&& value) {
        // 如果提示是end()，从根开始搜索
        if (hint.node_ == nullptr) {
            return insert(mystl::move(value)).first;
        }
        
        // 获取提示节点的键
        const key_type& hint_key = key_of_(*hint);
        const key_type& new_key = key_of_(value);
        
        // 检查提示是否有效
        // 情况1：新键小于提示键，检查提示是否是第一个不小于新键的元素
        if (comp_(new_key, hint_key)) {
            // 获取提示的前驱
            auto prev = const_iterator(static_cast<node_ptr>(hint.node_->predecessor()));
            if (prev.node_ == nullptr || comp_(key_of_(*prev), new_key)) {
                // 提示是第一个不小于新键的元素，从提示开始搜索
                return insert_impl(hint.node_, mystl::move(value)).first;
            }
        }
        // 情况2：新键大于提示键，检查提示是否是最后一个不大于新键的元素
        else if (comp_(hint_key, new_key)) {
            // 获取提示的后继
            auto next = const_iterator(static_cast<node_ptr>(hint.node_->successor()));
            if (next.node_ == nullptr || comp_(new_key, key_of_(*next))) {
                // 提示是最后一个不大于新键的元素，从提示开始搜索
                return insert_impl(hint.node_, mystl::move(value)).first;
            }
        }
        // 情况3：键相等
        else {
            // 如果允许重复，从提示开始搜索
            if (AllowDuplicates) {
                return insert_impl(hint.node_, mystl::move(value)).first;
            } else {
                // 不允许重复，返回现有元素
                return iterator(const_cast<node_ptr>(hint.node_));
            }
        }
        
        // 提示无效，使用常规插入
        return insert(mystl::move(value)).first;
    }
    
private:
    /**
     * @brief Internal implementation of insert starting from a specific node
     * @brief 从特定节点开始插入的内部实现
     * 
     * @param start Starting node for the search
     * @param start 搜索的起始节点
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return pair<iterator, bool> Iterator to the inserted element and bool indicating success
     * @return pair<iterator, bool> 指向插入元素的迭代器和表示成功的布尔值
     */
    pair<iterator, bool> insert_impl(base_ptr start, const value_type& value) {
        base_ptr y = nil_;
        base_ptr x = start;
        
        // 从起始节点开始搜索
        while (x != nil_) {
            y = x;
            if (comp_(key_of_(value), key_of_(static_cast<node_ptr>(x)->value))) {
                x = x->left;
            } else if (comp_(key_of_(static_cast<node_ptr>(x)->value), key_of_(value))) {
                x = x->right;
            } else {
                // 如果允许重复，继续向右子树查找
                if (AllowDuplicates) {
                    x = x->right;
                } else {
                    return {iterator(static_cast<node_ptr>(x)), false};
                }
            }
        }
        
        node_ptr z = create_node(value);
        z->parent = y;
        
        if (y == nil_) {
            root_ = z;
        } else if (comp_(key_of_(value), key_of_(static_cast<node_ptr>(y)->value))) {
            y->left = z;
        } else {
            y->right = z;
        }
        
        z->left = nil_;
        z->right = nil_;
        z->color = detail::RED;
        
        insert_fixup(z);
        ++size_;
        return {iterator(z), true};
    }
    
    /**
     * @brief Internal implementation of insert starting from a specific node (move version)
     * @brief 从特定节点开始插入的内部实现（移动版本）
     * 
     * @param start Starting node for the search
     * @param start 搜索的起始节点
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return pair<iterator, bool> Iterator to the inserted element and bool indicating success
     * @return pair<iterator, bool> 指向插入元素的迭代器和表示成功的布尔值
     */
    pair<iterator, bool> insert_impl(base_ptr start, value_type&& value) {
        base_ptr y = nil_;
        base_ptr x = start;
        
        // 从起始节点开始搜索
        while (x != nil_) {
            y = x;
            if (comp_(key_of_(value), key_of_(static_cast<node_ptr>(x)->value))) {
                x = x->left;
            } else if (comp_(key_of_(static_cast<node_ptr>(x)->value), key_of_(value))) {
                x = x->right;
            } else {
                // 如果允许重复，继续向右子树查找
                if (AllowDuplicates) {
                    x = x->right;
                } else {
                    return {iterator(static_cast<node_ptr>(x)), false};
                }
            }
        }
        
        node_ptr z = create_node(mystl::move(value));
        z->parent = y;
        
        if (y == nil_) {
            root_ = z;
        } else if (comp_(key_of_(z->value), key_of_(static_cast<node_ptr>(y)->value))) {
            y->left = z;
        } else {
            y->right = z;
        }
        
        z->left = nil_;
        z->right = nil_;
        z->color = detail::RED;
        
        insert_fixup(z);
        ++size_;
        return {iterator(z), true};
    }
    
public:
    
    iterator find(const key_type& key) {
        base_ptr x = root_;
        while (x != nil_) {
            if (comp_(key, key_of_(static_cast<node_ptr>(x)->value))) {
                x = x->left;
            } else if (comp_(key_of_(static_cast<node_ptr>(x)->value), key)) {
                x = x->right;
            } else {
                return iterator(static_cast<node_ptr>(x));
            }
        }
        return end();
    }
    
    const_iterator find(const key_type& key) const {
        base_ptr x = root_;
        while (x != nil_) {
            if (comp_(key, key_of_(static_cast<node_ptr>(x)->value))) {
                x = x->left;
            } else if (comp_(key_of_(static_cast<node_ptr>(x)->value), key)) {
                x = x->right;
            } else {
                return const_iterator(static_cast<node_ptr>(x));
            }
        }
        return end();
    }
    
    size_type count(const key_type& key) const {
        if (!AllowDuplicates) {
            // 对于set，最多有一个元素
            return find(key) != end() ? 1 : 0;
        } else {
            // 对于multiset，需要计算所有等于key的元素
            auto range = equal_range(key);
            size_type cnt = 0;
            for (auto it = range.first; it != range.second; ++it) {
                ++cnt;
            }
            return cnt;
        }
    }
    
    size_type erase(const key_type& key) {
        if (!AllowDuplicates) {
            // 对于set，最多删除一个元素
            iterator it = find(key);
            if (it == end()) {
                return 0;
            }
            erase(it);
            return 1;
        } else {
            // 对于multiset，删除所有等于key的元素
            size_type count = 0;
            auto range = equal_range(key);
            while (range.first != range.second) {
                erase(range.first);
                ++count;
                range = equal_range(key);  // 重新获取范围，因为迭代器失效了
            }
            return count;
        }
    }
    
    iterator erase(iterator first, iterator last) {
        // 保存下一个迭代器，因为erase会使当前迭代器失效
        while (first != last) {
            iterator next = first;
            ++next;
            erase(first);
            first = next;
        }
        return last;
    }
    
    iterator erase(const_iterator pos) {
        return erase(iterator(const_cast<node_ptr>(pos.node_)));
    }
    
    iterator erase(const_iterator first, const_iterator last) {
        return erase(iterator(const_cast<node_ptr>(first.node_)), 
                     iterator(const_cast<node_ptr>(last.node_)));
    }
    
    iterator lower_bound(const key_type& key) {
        base_ptr x = root_;
        base_ptr y = nil_;
        
        while (x != nil_) {
            if (!comp_(key_of_(static_cast<node_ptr>(x)->value), key)) {
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        
        if (y == nil_) {
            return end();
        }
        return iterator(static_cast<node_ptr>(y));
    }
    
    const_iterator lower_bound(const key_type& key) const {
        base_ptr x = root_;
        base_ptr y = nil_;
        
        while (x != nil_) {
            if (!comp_(key_of_(static_cast<node_ptr>(x)->value), key)) {
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        
        if (y == nil_) {
            return end();
        }
        return const_iterator(static_cast<node_ptr>(y));
    }
    
    iterator upper_bound(const key_type& key) {
        base_ptr x = root_;
        base_ptr y = nil_;
        
        while (x != nil_) {
            if (comp_(key, key_of_(static_cast<node_ptr>(x)->value))) {
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        
        if (y == nil_) {
            return end();
        }
        return iterator(static_cast<node_ptr>(y));
    }
    
    const_iterator upper_bound(const key_type& key) const {
        base_ptr x = root_;
        base_ptr y = nil_;
        
        while (x != nil_) {
            if (comp_(key, key_of_(static_cast<node_ptr>(x)->value))) {
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        
        if (y == nil_) {
            return end();
        }
        return const_iterator(static_cast<node_ptr>(y));
    }
    
    pair<iterator, iterator> equal_range(const key_type& key) {
        return {lower_bound(key), upper_bound(key)};
    }
    
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return {lower_bound(key), upper_bound(key)};
    }
    
    iterator erase(iterator pos) {
        if (pos == end()) {
            return end();
        }
        
        node_ptr z = pos.node_;
        base_ptr y = z;
        base_ptr x;
        color_type y_original_color = y->color;
        
        if (z->left == nil_) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == nil_) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            
            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        
        destroy_node(z);
        --size_;
        
        if (y_original_color == detail::BLACK) {
            erase_fixup(x);
        }
        
        return iterator(static_cast<node_ptr>(minimum(root_)));
    }
    
    void clear() {
        clear_subtree(root_);
        root_ = nil_;
        size_ = 0;
    }
    
    void swap(rb_tree& other) noexcept {
        mystl::swap(alloc_, other.alloc_);
        mystl::swap(comp_, other.comp_);
        mystl::swap(key_of_, other.key_of_);
        mystl::swap(root_, other.root_);
        mystl::swap(nil_, other.nil_);
        mystl::swap(size_, other.size_);
    }
    
    Compare key_comp() const { return comp_; }
    allocator_type get_allocator() const { return allocator_type(alloc_); }
    
    // 比较操作符
    bool operator==(const rb_tree& other) const {
        if (size_ != other.size_) return false;
        auto it1 = begin();
        auto it2 = other.begin();
        while (it1 != end()) {
            if (!(*it1 == *it2)) return false;
            ++it1;
            ++it2;
        }
        return true;
    }
    
    bool operator!=(const rb_tree& other) const {
        return !(*this == other);
    }
    
    bool operator<(const rb_tree& other) const {
        return mystl::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }
    
    bool operator<=(const rb_tree& other) const {
        return !(other < *this);
    }
    
    bool operator>(const rb_tree& other) const {
        return other < *this;
    }
    
    bool operator>=(const rb_tree& other) const {
        return !(*this < other);
    }
};

// 非成员函数swap
template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& lhs,
          rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace mystl
