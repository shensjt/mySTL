#pragma once

/**
 * @file rb_tree_node.hpp
 * @brief Red-black tree node implementation for mySTL
 * @brief mySTL红黑树节点实现
 * 
 * This file defines the node structure for red-black trees, which are used
 * as the underlying data structure for associative containers (set, map, etc.).
 * 
 * 本文件定义红黑树的节点结构，红黑树用作关联容器（set、map等）的底层数据结构。
 * 
 * Red-black tree properties (五条规则):
 * 1. Every node is either red or black.
 * 2. The root is black.
 * 3. All leaves (NIL nodes) are black.
 * 4. If a red node has children, they must be black (no two red nodes adjacent).
 * 5. Every path from a node to its descendant leaves contains the same number of black nodes.
 * 
 * 红黑树特性（五条规则）：
 * 1. 每个节点是红色或黑色
 * 2. 根节点是黑色
 * 3. 所有叶子节点（NIL节点）是黑色
 * 4. 红色节点的两个子节点都是黑色（不能有连续的红色节点）
 * 5. 从任一节点到其每个叶子节点的所有路径都包含相同数目的黑色节点
 * 
 * @author shensjt
 * @date 2026-03-29
 * 
 * @see include/mystl/rb_tree.hpp
 * @see doc/规划/红黑树实现思路步骤划分.md
 */

#include "../../memory/allocator.hpp"
#include "../../utility/type_traits.hpp"
#include "../../utility/pair.hpp"

namespace mystl {
namespace detail {

// 前向声明NIL节点类
class rb_tree_nil_node;

// ============================================================================
// 颜色枚举定义
// ============================================================================

/**
 * @brief Red-black tree color enumeration
 * @brief 红黑树颜色枚举
 * 
 * 使用bool类型表示颜色，true表示黑色，false表示红色。
 * 这种表示方式可以节省内存（使用位域压缩）。
 * 
 * Using bool type for color, true for black, false for red.
 * This representation saves memory (using bit-field compression).
 */
enum rb_tree_color {
    RED = false,    ///< 红色节点 / Red node
    BLACK = true    ///< 黑色节点 / Black node
};

// ============================================================================
// 红黑树节点结构
// ============================================================================

/**
 * @brief Red-black tree node base structure
 * @brief 红黑树节点基类结构
 * 
 * 包含所有红黑树节点共有的成员（颜色、指针），但不包含值。
 * 用于实现NIL节点和作为值节点的基类。
 * 
 * Contains common members for all red-black tree nodes (color, pointers),
 * but does not contain the value. Used for NIL nodes and as base class for value nodes.
 */
struct rb_tree_node_base {
    using color_type = rb_tree_color;
    using base_ptr = rb_tree_node_base*;
    
    color_type color;   ///< 节点颜色 / Node color
    base_ptr parent;    ///< 父节点指针 / Parent node pointer
    base_ptr left;      ///< 左子节点指针 / Left child pointer
    base_ptr right;     ///< 右子节点指针 / Right child pointer
    
    /**
     * @brief 构造函数
     * @param col 节点颜色，默认为红色 / Node color, default is RED
     */
    explicit rb_tree_node_base(color_type col = RED) noexcept
        : color(col), parent(nullptr), left(nullptr), right(nullptr) {}
    
    /**
     * @brief 虚析构函数
     * 
     * 确保派生类正确析构。
     * Ensures proper destruction of derived classes.
     */
    virtual ~rb_tree_node_base() = default;
    
    // ==================== 节点关系操作 ====================
    
    /**
     * @brief 获取最小节点（最左节点）
     * @return 以当前节点为根的子树中的最小节点
     * 
     * 沿着左子树一直向下查找。
     * Traverse down the left subtree.
     */
    base_ptr minimum() noexcept;
    
    /**
     * @brief 获取最大节点（最右节点）
     * @return 以当前节点为根的子树中的最大节点
     * 
     * 沿着右子树一直向下查找。
     * Traverse down the right subtree.
     */
    base_ptr maximum() noexcept;
    
