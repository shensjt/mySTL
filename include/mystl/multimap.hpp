/**
 * @file multimap.hpp
 * @brief Multimap container implementation for mySTL
 * @brief mySTL多重映射容器实现
 * 
 * This file implements the multimap container based on red-black tree.
 * Multimap is an associative container that contains key-value pairs with possibly duplicate keys.
 * 
 * 本文件基于红黑树实现多重映射容器。
 * Multimap是一个关联容器，包含可能具有重复键的键值对。
 * 
 * @author shensjt
 * @date 2026-03-30
 */

#pragma once

#include "rb_tree.hpp"
#include "detail/select1st.hpp"
#include "../utility/less.hpp"
#include "../utility/move.hpp"
#include "../utility/pair.hpp"
#include "../memory/allocator.hpp"
#include <initializer_list>

namespace mystl {

/**
 * @brief Multimap container class
 * @brief 多重映射容器类
 * 
 * Multimap is an associative container that contains key-value pairs with possibly duplicate keys.
 * Implemented as a wrapper around red-black tree with AllowDuplicates = true.
 * 
 * Multimap是一个关联容器，包含可能具有重复键的键值对。
 * 基于红黑树包装实现，AllowDuplicates = true。
 * 
 * @tparam Key Type of the keys
 * @tparam Key 键类型
 * @tparam T Type of the mapped values
 * @tparam T 映射值类型
 * @tparam Compare Comparison function object type
 * @tparam Compare 比较函数对象类型
 * @tparam Alloc Allocator type
 * @tparam Alloc 分配器类型
 */
template<typename Key, typename T, typename Compare = less<Key>,
         typename Alloc = allocator<pair<const Key, T>>>
class multimap {
private:
    // 内部类型定义 / Internal type definitions
    using tree_value_type = pair<const Key, T>;  ///< Type stored in the tree / 树中存储的类型
    using key_of_value = detail::select1st<tree_value_type>;  ///< Key extractor / 键提取器
    
    // 内部红黑树类型定义 / Internal red-black tree type definition
    using tree_type = rb_tree<Key, tree_value_type, key_of_value, Compare, Alloc, true>;
    tree_type tree_;  ///< Internal red-black tree / 内部红黑树
    
public:
    // ==================== 类型定义 / Type Definitions ====================
    
    using key_type = Key;                     ///< Key type / 键类型
    using mapped_type = T;                    ///< Mapped type / 映射类型
    using value_type = pair<const Key, T>;    ///< Value type (key-value pair) / 值类型（键值对）
    using size_type = typename tree_type::size_type;          ///< Size type / 大小类型
    using difference_type = typename tree_type::difference_type;  ///< Difference type / 差值类型
    using key_compare = Compare;              ///< Key comparison function type / 键比较函数类型
    using allocator_type = Alloc;             ///< Allocator type / 分配器类型
    
    using reference = value_type&;            ///< Reference type / 引用类型
    using const_reference = const value_type&; ///< Const reference type / 常量引用类型
    using pointer = typename allocator_type::pointer;  ///< Pointer type / 指针类型
    using const_pointer = typename allocator_type::const_pointer;  ///< Const pointer type / 常量指针类型
    
    using iterator = typename tree_type::iterator;      ///< Iterator type / 迭代器类型
    using const_iterator = typename tree_type::const_iterator;  ///< Const iterator type / 常量迭代器类型
    using reverse_iterator = typename tree_type::reverse_iterator;  ///< Reverse iterator type / 反向迭代器类型
    using const_reverse_iterator = typename tree_type::const_reverse_iterator;  ///< Const reverse iterator type / 常量反向迭代器类型
    
    /**
     * @brief Value comparison function object
     * @brief 值比较函数对象
     * 
     * Compares two value_type objects by comparing their keys.
     * 通过比较键来比较两个value_type对象。
     */
    class value_compare {
        friend class multimap;
    protected:
        Compare comp_;
        value_compare(Compare c) : comp_(c) {}
    public:
        bool operator()(const value_type& x, const value_type& y) const {
            return comp_(x.first, y.first);
        }
    };
    
    // ==================== 构造函数 / Constructors ====================
    
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * Constructs an empty multimap.
     * 构造一个空多重映射。
     */
    multimap() : multimap(Compare()) {}
    
