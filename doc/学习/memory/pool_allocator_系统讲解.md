# 内存池分配器详解

## 概述

`pool_allocator.hpp` 实现了基于内存池的分配器，专门优化小对象的内存分配性能。与标准分配器相比，内存池分配器通过预分配大块内存并分割成小块来减少系统调用和内存碎片，特别适合频繁分配和释放小对象的场景。

## 设计理念

### 1. 性能优化目标
- **减少系统调用**：预分配大块内存，避免频繁调用 `malloc`/`free`
- **减少内存碎片**：固定大小的内存块减少外部碎片
- **提高缓存局部性**：连续分配的内存块提高缓存命中率
- **避免锁争用**：使用线程局部存储，每个线程有自己的内存池

### 2. 分配策略
```cpp
// 分配决策逻辑
std::size_t total_bytes = n * sizeof(T);
if (total_bytes > 1024) {
    // 大对象：使用系统分配器
    return ::operator new(total_bytes);
} else {
    // 小对象：使用内存池
    return memory_pool.allocate(total_bytes);
}
```

## 核心实现

### 1. 类型定义和特性
```cpp
template<typename T>
class pool_allocator {
public:
    // 基本类型定义（与标准分配器相同）
    using value_type = T;
    using pointer = T*;
    using size_type = std::size_t;
    
    // 传播特性（与标准分配器不同）
    using propagate_on_container_copy_assignment = false_type;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap = false_type;
    using is_always_equal = false_type;  // 有状态分配器
};
```

### 2. 与标准分配器的区别

| 特性 | 标准分配器 | 内存池分配器 |
|------|-----------|-------------|
| 状态 | 无状态 | 有状态（持有内存池） |
| `is_always_equal` | `true_type` | `false_type` |
| 传播拷贝赋值 | `false_type` | `false_type` |
| 传播移动赋值 | `true_type` | `true_type` |
| 小对象优化 | 无 | 有（≤1024字节） |
| 线程安全 | 是（全局锁） | 是（线程局部存储） |

### 3. 重新绑定机制
```cpp
template<typename U>
struct rebind {
    using other = pool_allocator<U>;
};
```
**作用**：支持节点式容器（如 `std::list`、`std::map`）。

## 内存池设计

### 1. 线程局部存储优化
```cpp
// 每个线程有自己的内存池实例
detail::MemoryPool& get_thread_pool() {
    thread_local detail::MemoryPool pool;
    return pool;
}
```

**优点**：
- 避免锁争用
- 提高多线程性能
- 内存池生命周期与线程绑定

### 2. 内存块管理
```cpp
class MemoryPool {
private:
    struct Block {
        Block* next;
    };
    
    struct Chunk {
        Chunk* next;
        char data[1];
    };
    
    Block* free_list_[NUM_FREELISTS];  // 自由链表数组
    Chunk* chunks_;                    // 大块内存链表
    std::size_t chunk_size_;           // 块大小
};
```

### 3. 分配算法
```cpp
void* MemoryPool::allocate(std::size_t size) {
    // 1. 对齐调整
    size = align_size(size);
    
    // 2. 查找合适大小的自由链表
    int index = freelist_index(size);
    
    // 3. 如果自由链表为空，分配新块
    if (!free_list_[index]) {
        refill_freelist(index, size);
    }
    
    // 4. 从自由链表获取块
    Block* block = free_list_[index];
    free_list_[index] = block->next;
    
    return static_cast<void*>(block);
}
```

## 使用示例

### 1. 基本使用
```cpp
// 创建内存池分配器
mystl::pool_allocator<int> alloc;

// 分配内存（小对象使用内存池）
int* p1 = alloc.allocate(10);  // 40字节，使用内存池

// 分配内存（大对象使用系统分配器）
int* p2 = alloc.allocate(1000);  // 4000字节，使用系统分配器

// 构造对象
for (int i = 0; i < 10; ++i) {
    alloc.construct(&p1[i], i);
}

// 使用对象...

// 销毁对象
for (int i = 0; i < 10; ++i) {
    alloc.destroy(&p1[i]);
}

// 释放内存
alloc.deallocate(p1, 10);
alloc.deallocate(p2, 1000);
```

### 2. 容器集成
```cpp
// 使用内存池分配器的vector
std::vector<int, mystl::pool_allocator<int>> vec;

// 性能测试：频繁push_back/pop_back
for (int i = 0; i < 1000000; ++i) {
    vec.push_back(i);
    if (i % 100 == 0) {
        vec.pop_back();
    }
}
// 内存池分配器比标准分配器快2-3倍
```

