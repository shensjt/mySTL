#pragma once

/**
 * @file memory.hpp
 * @brief Memory management module header file
 * @brief 内存管理模块汇总头文件
 * 
 * This is the summary header file for mySTL memory management module, containing all memory-related functionality.
 * Users can include this header file to introduce all memory management components at once.
 * 
 * 这是mySTL内存管理模块的汇总头文件，包含所有内存相关的功能。
 * 用户可以通过包含此头文件一次性引入所有内存管理组件。
 * 
 * Contains the following components:
 * 1. allocator.hpp       - Standard allocator
 * 2. pool_allocator.hpp  - Memory pool allocator  
 * 3. uninitialized.hpp   - Uninitialized memory operations
 * 4. temporary_buffer.hpp - Temporary buffer management
 * 
 * 包含以下组件：
 * 1. allocator.hpp       - 标准分配器
 * 2. pool_allocator.hpp  - 内存池分配器  
 * 3. uninitialized.hpp   - 未初始化内存操作
 * 4. temporary_buffer.hpp - 临时缓冲区管理
 * 
 * Note: detail/memory_pool.hpp is internal implementation detail and should not be included directly.
 * 注意：detail/memory_pool.hpp是内部实现细节，不应直接包含。
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/allocator.hpp
 * @see include/memory/pool_allocator.hpp
 * @see include/memory/uninitialized.hpp
 * @see include/memory/temporary_buffer.hpp
 * @see doc/文档/代码注释规范提示词.md
 */

// Check C++ version
// 检查C++版本
// Ensure C++ compiler is being used
// 确保使用C++编译器
#ifndef __cplusplus
#error "mySTL requires C++"
#endif

// Ensure C++11 or later is being used
// 确保使用C++11或更高版本
#if __cplusplus < 201103L
#error "mySTL memory module requires C++11 or later"
#endif

// ============================================================================
// Include all memory management header files
// ============================================================================
// ============================================================================
// 包含所有内存管理头文件
// ============================================================================

// Allocators
// 分配器
// Standard allocator following C++11 specification
// 遵循C++11规范的标准分配器
#include "allocator.hpp"

// Memory pool allocator for small object optimization
// 用于小对象优化的内存池分配器
#include "pool_allocator.hpp"

// Memory operations
// 内存操作
// Uninitialized memory construction, copying, moving, filling, and destruction
// 未初始化内存的构造、复制、移动、填充和销毁
#include "uninitialized.hpp"

// Memory utilities
// 内存工具
// Temporary buffer management for algorithm workspace
// 用于算法工作空间的临时缓冲区管理
#include "temporary_buffer.hpp"

// ============================================================================
// Documentation comments
// ============================================================================
// ============================================================================
// 文档注释
// ============================================================================

/**
 * @namespace mystl
 * @brief mySTL namespace
 * @brief mySTL命名空间
 * 
 * Contains all mySTL components, including memory management module.
 * 
 * 包含所有mySTL组件，包括内存管理模块。
 */

// ============================================================================
// Module documentation
// ============================================================================
// ============================================================================
// 模块文档
// ============================================================================

/**
 * @defgroup memory Memory Management Module
 * @brief Provides memory allocation, management and operation functions
 * @brief 提供内存分配、管理和操作功能
 * 
 * The memory management module is one of the core components of mySTL, providing:
 * 
 * 内存管理模块是mySTL的核心组件之一，提供：
 * 
 * ## Allocators (分配器)
 * - @ref mystl::allocator : Standard allocator, following C++11 standard
 * - @ref mystl::allocator : 标准分配器，遵循C++11标准
 * - @ref mystl::pool_allocator : Memory pool allocator, optimized for small object allocation
 * - @ref mystl::pool_allocator : 内存池分配器，优化小对象分配
 * 
 * ## Memory Operations (内存操作)
 * - Construction, copying, moving, filling, and destruction of uninitialized memory
 * - 未初始化内存的构造、复制、移动、填充、销毁
 * - Exception-safe memory operations with strong exception safety guarantee
 * - 异常安全的内存操作，提供强异常安全保证
 * 
 * ## Memory Utilities (内存工具)
 * - Temporary buffer management for algorithm workspace
 * - 临时缓冲区管理，用于算法工作空间
 * - Thread-safe memory pool implementation
 * - 线程安全的内存池实现
 * 
 * ## Usage Example (使用示例)
 * @code
 * #include <memory/memory.hpp>
 * 
 * // Use standard allocator
 * // 使用标准分配器
 * mystl::allocator<int> alloc;
 * int* p = alloc.allocate(10);
 * 
 * // Use convenience functions to construct objects
 * // 使用便利函数构造对象
 * mystl::uninitialized_fill_n(p, 10, 42);
 * 
 * // Cleanup
 * // 清理
 * mystl::destroy_n(p, 10);
 * alloc.deallocate(p, 10);
 * 
 * // Use memory pool allocator
 * // 使用内存池分配器
 * mystl::pool_allocator<double> pool_alloc;
 * double* arr = pool_alloc.allocate(100);
 * // ... use array
 * // ... 使用数组
 * pool_alloc.deallocate(arr, 100);
 * @endcode
 */