    /**
     * @brief Constructor with comparison function
     * @brief 带比较函数的构造函数
     * 
     * @param comp Comparison function object
     * @param comp 比较函数对象
     */
    explicit multimap(const Compare& comp, const Alloc& alloc = Alloc())
        : tree_(comp, alloc) {}
    
    /**
     * @brief Range constructor
     * @brief 范围构造函数
     * 
     * @tparam InputIt Input iterator type
     * @tparam InputIt 输入迭代器类型
     * @param first Iterator to the first element
     * @param first 指向第一个元素的迭代器
     * @param last Iterator to one past the last element
     * @param last 指向最后一个元素之后位置的迭代器
     */
    template<typename InputIt>
    multimap(InputIt first, InputIt last,
             const Compare& comp = Compare(),
             const Alloc& alloc = Alloc())
        : tree_(comp, alloc) {
        insert(first, last);
    }
    
    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other Another multimap to copy from
     * @param other 要拷贝的另一个多重映射
     */
    multimap(const multimap& other) : tree_(other.tree_) {}
    
    /**
     * @brief Move constructor
     * @brief 移动构造函数
     * 
     * @param other Another multimap to move from
     * @param other 要移动的另一个多重映射
     */
    multimap(multimap&& other) noexcept : tree_(mystl::move(other.tree_)) {}
    
    /**
     * @brief Initializer list constructor
     * @brief 初始化列表构造函数
     * 
     * @param init Initializer list
     * @param init 初始化列表
     */
    multimap(std::initializer_list<value_type> init,
             const Compare& comp = Compare(),
             const Alloc& alloc = Alloc())
        : tree_(comp, alloc) {
        insert(init.begin(), init.end());
    }
    
    // ==================== 析构函数 / Destructor ====================
    
    /**
     * @brief Destructor
     * @brief 析构函数
     */
    ~multimap() = default;
    
    // ==================== 赋值操作符 / Assignment Operators ====================
    
    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值操作符
     * 
     * @param other Another multimap to copy from
     * @param other 要拷贝的另一个多重映射
     * @return multimap& Reference to this multimap
     * @return multimap& 此多重映射的引用
     */
    multimap& operator=(const multimap& other) {
        if (this != &other) {
            tree_ = other.tree_;
        }
        return *this;
    }
    
    /**
     * @brief Move assignment operator
     * @brief 移动赋值操作符
     * 
     * @param other Another multimap to move from
     * @param other 要移动的另一个多重映射
     * @return multimap& Reference to this multimap
     * @return multimap& 此多重映射的引用
     */
    multimap& operator=(multimap&& other) noexcept {
        if (this != &other) {
            tree_ = mystl::move(other.tree_);
        }
        return *this;
    }
    
    /**
     * @brief Initializer list assignment operator
     * @brief 初始化列表赋值操作符
     * 
     * @param ilist Initializer list
     * @param ilist 初始化列表
     * @return multimap& Reference to this multimap
     * @return multimap& 此多重映射的引用
     */
    multimap& operator=(std::initializer_list<value_type> ilist) {
        clear();
        insert(ilist.begin(), ilist.end());
        return *this;
    }
    
    // ==================== 分配器 / Allocator ====================
    
    /**
     * @brief Get the allocator
     * @brief 获取分配器
     * 
     * @return allocator_type The allocator
     * @return allocator_type 分配器
     */
    allocator_type get_allocator() const noexcept {
        return tree_.get_allocator();
    }
    
    // ==================== 迭代器 / Iterators ====================
    
    /**
     * @brief Get iterator to beginning
     * @brief 获取指向开始的迭代器
     * 
     * @return iterator Iterator to the first element
     * @return iterator 指向第一个元素的迭代器
     */
    iterator begin() noexcept { return tree_.begin(); }
    
    /**
     * @brief Get const iterator to beginning
     * @brief 获取指向开始的常量迭代器
     * 
     * @return const_iterator Const iterator to the first element
     * @return const_iterator 指向第一个元素的常量迭代器
     */
    const_iterator begin() const noexcept { return tree_.begin(); }
    
    /**
     * @brief Get const iterator to beginning
     * @brief 获取指向开始的常量迭代器
     * 
     * @return const_iterator Const iterator to the first element
     * @return const_iterator 指向第一个元素的常量迭代器
     */
    const_iterator cbegin() const noexcept { return tree_.begin(); }
    
