#pragma once

/**
 * @file allocator.hpp
 * @brief Standard allocator implementation (C++11)
 * @brief 标准分配器实现 (C++11)
 * 
 * This is mySTL's standard allocator implementation, following C++11 standard requirements.
 * Contains complete implementation of the allocator primary template and void specialization.
 * 
 * 这是mySTL的标准分配器实现，遵循C++11标准要求。
 * 包含了allocator主模板和void特化的完整实现。
 * 
 * Version notes:
 * - C++11: Basic implementation, includes construct/destroy member functions
 * - C++17: construct/destroy deprecated, adds aligned allocation support
 * - C++20: Removes construct/destroy member functions, adds allocation hints
 * 
 * 版本说明：
 * - C++11: 基础实现，包含construct/destroy成员函数
 * - C++17: construct/destroy被弃用，增加对齐分配支持
 * - C++20: 移除construct/destroy成员函数，增加分配提示
 * 
 * Note: This implementation is based on C++11, but notes differences in later versions.
 * 注意：本实现以C++11为基础，但在注释中说明了后续版本的差异。
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/memory.hpp
 * @see doc/文档/代码注释规范提示词.md
 */

#include <cstddef>      // for std::size_t, std::ptrdiff_t
#include <new>          // for std::bad_alloc, ::operator new/delete
#include "../utility/type_traits.hpp"
#include "../utility/move.hpp"   // for mystl::forward

namespace mystl {

// ============================================================================
// 主模板：通用分配器 (C++11标准实现)
// ============================================================================

/**
 * @brief 标准分配器模板类 (C++11)
 * 
 * 这是mySTL的默认分配器实现，遵循C++11标准要求。
 * 注意：C++17中construct/destroy被弃用，C++20中移除了这些成员函数。
 * 但在本学习项目中，我们保留这些函数以便理解分配器的完整工作原理。
 */
template<typename T>
class allocator {
public:
    // ==================== 类型定义 (C++11要求) ====================
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    // ==================== C++11新增类型定义 ====================
    using void_pointer = void*;
    using const_void_pointer = const void*;
    
    // 传播特性：控制分配器在容器操作中的行为
    using propagate_on_container_move_assignment = mystl::true_type;
    using propagate_on_container_copy_assignment = mystl::false_type;
    using propagate_on_container_swap = mystl::false_type;
    
    // 相等性特性：指示所有实例是否总是相等
    using is_always_equal = mystl::true_type;
    
    // ==================== 重新绑定机制 ====================
    /**
     * @brief 重新绑定模板 (C++11)
     * 
     * 允许容器从allocator<T>获取allocator<U>类型。
     * 例如：std::list<int, allocator<int>>需要allocator<std::_List_node<int>>。
     */
    template<typename U>
    struct rebind {
        using other = allocator<U>;
    };
    
    // ==================== 构造函数和析构函数 ====================
    allocator() noexcept = default;
    allocator(const allocator&) noexcept = default;
    
    /**
     * @brief 从其他类型分配器的转换构造函数
     * 
     * 允许从allocator<U>构造allocator<T>，用于重新绑定场景。
     */
    template<typename U>
    allocator(const allocator<U>&) noexcept {}
    
    ~allocator() = default;
    
    // ==================== 核心内存管理接口 ====================
    
    /**
     * @brief 分配内存 (C++11)
     * @param n 要分配的元素数量
     * @return 指向分配内存的指针
     * @throw std::bad_alloc 如果内存分配失败
     * 
     * 注意：C++17增加了对齐分配重载：
     * pointer allocate(size_type n, std::align_val_t align);
     */
    [[nodiscard]] pointer allocate(size_type n);
    
    /**
     * @brief 释放内存 (C++11)
     * @param p 要释放的内存指针
     * @param n 元素数量（通常被忽略，但标准要求提供）
     * 
     * 注意：C++17增加了对齐释放重载：
     * void deallocate(pointer p, size_type n, std::align_val_t align);
     */
    void deallocate(pointer p, size_type n) noexcept;
    
    // ==================== 对象生命周期管理 ====================
    
    /**
     * @brief 构造对象 (C++11)
     * @param p 要构造对象的内存位置
     * @param args 构造参数
     * 
     * 注意：C++17中此函数被弃用，建议使用allocator_traits::construct。
     * C++20中此成员函数被移除。
     */
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args);
    
    /**
     * @brief 销毁对象 (C++11)
     * @param p 要销毁对象的指针
     * 
     * 注意：C++17中此函数被弃用，建议使用allocator_traits::destroy。
     * C++20中此成员函数被移除。
     */
    template<typename U>
    void destroy(U* p);
    
