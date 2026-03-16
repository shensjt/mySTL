#pragma once

/**
 * @file iterator_functions.hpp
 * @brief Iterator operation functions with tag dispatching and optimizations
 * @brief 使用标签分发和优化的迭代器操作函数
 * 
 * This file implements iterator operations using tag dispatching for optimal performance.
 * Functions include advance, distance, next, prev, and various optimized versions.
 * 
 * 本文件使用标签分发实现迭代器操作以获得最佳性能。
 * 函数包括 advance、distance、next、prev 和各种优化版本。
 * 
 * Optimization versions:
 * 1. advance() - Standard implementation using tag dispatching, suitable for general scenarios
 * 2. advance_optimized() - Compile-time optimized version using if constexpr, suitable for known iterator types
 * 3. advance_unrolled() - Loop unrolled version, suitable for small distance moves, reduces loop overhead
 * 4. advance_const() - Compile-time constant version, suitable when distance is a compile-time constant
 * 
 * 优化版本说明：
 * 1. advance() - 标准实现，使用标签分发，适用于通用场景
 * 2. advance_optimized() - 编译时优化版本，使用if constexpr，适用于已知迭代器类型的场景
 * 3. advance_unrolled() - 循环展开版本，适用于小距离移动，减少循环开销
 * 4. advance_const() - 编译时常量版本，适用于距离为编译时常量的场景
 * 
 * Selection recommendations:
 * - General scenarios: use advance()
 * - Known iterator types requiring compile-time optimization: use advance_optimized()
 * - Small distance moves (n < 16): use advance_unrolled()
 * - Compile-time constant distance: use advance_const<n>()
 * 
 * 选择建议：
 * - 通用场景：使用 advance()
 * - 已知迭代器类型且需要编译时优化：使用 advance_optimized()
 * - 小距离移动（n < 16）：使用 advance_unrolled()
 * - 距离为编译时常量：使用 advance_const<n>()
 * 
 * @author mySTL shensjt
 * @date 2026-03-17
 * 
 * @see iterator_tags.hpp
 * @see iterator_traits.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/operations
 */

#include "iterator_traits.hpp"
#include "../utility/swap.hpp"   // for mystl::swap
#include "../utility/move.hpp"   // for mystl::move
#include <cstddef>               // for std::size_t

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

// ==================== Implementation Details ====================
// ==================== 实现细节 ====================