    /**
     * @brief Get iterator to end
     * @brief 获取指向结束的迭代器
     * 
     * @return iterator Iterator to one past the last element
     * @return iterator 指向最后一个元素之后位置的迭代器
     */
    iterator end() noexcept { return tree_.end(); }
    
    /**
     * @brief Get const iterator to end
     * @brief 获取指向结束的常量迭代器
     * 
     * @return const_iterator Const iterator to one past the last element
     * @return const_iterator 指向最后一个元素之后位置的常量迭代器
     */
    const_iterator end() const noexcept { return tree_.end(); }
    
    /**
     * @brief Get const iterator to end
     * @brief 获取指向结束的常量迭代器
     * 
     * @return const_iterator Const iterator to one past the last element
     * @return const_iterator 指向最后一个元素之后位置的常量迭代器
     */
    const_iterator cend() const noexcept { return tree_.end(); }
    
    /**
     * @brief Get reverse iterator to beginning
     * @brief 获取指向开始的反向迭代器
     * 
     * @return reverse_iterator Reverse iterator to the last element
     * @return reverse_iterator 指向最后一个元素的反向迭代器
     */
    reverse_iterator rbegin() noexcept { return tree_.rbegin(); }
    
    /**
     * @brief Get const reverse iterator to beginning
     * @brief 获取指向开始的常量反向迭代器
     * 
     * @return const_reverse_iterator Const reverse iterator to the last element
     * @return const_reverse_iterator 指向最后一个元素的常量反向迭代器
     */
    const_reverse_iterator rbegin() const noexcept { return tree_.rbegin(); }
    
    /**
     * @brief Get const reverse iterator to beginning
     * @brief 获取指向开始的常量反向迭代器
     * 
     * @return const_reverse_iterator Const reverse iterator to the last element
     * @return const_reverse_iterator 指向最后一个元素的常量反向迭代器
     */
    const_reverse_iterator crbegin() const noexcept { return tree_.rbegin(); }
    
    /**
     * @brief Get reverse iterator to end
     * @brief 获取指向结束的反向迭代器
     * 
     * @return reverse_iterator Reverse iterator to one before the first element
     * @return reverse_iterator 指向第一个元素之前位置的反向迭代器
     */
    reverse_iterator rend() noexcept { return tree_.rend(); }
    
    /**
     * @brief Get const reverse iterator to end
     * @brief 获取指向结束的常量反向迭代器
     * 
     * @return const_reverse_iterator Const reverse iterator to one before the first element
     * @return const_reverse_iterator 指向第一个元素之前位置的常量反向迭代器
     */
    const_reverse_iterator rend() const noexcept { return tree_.rend(); }
    
    /**
     * @brief Get const reverse iterator to end
     * @brief 获取指向结束的常量反向迭代器
     * 
     * @return const_reverse_iterator Const reverse iterator to one before the first element
     * @return const_reverse_iterator 指向第一个元素之前位置的常量反向迭代器
     */
    const_reverse_iterator crend() const noexcept { return tree_.rend(); }
    
    // ==================== 容量 / Capacity ====================
    
    /**
     * @brief Check if the multimap is empty
     * @brief 检查多重映射是否为空
     * 
     * @return true if the multimap is empty, false otherwise
     * @return 如果多重映射为空返回true，否则返回false
     */
    bool empty() const noexcept { return tree_.empty(); }
    
    /**
     * @brief Get the number of elements
     * @brief 获取元素数量
     * 
     * @return size_type Number of elements in the multimap
     * @return size_type 多重映射中的元素数量
     */
    size_type size() const noexcept { return tree_.size(); }
    
    /**
     * @brief Get the maximum possible number of elements
     * @brief 获取可能的最大元素数量
     * 
     * @return size_type Maximum possible number of elements
     * @return size_type 可能的最大元素数量
     */
    size_type max_size() const noexcept { return tree_.max_size(); }
    
    // ==================== 修改操作 / Modifiers ====================
    
    /**
     * @brief Clear the multimap
     * @brief 清空多重映射
     */
    void clear() noexcept { tree_.clear(); }
    
    /**
     * @brief Insert element
     * @brief 插入元素
     * 
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return iterator Iterator to the inserted element
     * @return iterator 指向插入元素的迭代器
     * 
     * @note For multimap, insert always succeeds (duplicates allowed)
     * @note 对于multimap，插入总是成功（允许重复）
     */
    iterator insert(const value_type& value) {
        return tree_.insert(value).first;
    }
    
