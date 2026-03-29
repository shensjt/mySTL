# 内存池管理器详解

## 概述

`memory_pool.hpp` 是内存池分配器的核心实现，模仿 GCC libstdc++ 的 `__pool_alloc` 实现。它提供了一个高效的内存池管理器，专门优化小对象的内存分配性能，通过预分配大块内存、分割成固定大小的小块、使用空闲链表管理等技术，显著减少系统调用和内存碎片。

## 设计原理

### 1. 核心设计思想
- **预分配策略**：一次性分配 8KB 大块内存，减少 `malloc`/`free` 调用
- **固定大小池**：8 个不同大小的池（8, 16, 32, 64, 128, 256, 512, 1024 字节）
- **空闲链表管理**：已释放的内存块放入空闲链表，实现快速重用
- **线程局部存储**：每个线程有自己的内存池实例，避免锁争用
- **严格内存对齐**：使用 `max_align_t` 对齐，确保兼容性

### 2. 与 GCC libstdc++ 对比

| 特性 | GCC libstdc++ | 本实现 |
|------|--------------|--------|
| 池大小 | 8 个固定大小池 | 相同（8, 16, ..., 1024） |
| 块大小 | 8KB | 相同（8KB） |
| 对齐要求 | `max_align_t` | 相同 |
| 线程安全 | 线程局部存储 | 相同 |
| 大对象处理 | 直接系统分配 | 相同（>1024 字节） |
| 实现复杂度 | 高（多级优化） | 中等（简化版） |

## 核心实现

### 1. 配置结构体
```cpp
struct PoolConfig {
    static constexpr std::size_t num_pools = 8;        // 池的数量
    static constexpr std::size_t block_size = 8192;    // 每个内存块大小（8KB）
    static constexpr std::size_t max_small_object = 1024; // 小对象最大大小
    static constexpr std::size_t alignment = alignof(std::max_align_t);
    
    // 池大小数组（2的幂次方）
    static constexpr std::size_t pool_sizes[num_pools] = {
        8, 16, 32, 64, 128, 256, 512, 1024
    };
};
```

### 2. 空闲块节点设计
```cpp
union FreeNode {
    FreeNode* next;    // 空闲时：指向下一个空闲块
    char data[1];      // 已分配时：数据存储区域
};
```
**使用联合体的优势**：
- 节省空间：空闲时存储指针，已分配时存储数据
- 类型安全：明确区分两种状态
- 内存对齐：自然对齐到指针大小

### 3. 内存池管理器类
```cpp
class MemoryPool {
private:
    FreeNode* free_lists_[PoolConfig::num_pools];  // 空闲链表数组
    char* current_block_;                          // 当前内存块指针
    std::size_t remaining_;                        // 当前块剩余字节数
    BlockHeader* allocated_blocks_;                // 已分配大块链表
    
public:
    void* allocate(std::size_t n);
    void deallocate(void* p, std::size_t n) noexcept;
};
```

## 分配算法

### 1. 分配流程
```cpp
void* MemoryPool::allocate(std::size_t n) {
    // 1. 处理 n == 0
    if (n == 0) return nullptr;
    
    // 2. 对齐请求大小
    std::size_t aligned_n = PoolConfig::aligned_size(n);
    
    // 3. 大对象直接分配
    if (aligned_n > PoolConfig::max_small_object) {
        return allocate_aligned(aligned_n);
    }
    
    // 4. 小对象使用内存池
    return allocate_from_pool(aligned_n);
}
```

### 2. 从内存池分配
```cpp
void* MemoryPool::allocate_from_pool(std::size_t n) {
    // 1. 获取池索引
    std::size_t index = get_pool_index(n);
    
    // 2. 尝试从空闲链表获取
    if (FreeNode* node = free_lists_[index]) {
        free_lists_[index] = node->next;
        return static_cast<void*>(node);
    }
    
    // 3. 空闲链表为空，分配新块
    return allocate_new_block(PoolConfig::pool_sizes[index]);
}
```

### 3. 分配新内存块
```cpp
void* MemoryPool::allocate_new_block(std::size_t size) {
    // 1. 检查当前块剩余空间
    if (remaining_ < size) {
        // 分配新的大块（8KB）
        void* new_block = allocate_aligned(PoolConfig::block_size);
        
        // 添加到跟踪链表
        BlockHeader* header = static_cast<BlockHeader*>(new_block);
        header->next = allocated_blocks_;
        header->size = PoolConfig::block_size;
        allocated_blocks_ = header;
        
        // 更新当前块指针（跳过 BlockHeader）
        current_block_ = static_cast<char*>(new_block) + sizeof(BlockHeader);
        remaining_ = PoolConfig::block_size - sizeof(BlockHeader);
    }
    
    // 2. 从当前块分配
    void* result = current_block_;
    current_block_ += size;
    remaining_ -= size;
    
    return result;
}
```

