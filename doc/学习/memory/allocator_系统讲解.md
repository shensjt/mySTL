# 标准分配器详解

## 概述

`allocator.hpp` 实现了 C++ 标准库中的标准分配器（`std::allocator`）。分配器是 STL 容器的内存管理核心，负责内存的分配、释放以及对象的构造和销毁。本实现遵循 C++11 标准，同时注释说明了 C++17 和 C++20 的变化。

## 核心概念

### 1. 分配器的作用
分配器将内存管理与对象管理分离：
- **内存管理**：分配和释放原始内存
- **对象管理**：在已分配内存上构造和销毁对象
- **类型擦除**：通过 `void` 特化支持泛型编程

### 2. 标准演进
- **C++11**：基础实现，包含 `construct`/`destroy` 成员函数
- **C++17**：`construct`/`destroy` 被弃用，建议使用 `allocator_traits`
- **C++20**：移除 `construct`/`destroy` 成员函数，增加分配提示

## 主模板实现

### 1. 类型定义
```cpp
template<typename T>
class allocator {
public:
    // 基本类型
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    // C++11 新增
    using void_pointer = void*;
    using const_void_pointer = const void*;
    
    // 传播特性
    using propagate_on_container_move_assignment = mystl::true_type;
    using propagate_on_container_copy_assignment = mystl::false_type;
    using propagate_on_container_swap = mystl::false_type;
    
    // 相等性特性
    using is_always_equal = mystl::true_type;
};
```

### 2. 重新绑定机制
```cpp
template<typename U>
struct rebind {
    using other = allocator<U>;
};
```
**作用**：允许容器从 `allocator<T>` 获取 `allocator<U>` 类型。

**示例**：
```cpp
// std::list<int> 需要两种分配器：
// 1. allocator<int> - 分配 int 元素
// 2. allocator<std::_List_node<int>> - 分配链表节点
// 通过 rebind 机制自动转换
using ListAllocator = std::allocator<int>;
using NodeAllocator = typename ListAllocator::template rebind<ListNode>::other;
```

### 3. 核心成员函数

#### `allocate`
```cpp
[[nodiscard]] pointer allocate(size_type n);
```
分配足够存储 `n` 个 `T` 类型对象的内存。

**实现细节**：
1. 检查 `n > max_size()` → 抛出 `std::bad_alloc`
2. 处理 `n == 0` → 返回 `nullptr`
3. 使用 `::operator new` 分配内存
4. 分配失败 → 抛出 `std::bad_alloc`

#### `deallocate`
```cpp
void deallocate(pointer p, size_type n) noexcept;
```
释放之前分配的内存。

**注意**：`n` 参数通常被忽略，但标准要求提供（用于调试或特殊分配器）。

#### `construct`（C++11，C++17弃用）
```cpp
template<typename U, typename... Args>
void construct(U* p, Args&&... args);
```
在已分配内存上构造对象。

**实现**：使用 placement new 和完美转发。

#### `destroy`（C++11，C++17弃用）
```cpp
template<typename U>
void destroy(U* p);
```
销毁对象但不释放内存。

**实现**：显式调用析构函数 `p->~U()`。

#### `max_size`
```cpp
size_type max_size() const noexcept;
```
返回理论上可分配的最大元素数量。

**计算**：`size_type最大值 / sizeof(T)`

### 4. C++20 扩展
```cpp
[[nodiscard]] pointer allocate(size_type n, const_void_pointer hint);
```
带分配提示的内存分配（可选接口）。

## void 特化

### 1. 为什么需要 void 特化？
```cpp
template<>
class allocator<void> {
    // 没有 reference 和 const_reference 类型
    // 没有 allocate、deallocate、construct、destroy 成员函数
};
```
**原因**：
- `void` 对象不能被分配、释放、构造或销毁
- 用于类型擦除场景
- C++17 弃用，C++20 移除，但为了兼容性仍然提供

### 2. 使用场景
```cpp
// 类型擦除的容器
class AnyContainer {
    mystl::allocator<void> alloc_;
    void* data_;
    
public:
    template<typename T>
    void store(const T& value) {
        // 使用 allocator_traits 进行类型安全的分配
        using traits = std::allocator_traits<mystl::allocator<T>>;
        data_ = traits::allocate(alloc_, 1);
        traits::construct(alloc_, static_cast<T*>(data_), value);
    }
};
```