    // ==================== 工具函数 ====================
    
    /**
     * @brief 返回理论上可分配的最大元素数量 (C++11)
     */
    size_type max_size() const noexcept;
    
    /**
     * @brief 容器复制构造选择 (C++11)
     * 
     * 当容器被复制构造时调用，返回用于新容器的分配器。
     * 默认实现返回*this（浅拷贝）。
     */
    allocator select_on_container_copy_construction() const {
        return *this;
    }
    
    // ==================== 比较操作符 ====================
    // 注意：比较操作符应该作为全局函数提供，而不是成员函数
    // 这里不定义成员比较操作符，以避免与全局函数产生歧义
    
    // ==================== C++20扩展 ====================
    
    /**
     * @brief 带提示的内存分配 (C++20)
     * @param n 要分配的元素数量
     * @param hint 分配提示（通常被忽略）
     * @return 指向分配内存的指针
     * 
     * 注意：这是C++20新增的可选接口，用于优化分配位置。
     * 标准允许实现忽略hint参数。
     */
    [[nodiscard]] pointer allocate(size_type n, const_void_pointer hint);
};


// ============================================================================
// 特化：void类型的分配器 (C++11要求)
// ============================================================================

/**
 * @brief void类型分配器特化 (C++11)
 * 
 * C++11标准要求提供allocator<void>特化，用于类型擦除场景。
 * 注意：void特化没有reference类型，也没有内存管理成员函数，
 * 因为void对象不能被分配、构造、销毁或引用。
 * 
 * 在C++17中，std::allocator<void>被弃用，建议使用std::allocator_traits。
 * 在C++20中，std::allocator<void>被移除。
 * 但为了兼容性和学习目的，我们仍然提供此特化。
 */
template<>
class allocator<void> {
public:
    // ==================== 类型定义 ====================
    // 注意：void特化没有reference和const_reference类型
    using value_type = void;
    using pointer = void*;
    using const_pointer = const void*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    // ==================== C++11新增类型定义 ====================
    using void_pointer = void*;
    using const_void_pointer = const void*;
    
    // 传播特性
    using propagate_on_container_move_assignment = mystl::true_type;
    using propagate_on_container_copy_assignment = mystl::false_type;
    using propagate_on_container_swap = mystl::false_type;
    
    // 相等性特性
    using is_always_equal = mystl::true_type;
    
    // ==================== 重新绑定机制 ====================
    template<typename U>
    struct rebind {
        using other = allocator<U>;
    };
    
    // ==================== 构造函数和析构函数 ====================
    allocator() noexcept = default;
    allocator(const allocator&) noexcept = default;
    
    /**
     * @brief 从其他类型分配器的转换构造函数
     */
    template<typename U>
    allocator(const allocator<U>&) noexcept {}
    
    ~allocator() = default;
    
    // ==================== 注意 ====================
    // 没有allocate、deallocate、construct、destroy成员函数
    // 因为void对象不能被分配、释放、构造或销毁
    // 这是void特化与主模板的主要区别
};


// ============================================================================
// 成员函数实现
// ============================================================================

/**
 * @brief allocate函数实现 (C++11)
 * 
 * 分配足够存储n个T类型对象的内存。
 * 实现细节：
 * 1. 检查请求大小是否超过max_size()
 * 2. 处理n==0的特殊情况（返回nullptr）
 * 3. 使用全局operator new分配内存
 * 4. 如果分配失败，抛出std::bad_alloc
 * 
 * 注意：C++17标准增加了对齐分配重载，但本实现仅支持C++11。
 */
template<typename T>
typename allocator<T>::pointer allocator<T>::allocate(size_type n) {
    if (n > max_size()) {
        throw std::bad_alloc();
    }

    if (n == 0) {
        return nullptr;
    }

    // 使用全局operator new分配内存
    // 注意：这里没有考虑对齐要求，C++17增加了对齐分配支持
    if (auto p = static_cast<pointer>(::operator new(n * sizeof(T)))) {
        return p;
    }

    throw std::bad_alloc();
}

/**
 * @brief deallocate函数实现 (C++11)
 * 
 * 释放之前分配的内存。
 * 实现细节：
 * 1. 检查指针是否为nullptr（空操作）
 * 2. 使用全局operator delete释放内存
 * 3. n参数被忽略，但标准要求提供（用于调试或特殊分配器）
 * 
 * 注意：函数标记为noexcept，因为内存释放不应抛出异常。
 */