namespace detail {

// ==================== advance Implementations ====================
// ==================== advance 实现 ====================

/**
 * @brief Input iterator advance implementation
 * @brief 输入迭代器 advance 实现
 * 
 * @tparam InputIt Input iterator type
 * @tparam InputIt 输入迭代器类型
 * @tparam Distance Distance type
 * @tparam Distance 距离类型
 * @param it Iterator to advance (reference)
 * @param it 要前进的迭代器（引用）
 * @param n Distance to advance (must be >= 0 for input iterators)
 * @param n 前进的距离（对于输入迭代器必须 >= 0）
 * @param tag Input iterator tag for tag dispatching
 * @param tag 用于标签分发的输入迭代器标签
 * 
 * @details Can only move forward, uses loop to advance one by one.
 * Time complexity: O(n)
 * 
 * @details 只能向前移动，使用循环逐个前进。
 * 时间复杂度：O(n)
 */
template<typename InputIt, typename Distance>
void advance_impl(InputIt& it, Distance n, input_iterator_tag) {
    while (n > 0) {
        ++it;
        --n;
    }
}

/**
 * @brief Forward iterator advance implementation
 * @brief 前向迭代器 advance 实现
 * 
 * @tparam ForwardIt Forward iterator type
 * @tparam ForwardIt 前向迭代器类型
 * @tparam Distance Distance type
 * @tparam Distance 距离类型
 * @param it Iterator to advance (reference)
 * @param it 要前进的迭代器（引用）
 * @param n Distance to advance (must be >= 0 for forward iterators)
 * @param n 前进的距离（对于前向迭代器必须 >= 0）
 * @param tag Forward iterator tag for tag dispatching
 * @param tag 用于标签分发的前向迭代器标签
 * 
 * @details Same as input iterator implementation but can traverse multiple times.
 * Time complexity: O(n)
 * 
 * @details 与输入迭代器实现相同，但可以多次遍历。
 * 时间复杂度：O(n)
 */
template<typename ForwardIt, typename Distance>
void advance_impl(ForwardIt& it, Distance n, forward_iterator_tag) {
    while (n > 0) {
        ++it;
        --n;
    }
}

/**
 * @brief Bidirectional iterator advance implementation
 * @brief 双向迭代器 advance 实现
 * 
 * @tparam BidirIt Bidirectional iterator type
 * @tparam BidirIt 双向迭代器类型
 * @tparam Distance Distance type
 * @tparam Distance 距离类型
 * @param it Iterator to advance (reference)
 * @param it 要前进的迭代器（引用）
 * @param n Distance to advance (positive for forward, negative for backward)
 * @param n 前进的距离（正数向前，负数向后）
 * @param tag Bidirectional iterator tag for tag dispatching
 * @param tag 用于标签分发的双向迭代器标签
 * 
 * @details Supports both forward and backward movement.
 * Time complexity: O(|n|)
 * 
 * @details 支持向前和向后移动。
 * 时间复杂度：O(|n|)
 */
template<typename BidirIt, typename Distance>
void advance_impl(BidirIt& it, Distance n, bidirectional_iterator_tag) {
    if (n >= 0) {
        while (n > 0) {
            ++it;
            --n;
        }
    } else {
        while (n < 0) {
            --it;
            ++n;
        }
    }
}

/**
 * @brief Random access iterator advance implementation
 * @brief 随机访问迭代器 advance 实现
 * 
 * @tparam RandomIt Random access iterator type
 * @tparam RandomIt 随机访问迭代器类型
 * @tparam Distance Distance type
 * @tparam Distance 距离类型
 * @param it Iterator to advance (reference)
 * @param it 要前进的迭代器（引用）
 * @param n Distance to advance (positive for forward, negative for backward)
 * @param n 前进的距离（正数向前，负数向后）
 * @param tag Random access iterator tag for tag dispatching
 * @param tag 用于标签分发的随机访问迭代器标签
 * 
 * @details Uses direct arithmetic operations, constant time.
 * Time complexity: O(1)
 * 
 * @details 直接使用算术运算，常数时间。
 * 时间复杂度：O(1)
 */
template<typename RandomIt, typename Distance>
void advance_impl(RandomIt& it, Distance n, random_access_iterator_tag) {
    it += n;
}

// ==================== distance Implementations ====================
// ==================== distance 实现 ====================

/**
 * @brief Input/forward/bidirectional iterator distance implementation
 * @brief 输入/前向/双向迭代器 distance 实现
 * 
 * @tparam InputIt Input iterator type
 * @tparam InputIt 输入迭代器类型
 * @param first First iterator
 * @param first 第一个迭代器
 * @param last Last iterator
 * @param last 最后一个迭代器
 * @param tag Input iterator tag for tag dispatching
 * @param tag 用于标签分发的输入迭代器标签
 * @return Distance between first and last
 * @return first 和 last 之间的距离
 * 
 * @details Uses loop counting, linear time complexity.
 * Suitable for input, forward, and bidirectional iterators.
 * 
 * @details 使用循环计数，线性时间复杂度。
 * 适用于输入、前向和双向迭代器。
 * 
 * @note Time complexity: O(n)
 * @note 时间复杂度：O(n)
 */
template<typename InputIt>
typename iterator_traits<InputIt>::difference_type
distance_impl(InputIt first, InputIt last, input_iterator_tag) {
    typename iterator_traits<InputIt>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

/**
 * @brief Random access iterator distance implementation
 * @brief 随机访问迭代器 distance 实现
 * 
 * @tparam RandomIt Random access iterator type
 * @tparam RandomIt 随机访问迭代器类型
 * @param first First iterator
 * @param first 第一个迭代器
 * @param last Last iterator
 * @param last 最后一个迭代器
 * @param tag Random access iterator tag for tag dispatching
 * @param tag 用于标签分发的随机访问迭代器标签
 * @return Distance between first and last
 * @return first 和 last 之间的距离
 * 
 * @details Direct distance calculation, constant time complexity.
 * 
 * @details 直接计算距离，常数时间复杂度。
 * 
 * @note Time complexity: O(1)
 * @note 时间复杂度：O(1)
 */
template<typename RandomIt>
typename iterator_traits<RandomIt>::difference_type
distance_impl(RandomIt first, RandomIt last, random_access_iterator_tag) {
    return last - first;
}

} // namespace detail

// ==================== Public Interfaces ====================
// ==================== 公开接口 ====================

/**
 * @brief Advance iterator by n positions (forward or backward)
 * @brief 使迭代器前进（或后退）n个位置
 * 
 * @tparam InputIt Iterator type
 * @tparam InputIt 迭代器类型
 * @tparam Distance Distance type (usually difference_type)
 * @tparam Distance 距离类型（通常为 difference_type）
 * @param it Iterator to advance (reference)
 * @param it 要移动的迭代器（引用）
 * @param n Distance to advance, positive for forward, negative for backward
 *          (only bidirectional and random access iterators support negative)
 * @param n 移动的距离，正数向前，负数向后
 *          （仅双向和随机访问迭代器支持负数）
 * 
 * @details Uses tag dispatching to select optimal implementation based on
 * iterator category. For random access iterators: O(1) time.
 * For bidirectional iterators: O(|n|) time.
 * For forward/input iterators: O(n) time (n must be >= 0).
 * 
 * @details 使用标签分发根据迭代器分类选择最优实现。
 * 对于随机访问迭代器：O(1) 时间复杂度。
 * 对于双向迭代器：O(|n|) 时间复杂度。
 * 对于前向/输入迭代器：O(n) 时间复杂度（n 必须 >= 0）。
 * 
 * @note Negative distances are only supported for bidirectional and
 *       random access iterators.
 * @note 负数距离仅双向和随机访问迭代器支持。
 */
template<typename InputIt, typename Distance>
void advance(InputIt& it, Distance n) {
    static_assert(is_iterator_v<InputIt>, 
                  "advance requires an iterator type");
    static_assert(is_integral_v<Distance> || is_signed_v<Distance>,
                  "Distance must be an integral type");
    
    using category = iterator_category_t<InputIt>;
    detail::advance_impl(it, n, category{});
}

/**
 * @brief Calculate distance between two iterators
 * @brief 计算两个迭代器之间的距离
 * 
 * @tparam InputIt Iterator type
 * @tparam InputIt 迭代器类型
 * @param first First iterator
 * @param first 起始迭代器
 * @param last Last iterator
 * @param last 结束迭代器
 * @return Distance from first to last (difference_type)
 * @return 从 first 到 last 的距离（difference_type）
 * 
 * @details For random access iterators: O(1) time complexity.
 * For other iterator categories: O(n) time complexity.
 * 
 * @details 对于随机访问迭代器：O(1) 时间复杂度。
 * 对于其他迭代器类别：O(n) 时间复杂度。
 * 
 * @note The distance is calculated as the number of increments needed
 *       to go from first to last.
 * @note 距离计算为从 first 到 last 所需的递增次数。
 */
template<typename InputIt>
typename iterator_traits<InputIt>::difference_type
distance(InputIt first, InputIt last) {
    using category = iterator_category_t<InputIt>;
    return detail::distance_impl(first, last, category{});
}

/**
 * @brief Return iterator advanced by n positions
 * @brief 返回前进 n 个位置后的迭代器
 * 
 * @tparam ForwardIt Forward iterator type
 * @tparam ForwardIt 前向迭代器类型
 * @param it Starting iterator
 * @param it 起始迭代器
 * @param n Distance to advance, default is 1
 * @param n 前进的距离，默认为 1
 * @return Iterator advanced by n positions
 * @return 前进 n 个位置后的迭代器
 * 
 * @details Does not modify the original iterator, returns a new iterator.
 * Equivalent to: auto result = it; advance(result, n); return result;
 * 
 * @details 不修改原迭代器，返回新迭代器。
 * 等价于：auto result = it; advance(result, n); return result;
 * 
 * @note Requires at least a forward iterator.
 * @note 需要至少前向迭代器。
 */
template<typename ForwardIt>
ForwardIt next(ForwardIt it,
               typename iterator_traits<ForwardIt>::difference_type n = 1) {
    static_assert(is_forward_iterator_v<ForwardIt> || is_input_iterator_v<ForwardIt>,
                  "next requires at least a forward iterator");
    advance(it, n);
    return it;
}

/**
 * @brief Return iterator advanced by n positions with bounds checking
 * @brief 带边界检查的 next 版本
 * 
 * @tparam ForwardIt Forward iterator type
 * @tparam ForwardIt 前向迭代器类型
 * @param it Starting iterator
 * @param it 起始迭代器
 * @param last Boundary iterator (end of range)
 * @param last 边界迭代器（范围结束）
 * @param n Distance to advance, default is 1
 * @param n 前进的距离，默认为 1
 * @return Iterator advanced by n positions, not exceeding last
 * @return 前进 n 个位置后的迭代器，不超过 last
 * 
 * @details Advances the iterator by n positions, but ensures it doesn't go
 * beyond the last iterator. If n would move past last, stops at last.
 * 
 * @details 将迭代器前进 n 个位置，但确保不会超过 last 迭代器。
 * 如果 n 会移动到 last 之后，则停在 last 处。
 * 
 * @note Requires at least a forward iterator.
 * @note 需要至少前向迭代器。
 */
template<typename ForwardIt>
ForwardIt next(ForwardIt it, ForwardIt last,
               typename iterator_traits<ForwardIt>::difference_type n = 1) {
    static_assert(is_forward_iterator_v<ForwardIt> || is_input_iterator_v<ForwardIt>,
                  "next requires at least a forward iterator");
    
    using diff_type = typename iterator_traits<ForwardIt>::difference_type;
    
    if (n > 0) {
        diff_type remaining = distance(it, last);
        if (n > remaining) n = remaining;
    }
    
    advance(it, n);
    return it;
}

/**
 * @brief Return iterator moved backward by n positions
 * @brief 返回后退 n 个位置后的迭代器
 * 
 * @tparam BidirIt Bidirectional iterator type
 * @tparam BidirIt 双向迭代器类型
 * @param it Starting iterator
 * @param it 起始迭代器
 * @param n Distance to move backward, default is 1
 * @param n 后退的距离，默认为 1
 * @return Iterator moved backward by n positions
 * @return 后退 n 个位置后的迭代器
 * 
 * @details Does not modify the original iterator, returns a new iterator.
 * Equivalent to: auto result = it; advance(result, -n); return result;
 * 
 * @details 不修改原迭代器，返回新迭代器。
 * 等价于：auto result = it; advance(result, -n); return result;
 * 
 * @note Requires a bidirectional iterator.
 * @note 需要双向迭代器。
 */
template<typename BidirIt>
BidirIt prev(BidirIt it,
             typename iterator_traits<BidirIt>::difference_type n = 1) {
    static_assert(is_bidirectional_iterator_v<BidirIt>,
                  "prev requires a bidirectional iterator");
    advance(it, -n);
    return it;
}

/**
 * @brief Return iterator moved backward by n positions with bounds checking
 * @brief 带边界检查的 prev 版本
 * 
 * @tparam BidirIt Bidirectional iterator type
 * @tparam BidirIt 双向迭代器类型
 * @param it Starting iterator
 * @param it 起始迭代器
 * @param first Boundary iterator (beginning of range)
 * @param first 边界迭代器（范围开始）
 * @param n Distance to move backward, default is 1
 * @param n 后退的距离，默认为 1
 * @return Iterator moved backward by n positions, not less than first
 * @return 后退 n 个位置后的迭代器，不小于 first
 * 
 * @details Moves the iterator backward by n positions, but ensures it doesn't go
 * before the first iterator. If n would move before first, stops at first.
 * 
 * @details 将迭代器向后移动 n 个位置，但确保不会移动到 first 迭代器之前。
 * 如果 n 会移动到 first 之前，则停在 first 处。
 * 
 * @note Requires a bidirectional iterator.
 * @note 需要双向迭代器。
 */
template<typename BidirIt>
BidirIt prev(BidirIt it, BidirIt first,
             typename iterator_traits<BidirIt>::difference_type n = 1) {
    static_assert(is_bidirectional_iterator_v<BidirIt>,
                  "prev requires a bidirectional iterator");
    
    using diff_type = typename iterator_traits<BidirIt>::difference_type;
    
    if (n > 0) {
        diff_type remaining = distance(first, it);
        if (n > remaining) n = remaining;
    }
    
    advance(it, -n);
    return it;
}

// ==================== Extended Features ====================
// ==================== 扩展功能 ====================

/**
 * @brief Swap values pointed to by two iterators
 * @brief 交换两个迭代器指向的值
 * 
 * @tparam ForwardIt1 First iterator type
 * @tparam ForwardIt1 第一个迭代器类型
 * @tparam ForwardIt2 Second iterator type
 * @tparam ForwardIt2 第二个迭代器类型
 * @param a First iterator
 * @param a 第一个迭代器
 * @param b Second iterator
 * @param b 第二个迭代器
 * 
 * @details Swaps the values pointed to by the two iterators using mystl::swap.
 * Equivalent to: mystl::swap(*a, *b);
 * 
 * @details 使用 mystl::swap 交换两个迭代器指向的值。
 * 等价于：mystl::swap(*a, *b);
 * 
 * @note Requires forward iterators that can be dereferenced.
 * @note 需要可以解引用的前向迭代器。
 */
template<typename ForwardIt1, typename ForwardIt2>
void iter_swap(ForwardIt1 a, ForwardIt2 b) {
    using mystl::swap;
    swap(*a, *b);
}

/**
 * @brief Safe version of advance with bounds checking (forward only)
 * @brief 安全版本的 advance，防止越界（只支持向前移动）
 * 
 * @tparam InputIt Input iterator type
 * @tparam InputIt 输入迭代器类型
 * @tparam Distance Distance type
 * @tparam Distance 距离类型
 * @param it Iterator to advance (reference)
 * @param it 要移动的迭代器（引用）
 * @param end Boundary iterator (forward movement boundary)
 * @param end 边界迭代器（向前移动的边界）
 * @param n Distance to advance (must be >= 0)
 * @param n 移动的距离（必须 >= 0）
 * @return true if movement successful, false if out of bounds
 * @return true 移动成功，false 移动失败（越界）
 * 
 * @details Advances the iterator by n positions, but checks if it would go
 * beyond the end iterator. Returns false if movement would exceed bounds.
 * 
 * @details 将迭代器前进 n 个位置，但检查是否会超过 end 迭代器。
 * 如果移动会超出边界，则返回 false。
 * 
 * @note Only supports forward movement. For bidirectional movement with bounds
 *       checking, use the other overload.
 * @note 仅支持向前移动。对于带边界检查的双向移动，请使用另一个重载。
 */
template<typename InputIt, typename Distance>
bool safe_advance(InputIt& it, InputIt end, Distance n) {
    static_assert(is_iterator_v<InputIt>, "safe_advance requires an iterator type");
    static_assert(is_integral_v<Distance> || is_signed_v<Distance>,
                  "Distance must be an integral type");
    
    if (n < 0) {
        // Negative distance not supported, use safe_advance_backward version
        // 负数距离不支持，使用 safe_advance_backward 版本
        return false;
    }
    
    using diff_type = typename iterator_traits<InputIt>::difference_type;
    
    // Forward movement / 向前移动
    diff_type remaining = distance(it, end);
    if (n > remaining) return false;
    advance(it, n);
    return true;
}

/**
 * @brief Safe version of advance with bounds checking (bidirectional)
 * @brief 安全版本的 advance，支持双向移动
 * 
 * @tparam BidirIt Bidirectional iterator type
 * @tparam BidirIt 双向迭代器类型
 * @tparam Distance Distance type
 * @tparam Distance 距离类型
 * @param it Iterator to advance (reference)
 * @param it 要移动的迭代器（引用）
 * @param begin Beginning boundary iterator
 * @param begin 开始边界迭代器
 * @param end Ending boundary iterator
 * @param end 结束边界迭代器
 * @param n Distance to advance (positive for forward, negative for backward)
 * @param n 移动的距离（正数向前，负数向后）
 * @return true if movement successful, false if out of bounds
 * @return true 移动成功，false 移动失败（越界）
 * 
 * @details Advances the iterator by n positions with full bounds checking.
 * Checks both forward and backward boundaries.
 * 
 * @details 将迭代器前进 n 个位置，进行完整的边界检查。
 * 检查向前和向后的边界。
 * 
 * @note Requires bidirectional iterator.
 * @note 需要双向迭代器。
 */
template<typename BidirIt, typename Distance>
bool safe_advance(BidirIt& it, BidirIt begin, BidirIt end, Distance n) {
    static_assert(is_iterator_v<BidirIt>, "safe_advance requires an iterator type");
    static_assert(is_bidirectional_iterator_v<BidirIt>,
                  "Bidirectional version requires bidirectional iterator");
    static_assert(is_integral_v<Distance> || is_signed_v<Distance>,
                  "Distance must be an integral type");
    
    using diff_type = typename iterator_traits<BidirIt>::difference_type;
    
    if (n > 0) {
        // Forward movement / 向前移动
        diff_type remaining = distance(it, end);
        if (n > remaining) return false;
        advance(it, n);
        return true;
    } else if (n < 0) {
        // Backward movement / 向后移动
        diff_type before = distance(begin, it);
        if (-n > before) return false;  // n is negative, so -n is positive / n是负数，所以-n是正数
        advance(it, n);
        return true;
    }
    
    return true;  // n == 0
}

/**
 * @brief Compile-time optimized advance version (using if constexpr)
 * @brief 编译时优化的 advance 版本（使用 if constexpr）
 * 
 * @tparam InputIt Iterator type
 * @tparam InputIt 迭代器类型
 * @tparam Distance Distance type
 * @tparam Distance 距离类型
 * @param it Iterator to advance (reference)
 * @param it 要移动的迭代器（引用）
 * @param n Distance to advance
 * @param n 移动的距离
 * 
 * @details Uses if constexpr to select optimal implementation at compile time
 * based on iterator category. Provides better performance by avoiding
 * runtime tag dispatching overhead.
 * 
 * @details 使用 if constexpr 在编译时根据迭代器类别选择最优实现。
 * 通过避免运行时标签分发开销提供更好的性能。
 * 
 * @note Use when iterator category is known at compile time.
 * @note 当迭代器类别在编译时已知时使用。
 */
template<typename InputIt, typename Distance>
void advance_optimized(InputIt& it, Distance n) {
    static_assert(is_iterator_v<InputIt>, "advance_optimized requires an iterator type");
    static_assert(is_integral_v<Distance> || is_signed_v<Distance>,
                  "Distance must be an integral type");
    
    using traits = iterator_traits<InputIt>;
    using category = typename traits::iterator_category;
    
    if constexpr (is_base_of_v<random_access_iterator_tag, category>) {
        // Compile-time determined: random access iterator - direct arithmetic
        // 编译时确定：随机访问迭代器 - 直接算术运算
        it += n;
    } else if constexpr (is_base_of_v<bidirectional_iterator_tag, category>) {
        // Compile-time determined: bidirectional iterator - optimized loop
        // 编译时确定：双向迭代器 - 优化循环
        if (n >= 0) {
            // Positive distance: block processing / 正数距离：分块处理
            while (n >= 4) {
                ++it; ++it; ++it; ++it;
                n -= 4;
            }
            // Process remaining / 处理剩余部分
            while (n > 0) {
                ++it;
                --n;
            }
        } else {
            // Negative distance: block processing / 负数距离：分块处理
            while (n <= -4) {
                --it; --it; --it; --it;
                n += 4;
            }
            // Process remaining / 处理剩余部分
            while (n < 0) {
                --it;
                ++n;
            }
        }
    } else {
        // Compile-time determined: input/forward iterator - forward only
        // 编译时确定：输入/前向迭代器 - 只能向前
        if (n < 0) {
            // Negative distance not supported / 负数距离不支持
            n = 0;
        }
        // Block processing / 分块处理
        while (n >= 4) {
            ++it; ++it; ++it; ++it;
            n -= 4;
        }
        // Process remaining / 处理剩余部分
        while (n > 0) {
            ++it;
            --n;
        }
    }
}


// ==================== iter_move Implementation ====================
// ==================== iter_move 实现 ====================

/**
 * @brief Move the value pointed to by an iterator
 * @brief 移动迭代器指向的值
 * 
 * @tparam InputIt Input iterator type
 * @tparam InputIt 输入迭代器类型
 * @param it Iterator
 * @param it 迭代器
 * @return Moved value (rvalue reference)
 * @return 移动后的值（右值引用）
 * 
 * @details Returns an rvalue reference to the value pointed to by the iterator,
 * enabling move semantics optimization. Equivalent to std::move(*it).
 * 
 * @details 返回指向迭代器指向的值的右值引用，启用移动语义优化。
 * 等价于 std::move(*it)。
 * 
 * @note Introduced in C++17 standard for move semantics optimization.
 * @note C++17 标准引入，支持移动语义优化。
 */
template<typename InputIt>
auto iter_move(InputIt it) 
-> typename iterator_traits<InputIt>::value_type&& {
    using mystl::move;
    return move(*it);
}

// ==================== size Function Implementations ====================
// ==================== size 函数实现 ====================

/**
 * @brief Calculate size of a container
 * @brief 计算容器大小
 * 
 * @tparam Container Container type
 * @tparam Container 容器类型
 * @param c Container
 * @param c 容器
 * @return Size of the container
 * @return 容器大小
 * 
 * @details Uses the container's size() member function.
 * 
 * @details 使用容器的 size() 成员函数。
 */
template<typename Container>
constexpr auto size(const Container& c) 
-> decltype(c.size()) {
    return c.size();
}

/**
 * @brief Calculate size of an array
 * @brief 计算数组大小
 * 
 * @tparam T Array element type
 * @tparam T 数组元素类型
 * @tparam N Array size
 * @tparam N 数组大小
 * @param array Array
 * @param array 数组
 * @return Size of the array
 * @return 数组大小
 * 
 * @details Returns the compile-time size N of the array.
 * 
 * @details 返回数组的编译时大小 N。
 */
template<typename T, ::std::size_t N>
constexpr ::std::size_t size(const T (&array)[N]) noexcept {
    return N;
}

/**
 * @brief Calculate size of an iterator range
 * @brief 计算迭代器范围大小
 * 
 * @tparam InputIt Input iterator type
 * @tparam InputIt 输入迭代器类型
 * @param first First iterator
 * @param first 起始迭代器
 * @param last Last iterator
 * @param last 结束迭代器
 * @return Size of the range
 * @return 范围大小
 * 
 * @details Calculates the distance between two iterators.
 * 
 * @details 计算两个迭代器之间的距离。
 */
template<typename InputIt>
constexpr auto size(InputIt first, InputIt last) 
-> typename iterator_traits<InputIt>::difference_type {
    return distance(first, last);
}

// ==================== empty Function Implementations ====================
// ==================== empty 函数实现 ====================

/**
 * @brief Check if a container is empty
 * @brief 检查容器是否为空
 * 
 * @tparam Container Container type
 * @tparam Container 容器类型
 * @param c Container
 * @param c 容器
 * @return true if container is empty, false otherwise
 * @return true 如果容器为空，否则 false
 * 
 * @details Uses the container's empty() member function.
 * 
 * @details 使用容器的 empty() 成员函数。
 */
template<typename Container>
constexpr auto empty(const Container& c) 
-> decltype(c.empty()) {
    return c.empty();
}

/**
 * @brief Check if an array is empty
 * @brief 检查数组是否为空
 * 
 * @tparam T Array element type
 * @tparam T 数组元素类型
 * @tparam N Array size
 * @tparam N 数组大小
 * @param array Array
 * @param array 数组
 * @return true if array is empty (N == 0), false otherwise
 * @return true 如果数组为空 (N == 0)，否则 false
 * 
 * @details Checks if the array size N is zero.
 * 
 * @details 检查数组大小 N 是否为零。
 */
template<typename T, ::std::size_t N>
constexpr bool empty(const T (&array)[N]) noexcept {
    return N == 0;
}

/**
 * @brief Check if an iterator range is empty
 * @brief 检查迭代器范围是否为空
 * 
 * @tparam InputIt Input iterator type
 * @tparam InputIt 输入迭代器类型
 * @param first First iterator
 * @param first 起始迭代器
 * @param last Last iterator
 * @param last 结束迭代器
 * @return true if range is empty (first == last), false otherwise
 * @return true 如果范围为空 (first == last)，否则 false
 * 
 * @details Checks if the iterators are equal.
 * 
 * @details 检查迭代器是否相等。
 */
template<typename InputIt>
constexpr bool empty(InputIt first, InputIt last) {
    return first == last;
}

// ==================== Loop Unrolling Optimization ====================
// ==================== 循环展开优化 ====================

/**
 * @brief Loop unrolled advance optimization
 * @brief 循环展开优化的 advance
 * 
 * @tparam InputIt Iterator type
 * @tparam InputIt 迭代器类型
 * @tparam Distance Distance type
 * @tparam Distance 距离类型
 * @param it Iterator to advance (reference)
 * @param it 要移动的迭代器（引用）
 * @param n Distance to advance
 * @param n 移动的距离
 * 
 * @details Manually unrolls loops for small distances to reduce loop overhead.
 * Supports both positive and negative distances (for bidirectional iterators).
 * 
 * @details 对小距离情况手动展开循环，减少循环开销。
 * 支持正数和负数距离（对于双向迭代器）。
 * 
 * @note Recommended for small distance moves (n < 16).
 * @note 推荐用于小距离移动（n < 16）。
 */
template<typename InputIt, typename Distance>
void advance_unrolled(InputIt& it, Distance n) {
    using traits = iterator_traits<InputIt>;
    using category = typename traits::iterator_category;
    
    if (n >= 0) {
        // Positive distance: forward movement / 正数距离：向前移动
        while (n >= 8) {
            ++it; ++it; ++it; ++it;
            ++it; ++it; ++it; ++it;
            n -= 8;
        }
        
        // Process remaining / 处理剩余部分
        if (n >= 4) {
            ++it; ++it; ++it; ++it;
            n -= 4;
        }
        
        // Process last 0-3 / 处理最后0-3个
        switch (n) {
            case 3: ++it; [[fallthrough]];
            case 2: ++it; [[fallthrough]];
            case 1: ++it; [[fallthrough]];
            case 0: break;
        }
    } else {
        // Negative distance: backward movement (requires bidirectional iterator)
        // 负数距离：向后移动（需要双向迭代器）
        // Check if bidirectional iterator / 检查是否为双向迭代器
        if constexpr (is_base_of_v<bidirectional_iterator_tag, category>) {
            n = -n;  // Convert to positive for processing / 转换为正数处理
            
            while (n >= 8) {
                --it; --it; --it; --it;
                --it; --it; --it; --it;
                n -= 8;
            }
            
            // Process remaining / 处理剩余部分
            if (n >= 4) {
                --it; --it; --it; --it;
                n -= 4;
            }
            
            // Process last 0-3 / 处理最后0-3个
            switch (n) {
                case 3: --it; [[fallthrough]];
                case 2: --it; [[fallthrough]];
                case 1: --it; [[fallthrough]];
                case 0: break;
            }
        } else {
            // Non-bidirectional iterator, use regular advance (runtime check)
            // 非双向迭代器，使用普通 advance（会进行运行时检查）
            advance(it, n);
        }
    }
}

// ==================== Compile-time Constant Optimization ====================
// ==================== 编译时常量优化 ====================

/**
 * @brief Compile-time constant distance advance optimization
 * @brief 编译时常量距离的 advance 优化
 * 
 * @tparam InputIt Iterator type
 * @tparam InputIt 迭代器类型
 * @tparam n Compile-time constant distance
 * @tparam n 编译时常量距离
 * @param it Iterator to advance (reference)
 * @param it 要移动的迭代器（引用）
 * 
 * @details Special optimization when distance is a compile-time constant.
 * Uses if constexpr to generate optimal code for common small distances.
 * 
 * @details 当距离是编译时常量时的特殊优化。
 * 使用 if constexpr 为常见的小距离生成最优代码。
 * 
 * @note Use when distance is known at compile time.
 * @note 当距离在编译时已知时使用。
 */
template<typename InputIt, int n>
void advance_const(InputIt& it) {
    if constexpr (n == 0) {
        // Do nothing / 什么都不做
    } else if constexpr (n == 1) {
        ++it;
    } else if constexpr (n == 2) {
        ++it; ++it;
    } else if constexpr (n == 3) {
        ++it; ++it; ++it;
    } else if constexpr (n == 4) {
        ++it; ++it; ++it; ++it;
    } else if constexpr (n > 0) {
        // Generic positive implementation / 通用正数实现
        for (int i = 0; i < n; ++i) {
            ++it;
        }
    } else {
        // Negative distance requires bidirectional iterator
        // 负数距离需要双向迭代器
        static_assert(n >= 0 || is_bidirectional_iterator_v<InputIt>,
                     "Negative advance requires bidirectional iterator");
        for (int i = 0; i > n; --i) {
            --it;
        }
    }
}

} // namespace mystl
