#pragma once

/**
 * @file uninitialized.hpp
 * @brief Uninitialized memory operation functions (C++11)
 * @brief 未初始化内存操作函数 (C++11)
 * 
 * This is mySTL's uninitialized memory operation function implementation, used to perform
 * construction, copying, moving, filling, and destruction operations on uninitialized memory regions.
 * 
 * 这是mySTL的未初始化内存操作函数实现，用于在未初始化的内存区域上
 * 执行构造、复制、移动、填充和销毁操作。
 * 
 * These functions are the foundation of container implementation, ensuring that resources do not leak in case of exceptions.
 * All functions provide strong exception safety guarantee.
 * 
 * 这些函数是容器实现的基础，确保在异常情况下资源不会泄漏。
 * 所有函数都提供强异常安全保证。
 * 
 * Reference implementations:
 * - GCC libstdc++: bits/stl_uninitialized.h
 * - LLVM libc++: __memory/uninitialized_algorithms.h
 * - MSVC STL: xutility
 * 
 * 参考实现：
 * - GCC libstdc++: bits/stl_uninitialized.h
 * - LLVM libc++: __memory/uninitialized_algorithms.h
 * - MSVC STL: xutility
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/memory.hpp
 * @see doc/文档/代码注释规范提示词.md
 */

#include <cstring>      // for std::memcpy
#include "../iterator/iterator_traits.hpp"
#include "../utility/type_traits.hpp"
#include "../utility/move.hpp"

namespace mystl {

// ============================================================================
// 基础函数声明
// ============================================================================

/**
 * @brief 将范围中的元素复制到未初始化内存
 * @param first, last 要复制的输入范围
 * @param d_first 目标范围的起始位置
 * @return 指向最后复制的元素之后的位置的迭代器
 * 
 * 如果复制过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 */
template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first);

/**
 * @brief 从指定位置开始复制n个元素到未初始化内存
 */
template <typename InputIt, typename Size, typename ForwardIt>
ForwardIt uninitialized_copy_n(InputIt first, Size count, ForwardIt d_first);

/**
 * @brief 用指定值填充未初始化内存范围
 */
template <typename ForwardIt, typename T>
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value);

/**
 * @brief 从指定位置开始用指定值填充n个元素
 */
template <typename ForwardIt, typename Size, typename T>
ForwardIt uninitialized_fill_n(ForwardIt first, Size count, const T& value);

/**
 * @brief 将范围中的元素移动到未初始化内存
 */
template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first);

/**
 * @brief 从指定位置开始移动n个元素到未初始化内存
 */
template <typename InputIt, typename Size, typename ForwardIt>
ForwardIt uninitialized_move_n(InputIt first, Size count, ForwardIt d_first);

/**
 * @brief 在未初始化内存范围上默认构造元素
 */
template <typename ForwardIt>
void uninitialized_default_construct(ForwardIt first, ForwardIt last);

/**
 * @brief 从指定位置开始默认构造n个元素
 */
template <typename ForwardIt, typename Size>
ForwardIt uninitialized_default_construct_n(ForwardIt first, Size count);

/**
 * @brief 在未初始化内存范围上值初始化元素
 */
template <typename ForwardIt>
void uninitialized_value_construct(ForwardIt first, ForwardIt last);

/**
 * @brief 从指定位置开始值初始化n个元素
 */
template <typename ForwardIt, typename Size>
ForwardIt uninitialized_value_construct_n(ForwardIt first, Size count);

/**
 * @brief 销毁范围中的元素
 */
template <typename ForwardIt>
void destroy(ForwardIt first, ForwardIt last);

/**
 * @brief 从指定位置开始销毁n个元素
 */
template <typename ForwardIt, typename Size>
ForwardIt destroy_n(ForwardIt first, Size count);

/**
 * @brief 销毁指定位置的对象
 */
template <typename T>
void destroy_at(T* location);

// ============================================================================
// 实现细节（放在detail命名空间）
// ============================================================================

namespace detail {

// 类型特征检测 - 使用mySTL的type_traits
template <typename T>
using is_trivially_copyable = typename mystl::is_trivially_copyable<T>;

// 简化实现：假设所有类型都需要显式析构
// 在实际实现中，应该使用编译器内部函数或完整的type_traits
template <typename T>
struct is_trivially_destructible : mystl::false_type {};

// 平凡可复制类型的优化版本
template <typename InputIt, typename ForwardIt,
          typename = typename std::enable_if<
              is_trivially_copyable<
                  typename iterator_traits<ForwardIt>::value_type
              >::value
          >::type>
ForwardIt uninitialized_copy_trivial(InputIt first, InputIt last, ForwardIt d_first) {
    if (first != last) {
        using value_type = typename iterator_traits<ForwardIt>::value_type;
        std::size_t n = static_cast<std::size_t>(std::distance(first, last));
        std::memcpy(std::addressof(*d_first), std::addressof(*first), 
                   n * sizeof(value_type));
        std::advance(d_first, n);
    }
    return d_first;
}

// 非平凡类型的通用版本
template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy_non_trivial(InputIt first, InputIt last, ForwardIt d_first) {
    auto current = d_first;
    try {
        for (; first != last; ++first, ++current) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            ::new (static_cast<void*>(std::addressof(*current))) value_type(*first);
        }
        return current;
    } catch (...) {
        for (; d_first != current; ++d_first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*d_first))->~value_type();
        }
        throw;
    }
}

// 平凡可析构类型的优化版本
template <typename ForwardIt>
void destroy_trivial(ForwardIt, ForwardIt) {
    // 平凡可析构类型不需要显式析构
    // 简化实现：总是调用非平凡版本
}