    /**
     * @brief Insert element (move)
     * @brief 插入元素（移动）
     * 
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return iterator Iterator to the inserted element
     * @return iterator 指向插入元素的迭代器
     */
    iterator insert(value_type&& value) {
        return tree_.insert(mystl::move(value)).first;
    }
    
    /**
     * @brief Insert element with hint
     * @brief 带提示插入元素
     * 
     * @param hint Hint for the position where the element can be inserted
     * @param hint 元素可以插入位置的提示
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return iterator Iterator to the inserted element
     * @return iterator 指向插入元素的迭代器
     * 
     * @note The hint is used as a starting point for the search, which can improve
     *       performance if the hint is correct. If the hint is wrong, the performance
     *       is the same as the regular insert.
     * @note 提示用作搜索的起点，如果提示正确可以提高性能。如果提示错误，性能与常规插入相同。
     */
    iterator insert(const_iterator hint, const value_type& value) {
        return tree_.insert(hint, value);
    }
    
    /**
     * @brief Insert element with hint (move)
     * @brief 带提示插入元素（移动）
     * 
     * @param hint Hint for the position where the element can be inserted
     * @param hint 元素可以插入位置的提示
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return iterator Iterator to the inserted element
     * @return iterator 指向插入元素的迭代器
     */
    iterator insert(const_iterator hint, value_type&& value) {
        return tree_.insert(hint, mystl::move(value));
    }
    
    /**
     * @brief Insert elements from range
     * @brief 从范围插入元素
     * 
     * @tparam InputIt Input iterator type
     * @tparam InputIt 输入迭代器类型
     * @param first Iterator to the first element
     * @param first 指向第一个元素的迭代器
     * @param last Iterator to one past the last element
     * @param last 指向最后一个元素之后位置的迭代器
     */
    template<typename InputIt>
    void insert(InputIt first, InputIt last) {
        for (; first != last; ++first) {
            tree_.insert(*first);
        }
    }
    
    /**
     * @brief Insert elements from initializer list
     * @brief 从初始化列表插入元素
     * 
     * @param ilist Initializer list
     * @param ilist 初始化列表
     */
    void insert(std::initializer_list<value_type> ilist) {
        insert(ilist.begin(), ilist.end());
    }
    
    /**
     * @brief Construct element in-place
     * @brief 原位构造元素
     * 
     * @tparam Args Types of arguments to forward to the constructor
     * @tparam Args 要转发给构造函数的参数类型
     * @param args Arguments to forward to the constructor
     * @param args 转发给构造函数的参数
     * @return iterator Iterator to the inserted element
     * @return iterator 指向插入元素的迭代器
     */
    template<typename... Args>
    iterator emplace(Args&&... args) {
        return tree_.insert(value_type(mystl::forward<Args>(args)...)).first;
    }
    
    /**
     * @brief Construct element in-place with hint
     * @brief 带提示原位构造元素
     * 
     * @tparam Args Types of arguments to forward to the constructor
     * @tparam Args 要转发给构造函数的参数类型
     * @param hint Hint for the position where the element can be inserted
     * @param hint 元素可以插入位置的提示
     * @param args Arguments to forward to the constructor
     * @param args 转发给构造函数的参数
     * @return iterator Iterator to the inserted element
     * @return iterator 指向插入元素的迭代器
     */
    template<typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        // 简化实现：忽略提示，直接插入
        (void)hint;
        return tree_.insert(value_type(mystl::forward<Args>(args)...)).first;
    }
    
    /**
     * @brief Erase element by key
     * @brief 通过键删除元素
     * 
     * @param key Key of the element to erase
     * @param key 要删除元素的键
     * @return size_type Number of elements erased (may be >1 for multimap)
     * @return size_type 删除的元素数量（对于multimap可能大于1）
     */
    size_type erase(const key_type& key) {
        return tree_.erase(key);
    }
    
    /**
     * @brief Erase element by iterator
     * @brief 通过迭代器删除元素
     * 
     * @param pos Iterator to the element to erase
     * @param pos 指向要删除元素的迭代器
     * @return iterator Iterator following the last removed element
     * @return iterator 指向最后一个被删除元素之后的迭代器
     */
    iterator erase(const_iterator pos) {
        return tree_.erase(pos);
    }
    
