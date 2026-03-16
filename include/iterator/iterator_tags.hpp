#pragma once

#include "../utility/type_traits.hpp"

/**
 * @file iterator_tags.hpp
 * @brief Iterator category tags and related type traits
 * @brief 迭代器类别标签及相关类型特征
 * 
 * This file defines the standard C++ iterator category tags used to identify
 * the capabilities and characteristics of iterators. These tags form an
 * inheritance hierarchy representing capability inclusion, from the most
 * restrictive (input_iterator_tag) to the most capable (contiguous_iterator_tag).
 * 
 * 本文件定义了用于标识迭代器能力和特征的标准C++迭代器类别标签。
 * 这些标签形成了一个表示能力包含关系的继承层次结构，从最受限制的
 * (input_iterator_tag) 到能力最强的 (contiguous_iterator_tag)。
 * 
 * The file also provides comprehensive type traits for working with iterator
 * tags, including tag conversion checks, capability analysis, and utility
 * functions for tag manipulation in template metaprogramming.
 * 
 * 本文件还提供了用于处理迭代器标签的全面类型特征，包括标签转换检查、
 * 能力分析以及在模板元编程中操作标签的实用函数。
 * 
 * @author mySTL shensjt
 * @date 2026-03-17
 * 
 * @see iterator_traits.hpp
 * @see iterator_functions.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/iterator_tags
 */

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

/**
 * @brief Input iterator tag
 * @brief 输入迭代器标签
 * 
 * @details Represents iterators that can only read sequentially.
 * Supports:
 * - Element reading (*it)
 * - Forward movement (++it)
 * - Equality comparison (==, !=)
 * 
 * @details 表示只能顺序读取的迭代器。
 * 支持：
 * - 元素读取 (*it)
 * - 向前移动 (++it)
 * - 相等性比较 (==, !=)
 * 
 * @note Typical use: reading from input streams
 * @note 典型用途：从输入流读取
 * 
 * @note Limitation: can only traverse once
 * @note 限制：只能遍历一次
 */
struct input_iterator_tag {};

/**
 * @brief Output iterator tag
 * @brief 输出迭代器标签
 * 
 * @details Represents iterators that can only write sequentially.
 * Supports:
 * - Element writing (*it = value)
 * - Forward movement (++it)
 * 
 * @details 表示只能顺序写入的迭代器。
 * 支持：
 * - 元素写入 (*it = value)
 * - 向前移动 (++it)
 * 
 * @note Typical use: writing to output streams
 * @note 典型用途：写入输出流
 * 
 * @note Output iterator does not participate in the inheritance hierarchy
 *       as it has incompatible capabilities with input iterators.
 * @note 输出迭代器不参与继承层次结构，因为其能力与输入迭代器不兼容。
 */
struct output_iterator_tag {};

/**
 * @brief Forward iterator tag
 * @brief 前向迭代器标签
 * 
 * @details Inherits from input_iterator_tag, represents iterators that:
 * - Support all input iterator operations
 * - Can traverse the same range multiple times
 * - Can save iterator state
 * 
 * @details 继承自 input_iterator_tag，表示具有以下特性的迭代器：
 * - 支持所有输入迭代器操作
 * - 可以多次遍历同一范围
 * - 可以保存迭代器状态
 * 
 * @note Typical use: singly linked lists
 * @note 典型用途：单向链表
 */
struct forward_iterator_tag : public input_iterator_tag {};

/**
 * @brief Bidirectional iterator tag
 * @brief 双向迭代器标签
 * 
 * @details Inherits from forward_iterator_tag, represents iterators that:
 * - Support all forward iterator operations
 * - Can move backward (--it)
 * 
 * @details 继承自 forward_iterator_tag，表示具有以下特性的迭代器：
 * - 支持所有前向迭代器操作
 * - 可以向后移动 (--it)
 * 
 * @note Typical use: doubly linked lists
 * @note 典型用途：双向链表
 */
struct bidirectional_iterator_tag : public forward_iterator_tag {};