### 3. 多线程场景
```cpp
void worker_thread(int id) {
    // 每个线程有自己的内存池
    mystl::pool_allocator<int> alloc;
    
    std::vector<int, mystl::pool_allocator<int>> local_vec(alloc);
    
    for (int i = 0; i < 10000; ++i) {
        local_vec.push_back(i * id);
    }
    
    // 没有锁争用，性能线性扩展
}

void test_multithread() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back(worker_thread, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
}
```

## 性能优化

### 1. 大小分类优化
```cpp
// 根据对象大小选择不同的分配策略
void* allocate_optimized(std::size_t size) {
    if (size <= 16) {
        return allocate_small(size);      // 极小对象
    } else if (size <= 128) {
        return allocate_medium(size);     // 小对象
    } else if (size <= 1024) {
        return allocate_large(size);      // 中等对象
    } else {
        return ::operator new(size);      // 大对象
    }
}
```

### 2. 缓存友好性
```cpp
// 预取优化
template<typename T>
class CacheFriendlyPoolAllocator : public pool_allocator<T> {
public:
    T* allocate(size_t n) {
        T* p = pool_allocator<T>::allocate(n);
        
        // 预取到缓存
        for (size_t i = 0; i < n; i += CACHE_LINE_SIZE / sizeof(T)) {
            __builtin_prefetch(&p[i]);
        }
        
        return p;
    }
};
```

### 3. 内存回收策略
```cpp
class MemoryPoolWithReclaim {
    // 定期回收长时间未使用的内存
    void periodic_reclaim() {
        if (should_reclaim()) {
            reclaim_unused_memory();
        }
    }
    
    // 智能合并相邻空闲块
    void coalesce_free_blocks() {
        // 合并相邻空闲块减少碎片
    }
};
```

## 异常安全

### 1. 强异常安全保证
```cpp
template<typename T>
T* pool_allocator<T>::allocate(size_type n) {
    if (n > max_size()) {
        throw std::bad_alloc();
    }
    
    if (n == 0) {
        return nullptr;
    }
    
    void* p = detail::get_thread_pool().allocate(n * sizeof(T));
    if (!p) {
        throw std::bad_alloc();  // 内存池耗尽
    }
    
    return static_cast<T*>(p);
}
```

### 2. 资源泄漏防护
```cpp
class ScopedPoolAllocator {
    mystl::pool_allocator<int> alloc_;
    int* ptr_;
    size_t size_;
    
public:
    ScopedPoolAllocator(size_t n) : size_(n) {
        ptr_ = alloc_.allocate(n);
        try {
            for (size_t i = 0; i < n; ++i) {
                alloc_.construct(&ptr_[i], 0);
            }
        } catch (...) {
            alloc_.deallocate(ptr_, n);
            throw;
        }
    }
    
    ~ScopedPoolAllocator() {
        for (size_t i = 0; i < size_; ++i) {
            alloc_.destroy(&ptr_[i]);
        }
        alloc_.deallocate(ptr_, size_);
    }
};
```

## 最佳实践

### 1. 选择合适的分配器
```cpp
// 场景1：频繁分配小对象
using FastVector = std::vector<int, mystl::pool_allocator<int>>;

// 场景2：分配大对象或稀疏分配
using StandardVector = std::vector<int, std::allocator<int>>;

// 场景3：多线程高并发
using ThreadSafeVector = std::vector<int, mystl::pool_allocator<int>>;
```

### 2. 配置调优
```cpp
template<typename T, std::size_t Threshold = 1024>
class ConfigurablePoolAllocator : public pool_allocator<T> {
public:
    T* allocate(size_t n) {
        if (n * sizeof(T) > Threshold) {
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }
        return pool_allocator<T>::allocate(n);
    }
    
    void deallocate(T* p, size_t n) noexcept {
        if (n * sizeof(T) > Threshold) {
            ::operator delete(p);
        } else {
            pool_allocator<T>::deallocate(p, n);
        }
    }
};
```

### 3. 监控和调试
```cpp
class InstrumentedPoolAllocator : public pool_allocator<int> {
    std::atomic<size_t> allocations_{0};
    std::atomic<size_t> deallocations_{0};
    std::atomic<size_t> total_bytes_{0};
    
public:
    int* allocate(size_t n) {
        allocations_++;
        total_bytes_ += n * sizeof(int);
        return pool_allocator<int>::allocate(n);
    }
    
    void deallocate(int* p, size_t n) noexcept {
        deallocations_++;
        pool_allocator<int>::deallocate(p, n);
    }
    
    void print_stats() const {
        std::cout << "Allocations: " << allocations_ << "\n"
                  << "Deallocations: " << deallocations_ << "\n"
                  << "Total bytes: " << total_bytes_ << "\n"
                  << "Memory in use: " << (allocations_ - deallocations_) * sizeof(int) << "\n";
    }
};
```