    /**
     * @brief Erase elements in range
     * @brief 删除范围内的元素
     * 
     * @param first Iterator to the first element to erase
     * @param first 指向要删除的第一个元素的迭代器
     * @param last Iterator to one past the last element to erase
     * @param last 指向要删除的最后一个元素之后位置的迭代器
     * @return iterator Iterator following the last removed element
     * @return iterator 指向最后一个被删除元素之后的迭代器
     */
    iterator erase(const_iterator first, const_iterator last) {
        return tree_.erase(first, last);
    }
    
    /**
     * @brief Swap contents with another multimap
     * @brief 与另一个多重映射交换内容
     * 
     * @param other Another multimap to swap with
     * @param other 要交换的另一个多重映射
     */
    void swap(multimap& other) noexcept {
        tree_.swap(other.tree_);
    }
    
    // ==================== 查找操作 / Lookup Operations ====================
    
    /**
     * @brief Find element with specific key
     * @brief 查找具有特定键的元素
     * 
     * @param key Key value to search for
     * @param key 要搜索的键值
     * @return iterator Iterator to an element with the key if found, end() otherwise
     * @return iterator 如果找到则返回指向具有该键的元素的迭代器，否则返回end()
     */
    iterator find(const key_type& key) {
        return tree_.find(key);
    }
    
    /**
     * @brief Find element with specific key (const version)
     * @brief 查找具有特定键的元素（常量版本）
     * 
     * @param key Key value to search for
     * @param key 要搜索的键值
     * @return const_iterator Const iterator to an element with the key if found, end() otherwise
     * @return const_iterator 如果找到则返回指向具有该键的元素的常量迭代器，否则返回end()
     */
    const_iterator find(const key_type& key) const {
        return tree_.find(key);
    }
    
    /**
     * @brief Count elements with specific key
     * @brief 计算具有特定键的元素数量
     * 
     * @param key Key value to count
     * @param key 要计数的键值
     * @return size_type Number of elements with the key (may be >1 for multimap)
     * @return size_type 具有该键的元素数量（对于multimap可能大于1）
     */
    size_type count(const key_type& key) const {
        return tree_.count(key);
    }
    
    /**
     * @brief Find first element not less than the given key
     * @brief 查找第一个不小于给定键的元素
     * 
     * @param key Key value to compare
     * @param key 要比较的键值
     * @return iterator Iterator to the first element not less than key
     * @return iterator 指向第一个不小于key的元素的迭代器
     */
    iterator lower_bound(const key_type& key) {
        return tree_.lower_bound(key);
    }
    
    /**
     * @brief Find first element not less than the given key (const version)
     * @brief 查找第一个不小于给定键的元素（常量版本）
     * 
     * @param key Key value to compare
     * @param key 要比较的键值
     * @return const_iterator Const iterator to the first element not less than key
     * @return const_iterator 指向第一个不小于key的元素的常量迭代器
     */
    const_iterator lower_bound(const key_type& key) const {
        return tree_.lower_bound(key);
    }
    
    /**
     * @brief Find first element greater than the given key
     * @brief 查找第一个大于给定键的元素
     * 
     * @param key Key value to compare
     * @param key 要比较的键值
     * @return iterator Iterator to the first element greater than key
     * @return iterator 指向第一个大于key的元素的迭代器
     */
    iterator upper_bound(const key_type& key) {
        return tree_.upper_bound(key);
    }
    
    /**
     * @brief Find first element greater than the given key (const version)
     * @brief 查找第一个大于给定键的元素（常量版本）
     * 
     * @param key Key value to compare
     * @param key 要比较的键值
     * @return const_iterator Const iterator to the first element greater than key
     * @return const_iterator 指向第一个大于key的元素的常量迭代器
     */
    const_iterator upper_bound(const key_type& key) const {
        return tree_.upper_bound(key);
    }
    
    /**
     * @brief Get range of elements matching a specific key
     * @brief 获取匹配特定键的元素范围
     * 
     * @param key Key value to match
     * @param key 要匹配的键值
     * @return pair<iterator, iterator> Pair of iterators to the beginning and end of the range
     * @return pair<iterator, iterator> 指向范围开始和结束的迭代器对
     */
    pair<iterator, iterator> equal_range(const key_type& key) {
        return tree_.equal_range(key);
    }
    