/**
 * @brief Random access iterator tag
 * @brief 随机访问迭代器标签
 * 
 * @details Inherits from bidirectional_iterator_tag, represents iterators that:
 * - Support all bidirectional iterator operations
 * - Support random access (it + n, it[n])
 * - Support distance computation (it1 - it2)
 * - Support relational comparisons (<, >, <=, >=)
 * 
 * @details 继承自 bidirectional_iterator_tag，表示具有以下特性的迭代器：
 * - 支持所有双向迭代器操作
 * - 支持随机访问 (it + n, it[n])
 * - 支持距离计算 (it1 - it2)
 * - 支持关系比较 (<, >, <=, >=)
 * 
 * @note Typical use: arrays, vectors
 * @note 典型用途：数组、向量
 * 
 * @note Performance: constant time access to any position
 * @note 性能：常数时间访问任意位置
 */
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

/**
 * @brief Contiguous iterator tag (C++20)
 * @brief 连续迭代器标签 (C++20)
 * 
 * @details Inherits from random_access_iterator_tag, represents iterators that:
 * - Support all random access iterator operations
 * - Elements are stored contiguously in memory
 * 
 * @details 继承自 random_access_iterator_tag，表示具有以下特性的迭代器：
 * - 支持所有随机访问迭代器操作
 * - 元素在内存中连续存储
 * 
 * @note Typical use: arrays, vectors, strings
 * @note 典型用途：数组、向量、字符串
 * 
 * @note C++20 addition to the iterator category hierarchy
 * @note C++20 对迭代器类别层次结构的补充
 */
struct contiguous_iterator_tag : public random_access_iterator_tag {};

} // namespace mystl