## 设计模式

### 1. 空基类优化
标准分配器是无状态的空类：
```cpp
static_assert(sizeof(mystl::allocator<int>) == 1, 
              "allocator should use empty base optimization");
```

**好处**：
- 零内存开销
- 编译器可以优化掉分配器对象

### 2. 传播特性控制
```cpp
using propagate_on_container_move_assignment = mystl::true_type;
using propagate_on_container_copy_assignment = mystl::false_type;
using propagate_on_container_swap = mystl::false_type;
```

**作用**：控制容器操作时分配器的行为：
- **移动赋值**：传播分配器（`true_type`）
- **复制赋值**：不传播分配器（`false_type`）
- **交换**：不传播分配器（`false_type`）

### 3. 相等性特性
```cpp
using is_always_equal = mystl::true_type;
```
指示所有分配器实例是否总是相等。

**意义**：如果 `is_always_equal::value == true`，容器可以安全地交换内存而不交换分配器。

## 使用示例

### 1. 基本使用
```cpp
// 创建分配器
mystl::allocator<int> alloc;

// 分配内存
int* p = alloc.allocate(10);

// 构造对象
for (int i = 0; i < 10; ++i) {
    alloc.construct(&p[i], i * 2);
}

// 使用对象
for (int i = 0; i < 10; ++i) {
    std::cout << p[i] << " ";
}

// 销毁对象
for (int i = 0; i < 10; ++i) {
    alloc.destroy(&p[i]);
}

// 释放内存
alloc.deallocate(p, 10);
```

### 2. 容器集成
```cpp
template<typename T, typename Allocator = mystl::allocator<T>>
class Vector {
    T* data_;
    size_t size_;
    size_t capacity_;
    Allocator alloc_;
    
public:
    Vector(size_t n, const T& value = T(), const Allocator& alloc = Allocator())
        : alloc_(alloc), size_(n), capacity_(n) {
        data_ = alloc_.allocate(n);
        try {
            for (size_t i = 0; i < n; ++i) {
                alloc_.construct(&data_[i], value);
            }
        } catch (...) {
            for (size_t i = 0; i < n; ++i) {
                alloc_.destroy(&data_[i]);
            }
            alloc_.deallocate(data_, n);
            throw;
        }
    }
    
    ~Vector() {
        for (size_t i = 0; i < size_; ++i) {
            alloc_.destroy(&data_[i]);
        }
        alloc_.deallocate(data_, capacity_);
    }
};
```

### 3. 自定义分配器
```cpp
template<typename T>
class DebugAllocator : public mystl::allocator<T> {
public:
    template<typename U>
    struct rebind {
        using other = DebugAllocator<U>;
    };
    
    T* allocate(size_t n) {
        std::cout << "Allocating " << n << " elements of size " << sizeof(T) << "\n";
        return mystl::allocator<T>::allocate(n);
    }
    
    void deallocate(T* p, size_t n) noexcept {
        std::cout << "Deallocating " << n << " elements\n";
        mystl::allocator<T>::deallocate(p, n);
    }
};

// 使用自定义分配器
std::vector<int, DebugAllocator<int>> vec;
vec.push_back(1);  // 输出分配信息
```

## 最佳实践

### 1. 使用 allocator_traits
```cpp
// 现代 C++ 推荐方式
template<typename Alloc>
void construct_object(Alloc& alloc, typename Alloc::value_type* p, 
                      const typename Alloc::value_type& value) {
    using traits = std::allocator_traits<Alloc>;
    traits::construct(alloc, p, value);
}

template<typename Alloc>
void destroy_object(Alloc& alloc, typename Alloc::value_type* p) {
    using traits = std::allocator_traits<Alloc>;
    traits::destroy(alloc, p);
}
```

**好处**：
- 自动处理 C++ 版本差异
- 支持有状态分配器
- 更通用的接口

### 2. 异常安全
```cpp
template<typename T, typename Alloc>
T* create_objects(Alloc& alloc, size_t n) {
    T* p = alloc.allocate(n);
    size_t constructed = 0;
    
    try {
        for (size_t i = 0; i < n; ++i) {
            alloc.construct(&p[i], T());  // 可能抛出
            ++constructed;
        }
        return p;
    } catch (...) {
        // 回滚：销毁已构造的对象
        for (size_t i = 0; i < constructed; ++i) {
            alloc.destroy(&p[i]);
        }
        alloc.deallocate(p, n);
        throw;
    }
}
```