    /**
     * @brief 获取前驱节点（中序遍历的前一个节点）
     * @return 前驱节点，如果没有则返回nullptr
     * 
     * 算法：
     * 1. 如果有左子树，返回左子树的最大节点
     * 2. 否则，向上查找直到当前节点是父节点的右子节点
     * 
     * Algorithm:
     * 1. If has left child, return maximum of left subtree
     * 2. Otherwise, go up until current node is right child of its parent
     */
    base_ptr predecessor() noexcept;
    
    /**
     * @brief 获取后继节点（中序遍历的后一个节点）
     * @return 后继节点，如果没有则返回nullptr
     * 
     * 算法：
     * 1. 如果有右子树，返回右子树的最小节点
     * 2. 否则，向上查找直到当前节点是父节点的左子节点
     * 
     * Algorithm:
     * 1. If has right child, return minimum of right subtree
     * 2. Otherwise, go up until current node is left child of its parent
     */
    base_ptr successor() noexcept;
    
    // ==================== 节点状态检查 ====================
    
    /**
     * @brief 检查节点是否是左子节点
     * @return 如果是左子节点返回true，否则返回false
     */
    bool is_left_child() const noexcept {
        return parent != nullptr && this == parent->left;
    }
    
    /**
     * @brief 检查节点是否是右子节点
     * @return 如果是右子节点返回true，否则返回false
     */
    bool is_right_child() const noexcept {
        return parent != nullptr && this == parent->right;
    }
    
    /**
     * @brief 检查节点是否是根节点
     * @return 如果是根节点返回true，否则返回false
     */
    bool is_root() const noexcept {
        return parent == nullptr;
    }
    
    /**
     * @brief 检查节点是否是叶子节点（NIL节点）
     * @return 如果是叶子节点返回true，否则返回false
     * 
     * 注意：在红黑树中，真正的叶子节点是NIL节点。
     * 这里检查的是逻辑上的叶子节点（两个子节点都是nullptr）。
     */
    bool is_leaf() const noexcept {
        return left == nullptr && right == nullptr;
    }
    
    /**
     * @brief 获取兄弟节点
     * @return 兄弟节点，如果没有则返回nullptr
     */
    base_ptr sibling() const noexcept {
        if (parent == nullptr) {
            return nullptr;
        }
        return is_left_child() ? parent->right : parent->left;
    }
    
    /**
     * @brief 获取叔节点（父节点的兄弟节点）
     * @return 叔节点，如果没有则返回nullptr
     */
    base_ptr uncle() const noexcept {
        if (parent == nullptr || parent->parent == nullptr) {
            return nullptr;
        }
        return parent->sibling();
    }
    
    // ==================== 静态辅助函数 ====================
    
    /**
     * @brief 获取节点的颜色（安全处理nullptr）
     * @param node 节点指针，可以是nullptr
     * @return 节点颜色，如果节点是nullptr则返回BLACK（NIL节点是黑色）
     */
    static color_type get_color(base_ptr node) noexcept {
        return node == nullptr ? BLACK : node->color;
    }
    
    /**
     * @brief 设置节点颜色（安全处理nullptr）
     * @param node 节点指针
     * @param col 要设置的颜色
     * 
     * 如果node是nullptr，则什么也不做（NIL节点总是黑色）。
     */
    static void set_color(base_ptr node, color_type col) noexcept {
        if (node != nullptr) {
            node->color = col;
        }
    }
    
    /**
     * @brief 交换两个节点的颜色
     * @param a 第一个节点
     * @param b 第二个节点
     */
    static void swap_color(base_ptr a, base_ptr b) noexcept {
        if (a != nullptr && b != nullptr) {
            color_type temp = a->color;
            a->color = b->color;
            b->color = temp;
        }
    }
};

// ============================================================================
// 红黑树值节点结构
// ============================================================================

/**
 * @brief Red-black tree value node structure
 * @brief 红黑树值节点结构
 * 
 * 继承自rb_tree_node_base，添加值存储。
 * 用于存储实际的数据。
 * 
 * Inherits from rb_tree_node_base, adds value storage.
 * Used to store actual data.
 * 
 * @tparam Value 存储的值类型
 */
template<typename Value>
struct rb_tree_node : public rb_tree_node_base {
    using value_type = Value;
    using node_ptr = rb_tree_node<value_type>*;
    using base_ptr = rb_tree_node_base*;
    
    value_type value;   ///< 存储的值 / Stored value
    