template<typename T>
void allocator<T>::deallocate(pointer p, size_type n) noexcept {
    if (p) {
        // 忽略n参数，标准operator delete不需要知道大小
        // 但某些自定义分配器可能使用此信息
        ::operator delete(static_cast<void*>(p));
    }
}

/**
 * @brief construct函数实现 (C++11)
 * 
 * 在已分配的内存上构造对象。
 * 实现细节：
 * 1. 使用placement new和完美转发构造对象
 * 
 * 注意：C++17中此函数被标记为deprecated，C++20中移除。
 * 现代C++应使用allocator_traits::construct。
 * 
 * 注意：标准库分配器允许构造任何类型，不仅仅是T。
 * 这是为了支持节点式容器（如list、map）的需要。
 */
template<typename T>
template<typename U, typename... Args>
void allocator<T>::construct(U* p, Args&&... args) {
    // 使用placement new构造对象
    // 注意：这里使用完美转发保持参数的值类别
    ::new (static_cast<void*>(p)) U(mystl::forward<Args>(args)...);
}

/**
 * @brief destroy函数实现 (C++11)
 * 
 * 销毁对象但不释放内存。
 * 实现细节：
 * 1. 显式调用析构函数
 * 
 * 注意：C++17中此函数被标记为deprecated，C++20中移除。
 * 现代C++应使用allocator_traits::destroy。
 * 
 * 对于平凡析构类型，此调用是空操作，但为了通用性仍然调用。
 * 
 * 注意：标准库分配器允许销毁任何类型，不仅仅是T。
 * 这是为了支持节点式容器（如list、map）的需要。
 */
template<typename T>
template<typename U>
void allocator<T>::destroy(U* p) {
    // 显式调用析构函数
    // 注意：对于void类型，此函数不应被实例化
    p->~U();
}

/**
 * @brief max_size函数实现 (C++11)
 * 
 * 返回分配器理论上可以分配的最大元素数量。
 * 实现细节：
 * 1. 计算size_type的最大值除以sizeof(T)
 * 2. 确保不会发生算术溢出
 * 
 * 注意：这是理论最大值，实际可能受系统内存限制。
 * C++17中可以使用std::numeric_limits，但为了兼容C++11使用传统方法。
 */
template<typename T>
typename allocator<T>::size_type
allocator<T>::max_size() const noexcept {
    // 返回理论上可以分配的最大元素数量
    // 使用static_cast<size_type>(-1)获取size_type的最大值
    // 除以sizeof(T)得到最大元素数量
    return static_cast<size_type>(-1) / sizeof(T);
}

/**
 * @brief allocate with hint函数实现 (C++20可选)
 * 
 * 带分配提示的内存分配。
 * 实现细节：
 * 1. hint参数通常被忽略，但可用于优化分配位置
 * 2. 简单委托给无hint版本的allocate
 * 
 * 注意：这是C++20新增的可选接口。
 * 标准允许实现忽略hint，但某些平台特定分配器可能利用此信息。
 */
template<typename T>
typename allocator<T>::pointer
allocator<T>::allocate(size_type n, const_void_pointer hint) {
    // hint参数通常被忽略，但可以用于优化
    // 例如，某些内存分配器可能尝试在hint附近分配
    (void)hint;  // 避免未使用参数警告
    
    // 简单委托给无hint版本
    // 在实际实现中，可以根据hint进行优化
    return allocate(n);
}

// ============================================================================
// 全局比较操作符
// ============================================================================

/**
 * @brief 全局相等比较操作符 (C++11)
 * 
 * 比较两个不同类型的分配器是否相等。
 * 实现细节：
 * 1. 标准分配器总是返回true，因为它们是空类且无状态
 * 2. 有状态分配器需要重载此操作符
 * 
 * 注意：这是C++11标准要求的全局函数，不是成员函数。
 * 所有标准分配器实例都被认为是相等的。
 */
template<typename T1, typename T2>
bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept {
    return true;  // 标准分配器总是相等（无状态）
}

/**
 * @brief 全局不等比较操作符 (C++11)
 * 
 * 比较两个不同类型的分配器是否不相等。
 * 实现细节：
 * 1. 简单委托给operator==并取反
 * 
 * 注意：C++20中可以使用默认的operator!=，但为了兼容C++11提供显式实现。
 */
template<typename T1, typename T2>
bool operator!=(const allocator<T1>& lhs, const allocator<T2>& rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace mystl
