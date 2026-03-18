#pragma once

/**
 * @file pool_allocator.hpp
 * @brief Memory pool allocator implementation
 * @brief 内存池分配器实现
 * 
 * This is mySTL's memory pool allocator implementation, optimizing small object allocation based on memory pool manager.
 * References: GCC libstdc++'s pool_allocator and LLVM libc++'s __pool_alloc implementation.
 * 
 * 这是mySTL的内存池分配器实现，基于内存池管理器优化小对象分配。
 * 参考GCC libstdc++的pool_allocator和LLVM libc++的__pool_alloc实现。
 * 
 * Design features:
 * 1. Small objects (≤1024 bytes) use memory pool, reducing system calls and memory fragmentation
 * 2. Large objects (>1024 bytes) directly use system allocator
 * 3. Thread safety: Use thread-local storage, each thread has its own memory pool instance, avoiding lock contention
 * 4. Fully compatible with standard allocator interface, can replace standard allocator
 * 
 * 设计特点：
 * 1. 小对象（≤1024字节）使用内存池，减少系统调用和内存碎片
 * 2. 大对象（>1024字节）直接使用系统分配器
 * 3. 线程安全：使用线程局部存储，每个线程有自己的内存池实例，避免锁争用
 * 4. 与标准分配器接口完全兼容，可替换标准分配器使用
 * 
 * Comparison with official implementations:
 * - GCC libstdc++: Uses complex multi-level memory pool, supports multiple allocation strategies, uses thread-local storage for performance optimization
 * - LLVM libc++: Emphasizes exception safety and thread safety, fully compatible with standard allocator interface
 * - This implementation: Simplified version, uses thread-local storage for performance optimization, suitable for learning purposes
 * 
 * 与官方实现对比：
 * - GCC libstdc++: 使用复杂的多级内存池，支持多种分配策略，使用线程局部存储优化性能
 * - LLVM libc++: 强调异常安全和线程安全，与标准分配器接口完全兼容
 * - 本实现: 简化版本，使用线程局部存储优化性能，适合学习目的
 * 
 * Note: The memory pool allocator is stateful (is_always_equal = false_type),
 * which is different from the standard allocator (stateless), affecting container copy/move semantics.
 * 
 * 注意：内存池分配器是有状态的（is_always_equal = false_type），
 * 这与标准分配器（无状态）不同，影响容器复制/移动语义。
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/allocator.hpp
 * @see include/memory/detail/memory_pool.hpp
 * @see doc/文档/代码注释规范提示词.md
 */

#include <cstddef>      // for std::size_t, std::ptrdiff_t
#include <new>          // for std::bad_alloc
#include "allocator.hpp"
#include "detail/memory_pool.hpp"
#include "../utility/type_traits.hpp"
#include "../utility/move.hpp"   // for mystl::forward

namespace mystl {

// ============================================================================
// 主模板：内存池分配器
// ============================================================================

/**
 * @brief 内存池分配器模板类
 * 
 * 基于内存池的分配器实现，优化小对象分配性能。
 * 遵循C++11标准分配器接口，提供与标准分配器完全相同的API。
 * 
 * Memory pool-based allocator implementation, optimizing small object allocation performance.
 * Follows C++11 standard allocator interface, providing exactly the same API as standard allocator.
 * 
 * @tparam T 分配的元素类型 / Element type to allocate
 * 
 * 注意：与标准分配器的主要区别：
 * 1. is_always_equal = false_type（有状态分配器）
 * 2. propagate_on_container_copy_assignment = false_type（不传播拷贝赋值）
 * 3. 使用内存池优化小对象分配
 * 
 * Note: Main differences from standard allocator:
 * 1. is_always_equal = false_type (stateful allocator)
 * 2. propagate_on_container_copy_assignment = false_type (does not propagate on copy assignment)
 * 3. Uses memory pool to optimize small object allocation
 */
template<typename T>
class pool_allocator {
private:
    using Pool = detail::MemoryPool;  ///< 内存池类型别名
    
public:
    // ==================== 类型定义 (C++11要求) ====================
    using value_type = T;          ///< 分配的元素类型 / Element type to allocate
    using pointer = T*;            ///< 元素指针类型 / Pointer to element type
    using const_pointer = const T*; ///< 常量元素指针类型 / Const pointer to element type
    using reference = T&;          ///< 元素引用类型 / Reference to element type
    using const_reference = const T&; ///< 常量元素引用类型 / Const reference to element type
    using size_type = std::size_t; ///< 大小类型，用于表示元素数量 / Size type for representing number of elements
    using difference_type = std::ptrdiff_t; ///< 指针差异类型 / Pointer difference type
    