namespace mystl {

/**
 * @brief Type alias for input iterator tag
 * @brief 输入迭代器标签的类型别名
 */
using input_iterator = input_iterator_tag;

/**
 * @brief Type alias for output iterator tag
 * @brief 输出迭代器标签的类型别名
 */
using output_iterator = output_iterator_tag;

/**
 * @brief Type alias for forward iterator tag
 * @brief 前向迭代器标签的类型别名
 */
using forward_iterator = forward_iterator_tag;

/**
 * @brief Type alias for bidirectional iterator tag
 * @brief 双向迭代器标签的类型别名
 */
using bidirectional_iterator = bidirectional_iterator_tag;

/**
 * @brief Type alias for random access iterator tag
 * @brief 随机访问迭代器标签的类型别名
 */
using random_access_iterator = random_access_iterator_tag;

/**
 * @brief Type alias for contiguous iterator tag (C++20)
 * @brief 连续迭代器标签的类型别名 (C++20)
 */
using contiguous_iterator = contiguous_iterator_tag;

/**
 * @brief Type trait to check if a type is an iterator tag
 * @brief 检查类型是否为迭代器标签的类型特征
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 * 
 * @details Inherits from true_type if T is one of the standard iterator tags,
 * otherwise inherits from false_type.
 * 
 * @details 如果T是标准迭代器标签之一，则继承自 true_type，
 * 否则继承自 false_type。
 */
template<typename T>
struct is_iterator_tag : false_type {};

/**
 * @brief Specialization for input_iterator_tag
 * @brief input_iterator_tag 的特化
 */
template<>
struct is_iterator_tag<input_iterator_tag> : true_type {};

/**
 * @brief Specialization for output_iterator_tag
 * @brief output_iterator_tag 的特化
 */
template<>
struct is_iterator_tag<output_iterator_tag> : true_type {};

/**
 * @brief Specialization for forward_iterator_tag
 * @brief forward_iterator_tag 的特化
 */
template<>
struct is_iterator_tag<forward_iterator_tag> : true_type {};

/**
 * @brief Specialization for bidirectional_iterator_tag
 * @brief bidirectional_iterator_tag 的特化
 */
template<>
struct is_iterator_tag<bidirectional_iterator_tag> : true_type {};

/**
 * @brief Specialization for random_access_iterator_tag
 * @brief random_access_iterator_tag 的特化
 */
template<>
struct is_iterator_tag<random_access_iterator_tag> : true_type {};

/**
 * @brief Specialization for contiguous_iterator_tag
 * @brief contiguous_iterator_tag 的特化
 */
template<>
struct is_iterator_tag<contiguous_iterator_tag> : true_type {};

/**
 * @brief Variable template version of is_iterator_tag
 * @brief is_iterator_tag 的变量模板版本
 * 
 * @tparam T Type to check
 * @tparam T 要检查的类型
 * 
 * @details Convenient compile-time boolean value indicating whether T is an iterator tag.
 * 
 * @details 方便的编译时布尔值，指示T是否为迭代器标签。
 */
template<typename T>
inline constexpr bool is_iterator_tag_v = is_iterator_tag<T>::value;

// ==================== Extended Features ====================
// ==================== 扩展功能 ====================

/**
 * @brief Type trait to check if one iterator tag can be converted to another
 * @brief 检查一个迭代器标签是否可以转换为另一个的类型特征
 * 
 * @tparam From Source iterator tag type
 * @tparam From 源迭代器标签类型
 * @tparam To Target iterator tag type
 * @tparam To 目标迭代器标签类型
 * 
 * @details Checks if From tag can be converted to To tag through inheritance hierarchy.
 * For example: random_access_iterator_tag can be converted to input_iterator_tag,
 * but input_iterator_tag cannot be converted to random_access_iterator_tag.
 * 
 * @details 检查From标签是否可以通过继承层次结构转换为To标签。
 * 例如：random_access_iterator_tag可以转换为input_iterator_tag，
 * 但input_iterator_tag不能转换为random_access_iterator_tag。
 * 
 * @note Output iterator tags have special handling as they don't participate
 *       in the normal inheritance hierarchy.
 * @note 输出迭代器标签有特殊处理，因为它们不参与正常的继承层次结构。
 */
template<typename From, typename To>
struct is_iterator_tag_convertible : integral_constant<bool,
    // From can be converted to To if:
    // 1. To is a base class of From (inheritance relationship)
    // 2. From and To are the same
    // 3. Special case: output_iterator_tag can only convert to output_iterator_tag
    (is_base_of_v<To, From> && !is_same_v<To, output_iterator_tag>) ||
    is_same_v<From, To>
> {};

/**
 * @brief Specialization for output_iterator_tag to output_iterator_tag conversion
 * @brief output_iterator_tag 到 output_iterator_tag 转换的特化
 */
template<>
struct is_iterator_tag_convertible<output_iterator_tag, output_iterator_tag> : true_type {};

/**
 * @brief Specialization for conversion to output_iterator_tag
 * @brief 转换为 output_iterator_tag 的特化
 * 
 * @tparam From Source iterator tag type
 * @tparam From 源迭代器标签类型
 * 
 * @note Only output_iterator_tag can convert to output_iterator_tag
 * @note 只有 output_iterator_tag 可以转换为 output_iterator_tag
 */
template<typename From>
struct is_iterator_tag_convertible<From, output_iterator_tag> : false_type {};

/**
 * @brief Specialization for conversion from output_iterator_tag
 * @brief 从 output_iterator_tag 转换的特化
 * 
 * @tparam To Target iterator tag type
 * @tparam To 目标迭代器标签类型
 * 
 * @note output_iterator_tag cannot convert to any other iterator tag
 * @note output_iterator_tag 不能转换为任何其他迭代器标签
 */
template<typename To>
struct is_iterator_tag_convertible<output_iterator_tag, To> : false_type {};

/**
 * @brief Variable template version of is_iterator_tag_convertible
 * @brief is_iterator_tag_convertible 的变量模板版本
 * 
 * @tparam From Source iterator tag type
 * @tparam From 源迭代器标签类型
 * @tparam To Target iterator tag type
 * @tparam To 目标迭代器标签类型
 * 
 * @details Convenient compile-time boolean value indicating whether From tag
 * can be converted to To tag.
 * 
 * @details 方便的编译时布尔值，指示From标签是否可以转换为To标签。
 */
template<typename From, typename To>
inline constexpr bool is_iterator_tag_convertible_v = 
    is_iterator_tag_convertible<From, To>::value;

/**
 * @brief Type trait to compute the common iterator tag of two tags
 * @brief 计算两个迭代器标签的公共标签的类型特征
 * 
 * @tparam Tag1 First iterator tag type
 * @tparam Tag1 第一个迭代器标签类型
 * @tparam Tag2 Second iterator tag type
 * @tparam Tag2 第二个迭代器标签类型
 * 
 * @details Returns the more general (base class) tag between two tags.
 * Used to determine the common operation capabilities of two iterators.
 * 
 * @details 返回两个标签中更通用（基类）的标签。
 * 用于确定两个迭代器的公共操作能力。
 * 
 * @note Special handling for output_iterator_tag: if either tag is
 *       output_iterator_tag, the result is output_iterator_tag.
 * @note 对 output_iterator_tag 的特殊处理：如果任一标签是
 *       output_iterator_tag，则结果为 output_iterator_tag。
 */
template<typename Tag1, typename Tag2>
struct common_iterator_tag {
private:
    // Handle output_iterator_tag special case
    // 处理 output_iterator_tag 特殊情况
    static constexpr bool tag1_is_output = is_same_v<Tag1, output_iterator_tag>;
    static constexpr bool tag2_is_output = is_same_v<Tag2, output_iterator_tag>;
    
public:
    using type = conditional_t<
        tag1_is_output || tag2_is_output,
        // If either tag is output_iterator_tag, return output_iterator_tag
        // 如果任一标签是 output_iterator_tag，返回 output_iterator_tag
        output_iterator_tag,
        // Otherwise return the more general tag (base class in inheritance hierarchy)
        // Note: more specific tags inherit from more general tags
        // e.g., random_access_iterator_tag inherits from bidirectional_iterator_tag
        // 否则返回更通用的标签（继承关系中的基类）
        // 注意：更具体的标签继承自更通用的标签
        // 例如：random_access_iterator_tag 继承自 bidirectional_iterator_tag
        conditional_t<
            is_base_of_v<Tag2, Tag1>, Tag2,  // Tag1 is more specific, Tag2 is more general
            conditional_t<
                is_base_of_v<Tag1, Tag2>, Tag1,  // Tag2 is more specific, Tag1 is more general
                input_iterator_tag  // No inheritance relationship, return most general input_iterator_tag
            >
        >
    >;
};

/**
 * @brief Type alias for common_iterator_tag::type
 * @brief common_iterator_tag::type 的类型别名
 * 
 * @tparam Tag1 First iterator tag type
 * @tparam Tag1 第一个迭代器标签类型
 * @tparam Tag2 Second iterator tag type
 * @tparam Tag2 第二个迭代器标签类型
 * 
 * @details Convenient alias for accessing the common tag type.
 * 
 * @details 访问公共标签类型的方便别名。
 */
template<typename Tag1, typename Tag2>
using common_iterator_tag_t = typename common_iterator_tag<Tag1, Tag2>::type;

/**
 * @brief Type trait to analyze capabilities of an iterator tag
 * @brief 分析迭代器标签能力的类型特征
 * 
 * @tparam Tag Iterator tag type to analyze
 * @tparam Tag 要分析的迭代器标签类型
 * 
 * @details Provides compile-time checks for operations supported by an iterator tag.
 * Includes basic capabilities, combined capabilities, and performance characteristics.
 * 
 * @details 提供对迭代器标签支持的操作的编译时检查。
 * 包括基本能力、组合能力和性能特征。
 * 
 * @note This trait is useful for template metaprogramming and SFINAE.
 * @note 此特征对于模板元编程和SFINAE很有用。
 */
template<typename Tag>
struct iterator_tag_capabilities {
    static_assert(is_iterator_tag_v<Tag>, "Tag must be an iterator tag");
    
