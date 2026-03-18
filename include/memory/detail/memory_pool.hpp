#pragma once

/**
 * @file memory_pool.hpp
 * @brief Memory pool manager implementation (mimicking GCC libstdc++ implementation)
 * @brief 内存池管理器实现（模仿GCC libstdc++实现）
 * 
 * This is mySTL's memory pool manager implementation, mimicking GCC libstdc++'s __pool_alloc.
 * Reference implementation: GCC libstdc++-v3/include/ext/pool_allocator.h
 * 
 * 这是mySTL的内存池管理器实现，模仿GCC libstdc++的__pool_alloc。
 * 参考实现：GCC libstdc++-v3/include/ext/pool_allocator.h
 * 
 * Design principles:
 * 1. Pre-allocate large memory blocks (8KB) to reduce system calls
 * 2. Split large blocks into fixed-size small blocks (8, 16, 32, 64, 128, 256, 512, 1024 bytes)
 * 3. Use free lists to manage released small blocks for fast reuse
 * 4. Large objects (>1024 bytes) directly use system allocator
 * 5. Thread safety: Use thread-local storage, each thread has its own memory pool instance
 * 6. Strict memory alignment: Use max_align_t for alignment
 * 
 * 设计原理：
 * 1. 预分配大块内存（8KB），减少系统调用
 * 2. 将大块分割为固定大小的小块（8, 16, 32, 64, 128, 256, 512, 1024字节）
 * 3. 使用空闲链表管理已释放的小块，实现快速重用
 * 4. 大对象（>1024字节）直接使用系统分配器
 * 5. 线程安全：使用线程局部存储，每个线程有自己的内存池实例
 * 6. 严格内存对齐：使用max_align_t对齐
 * 
 * Comparison with official implementations:
 * - GCC libstdc++: Uses complex multi-level memory pools, supports multiple allocation strategies, uses thread-local storage for performance optimization
 * - LLVM libc++: Emphasizes exception safety and thread safety, fully compatible with standard allocator interface
 * - This implementation: Mimics GCC implementation, uses thread-local storage, strict memory alignment
 * 
 * 与官方实现对比：
 * - GCC libstdc++: 使用复杂的多级内存池，支持多种分配策略，使用线程局部存储优化性能
 * - LLVM libc++: 强调异常安全和线程安全，与标准分配器接口完全兼容
 * - 本实现: 模仿GCC实现，使用线程局部存储，严格内存对齐
 * 
 * @author shensjt
 * @date 2026-03-18
 */

#include <cstddef>      // for std::size_t, std::max_align_t
#include <cstdint>      // for std::uintptr_t
#include <new>          // for std::bad_alloc, ::operator new/delete
#include <cstdlib>      // for std::aligned_alloc (C++17)
#include "../../utility/type_traits.hpp"

namespace mystl {
namespace detail {

// ============================================================================
// Memory pool configuration (mimicking GCC implementation)
// ============================================================================
// ============================================================================
// 内存池配置（模仿GCC实现）
// ============================================================================

/**
 * @brief Memory pool configuration structure
 * @brief 内存池配置结构体
 * 
 * Mimics GCC libstdc++ configuration parameters.
 * 模仿GCC libstdc++的配置参数。
 */
struct PoolConfig {
    static constexpr std::size_t num_pools = 8;        ///< Number of pools (different sizes) / 池的数量（不同大小的池）
    static constexpr std::size_t block_size = 8192;    ///< Size of each memory block (8KB) / 每个内存块大小（8KB）
    static constexpr std::size_t max_small_object = 1024; ///< Maximum size of small objects (1024 bytes) / 小对象最大大小（1024字节）
    static constexpr std::size_t alignment = alignof(std::max_align_t); ///< Memory alignment requirement / 内存对齐要求
    
    /**
     * @brief Pool sizes array (mimicking GCC)
     * @brief 池大小数组（模仿GCC）
     * 
     * Defines 8 pools of different sizes, each managing fixed-size memory blocks.
     * Uses power-of-two sizes to reduce memory fragmentation.
     * 
     * 定义8个不同大小的池，每个池管理固定大小的内存块。
     * 使用2的幂次方大小，减少内存碎片。
     */
    static constexpr std::size_t pool_sizes[num_pools] = {
        8, 16, 32, 64, 128, 256, 512, 1024
    };
    
    /**
     * @brief Calculate aligned size
     * @brief 计算对齐后的尺寸
     * @param n Original size / 原始尺寸
     * @return Aligned size / 对齐后的尺寸
     * 
     * Mimics GCC implementation to ensure memory is aligned to max_align_t.
     * 模仿GCC实现，确保内存按max_align_t对齐。
     */
    static constexpr std::size_t aligned_size(std::size_t n) noexcept {
        return (n + alignment - 1) & ~(alignment - 1);
    }
    
