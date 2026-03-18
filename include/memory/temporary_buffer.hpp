#pragma once

/**
 * @file temporary_buffer.hpp
 * @brief Temporary buffer management functions (C++11)
 * @brief 临时缓冲区管理函数 (C++11)
 * 
 * This is mySTL's temporary buffer management function implementation, providing temporary workspace for algorithms.
 * These functions allow algorithms to efficiently acquire and release temporary memory when needed,
 * without needing to know specific allocator details.
 * 
 * 这是mySTL的临时缓冲区管理函数实现，用于为算法提供临时工作空间。
 * 这些函数允许算法在需要临时内存时高效地获取和释放内存，
 * 而不需要知道具体的分配器细节。
 * 
 * Reference implementations:
 * - GCC libstdc++: bits/stl_tempbuf.h
 * - LLVM libc++: __memory/temporary_buffer.h
 * - MSVC STL: xutility
 * 
 * 参考实现：
 * - GCC libstdc++: bits/stl_tempbuf.h
 * - LLVM libc++: __memory/temporary_buffer.h
 * - MSVC STL: xutility
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/memory.hpp
 * @see doc/文档/代码注释规范提示词.md
 */

#include <cstddef>      // for std::size_t, std::ptrdiff_t
#include <new>          // for operator new, operator delete
#include <limits>       // for std::numeric_limits
#include "../utility/type_traits.hpp"
#include "../utility/pair.hpp"

namespace mystl {

// ============================================================================
// 基础函数声明
// ============================================================================

/**
 * @brief 获取临时缓冲区
 * @tparam T 元素类型
 * @param count 请求的元素数量
 * @return mystl::pair<T*, std::ptrdiff_t> 分配的缓冲区和实际大小
 * 
 * 尝试分配至少能容纳count个T类型对象的临时内存。
 * 如果分配失败，可能返回少于count的内存。
 * 如果完全无法分配，返回{nullptr, 0}。
 * 
 * 保证不抛出异常（noexcept）。
 */
template <typename T>
mystl::pair<T*, std::ptrdiff_t> get_temporary_buffer(std::ptrdiff_t count) noexcept;

/**
 * @brief 释放临时缓冲区
 * @tparam T 元素类型
 * @param p 要释放的缓冲区指针
 * 
 * 释放由get_temporary_buffer分配的内存。
 * p必须是由get_temporary_buffer返回的指针。
 * 
 * 保证不抛出异常（noexcept）。
 */
template <typename T>
void return_temporary_buffer(T* p) noexcept;

// ============================================================================
// RAII包装器类
// ============================================================================

/**
 * @brief 临时缓冲区RAII包装器
 * @tparam T 元素类型
 * 
 * 提供临时缓冲区的自动资源管理。
 * 构造时获取缓冲区，析构时自动释放。
 * 支持移动语义，禁止拷贝。
 */
template <typename T>
class temporary_buffer {
public:
    // ==================== 类型定义 ====================
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::ptrdiff_t;
    using difference_type = std::ptrdiff_t;
    
    // ==================== 构造函数和析构函数 ====================
    
    /**
     * @brief 构造函数
     * @param count 请求的缓冲区大小（元素数量）
     * 
     * 构造时尝试获取指定大小的临时缓冲区。
     * 如果获取失败，缓冲区为空。
     */
    explicit temporary_buffer(size_type count) noexcept;
    
    /**
     * @brief 析构函数
     * 
     * 自动释放持有的缓冲区。
     */
    ~temporary_buffer() noexcept;
    
    // ==================== 禁止拷贝 ====================
    temporary_buffer(const temporary_buffer&) = delete;
    temporary_buffer& operator=(const temporary_buffer&) = delete;
    
    // ==================== 移动语义 ====================
    
    /**
     * @brief 移动构造函数
     */
    temporary_buffer(temporary_buffer&& other) noexcept;
    
    /**
     * @brief 移动赋值运算符
     */
    temporary_buffer& operator=(temporary_buffer&& other) noexcept;
    
    // ==================== 访问接口 ====================
    
    /**
     * @brief 获取缓冲区指针
     */
    pointer data() const noexcept { return data_; }
    
    /**
     * @brief 获取缓冲区大小
     */
    size_type size() const noexcept { return size_; }
    
    /**
     * @brief 检查缓冲区是否为空
     */
    bool empty() const noexcept { return size_ == 0; }
    
    /**
     * @brief 布尔转换运算符
     */
    explicit operator bool() const noexcept { return data_ != nullptr; }
    