    // ==================== C++11新增类型定义 ====================
    using void_pointer = void*;    ///< void指针类型 / Void pointer type
    using const_void_pointer = const void*; ///< 常量void指针类型 / Const void pointer type
    
    // ==================== 传播特性 ====================
    /**
     * @brief 容器拷贝赋值时是否传播分配器
     * 
     * 设置为false_type，表示容器拷贝赋值时不传播分配器。
     * 这是内存池分配器与标准分配器的主要区别之一。
     * 标准分配器通常使用true_type（无状态，总是相等）。
     */
    using propagate_on_container_copy_assignment = false_type;
    
    /**
     * @brief 容器移动赋值时是否传播分配器
     * 
     * 设置为true_type，表示容器移动赋值时传播分配器。
     * 这允许移动操作转移内存池的所有权，提高性能。
     */
    using propagate_on_container_move_assignment = true_type;
    
    /**
     * @brief 容器交换时是否传播分配器
     * 
     * 设置为false_type，表示容器交换时不传播分配器。
     * 交换两个使用不同内存池的容器可能导致未定义行为。
     */
    using propagate_on_container_swap = false_type;
    
    /**
     * @brief 分配器是否总是相等
     * 
     * 设置为false_type，表示内存池分配器不是总是相等的。
     * 这是内存池分配器与标准分配器的主要区别。
     * 标准分配器使用true_type（无状态，所有实例相等）。
     */
    using is_always_equal = false_type;
    
    // ==================== 重新绑定机制 ====================
    /**
     * @brief 重新绑定模板
     * 
     * 允许容器从pool_allocator<T>获取pool_allocator<U>类型。
     * 例如：std::list<int, pool_allocator<int>>需要pool_allocator<std::_List_node<int>>。
     * 
     * 注意：这是C++11标准要求的部分，用于支持节点式容器。
     */
    template<typename U>
    struct rebind {
        using other = pool_allocator<U>;
    };
    
    // ==================== 构造函数和析构函数 ====================
    /**
     * @brief 默认构造函数
     * 
     * 构造一个使用全局内存池的分配器。
     * 所有默认构造的pool_allocator共享同一个全局内存池。
     */
    pool_allocator() noexcept = default;
    
    /**
     * @brief 拷贝构造函数
     * 
     * 默认实现，共享同一个全局内存池。
     */
    pool_allocator(const pool_allocator&) noexcept = default;
    
    /**
     * @brief 从其他类型分配器的转换构造函数
     * 
     * 允许从pool_allocator<U>构造pool_allocator<T>，用于重新绑定场景。
     * 所有pool_allocator实例共享同一个全局内存池。
     * 
     * Allows constructing pool_allocator<T> from pool_allocator<U>, used in rebinding scenarios.
     * All pool_allocator instances share the same global memory pool.
     * 
     * @tparam U 源分配器的元素类型 / Element type of source allocator
     */
    template<typename U>
    pool_allocator(const pool_allocator<U>&) noexcept {}
    
    /**
     * @brief 析构函数
     */
    ~pool_allocator() = default;
    
    // ==================== 核心内存管理接口 ====================
    