    // Basic capabilities / 基本能力
    static constexpr bool is_input = 
        is_same_v<Tag, input_iterator_tag> || 
        is_base_of_v<input_iterator_tag, Tag>;
    
    static constexpr bool is_output = 
        is_same_v<Tag, output_iterator_tag>;
    
    static constexpr bool is_forward = 
        is_same_v<Tag, forward_iterator_tag> || 
        is_base_of_v<forward_iterator_tag, Tag>;
    
    static constexpr bool is_bidirectional = 
        is_same_v<Tag, bidirectional_iterator_tag> || 
        is_base_of_v<bidirectional_iterator_tag, Tag>;
    
    static constexpr bool is_random_access = 
        is_same_v<Tag, random_access_iterator_tag> || 
        is_base_of_v<random_access_iterator_tag, Tag>;
    
    static constexpr bool is_contiguous = 
        is_same_v<Tag, contiguous_iterator_tag> || 
        is_base_of_v<contiguous_iterator_tag, Tag>;
    
    // Combined capabilities / 组合能力
    static constexpr bool can_read = is_input;
    static constexpr bool can_write = is_output;
    static constexpr bool can_move_forward = is_input || is_output || is_forward;
    static constexpr bool can_move_backward = is_bidirectional;
    static constexpr bool can_jump = is_random_access;
    static constexpr bool is_contiguous_memory = is_contiguous;
    