    /**
     * @brief Check if pointer is aligned
     * @brief 检查是否是对齐的指针
     * @param p Pointer / 指针
     * @return Whether aligned / 是否对齐
     */
    static bool is_aligned(void* p) noexcept {
        return (reinterpret_cast<std::uintptr_t>(p) & (alignment - 1)) == 0;
    }
};

// ============================================================================
// Free block node (mimicking GCC implementation)
// ============================================================================
// ============================================================================
// 空闲块节点（模仿GCC实现）
// ============================================================================

/**
 * @brief Free block node union
 * @brief 空闲块节点联合体
 * 
 * Mimics GCC implementation, uses union to save space.
 * 模仿GCC实现，使用联合体节省空间。
 */
union FreeNode {
    FreeNode* next;    ///< Pointer to next free block (when block is free) / 指向下一个空闲块的指针（当块空闲时）
    char data[1];      ///< Data storage area (when block is allocated) / 数据存储区域（当块已分配时）
    
    /**
     * @brief Get aligned node size
     * @brief 获取对齐后的节点大小
     */
    static constexpr std::size_t aligned_node_size(std::size_t size) noexcept {
        // Ensure node size is at least large enough to hold a pointer
        // 确保节点大小至少能容纳一个指针
        constexpr std::size_t min_size = sizeof(FreeNode*);
        return size < min_size ? min_size : size;
    }
};

// ============================================================================
// 内存池管理器类（模仿GCC实现）
// ============================================================================

/**
 * @brief 内存池管理器类
 * 
 * 模仿GCC libstdc++的__pool_alloc实现。
 * 每个线程有自己的内存池实例，避免锁争用。
 */
class MemoryPool {
private:
    // ==================== 成员变量 ====================
    
    FreeNode* free_lists_[PoolConfig::num_pools];  ///< 空闲链表数组
    char* current_block_;                          ///< 当前正在使用的内存块
    std::size_t remaining_;                        ///< 当前块剩余字节数
    
    // 跟踪所有分配的大块内存（模仿GCC的块管理）
    struct BlockHeader {
        BlockHeader* next;
        std::size_t size;
    };
    BlockHeader* allocated_blocks_;                ///< 所有分配的大块内存链表
    
    // ==================== 私有方法 ====================
    
    /**
     * @brief 根据请求大小获取池索引（模仿GCC）
     */
    std::size_t get_pool_index(std::size_t n) const noexcept;
    
    /**
     * @brief 从内存池分配内存（模仿GCC）
     */
    void* allocate_from_pool(std::size_t n);
    
    /**
     * @brief 分配新内存块（模仿GCC）
     */
    void* allocate_new_block(std::size_t size);
    
    /**
     * @brief 分配对齐的内存（模仿GCC对齐处理）
     */
    void* allocate_aligned(std::size_t size);
    
    /**
     * @brief 释放对齐的内存
     */
    void deallocate_aligned(void* p, std::size_t size) noexcept;
    
    /**
     * @brief 将内存块添加到空闲链表（模仿GCC）
     */
    void add_block_to_free_list(void* block, std::size_t size, std::size_t index);
    
public:
    // ==================== 构造函数和析构函数 ====================
    
    MemoryPool() noexcept;
    ~MemoryPool();
    
    // 禁用拷贝
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    // ==================== 核心接口 ====================
    
    /**
     * @brief 分配内存（模仿GCC接口）
     */
    void* allocate(std::size_t n);
    
    /**
     * @brief 释放内存（模仿GCC接口）
     */
    void deallocate(void* p, std::size_t n) noexcept;
    
    // ==================== 工具函数 ====================
    