    /**
     * @brief Get range of elements matching a specific key (const version)
     * @brief 获取匹配特定键的元素范围（常量版本）
     * 
     * @param key Key value to match
     * @param key 要匹配的键值
     * @return pair<const_iterator, const_iterator> Pair of const iterators to the beginning and end of the range
     * @return pair<const_iterator, const_iterator> 指向范围开始和结束的常量迭代器对
     */
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return tree_.equal_range(key);
    }
    
    // ==================== 观察器 / Observers ====================
    
    /**
     * @brief Get the key comparison function
     * @brief 获取键比较函数
     * 
     * @return key_compare The key comparison function
     * @return key_compare 键比较函数
     */
    key_compare key_comp() const { return tree_.key_comp(); }
    
    /**
     * @brief Get the value comparison function
     * @brief 获取值比较函数
     * 
     * @return value_compare The value comparison function
     * @return value_compare 值比较函数
     */
    value_compare value_comp() const { return value_compare(tree_.key_comp()); }
    
    // ==================== 比较操作符 / Comparison Operators ====================
    
    /**
     * @brief Equality comparison operator
     * @brief 相等比较操作符
     * 
     * @param other Another multimap to compare with
     * @param other 要比较的另一个多重映射
     * @return true if multimaps are equal, false otherwise
     * @return 如果多重映射相等返回true，否则返回false
     */
    bool operator==(const multimap& other) const {
        return tree_ == other.tree_;
    }
    
    /**
     * @brief Inequality comparison operator
     * @brief 不等比较操作符
     * 
     * @param other Another multimap to compare with
     * @param other 要比较的另一个多重映射
     * @return true if multimaps are not equal, false otherwise
     * @return 如果多重映射不相等返回true，否则返回false
     */
    bool operator!=(const multimap& other) const {
        return !(*this == other);
    }
    
    /**
     * @brief Less-than comparison operator
     * @brief 小于比较操作符
     * 
     * @param other Another multimap to compare with
     * @param other 要比较的另一个多重映射
     * @return true if this multimap is lexicographically less than other, false otherwise
     * @return 如果此多重映射字典序小于另一个多重映射返回true，否则返回false
     */
    bool operator<(const multimap& other) const {
        return tree_ < other.tree_;
    }
    
    /**
     * @brief Less-than-or-equal comparison operator
     * @brief 小于等于比较操作符
     * 
     * @param other Another multimap to compare with
     * @param other 要比较的另一个多重映射
     * @return true if this multimap is lexicographically less than or equal to other, false otherwise
     * @return 如果此多重映射字典序小于等于另一个多重映射返回true，否则返回false
     */
    bool operator<=(const multimap& other) const {
        return !(other < *this);
    }
    
    /**
     * @brief Greater-than comparison operator
     * @brief 大于比较操作符
     * 
     * @param other Another multimap to compare with
     * @param other 要比较的另一个多重映射
     * @return true if this multimap is lexicographically greater than other, false otherwise
     * @return 如果此多重映射字典序大于另一个多重映射返回true，否则返回false
     */
    bool operator>(const multimap& other) const {
        return other < *this;
    }
    
    /**
     * @brief Greater-than-or-equal comparison operator
     * @brief 大于等于比较操作符
     * 
     * @param other Another multimap to compare with
     * @param other 要比较的另一个多重映射
     * @return true if this multimap is lexicographically greater than or equal to other, false otherwise
     * @return 如果此多重映射字典序大于等于另一个多重映射返回true，否则返回false
     */
    bool operator>=(const multimap& other) const {
        return !(*this < other);
    }
};

// ==================== 非成员函数 / Non-member Functions ====================

/**
 * @brief Swap two multimaps
 * @brief 交换两个多重映射
 * 
 * @tparam Key Key type
 * @tparam Key 键类型
 * @tparam T Mapped type
 * @tparam T 映射类型
 * @tparam Compare Comparison function type
 * @tparam Compare 比较函数类型
 * @tparam Alloc Allocator type
 * @tparam Alloc 分配器类型
 * @param lhs First multimap
 * @param lhs 第一个多重映射
 * @param rhs Second multimap
 * @param rhs 第二个多重映射
 */
template<typename Key, typename T, typename Compare, typename Alloc>
void swap(multimap<Key, T, Compare, Alloc>& lhs,
          multimap<Key, T, Compare, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace mystl