## 常见问题

### Q: 内存池分配器什么时候比标准分配器快？
A:
- 频繁分配/释放小对象（≤1024字节）
- 多线程环境（线程局部存储避免锁争用）
- 对象大小固定或范围有限

### Q: 内存池分配器有什么缺点？
A:
- 内存使用可能更高（预分配）
- 不适合分配大对象
- 有状态，影响容器语义
- 实现更复杂

### Q: 如何选择阈值大小？
A: 基于性能测试：
```cpp
// 性能测试不同阈值
for (size_t threshold : {256, 512, 1024, 2048}) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // 使用该阈值的分配器进行测试
    test_allocation_performance(threshold);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Threshold " << threshold << ": " << duration.count() << "ms\n";
}
```

### Q: 内存池会导致内存泄漏吗？
A: 正确实现不会：
- RAII 管理内存池生命周期
- 线程退出时自动清理
- 定期回收未使用内存

## 扩展应用

### 1. 对象池模式
```cpp
template<typename T>
class ObjectPool {
    mystl::pool_allocator<T> alloc_;
    std::vector<T*, mystl::pool_allocator<T*>> free_objects_;
    
public:
    T* acquire() {
        if (free_objects_.empty()) {
            return alloc_.allocate(1);
        }
        
        T* obj = free_objects_.back();
        free_objects_.pop_back();
        return obj;
    }
    
    void release(T* obj) {
        free_objects_.push_back(obj);
    }
    
    ~ObjectPool() {
        for (T* obj : free_objects_) {
            alloc_.destroy(obj);
            alloc_.deallocate(obj, 1);
        }
    }
};
```

### 2. 领域特定分配器
```cpp
// 游戏开发：帧分配器
class FrameAllocator : public pool_allocator<char> {
    char* current_frame_;
    size_t frame_size_;
    size_t used_;
    
public:
    FrameAllocator(size_t frame_size) 
        : frame_size_(frame_size), used_(0) {
        current_frame_ = pool_allocator<char>::allocate(frame_size);
    }
    
    void* allocate(size_t size) {
        if (used_ + size > frame_size_) {
            throw std::bad_alloc();
        }
        
        void* p = current_frame_ + used_;
        used_ += size;
        return p;
    }
    
    void reset() noexcept {
        used_ = 0;  // 下一帧重用内存
    }
    
    ~FrameAllocator() {
        pool_allocator<char>::deallocate(current_frame_, frame_size_);
    }
};
```

### 3. 内存分析工具
```cpp
class ProfilingPoolAllocator : public pool_allocator<int> {
    struct AllocationRecord {
        void* ptr;
        size_t size;
        void* stack_trace[10];
        std::chrono::steady_clock::time_point time;
    };
    
    std::vector<AllocationRecord> records_;
    
public:
    int* allocate(size_t n) {
        int* p = pool_allocator<int>::allocate(n);
        
        AllocationRecord record;
        record.ptr = p;
        record.size = n * sizeof(int);
        capture_stack_trace(record.stack_trace);
        record.time = std::chrono::steady_clock::now();
        
        records_.push_back(record);
        return p;
    }
    
    void deallocate(int* p, size_t n) noexcept {
        // 从records_中移除对应记录
        auto it = std::find_if(records_.begin(), records_.end(),
                              [p](const AllocationRecord& r) { return r.ptr == p; });
        if (it != records_.end()) {
            records_.erase(it);
        }
        
        pool_allocator<int>::deallocate(p, n);
    }
    
    void print_leak_report() const {
        if (!records_.empty()) {
            std::cout << "Memory leaks detected:\n";
            for (const auto& record : records_) {
                std::cout << "  " << record.ptr << ": " << record.size << " bytes\n";
                print_stack_trace(record.stack_trace);
            }
        }
    }
};
```

## 总结

`pool_allocator.hpp` 提供了高性能的内存池分配器：

1. **性能优化**：小对象使用内存池，减少系统调用和碎片
2. **线程安全**：线程局部存储避免锁争用
3. **标准兼容**：与标准分配器接口完全兼容
4. **灵活配置**：可调整阈值和策略

内存池分配器特别适合以下场景：
- 频繁分配/释放小对象
- 多线程高并发应用
- 对性能有严格要求的系统

理解内存池分配器的工作原理对于编写高性能 C++ 程序至关重要。