    // Performance characteristics / 性能特征
    static constexpr bool has_constant_time_access = is_random_access;
    static constexpr bool has_constant_time_distance = is_random_access;
    static constexpr bool supports_multipass = is_forward;
};

/**
 * @brief Get the string name of an iterator tag (for debugging)
 * @brief 获取迭代器标签的字符串名称（用于调试）
 * 
 * @tparam Tag Iterator tag type
 * @tparam Tag 迭代器标签类型
 * 
 * @return String representation of the iterator tag
 * @return 迭代器标签的字符串表示
 * 
 * @details Returns a human-readable string name for the iterator tag.
 * Useful for debugging, logging, and error messages.
 * 
 * @details 返回迭代器标签的人类可读字符串名称。
 * 对于调试、日志记录和错误消息很有用。
 */
template<typename Tag>
constexpr const char* iterator_tag_name() {
    static_assert(is_iterator_tag_v<Tag>, "Tag must be an iterator tag");
    
    if constexpr (is_same_v<Tag, input_iterator_tag>) {
        return "input_iterator";
    } else if constexpr (is_same_v<Tag, output_iterator_tag>) {
        return "output_iterator";
    } else if constexpr (is_same_v<Tag, forward_iterator_tag>) {
        return "forward_iterator";
    } else if constexpr (is_same_v<Tag, bidirectional_iterator_tag>) {
        return "bidirectional_iterator";
    } else if constexpr (is_same_v<Tag, random_access_iterator_tag>) {
        return "random_access_iterator";
    } else if constexpr (is_same_v<Tag, contiguous_iterator_tag>) {
        return "contiguous_iterator";
    } else {
        return "unknown_iterator_tag";
    }
}

/**
 * @brief Get a detailed description of an iterator tag
 * @brief 获取迭代器标签的详细描述
 * 
 * @tparam Tag Iterator tag type
 * @tparam Tag 迭代器标签类型
 * 
 * @return Detailed description of the iterator tag
 * @return 迭代器标签的详细描述
 * 
 * @details Returns a detailed human-readable description of the iterator tag.
 * Useful for documentation, error messages, and educational purposes.
 * 
 * @details 返回迭代器标签的详细人类可读描述。
 * 对于文档、错误消息和教育目的很有用。
 */
template<typename Tag>
constexpr const char* iterator_tag_description() {
    static_assert(is_iterator_tag_v<Tag>, "Tag must be an iterator tag");
    
    if constexpr (is_same_v<Tag, input_iterator_tag>) {
        return "Input iterator: can read sequentially, single-pass";
    } else if constexpr (is_same_v<Tag, output_iterator_tag>) {
        return "Output iterator: can write sequentially, single-pass";
    } else if constexpr (is_same_v<Tag, forward_iterator_tag>) {
        return "Forward iterator: can read sequentially, multi-pass";
    } else if constexpr (is_same_v<Tag, bidirectional_iterator_tag>) {
        return "Bidirectional iterator: can read forward and backward, multi-pass";
    } else if constexpr (is_same_v<Tag, random_access_iterator_tag>) {
        return "Random access iterator: can access any element in constant time";
    } else if constexpr (is_same_v<Tag, contiguous_iterator_tag>) {
        return "Contiguous iterator: elements stored contiguously in memory (C++20)";
    } else {
        return "Unknown iterator tag";
    }
}

/**
 * @brief Type trait to upgrade an iterator tag to a desired category
 * @brief 将迭代器标签升级到所需类别的类型特征
 * 
 * @tparam CurrentTag Current iterator tag type
 * @tparam CurrentTag 当前迭代器标签类型
 * @tparam DesiredTag Desired iterator tag type to upgrade to
 * @tparam DesiredTag 要升级到的所需迭代器标签类型
 * 
 * @details Upgrades the iterator tag to the desired category if possible.
 * Used for type conversions in template metaprogramming.
 * 
 * @details 如果可能，将迭代器标签升级到所需类别。
 * 用于模板元编程中的类型转换。
 * 
 * @note The upgrade operation checks if DesiredTag can be converted to CurrentTag.
 * This means the upgrade is safe because DesiredTag provides all capabilities of CurrentTag.
 * @note 升级操作检查DesiredTag是否可以转换为CurrentTag。
 * 这意味着升级是安全的，因为DesiredTag提供了CurrentTag的所有能力。
 */
template<typename CurrentTag, typename DesiredTag>
struct upgrade_iterator_tag {
    static_assert(is_iterator_tag_v<CurrentTag>, "CurrentTag must be an iterator tag");
    static_assert(is_iterator_tag_v<DesiredTag>, "DesiredTag must be an iterator tag");
    