## 释放算法

### 1. 释放流程
```cpp
void MemoryPool::deallocate(void* p, std::size_t n) noexcept {
    if (!p) return;
    
    // 1. 对齐请求大小
    std::size_t aligned_n = PoolConfig::aligned_size(n);
    
    // 2. 大对象直接释放
    if (aligned_n > PoolConfig::max_small_object) {
        deallocate_aligned(p, aligned_n);
        return;
    }
    
    // 3. 小对象放回空闲链表
    std::size_t index = get_pool_index(aligned_n);
    FreeNode* node = static_cast<FreeNode*>(p);
    node->next = free_lists_[index];
    free_lists_[index] = node;
}
```

### 2. 空闲链表管理
```cpp
// 将大块分割成小块并添加到空闲链表
void MemoryPool::add_block_to_free_list(void* block, std::size_t size, std::size_t index) {
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
    
    // 连接到空闲链表
    if (first) {
        if (free_lists_[index]) {
            last->next = free_lists_[index];
        }
        free_lists_[index] = first;
    }
}
```

## 线程安全设计

### 1. 线程局部存储
```cpp
inline MemoryPool& get_thread_pool() {
    // 每个线程有自己的内存池实例
    static thread_local MemoryPool thread_pool;
    return thread_pool;
}
```

**优势**：
- **无锁操作**：每个线程独立，无需同步
- **缓存友好**：线程局部数据在缓存中更高效
- **自动清理**：线程退出时自动释放内存

### 2. 使用示例
```cpp
// 多线程安全使用
void worker_thread() {
    // 每个线程获取自己的内存池
    auto& pool = mystl::detail::get_thread_pool();
    
    // 分配内存（无锁）
    void* p1 = pool.allocate(64);
    void* p2 = pool.allocate(128);
    
    // 使用内存...
    
    // 释放内存（无锁）
    pool.deallocate(p1, 64);
    pool.deallocate(p2, 128);
}
```

## 内存对齐处理

### 1. 对齐计算
```cpp
static constexpr std::size_t aligned_size(std::size_t n) noexcept {
    return (n + alignment - 1) & ~(alignment - 1);
}
```

### 2. 对齐验证
```cpp
static bool is_aligned(void* p) noexcept {
    return (reinterpret_cast<std::uintptr_t>(p) & (alignment - 1)) == 0;
}
```

### 3. 对齐分配
```cpp
void* MemoryPool::allocate_aligned(std::size_t size) {
    // 使用 operator new，它保证 max_align_t 对齐
    void* ptr = ::operator new(size);
    if (!ptr) throw std::bad_alloc();
    
    #ifndef NDEBUG
    // 调试模式下验证对齐
    if (!PoolConfig::is_aligned(ptr)) {
        ::operator delete(ptr);
        throw std::bad_alloc();
    }
    #endif
    
    return ptr;
}
```

## 性能优化技巧

### 1. 缓存优化
```cpp
// 预取优化：提前将数据加载到缓存
template<typename T>
class CacheOptimizedMemoryPool : public MemoryPool {
public:
    void* allocate(std::size_t n) {
        void* p = MemoryPool::allocate(n);
        
        // 预取到缓存
        if (p && n <= 1024) {
            char* cp = static_cast<char*>(p);
            for (std::size_t i = 0; i < n; i += 64) {
                __builtin_prefetch(cp + i);
            }
        }
        
        return p;
    }
};
```

### 2. 大小分类优化
```cpp
// 更精细的大小分类
class MultiSizeMemoryPool : public MemoryPool {
    enum SizeClass {
        TINY,      // ≤ 32字节
        SMALL,     // 33-128字节
        MEDIUM,    // 129-512字节
        LARGE,     // 513-1024字节
        HUGE       // >1024字节
    };
    
    SizeClass get_size_class(std::size_t n) const {
        if (n <= 32) return TINY;
        if (n <= 128) return SMALL;
        if (n <= 512) return MEDIUM;
        if (n <= 1024) return LARGE;
        return HUGE;
    }
};
```

### 3. 内存回收策略
```cpp
class MemoryPoolWithReclaim : public MemoryPool {
    std::chrono::steady_clock::time_point last_reclaim_;
    
    void periodic_reclaim() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = now - last_reclaim_;
        
        if (elapsed > std::chrono::minutes(5)) {
            reclaim_unused_memory();
            last_reclaim_ = now;
        }
    }
    
    void reclaim_unused_memory() {
        // 回收长时间未使用的内存块
        for (auto& list : free_lists_) {
            if (should_reclaim_list(list)) {
                reclaim_list(list);
            }
        }
    }
};
```

## 使用示例