    /**
     * @brief 分配内存
     * @param n 要分配的元素数量
     * @return 指向分配内存的指针
     * @throw std::bad_alloc 如果内存分配失败
     * 
     * 分配策略：
     * 1. n == 0: 返回nullptr
     * 2. n > max_size(): 抛出std::bad_alloc
     * 3. 计算总字节数 = n * sizeof(T)
     * 4. 总字节数 > 1024: 使用系统分配器
     * 5. 总字节数 ≤ 1024: 使用内存池
     * 
     * 与官方实现对齐：遵循相同的异常安全保证。
     * 注意：内存池分配可能比系统分配更快，特别是对于小对象。
     */
    [[nodiscard]] pointer allocate(size_type n);
    
    /**
     * @brief 释放内存
     * @param p 要释放的内存指针
     * @param n 元素数量
     * 
     * 释放策略：
     * 1. p == nullptr: 空操作
     * 2. 计算总字节数 = n * sizeof(T)
     * 3. 总字节数 > 1024: 使用系统释放器
     * 4. 总字节数 ≤ 1024: 放回内存池重用
     * 
     * 注意：标记为noexcept，与标准分配器一致。
     * 小对象放回内存池而不是立即释放，提高后续分配性能。
     */
    void deallocate(pointer p, size_type n) noexcept;
    
    // ==================== 对象生命周期管理 ====================
    
    /**
     * @brief 构造对象
     * @param p 要构造对象的内存位置 / Memory location to construct object at
     * @param args 构造参数 / Construction arguments
     * 
     * 委托给基类allocator的construct函数。
     * 注意：C++17中此函数被弃用，建议使用allocator_traits::construct。
     * C++20中此成员函数被移除。
     * 
     * Delegates to base class allocator's construct function.
     * Note: This function is deprecated in C++17, use allocator_traits::construct instead.
     * This member function is removed in C++20.
     * 
     * 与官方实现对齐：使用完美转发保持参数的值类别。
     * Aligned with official implementation: uses perfect forwarding to preserve value category of arguments.
     * 
     * @tparam U 要构造的对象类型（可以是T或其他类型） / Type of object to construct (can be T or other type)
     * @tparam Args 参数包类型 / Parameter pack type
     */
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        allocator<T>().construct(p, forward<Args>(args)...);
    }
    
    /**
     * @brief 销毁对象
     * @param p 要销毁对象的指针 / Pointer to object to destroy
     * 
     * 委托给基类allocator的destroy函数。
     * 注意：C++17中此函数被弃用，建议使用allocator_traits::destroy。
     * C++20中此成员函数被移除。
     * 
     * Delegates to base class allocator's destroy function.
     * Note: This function is deprecated in C++17, use allocator_traits::destroy instead.
     * This member function is removed in C++20.
     * 
     * @tparam U 要销毁的对象类型（可以是T或其他类型） / Type of object to destroy (can be T or other type)
     */
    template<typename U>
    void destroy(U* p) {
        allocator<T>().destroy(p);
    }
    
    // ==================== 工具函数 ====================
    
    /**
     * @brief 返回理论上可分配的最大元素数量
     * @return 理论上可分配的最大元素数量
     * 
     * 计算size_type的最大值除以sizeof(T)。
     * 这是理论最大值，实际可能受系统内存和内存池限制。
     * 
     * 与官方实现对齐：使用相同的计算方法。
     */
    size_type max_size() const noexcept;
    
    /**
     * @brief 容器复制构造选择
     * @return 用于新容器的分配器
     * 
     * 当容器被复制构造时调用，返回用于新容器的分配器。
     * 对于内存池分配器，返回新实例而不是*this。
     * 这与标准分配器不同（标准分配器返回*this）。
     * 
     * 注意：这是C++11标准要求的部分。
     */
    pool_allocator select_on_container_copy_construction() const {
        return pool_allocator();  // 返回新实例，而不是*this
    }
    