    using type = conditional_t<
        is_iterator_tag_convertible_v<DesiredTag, CurrentTag>,
        DesiredTag,  // DesiredTag can be converted to CurrentTag (safe upgrade)
        CurrentTag   // Cannot safely upgrade, keep as is
    >;
};

/**
 * @brief Type alias for upgrade_iterator_tag::type
 * @brief upgrade_iterator_tag::type 的类型别名
 * 
 * @tparam CurrentTag Current iterator tag type
 * @tparam CurrentTag 当前迭代器标签类型
 * @tparam DesiredTag Desired iterator tag type to upgrade to
 * @tparam DesiredTag 要升级到的所需迭代器标签类型
 * 
 * @details Convenient alias for accessing the upgraded tag type.
 * 
 * @details 访问升级后的标签类型的方便别名。
 */
template<typename CurrentTag, typename DesiredTag>
using upgrade_iterator_tag_t = typename upgrade_iterator_tag<CurrentTag, DesiredTag>::type;

/**
 * @brief Type trait to downgrade an iterator tag to a minimum category
 * @brief 将迭代器标签降级到最小类别的类型特征
 * 
 * @tparam CurrentTag Current iterator tag type
 * @tparam CurrentTag 当前迭代器标签类型
 * @tparam MinimumTag Minimum iterator tag type to downgrade to
 * @tparam MinimumTag 要降级到的最小迭代器标签类型
 * 
 * @details Downgrades the iterator tag to the minimum category if possible.
 * Used to ensure type safety in generic algorithms.
 * 
 * @details 如果可能，将迭代器标签降级到最小类别。
 * 用于确保通用算法中的类型安全。
 * 
 * @note The downgrade operation checks if CurrentTag can be converted to MinimumTag.
 * This ensures that the downgraded tag provides at least the capabilities of MinimumTag.
 * @note 降级操作检查CurrentTag是否可以转换为MinimumTag。
 * 这确保降级后的标签至少提供MinimumTag的能力。
 */
template<typename CurrentTag, typename MinimumTag>
struct downgrade_iterator_tag {
    static_assert(is_iterator_tag_v<CurrentTag>, "CurrentTag must be an iterator tag");
    static_assert(is_iterator_tag_v<MinimumTag>, "MinimumTag must be an iterator tag");
    
    using type = conditional_t<
        is_iterator_tag_convertible_v<CurrentTag, MinimumTag>,
        MinimumTag,  // CurrentTag can be converted to MinimumTag (downgrade)
        CurrentTag   // Cannot downgrade, keep as is
    >;
};

/**
 * @brief Type alias for downgrade_iterator_tag::type
 * @brief downgrade_iterator_tag::type 的类型别名
 * 
 * @tparam CurrentTag Current iterator tag type
 * @tparam CurrentTag 当前迭代器标签类型
 * @tparam MinimumTag Minimum iterator tag type to downgrade to
 * @tparam MinimumTag 要降级到的最小迭代器标签类型
 * 
 * @details Convenient alias for accessing the downgraded tag type.
 * 
 * @details 访问降级后的标签类型的方便别名。
 */
template<typename CurrentTag, typename MinimumTag>
using downgrade_iterator_tag_t = typename downgrade_iterator_tag<CurrentTag, MinimumTag>::type;

} // namespace mystl