### 1. 直接使用内存池
```cpp
#include "memory/detail/memory_pool.hpp"

void test_memory_pool() {
    // 获取线程局部内存池
    auto& pool = mystl::detail::get_thread_pool();
    
    // 分配小对象（使用内存池）
    void* small1 = pool.allocate(64);   // 使用64字节池
    void* small2 = pool.allocate(128);  // 使用128字节池
    
    // 分配大对象（直接系统分配）
    void* large = pool.allocate(2048);  // >1024字节，系统分配
    
    // 使用内存...
    
    // 释放内存
    pool.deallocate(small1, 64);
    pool.deallocate(small2, 128);
    pool.deallocate(large, 2048);
}
```

### 2. 集成到自定义分配器
```cpp
template<typename T>
class CustomAllocator {
    mystl::detail::MemoryPool& pool_;
    
public:
    CustomAllocator() : pool_(mystl::detail::get_thread_pool()) {}
    
    T* allocate(std::size_t n) {
        void* p = pool_.allocate(n * sizeof(T));
        return static_cast<T*>(p);
    }
    
    void deallocate(T* p, std::size_t n) noexcept {
        pool_.deallocate(p, n * sizeof(T));
    }
    
    // ... 其他分配器接口
};
```

### 3. 性能测试
```cpp
void benchmark_memory_pool() {
    auto& pool = mystl::detail::get_thread_pool();
    
    constexpr std::size_t num_allocations = 1000000;
    std::vector<void*> allocations;
    allocations.reserve(num_allocations);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 分配测试
    for (std::size_t i = 0; i < num_allocations; ++i) {
        allocations.push_back(pool.allocate(64));
    }
    
    // 释放测试
    for (void* p : allocations) {
        pool.deallocate(p, 64);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Memory pool: " << duration.count() << "ms\n";
    
    // 对比标准分配器
    start = std::chrono::high_resolution_clock::now();
    
    for (std::size_t i = 0; i < num_allocations; ++i) {
        allocations[i] = ::operator new(64);
    }
    
    for (void* p : allocations) {
        ::operator delete(p);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Standard allocator: " << duration.count() << "ms\n";
}
```

## 常见问题

### Q: 内存池会导致内存泄漏吗？
A: 正确实现不会：
- 析构函数释放所有分配的大块
- 线程退出时线程局部存储自动清理
- 可以使用内存分析工具检测

### Q: 如何选择池大小？
A: 基于工作负载分析：
```cpp
// 分析应用程序的内存分配模式
void analyze_allocation_pattern() {
    std::map<std::size_t, std::size_t> size_distribution;
    
    // 收集分配大小统计
    for (const auto& allocation : trace_allocations()) {
        size_distribution[allocation.size]++;
    }
    
    // 根据分布调整池大小
    for (const auto& [size, count] : size_distribution) {
        if (count > 1000) {  // 频繁分配的大小
            std::cout << "Consider adding pool for size " << size << "\n";
        }
    }
}
```

### Q: 内存池适合什么场景？
A:
- **适合**：频繁分配/释放小对象（≤1024字节）
- **适合**：多线程高并发应用
- **适合**：对性能有严格要求的系统
- **不适合**：分配大小变化很大的应用
- **不适合**：内存受限的嵌入式系统

### Q: 如何调试内存池问题？
A:
```cpp
class DebugMemoryPool : public MemoryPool {
    struct AllocationInfo {
        void* ptr;
        std::size_t size;
        void* stack_trace[10];
    };
    
    std::vector<AllocationInfo> active_allocations_;
    
public:
    void* allocate(std::size_t n) {
        void* p = MemoryPool::allocate(n);
        
        AllocationInfo info;
        info.ptr = p;
        info.size = n;
        capture_stack_trace(info.stack_trace);
        
        active_allocations_.push_back(info);
        return p;
    }
    
    void deallocate(void* p, std::size_t n) noexcept {
        // 从active_allocations_中移除
        auto it = std::find_if(active_allocations_.begin(), active_allocations_.end(),
                              [p](const AllocationInfo& info) { return info.ptr == p; });
        if (it != active_allocations_.end()) {
            active_allocations_.erase(it);
        }
        
        MemoryPool::deallocate(p, n);
    }
    
    void check_for_leaks() const {
        if (!active_allocations_.empty()) {
            std::cerr << "Memory leaks detected:\n";
            for (const auto& info : active_allocations_) {
                std::cerr << "  " << info.ptr << ": " << info.size << " bytes\n";
                print_stack_trace(info.stack_trace);
            }
        }
    }
};
```

## 扩展应用

### 1. 对象池模式
```cpp
template<typename T>
class ObjectPool {
    mystl::detail::MemoryPool& pool_;
    std::vector<T*> free_objects_;
    
public:
    ObjectPool() : pool_(mystl::detail::get_thread_pool()) {}
    
    T* acquire() {
        if (free_objects_.empty()) {
            void* p = pool_.allocate(sizeof(T));
            return new (p) T();
        }
        
        T* obj = free_objects_.back();
        free_objects_.pop_back();
        return obj;
    }
    
    void release(T
