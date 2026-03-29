/**
 * @file select1st.hpp
 * @brief Function object to extract the first element from a pair
 * @brief 从pair中提取第一个元素的函数对象
 * 
 * This header provides the select1st function object used by map and multimap
 * to extract the key from a key-value pair stored in the red-black tree.
 * 
 * 本头文件提供select1st函数对象，用于map和multimap从红黑树中存储的键值对中提取键。
 * 
 * @author shensjt
 * @date 2026-03-30
 */

#pragma once

namespace mystl {
namespace detail {

/**
 * @brief Function object to extract the first element from a pair
 * @brief 从pair中提取第一个元素的函数对象
 * 
 * This is used as the KeyOfValue template parameter for rb_tree when
 * implementing map and multimap containers.
 * 
 * 在实现map和multimap容器时，用作rb_tree的KeyOfValue模板参数。
 * 
 * @tparam Pair Type of the pair (should be pair<const Key, T>)
 * @tparam Pair pair的类型（应为pair<const Key, T>）
 */
template<typename Pair>
struct select1st {
    using first_type = typename Pair::first_type;  ///< Type of the first element / 第一个元素的类型
    using type = first_type;                       ///< Type alias for compatibility / 兼容性的类型别名
    
    /**
     * @brief Extract the first element from a pair
     * @brief 从pair中提取第一个元素
     * 
     * @param p The pair to extract from
     * @param p 要提取的pair
     * @return const first_type& Reference to the first element (the key)
     * @return const first_type& 对第一个元素（键）的引用
     */
    const first_type& operator()(const Pair& p) const {
        return p.first;
    }
};

} // namespace detail
} // namespace mystl