// 非平凡析构类型的通用版本
template <typename ForwardIt>
void destroy_non_trivial(ForwardIt first, ForwardIt last) {
    for (; first != last; ++first) {
        using value_type = typename iterator_traits<ForwardIt>::value_type;
        first->~value_type();
    }
}

} // namespace detail

// ============================================================================
// 公共接口实现
// ============================================================================

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first) {
    using value_type = typename iterator_traits<ForwardIt>::value_type;
    
    if constexpr (mystl::is_trivially_copyable<value_type>::value) {
        return detail::uninitialized_copy_trivial(first, last, d_first);
    } else {
        return detail::uninitialized_copy_non_trivial(first, last, d_first);
    }
}

template <typename InputIt, typename Size, typename ForwardIt>
ForwardIt uninitialized_copy_n(InputIt first, Size count, ForwardIt d_first) {
    auto current = d_first;
    try {
        for (Size i = 0; i < count; ++i, ++first, ++current) {
            ::new (static_cast<void*>(std::addressof(*current))) 
                typename iterator_traits<ForwardIt>::value_type(*first);
        }
        return current;
    } catch (...) {
        for (; d_first != current; ++d_first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*d_first))->~value_type();
        }
        throw;
    }
}

template <typename ForwardIt, typename T>
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) {
    auto current = first;
    try {
        for (; current != last; ++current) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            ::new (static_cast<void*>(std::addressof(*current))) value_type(value);
        }
    } catch (...) {
        for (; first != current; ++first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*first))->~value_type();
        }
        throw;
    }
}

template <typename ForwardIt, typename Size, typename T>
ForwardIt uninitialized_fill_n(ForwardIt first, Size count, const T& value) {
    auto current = first;
    try {
        for (Size i = 0; i < count; ++i, ++current) {
            ::new (static_cast<void*>(std::addressof(*current))) 
                typename iterator_traits<ForwardIt>::value_type(value);
        }
        return current;
    } catch (...) {
        for (; first != current; ++first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*first))->~value_type();
        }
        throw;
    }
}

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first) {
    auto current = d_first;
    try {
        for (; first != last; ++first, ++current) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            ::new (static_cast<void*>(std::addressof(*current))) value_type(mystl::move(*first));
        }
        return current;
    } catch (...) {
        for (; d_first != current; ++d_first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*d_first))->~value_type();
        }
        throw;
    }
}

template <typename InputIt, typename Size, typename ForwardIt>
ForwardIt uninitialized_move_n(InputIt first, Size count, ForwardIt d_first) {
    auto current = d_first;
    try {
        for (Size i = 0; i < count; ++i, ++first, ++current) {
            ::new (static_cast<void*>(std::addressof(*current))) 
                typename iterator_traits<ForwardIt>::value_type(mystl::move(*first));
        }
        return current;
    } catch (...) {
        for (; d_first != current; ++d_first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*d_first))->~value_type();
        }
        throw;
    }
}

template <typename ForwardIt>
void uninitialized_default_construct(ForwardIt first, ForwardIt last) {
    auto current = first;
    try {
        for (; current != last; ++current) {
            ::new (static_cast<void*>(std::addressof(*current))) 
                typename iterator_traits<ForwardIt>::value_type;
        }
    } catch (...) {
        for (; first != current; ++first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*first))->~value_type();
        }
        throw;
    }
}

template <typename ForwardIt, typename Size>
ForwardIt uninitialized_default_construct_n(ForwardIt first, Size count) {
    auto current = first;
    try {
        for (Size i = 0; i < count; ++i, ++current) {
            ::new (static_cast<void*>(std::addressof(*current))) 
                typename iterator_traits<ForwardIt>::value_type;
        }
        return current;
    } catch (...) {
        for (; first != current; ++first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*first))->~value_type();
        }
        throw;
    }
}

template <typename ForwardIt>
void uninitialized_value_construct(ForwardIt first, ForwardIt last) {
    auto current = first;
    try {
        for (; current != last; ++current) {
            ::new (static_cast<void*>(std::addressof(*current))) 
                typename iterator_traits<ForwardIt>::value_type();
        }
    } catch (...) {
        for (; first != current; ++first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*first))->~value_type();
        }
        throw;
    }
}

template <typename ForwardIt, typename Size>
ForwardIt uninitialized_value_construct_n(ForwardIt first, Size count) {
    auto current = first;
    try {
        for (Size i = 0; i < count; ++i, ++current) {
            ::new (static_cast<void*>(std::addressof(*current))) 
                typename iterator_traits<ForwardIt>::value_type();
        }
        return current;
    } catch (...) {
        for (; first != current; ++first) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            static_cast<value_type*>(std::addressof(*first))->~value_type();
        }
        throw;
    }
}

template <typename ForwardIt>
void destroy(ForwardIt first, ForwardIt last) {
    // 简化实现：总是调用非平凡析构
    // 在实际实现中，应该根据类型特征进行优化
    for (; first != last; ++first) {
        using value_type = typename iterator_traits<ForwardIt>::value_type;
        static_cast<value_type*>(std::addressof(*first))->~value_type();
    }
}

template <typename ForwardIt, typename Size>
ForwardIt destroy_n(ForwardIt first, Size count) {
    auto current = first;
    for (Size i = 0; i < count; ++i, ++current) {
        using value_type = typename iterator_traits<ForwardIt>::value_type;
        static_cast<value_type*>(std::addressof(*current))->~value_type();
    }
    return current;
}

template <typename T>
void destroy_at(T* location) {
    if (location) {
        location->~T();
    }
}

} // namespace mystl