    /**
     * @brief 默认构造函数
     * 
     * 值被默认构造，颜色为红色。
     */
    rb_tree_node() noexcept(noexcept(value_type()))
        : rb_tree_node_base(RED), value() {}
    
    /**
     * @brief 值构造函数
     * @param val 要存储的值
     * @param col 节点颜色，默认为红色
     */
    explicit rb_tree_node(const value_type& val, color_type col = RED)
        : rb_tree_node_base(col), value(val) {}
    
    /**
     * @brief 移动值构造函数
     * @param val 要移动的值
     * @param col 节点颜色，默认为红色
     */
    explicit rb_tree_node(value_type&& val, color_type col = RED)
        : rb_tree_node_base(col), value(mystl::move(val)) {}
    
    /**
     * @brief 完美转发构造函数
     * @tparam Args 参数类型
     * @param args 构造值的参数
     * @param col 节点颜色，默认为红色
     * 
     * 使用原位构造（emplace）方式构造值。
     */
    template<typename... Args>
    explicit rb_tree_node(color_type col, Args&&... args)
        : rb_tree_node_base(col), value(mystl::forward<Args>(args)...) {}
    
    /**
     * @brief 析构函数
     * 
     * 确保值被正确销毁。
     */
    ~rb_tree_node() = default;
    
    // ==================== 指针转换函数 ====================
    
    /**
     * @brief 从基类指针转换为派生类指针
     * @param p 基类指针
     * @return 派生类指针
     * 
     * 安全转换，假设p指向rb_tree_node<value_type>对象。
     */
    static node_ptr from_base(base_ptr p) noexcept {
        return static_cast<node_ptr>(p);
    }
    
    /**
     * @brief 从派生类指针转换为基类指针
     * @param p 派生类指针
     * @return 基类指针
     */
    static base_ptr to_base(node_ptr p) noexcept {
        return static_cast<base_ptr>(p);
    }
    
    /**
     * @brief 获取值的指针
     * @return 指向值的指针
     */
    value_type* valptr() noexcept {
        return &value;
    }
    
    /**
     * @brief 获取值的常量指针
     * @return 指向值的常量指针
     */
    const value_type* valptr() const noexcept {
        return &value;
    }
    
    // ==================== 节点分配和释放 ====================
    
    /**
     * @brief 分配一个节点
     * @tparam Alloc 分配器类型
     * @param alloc 分配器实例
     * @return 分配的节点指针
     */
    template<typename Alloc>
    static node_ptr allocate(Alloc& alloc) {
        return alloc.allocate(1);
    }
    
    /**
     * @brief 释放一个节点
     * @tparam Alloc 分配器类型
     * @param alloc 分配器实例
     * @param p 要释放的节点指针
     */
    template<typename Alloc>
    static void deallocate(Alloc& alloc, node_ptr p) {
        alloc.deallocate(p, 1);
    }
    
    /**
     * @brief 构造一个节点
     * @tparam Alloc 分配器类型
     * @param alloc 分配器实例
     * @param p 要构造节点的内存位置
     * @param args 构造节点的参数
     */
    template<typename Alloc, typename... Args>
    static void construct(Alloc& alloc, node_ptr p, Args&&... args) {
        alloc.construct(p, mystl::forward<Args>(args)...);
    }
    
    /**
     * @brief 销毁一个节点
     * @tparam Alloc 分配器类型
     * @param alloc 分配器实例
     * @param p 要销毁的节点指针
     */
    template<typename Alloc>
    static void destroy(Alloc& alloc, node_ptr p) {
        alloc.destroy(p);
    }
};

// ============================================================================
// NIL节点实现
// ============================================================================

/**
 * @brief NIL节点单例类
 * @brief NIL node singleton class
 * 
 * 红黑树中所有叶子节点都指向同一个NIL节点。
 * NIL节点是黑色的，简化边界条件处理。
 * 
 * All leaf nodes in a red-black tree point to the same NIL node.
 * NIL node is black, simplifying boundary condition handling.
 */
class rb_tree_nil_node : public rb_tree_node_base {
private:
    /**
     * @brief 私有构造函数
     * 
     * 确保只能通过instance()方法获取实例。
     */
    rb_tree_nil_node() noexcept : rb_tree_node_base(BLACK) {
        // NIL节点的父节点、左子节点、右子节点都指向自身
        // 形成一个自引用的结构，简化边界检查
        parent = this;
        left = this;
        right = this;
    }
    
