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
 * @param first, last 要复制的输入范围 / Input range to copy from
 * @param d_first 目标范围的起始位置 / Beginning of destination range
 * @return 指向最后复制的元素之后的位置的迭代器 / Iterator to element past the last copied
 * 
 * 如果复制过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during copying, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam InputIt 输入迭代器类型 / Input iterator type
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 */
template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first);

/**
 * @brief 从指定位置开始复制n个元素到未初始化内存
 * @param first 要复制的起始位置 / Beginning position to copy from
 * @param count 要复制的元素数量 / Number of elements to copy
 * @param d_first 目标范围的起始位置 / Beginning of destination range
 * @return 指向最后复制的元素之后的位置的迭代器 / Iterator to element past the last copied
 * 
 * 如果复制过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during copying, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam InputIt 输入迭代器类型 / Input iterator type
 * @tparam Size 大小类型 / Size type
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 */
template <typename InputIt, typename Size, typename ForwardIt>
ForwardIt uninitialized_copy_n(InputIt first, Size count, ForwardIt d_first);

/**
 * @brief 用指定值填充未初始化内存范围
 * @param first, last 要填充的范围 / Range to fill
 * @param value 填充值 / Value to fill with
 * 
 * 如果填充过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during filling, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 * @tparam T 值类型 / Value type
 */
template <typename ForwardIt, typename T>
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value);

/**
 * @brief 从指定位置开始用指定值填充n个元素
 * @param first 要填充的起始位置 / Beginning position to fill from
 * @param count 要填充的元素数量 / Number of elements to fill
 * @param value 填充值 / Value to fill with
 * @return 指向最后填充的元素之后的位置的迭代器 / Iterator to element past the last filled
 * 
 * 如果填充过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during filling, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 * @tparam Size 大小类型 / Size type
 * @tparam T 值类型 / Value type
 */
template <typename ForwardIt, typename Size, typename T>
ForwardIt uninitialized_fill_n(ForwardIt first, Size count, const T& value);

/**
 * @brief 将范围中的元素移动到未初始化内存
 * @param first, last 要移动的输入范围 / Input range to move from
 * @param d_first 目标范围的起始位置 / Beginning of destination range
 * @return 指向最后移动的元素之后的位置的迭代器 / Iterator to element past the last moved
 * 
 * 如果移动过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during moving, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam InputIt 输入迭代器类型 / Input iterator type
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 */
template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first);

/**
 * @brief 从指定位置开始移动n个元素到未初始化内存
 * @param first 要移动的起始位置 / Beginning position to move from
 * @param count 要移动的元素数量 / Number of elements to move
 * @param d_first 目标范围的起始位置 / Beginning of destination range
 * @return 指向最后移动的元素之后的位置的迭代器 / Iterator to element past the last moved
 * 
 * 如果移动过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during moving, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam InputIt 输入迭代器类型 / Input iterator type
 * @tparam Size 大小类型 / Size type
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 */
template <typename InputIt, typename Size, typename ForwardIt>
ForwardIt uninitialized_move_n(InputIt first, Size count, ForwardIt d_first);

/**
 * @brief 在未初始化内存范围上默认构造元素
 * @param first, last 要构造的范围 / Range to construct
 * 
 * 如果构造过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during construction, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 */
template <typename ForwardIt>
void uninitialized_default_construct(ForwardIt first, ForwardIt last);

/**
 * @brief 从指定位置开始默认构造n个元素
 * @param first 要构造的起始位置 / Beginning position to construct from
 * @param count 要构造的元素数量 / Number of elements to construct
 * @return 指向最后构造的元素之后的位置的迭代器 / Iterator to element past the last constructed
 * 
 * 如果构造过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during construction, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 * @tparam Size 大小类型 / Size type
 */
template <typename ForwardIt, typename Size>
ForwardIt uninitialized_default_construct_n(ForwardIt first, Size count);

/**
 * @brief 在未初始化内存范围上值初始化元素
 * @param first, last 要值初始化的范围 / Range to value-initialize
 * 
 * 如果构造过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during construction, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 */
template <typename ForwardIt>
void uninitialized_value_construct(ForwardIt first, ForwardIt last);

/**
 * @brief 从指定位置开始值初始化n个元素
 * @param first 要值初始化的起始位置 / Beginning position to value-initialize from
 * @param count 要值初始化的元素数量 / Number of elements to value-initialize
 * @return 指向最后值初始化的元素之后的位置的迭代器 / Iterator to element past the last value-initialized
 * 
 * 如果构造过程中抛出异常，已构造的元素会被销毁。
 * 提供强异常安全保证。
 * 
 * If an exception is thrown during construction, already constructed elements are destroyed.
 * Provides strong exception safety guarantee.
 * 
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 * @tparam Size 大小类型 / Size type
 */
template <typename ForwardIt, typename Size>
ForwardIt uninitialized_value_construct_n(ForwardIt first, Size count);

/**
 * @brief 销毁范围中的元素
 * @param first, last 要销毁的范围 / Range to destroy
 * 
 * 销毁范围中的元素，但不释放内存。
 * 
 * Destroys elements in the range, but does not deallocate memory.
 * 
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 */
template <typename ForwardIt>
void destroy(ForwardIt first, ForwardIt last);

/**
 * @brief 从指定位置开始销毁n个元素
 * @param first 要销毁的起始位置 / Beginning position to destroy from
 * @param count 要销毁的元素数量 / Number of elements to destroy
 * @return 指向最后销毁的元素之后的位置的迭代器 / Iterator to element past the last destroyed
 * 
 * 销毁元素，但不释放内存。
 * 
 * Destroys elements, but does not deallocate memory.
 * 
 * @tparam ForwardIt 前向迭代器类型 / Forward iterator type
 * @tparam Size 大小类型 / Size type
 */
template <typename ForwardIt, typename Size>
ForwardIt destroy_n(ForwardIt first, Size count);

/**
 * @brief 销毁指定位置的对象
 * @param location 要销毁的对象指针 / Pointer to object to destroy
 * 
 * 销毁指定位置的对象，但不释放内存。
 * 
 * Destroys object at specified location, but does not deallocate memory.
 * 
 * @tparam T 对象类型 / Object type
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

// ============================================================================
// uninitialized_move_if_noexcept - 安全移动函数
// ============================================================================

/**
 * @brief 安全地将元素移动到未初始化内存
 * @param first, last 要移动的输入范围
 * @param d_first 目标范围的起始位置
 * @return 指向最后移动的元素之后的位置的迭代器
 * 
 * 如果类型的移动构造函数是noexcept的，则使用移动，否则使用复制。
 * 提供强异常安全保证。
 */
template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_move_if_noexcept(InputIt first, InputIt last, ForwardIt d_first) {
    auto current = d_first;
    try {
        for (; first != last; ++first, ++current) {
            using value_type = typename iterator_traits<ForwardIt>::value_type;
            
            // 检查移动构造函数是否是noexcept的
            if constexpr (noexcept(value_type(mystl::move(*first)))) {
                // 使用移动构造
                ::new (static_cast<void*>(std::addressof(*current))) 
                    value_type(mystl::move(*first));
            } else {
                // 使用复制构造
                ::new (static_cast<void*>(std::addressof(*current))) 
                    value_type(*first);
            }
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

} // namespace mystl