    /**
     * @brief 获取起始迭代器
     */
    pointer begin() const noexcept { return data_; }
    
    /**
     * @brief 获取结束迭代器
     */
    pointer end() const noexcept { return data_ + size_; }
    
    /**
     * @brief 元素访问（不检查边界）
     */
    reference operator[](size_type n) const { return data_[n]; }
    
private:
    pointer data_;      ///< 缓冲区指针
    size_type size_;    ///< 缓冲区大小（元素数量）
};

// ============================================================================
// 实现细节（放在detail命名空间）
// ============================================================================

namespace detail {

/**
 * @brief 检查乘法是否溢出
 * @param count 元素数量
 * @param size 元素大小
 * @return 是否溢出
 */
inline bool multiply_overflow(std::ptrdiff_t count, std::size_t size) {
    // 检查count是否为负数
    if (count < 0) {
        return true;
    }
    
    // 检查乘法是否溢出
    if (size == 0) {
        return count != 0;  // 0 * count = 0，除非count为0
    }
    
    // 检查是否超过ptrdiff_t的最大值
    constexpr std::ptrdiff_t max_ptrdiff = std::numeric_limits<std::ptrdiff_t>::max();
    if (count > max_ptrdiff / static_cast<std::ptrdiff_t>(size)) {
        return true;
    }
    
    return false;
}

/**
 * @brief 指数退避分配策略
 * @tparam T 元素类型
 * @param count 请求的元素数量
 * @return 分配的缓冲区和实际大小
 * 
 * 实现指数退避策略：如果请求的大小分配失败，
 * 尝试分配一半的大小，直到成功或大小为0。
 */
template <typename T>
mystl::pair<T*, std::ptrdiff_t> allocate_with_backoff(std::ptrdiff_t count) noexcept {
    if (count <= 0) {
        return {nullptr, 0};
    }
    
    std::ptrdiff_t try_count = count;
    
    while (try_count > 0) {
        // 检查乘法溢出
        if (multiply_overflow(try_count, sizeof(T))) {
            constexpr std::ptrdiff_t max_ptrdiff = std::numeric_limits<std::ptrdiff_t>::max();
            try_count = max_ptrdiff / sizeof(T);
            if (try_count <= 0) {
                break;
            }
        }
        
        std::size_t bytes = static_cast<std::size_t>(try_count) * sizeof(T);
        
        if (bytes == 0) {
            // 大小为零，无法分配
            break;
        }
        
        // 尝试分配内存
        try {
            if (void* ptr = ::operator new(bytes, std::nothrow)) {
                // operator new保证正确对齐
                return mystl::pair<T*, std::ptrdiff_t>(static_cast<T*>(ptr), try_count);
            }
        } catch (...) {
            // operator new(nothrow)不应抛出异常，但为了安全起见
        }
        
        // 分配失败，尝试一半大小
        try_count /= 2;
    }
    
    return mystl::pair<T*, std::ptrdiff_t>(nullptr, 0);
}

} // namespace detail

// ============================================================================
// 公共接口实现
// ============================================================================

template <typename T>
mystl::pair<T*, std::ptrdiff_t> get_temporary_buffer(std::ptrdiff_t count) noexcept {
    return detail::allocate_with_backoff<T>(count);
}

template <typename T>
void return_temporary_buffer(T* p) noexcept {
    if (p) {
        ::operator delete(p);
    }
}

// ============================================================================
// temporary_buffer成员函数实现
// ============================================================================

template <typename T>
temporary_buffer<T>::temporary_buffer(size_type count) noexcept
    : data_(nullptr), size_(0) {
    auto buf = get_temporary_buffer<T>(count);
    data_ = buf.first;
    size_ = buf.second;
}

template <typename T>
temporary_buffer<T>::~temporary_buffer() noexcept {
    if (data_) {
        return_temporary_buffer(data_);
    }
}

template <typename T>
temporary_buffer<T>::temporary_buffer(temporary_buffer&& other) noexcept
    : data_(other.data_), size_(other.size_) {
    other.data_ = nullptr;
    other.size_ = 0;
}

template <typename T>
temporary_buffer<T>& temporary_buffer<T>::operator=(temporary_buffer&& other) noexcept {
    if (this != &other) {
        // 释放当前缓冲区
        if (data_) {
            return_temporary_buffer(data_);
        }
        
        // 接管其他缓冲区的资源
        data_ = other.data_;
        size_ = other.size_;
        
        // 清空其他缓冲区
        other.data_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

} // namespace mystl