    /**
     * @brief 私有析构函数
     * 
     * NIL节点在程序结束时自动销毁。
     */
    ~rb_tree_nil_node() = default;
    
    // 禁止拷贝和移动
    rb_tree_nil_node(const rb_tree_nil_node&) = delete;
    rb_tree_nil_node& operator=(const rb_tree_nil_node&) = delete;
    rb_tree_nil_node(rb_tree_nil_node&&) = delete;
    rb_tree_nil_node& operator=(rb_tree_nil_node&&) = delete;
    
public:
    /**
     * @brief 获取NIL节点单例实例
     * @return NIL节点指针
     */
    static rb_tree_node_base* instance() noexcept {
        static rb_tree_nil_node nil_node;
        return &nil_node;
    }
    
    /**
     * @brief 检查指针是否指向NIL节点
     * @param p 要检查的指针
     * @return 如果p指向NIL节点返回true，否则返回false
     */
    static bool is_nil(rb_tree_node_base* p) noexcept {
        return p == instance();
    }
    
    /**
     * @brief 检查指针是否不是NIL节点
     * @param p 要检查的指针
     * @return 如果p不指向NIL节点返回true，否则返回false
     */
    static bool not_nil(rb_tree_node_base* p) noexcept {
        return p != instance();
    }
};

// ============================================================================
// 节点操作辅助函数
// ============================================================================

/**
 * @brief 获取节点的最小节点（带类型）
 * @tparam NodePtr 节点指针类型
 * @param x 起始节点
 * @return 最小节点
 */
template<typename NodePtr>
NodePtr rb_tree_minimum(NodePtr x) noexcept {
    return NodePtr::from_base(x->minimum());
}

/**
 * @brief 获取节点的最大节点（带类型）
 * @tparam NodePtr 节点指针类型
 * @param x 起始节点
 * @return 最大节点
 */
template<typename NodePtr>
NodePtr rb_tree_maximum(NodePtr x) noexcept {
    return NodePtr::from_base(x->maximum());
}

/**
 * @brief 获取节点的前驱节点（带类型）
 * @tparam NodePtr 节点指针类型
 * @param x 起始节点
 * @return 前驱节点
 */
template<typename NodePtr>
NodePtr rb_tree_predecessor(NodePtr x) noexcept {
    return NodePtr::from_base(x->predecessor());
}

/**
 * @brief 获取节点的后继节点（带类型）
 * @tparam NodePtr 节点指针类型
 * @param x 起始节点
 * @return 后继节点
 */
template<typename NodePtr>
NodePtr rb_tree_successor(NodePtr x) noexcept {
    return NodePtr::from_base(x->successor());
}

// ============================================================================
// rb_tree_node_base 方法实现（必须在rb_tree_nil_node定义之后）
// ============================================================================

inline rb_tree_node_base::base_ptr rb_tree_node_base::minimum() noexcept {
    base_ptr x = this;
    while (x->left != nullptr && !rb_tree_nil_node::is_nil(x->left)) {
        x = x->left;
    }
    return x;
}

inline rb_tree_node_base::base_ptr rb_tree_node_base::maximum() noexcept {
    base_ptr x = this;
    while (x->right != nullptr && !rb_tree_nil_node::is_nil(x->right)) {
        x = x->right;
    }
    return x;
}

inline rb_tree_node_base::base_ptr rb_tree_node_base::predecessor() noexcept {
    if (left != nullptr && !rb_tree_nil_node::is_nil(left)) {
        return left->maximum();
    }
    
    base_ptr x = this;
    base_ptr y = parent;
    while (y != nullptr && !rb_tree_nil_node::is_nil(y) && x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

inline rb_tree_node_base::base_ptr rb_tree_node_base::successor() noexcept {
    if (right != nullptr && !rb_tree_nil_node::is_nil(right)) {
        return right->minimum();
    }
    
    base_ptr x = this;
    base_ptr y = parent;
    while (y != nullptr && !rb_tree_nil_node::is_nil(y) && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

} // namespace detail
} // namespace mystl