    /**
     * @brief 获取最大对齐要求
     */
    static constexpr std::size_t max_alignment() noexcept {
        return PoolConfig::alignment;
    }
};

// ============================================================================
// 内存池管理器实现（内联）
// ============================================================================

// 构造函数
inline MemoryPool::MemoryPool() noexcept 
    : current_block_(nullptr), remaining_(0), allocated_blocks_(nullptr) {
    // 初始化所有空闲链表为空
    for (auto& list : free_lists_) {
        list = nullptr;
    }
}

// 析构函数
inline MemoryPool::~MemoryPool() {
    // 释放所有分配的大块内存（模仿GCC的清理逻辑）
    BlockHeader* current = allocated_blocks_;
    while (current) {
        BlockHeader* next = current->next;
        deallocate_aligned(current, current->size);
        current = next;
    }
    
    // 注意：不释放free_lists_中的节点，因为它们指向已释放的块
}

// 获取池索引
inline std::size_t MemoryPool::get_pool_index(std::size_t n) const noexcept {
    // 模仿GCC实现：找到第一个大于等于n的池大小
    for (std::size_t i = 0; i < PoolConfig::num_pools; ++i) {
        if (n <= PoolConfig::pool_sizes[i]) {
            return i;
        }
    }
    return PoolConfig::num_pools;  // 表示大对象
}

// 分配对齐的内存（简化版，模仿GCC实际实现）
inline void* MemoryPool::allocate_aligned(std::size_t size) {
    // GCC实际实现通常使用operator new，它已经保证对齐
    // 对于大多数平台，operator new返回max_align_t对齐的内存
    
    void* ptr = ::operator new(size);
    if (!ptr) {
        throw std::bad_alloc();
    }
    
    // 验证对齐（调试用）
    #ifndef NDEBUG
    if (!PoolConfig::is_aligned(ptr)) {
        // 如果不对齐，这是一个严重错误
        ::operator delete(ptr);
        throw std::bad_alloc();
    }
    #endif
    
    return ptr;
}

// 释放对齐的内存
inline void MemoryPool::deallocate_aligned(void* p, std::size_t /*size*/) noexcept {
    if (!p) return;
    ::operator delete(p);
}

// 分配内存
inline void* MemoryPool::allocate(std::size_t n) {
    if (n == 0) {
        return nullptr;
    }
    
    // 对齐请求大小（模仿GCC）
    std::size_t aligned_n = PoolConfig::aligned_size(n);
    
    // 大对象直接分配（使用对齐分配）
    if (aligned_n > PoolConfig::max_small_object) {
        return allocate_aligned(aligned_n);
    }
    
    // 小对象使用内存池
    return allocate_from_pool(aligned_n);
}

// 从内存池分配
inline void* MemoryPool::allocate_from_pool(std::size_t n) {
    std::size_t index = get_pool_index(n);
    
    // 从空闲链表获取（模仿GCC）
    if (FreeNode* node = free_lists_[index]) {
        free_lists_[index] = node->next;
        return static_cast<void*>(node);
    }
    
    // 空闲链表为空，分配新块
    return allocate_new_block(PoolConfig::pool_sizes[index]);
}

// 分配新内存块
inline void* MemoryPool::allocate_new_block(std::size_t size) {
    // 如果当前块剩余空间不足，分配新块
    if (remaining_ < size) {
        // 分配新的大块（模仿GCC的块分配）
        std::size_t block_size = PoolConfig::block_size;
        void* new_block = allocate_aligned(block_size);
        
        // 将新块添加到跟踪链表
        BlockHeader* header = static_cast<BlockHeader*>(new_block);
        header->next = allocated_blocks_;
        header->size = block_size;
        allocated_blocks_ = header;
        
        // 更新当前块指针（跳过BlockHeader）
        current_block_ = static_cast<char*>(new_block) + sizeof(BlockHeader);
        remaining_ = block_size - sizeof(BlockHeader);
    }
    
    // 从当前块分配
    void* result = current_block_;
    current_block_ += size;
    remaining_ -= size;
    
    return result;
}

// 释放内存
inline void MemoryPool::deallocate(void* p, std::size_t n) noexcept {
    if (!p) return;
    
    // 对齐请求大小
    std::size_t aligned_n = PoolConfig::aligned_size(n);
    
    // 大对象直接释放
    if (aligned_n > PoolConfig::max_small_object) {
        deallocate_aligned(p, aligned_n);
        return;
    }
    
    // 小对象放回空闲链表
    std::size_t index = get_pool_index(aligned_n);
    FreeNode* node = static_cast<FreeNode*>(p);
    node->next = free_lists_[index];
    free_lists_[index] = node;
}

// 将内存块添加到空闲链表
inline void MemoryPool::add_block_to_free_list(void* block, std::size_t size, std::size_t index) {
    // 将一个大块分割成多个小块，添加到空闲链表
    char* current = static_cast<char*>(block);
    std::size_t block_size = PoolConfig::pool_sizes[index];
    std::size_t num_blocks = size / block_size;
    
    FreeNode* first = nullptr;
    FreeNode* last = nullptr;
    
    for (std::size_t i = 0; i < num_blocks; ++i) {
        FreeNode* node = reinterpret_cast<FreeNode*>(current);
        node->next = nullptr;
        
        if (last) {
            last->next = node;
            last = node;
        } else {
            first = last = node;
        }
        
        current += block_size;
    }
    
    // 将链表连接到空闲链表
    if (first) {
        if (free_lists_[index]) {
            last->next = free_lists_[index];
        }
        free_lists_[index] = first;
    }
}

// ============================================================================
// 线程局部内存池获取函数（模仿GCC）
// ============================================================================

/**
 * @brief 获取线程局部内存池实例
 * 
 * 模仿GCC实现，每个线程有自己的内存池实例。
 */
inline MemoryPool& get_thread_pool() {
    // 每个线程有自己的内存池实例
    static thread_local MemoryPool thread_pool;
    return thread_pool;
}

} // namespace detail
} // namespace mystl