### 3. 内存对齐
```cpp
// C++17 对齐分配
template<typename T>
class AlignedAllocator : public mystl::allocator<T> {
public:
    T* allocate(size_t n) {
        if (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            // 需要特殊对齐
            return static_cast<T*>(::operator new(n * sizeof(T), 
                                                 std::align_val_t{alignof(T)}));
        }
        return mystl::allocator<T>::allocate(n);
    }
    
    void deallocate(T* p, size_t n) noexcept {
        if (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            ::operator delete(p, std::align_val_t{alignof(T)});
        } else {
            mystl::allocator<T>::deallocate(p, n);
        }
    }
};
```

## 常见问题

### Q: 为什么需要 `rebind` 机制？
A: 容器需要两种类型的内存：
1. 元素类型的内存（如 `int`）
2. 内部数据结构的内存（如链表节点）
`rebind` 允许容器从元素分配器获取节点分配器。

### Q: `construct`/`destroy` 为什么被弃用？
A: C++17 引入 `allocator_traits` 提供更统一的接口：
- 处理不同 C++ 版本的差异
- 支持有状态分配器
- 提供默认实现

### Q: 标准分配器为什么是无状态的？
A: 设计决策：
- 简化实现
- 允许空基类优化
- 所有实例行为相同

### Q: 什么时候需要自定义分配器？
A:
- 需要特殊内存对齐
- 使用内存池或 arena
- 调试内存分配
- 统计内存使用

## 性能考虑

### 1. 分配开销
```cpp
// 标准分配器使用全局 operator new/delete
// 可能有锁开销（多线程环境）
// 考虑使用内存池分配器优化
```

### 2. 对象构造优化
```cpp
// 对于平凡类型，construct/destroy 可以被优化掉
if constexpr (std::is_trivially_constructible_v<T>) {
    // 直接内存操作，不调用构造函数
} else {
    alloc.construct(p, args...);
}
```

### 3. 缓存友好性
```cpp
// 分配连续内存提高缓存命中率
T* p = alloc.allocate(n);
// 连续访问比随机访问快
for (size_t i = 0; i < n; ++i) {
    process(p[i]);
}
```

## 扩展应用

### 1. 内存池分配器
```cpp
template<typename T>
class PoolAllocator {
    MemoryPool pool_;
    
public:
    T* allocate(size_t n) {
        return static_cast<T*>(pool_.allocate(n * sizeof(T)));
    }
    
    void deallocate(T* p, size_t n) noexcept {
        pool_.deallocate(p, n * sizeof(T));
    }
    
    // 继承其他特性从 mystl::allocator
    using propagate_on_container_move_assignment = mystl::true_type;
    // ...
};
```

### 2. 调试分配器
```cpp
template<typename T>
class DebugAllocator {
    struct Header {
        size_t size;
        void* stack_trace[10];
    };
    
public:
    T* allocate(size_t n) {
        size_t total_size = sizeof(Header) + n * sizeof(T);
        Header* header = static_cast<Header*>(::operator new(total_size));
        header->size = n;
        capture_stack_trace(header->stack_trace);
        
        return reinterpret_cast<T*>(header + 1);
    }
    
    void deallocate(T* p, size_t n) noexcept {
        Header* header = reinterpret_cast<Header*>(p) - 1;
        assert(header->size == n);
        ::operator delete(header);
    }
};
```

### 3. 共享内存分配器
```cpp
template<typename T>
class SharedMemoryAllocator {
    SharedMemorySegment& segment_;
    
public:
    T* allocate(size_t n) {
        return segment_.allocate<T>(n);
    }
    
    void deallocate(T* p, size_t n) noexcept {
        segment_.deallocate(p, n);
    }
    
    // 需要自定义传播特性
    using propagate_on_container_copy_assignment = mystl::true_type;
    using is_always_equal = mystl::false_type;
};
```

## 总结

`allocator.hpp` 实现了 C++ 标准分配器的核心功能：

1. **标准兼容**：遵循 C++11 标准，注释说明后续版本变化
2. **完整功能**：包含主模板和 void 特化
3. **设计优雅**：使用空基类优化和传播特性
4. **扩展性强**：支持自定义分配器继承

分配器是 STL 内存管理的基石，理解其工作原理对于编写高效、安全的 C++ 代码至关重要。