    // ==================== 比较操作符 ====================
    // 注意：比较操作符应该作为全局函数提供，而不是成员函数
    // 这里不定义成员比较操作符，以避免与全局函数产生歧义
};

// ============================================================================
// 成员函数实现
// ============================================================================

/**
 * @brief allocate函数实现
 * 
 * 分配足够存储n个T类型对象的内存。
 * 实现细节：
 * 1. 检查请求大小是否超过max_size()
 * 2. 处理n==0的特殊情况（返回nullptr）
 * 3. 计算总字节数，决定使用内存池还是系统分配器
 * 4. 如果分配失败，抛出std::bad_alloc
 * 
 * 注意：遵循C++11标准异常安全要求。
 */
template<typename T>
typename pool_allocator<T>::pointer 
pool_allocator<T>::allocate(size_type n) {
    if (n > max_size()) {
        throw std::bad_alloc();
    }

    if (n == 0) {
        return nullptr;
    }

    // 计算总字节数
    std::size_t total_bytes = n * sizeof(T);
    
    // 使用线程局部内存池分配（避免锁争用）
    void* p = detail::get_thread_pool().allocate(total_bytes);
    if (!p) {
        throw std::bad_alloc();
    }
    
    return static_cast<pointer>(p);
}

/**
 * @brief deallocate函数实现
 * 
 * 释放之前分配的内存。
 * 实现细节：
 * 1. 检查指针是否为nullptr（空操作）
 * 2. 计算总字节数，决定使用内存池还是系统释放器
 * 3. 小对象放回内存池重用，大对象直接释放
 * 
 * 注意：函数标记为noexcept，与标准分配器一致。
 */
template<typename T>
void pool_allocator<T>::deallocate(pointer p, size_type n) noexcept {
    if (!p) return;
    
    // 计算总字节数
    std::size_t total_bytes = n * sizeof(T);
    
    // 使用线程局部内存池释放（避免锁争用）
    detail::get_thread_pool().deallocate(p, total_bytes);
}

/**
 * @brief max_size函数实现
 * 
 * 返回分配器理论上可以分配的最大元素数量。
 * 实现细节：
 * 1. 计算size_type的最大值除以sizeof(T)
 * 2. 确保不会发生算术溢出
 * 
 * 注意：这是理论最大值，实际可能受系统内存限制。
 */
template<typename T>
typename pool_allocator<T>::size_type
pool_allocator<T>::max_size() const noexcept {
    // 返回理论上可以分配的最大元素数量
    // 使用static_cast<size_type>(-1)获取size_type的最大值
    // 除以sizeof(T)得到最大元素数量
    return static_cast<size_type>(-1) / sizeof(T);
}

// ============================================================================
// 全局比较操作符
// ============================================================================

/**
 * @brief 全局相等比较操作符
 * 
 * 比较两个不同类型的分配器是否相等。
 * 实现细节：
 * 1. 内存池分配器总是返回true（简化实现）
 * 2. 实际实现可能需要更复杂的比较逻辑
 * 
 * 注意：这是C++11标准要求的全局函数，不是成员函数。
 * 简化实现返回true，实际可能需要检查是否使用同一个内存池。
 */
template<typename T1, typename T2>
bool operator==(const pool_allocator<T1>&, const pool_allocator<T2>&) noexcept {
    // 简化实现：所有pool_allocator实例被认为是相等的
    // 实际实现可能需要检查是否使用同一个内存池实例
    return true;
}

/**
 * @brief 全局不等比较操作符
 * 
 * 比较两个不同类型的分配器是否不相等。
 * 实现细节：
 * 1. 简单委托给operator==并取反
 * 
 * 注意：C++20中可以使用默认的operator!=，但为了兼容C++11提供显式实现。
 */
template<typename T1, typename T2>
bool operator!=(const pool_allocator<T1>& lhs, const pool_allocator<T2>& rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace mystl
