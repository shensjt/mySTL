# iterator_functions 系统详解

## 概述

`iterator_functions` 实现了 C++ 迭代器的核心操作函数，包括 `advance`、`distance`、`next`、`prev` 等。这些函数通过标签分发 (tag dispatching) 技术为不同类型的迭代器提供最优实现，确保算法的高效性和通用性。

## 核心函数

### 1. advance - 迭代器移动

#### 基本功能
```cpp
template<typename InputIt, typename Distance>
void advance(InputIt& it, Distance n);
```
使迭代器前进（或后退）n 个位置。

#### 实现策略
根据迭代器类别选择不同实现：

| 迭代器类别 | 时间复杂度 | 支持方向 | 实现策略 |
|-----------|-----------|---------|---------|
| 输入迭代器 | O(n) | 只能向前 | 循环递增 |
| 前向迭代器 | O(n) | 只能向前 | 循环递增 |
| 双向迭代器 | O(\|n\|) | 双向 | 条件循环 |
| 随机访问迭代器 | O(1) | 双向 | 直接算术运算 |

#### 标签分发实现
```cpp
template<typename InputIt, typename Distance>
void advance(InputIt& it, Distance n) {
    using category = iterator_category_t<InputIt>;
    detail::advance_impl(it, n, category{});
}
```

### 2. distance - 距离计算

#### 基本功能
```cpp
template<typename InputIt>
typename iterator_traits<InputIt>::difference_type
distance(InputIt first, InputIt last);
```
计算两个迭代器之间的距离。

#### 实现策略
- **随机访问迭代器**：直接计算 `last - first`，O(1) 时间复杂度
- **其他迭代器**：循环计数，O(n) 时间复杂度

### 3. next/prev - 安全移动

#### next 函数
```cpp
template<typename ForwardIt>
ForwardIt next(ForwardIt it, difference_type n = 1);
```
返回前进 n 个位置后的迭代器，不修改原迭代器。

#### prev 函数
```cpp
template<typename BidirIt>
BidirIt prev(BidirIt it, difference_type n = 1);
```
返回后退 n 个位置后的迭代器，需要双向迭代器。

#### 带边界检查的版本
```cpp
template<typename ForwardIt>
ForwardIt next(ForwardIt it, ForwardIt last, difference_type n = 1);

template<typename BidirIt>
BidirIt prev(BidirIt it, BidirIt first, difference_type n = 1);
```
确保移动不会超出指定边界。

## 优化版本

### 1. advance_optimized - 编译时优化版本

#### 特点
- 使用 `if constexpr` 在编译时选择实现
- 避免运行时标签分发开销
- 支持循环展开优化

#### 实现示例
```cpp
template<typename InputIt, typename Distance>
void advance_optimized(InputIt& it, Distance n) {
    using category = iterator_category_t<InputIt>;
    
    if constexpr (is_base_of_v<random_access_iterator_tag, category>) {
        // 随机访问迭代器：直接算术运算
        it += n;
    } else if constexpr (is_base_of_v<bidirectional_iterator_tag, category>) {
        // 双向迭代器：分块处理
        if (n >= 0) {
            while (n >= 4) { ++it; ++it; ++it; ++it; n -= 4; }
            while (n > 0) { ++it; --n; }
        } else {
            while (n <= -4) { --it; --it; --it; --it; n += 4; }
            while (n < 0) { --it; ++n; }
        }
    } else {
        // 输入/前向迭代器：只能向前
        if (n < 0) n = 0;
        while (n >= 4) { ++it; ++it; ++it; ++it; n -= 4; }
        while (n > 0) { ++it; --n; }
    }
}
```

### 2. advance_unrolled - 循环展开版本

#### 特点
- 手动展开循环，减少循环开销
- 特别适合小距离移动 (n < 16)
- 支持双向移动

#### 实现示例
```cpp
template<typename InputIt, typename Distance>
void advance_unrolled(InputIt& it, Distance n) {
    if (n >= 0) {
        // 向前移动：8-4-1 展开策略
        while (n >= 8) {
            ++it; ++it; ++it; ++it;
            ++it; ++it; ++it; ++it;
            n -= 8;
        }
        
        if (n >= 4) {
            ++it; ++it; ++it; ++it;
            n -= 4;
        }
        
        // 处理最后 0-3 个
        switch (n) {
            case 3: ++it; [[fallthrough]];
            case 2: ++it; [[fallthrough]];
            case 1: ++it; [[fallthrough]];
            case 0: break;
        }
    } else {
        // 向后移动（需要双向迭代器）
        // 类似实现...
    }
}
```

### 3. advance_const - 编译时常量版本

#### 特点
- 距离在编译时已知
- 为常见小距离生成特化代码
- 完全消除循环

#### 实现示例
```cpp
template<typename InputIt, int n>
void advance_const(InputIt& it) {
    if constexpr (n == 0) {
        // 什么都不做
    } else if constexpr (n == 1) {
        ++it;
    } else if constexpr (n == 2) {
        ++it; ++it;
    } else if constexpr (n == 3) {
        ++it; ++it; ++it;
    } else if constexpr (n == 4) {
        ++it; ++it; ++it; ++it;
    } else if constexpr (n > 0) {
        // 通用正数实现
        for (int i = 0; i < n; ++i) ++it;
    } else {
        // 负数距离需要双向迭代器
        static_assert(n >= 0 || is_bidirectional_iterator_v<InputIt>,
                     "Negative advance requires bidirectional iterator");
        for (int i = 0; i > n; --i) --it;
    }
}
```

## 辅助函数

### 1. iter_swap - 迭代器值交换
```cpp
template<typename ForwardIt1, typename ForwardIt2>
void iter_swap(ForwardIt1 a, ForwardIt2 b) {
    using mystl::swap;
    swap(*a, *b);
}
```

### 2. iter_move - 迭代器值移动
```cpp
template<typename InputIt>
auto iter_move(InputIt it) 
-> typename iterator_traits<InputIt>::value_type&& {
    using mystl::move;
    return move(*it);
}
```

### 3. safe_advance - 安全移动

#### 单向版本
```cpp
template<typename InputIt, typename Distance>
bool safe_advance(InputIt& it, InputIt end, Distance n);
```
向前移动，检查是否越界。

#### 双向版本
```cpp
template<typename BidirIt, typename Distance>
bool safe_advance(BidirIt& it, BidirIt begin, BidirIt end, Distance n);
```
双向移动，检查两个边界。

### 4. 容器大小函数

#### size 函数
```cpp
// 容器版本
template<typename Container>
constexpr auto size(const Container& c) -> decltype(c.size());

// 数组版本
template<typename T, std::size_t N>
constexpr std::size_t size(const T (&array)[N]) noexcept;

// 迭代器范围版本
template<typename InputIt>
constexpr auto size(InputIt first, InputIt last);
```

#### empty 函数
```cpp
// 容器版本
template<typename Container>
constexpr auto empty(const Container& c) -> decltype(c.empty());

// 数组版本
template<typename T, std::size_t N>
constexpr bool empty(const T (&array)[N]) noexcept;

// 迭代器范围版本
template<typename InputIt>
constexpr bool empty(InputIt first, InputIt last);
```

## 性能优化策略

### 1. 标签分发 vs if constexpr

| 策略 | 优点 | 缺点 | 适用场景 |
|------|------|------|---------|
| 标签分发 | 运行时多态，支持动态类型 | 有虚函数调用开销 | 通用算法，类型未知 |
| if constexpr | 编译时决策，无运行时开销 | 需要编译时类型信息 | 已知迭代器类型 |

### 2. 循环展开策略

#### 展开因子选择
- **8-4-1 策略**：适合现代 CPU 流水线
- **4-1 策略**：适合简单场景
- **完全展开**：适合极小距离 (n ≤ 8)

#### 性能影响
```cpp
// 未展开
for (int i = 0; i < n; ++i) ++it;
// 循环开销：n 次比较 + n 次递增

// 展开后
while (n >= 4) { ++it; ++it; ++it; ++it; n -= 4; }
// 循环开销：n/4 次比较 + n 次递增
```

### 3. 编译时常量优化

#### 常见距离特化
```cpp
// 特化常见小距离
advance_const<0>(it);  // 无操作
advance_const<1>(it);  // ++it
advance_const<2>(it);  // ++it; ++it
advance_const<3>(it);  // ++it; ++it; ++it
advance_const<4>(it);  // ++it; ++it; ++it; ++it
```

#### 编译时计算
```cpp
// 编译时计算复杂表达式
advance_const<sizeof(T) / alignof(T)>(it);
```

## 使用示例

### 示例 1：通用算法实现
```cpp
template<typename InputIt, typename T>
InputIt find(InputIt first, InputIt last, const T& value) {
    while (first != last) {
        if (*first == value) return first;
        mystl::advance(first, 1);  // 使用通用 advance
    }
    return last;
}

template<typename RandomIt, typename T>
RandomIt find_optimized(RandomIt first, RandomIt last, const T& value) {
    // 已知是随机访问迭代器，使用优化版本
    auto n = mystl::distance(first, last);
    while (n > 0) {
        auto middle = first;
        mystl::advance_optimized(middle, n / 2);  // 编译时优化
        
        if (*middle < value) {
            first = ++middle;
            n -= n / 2 + 1;
        } else {
            n = n / 2;
        }
    }
    return first;
}
```

### 示例 2：安全边界检查
```cpp
template<typename BidirIt>
void process_range(BidirIt begin, BidirIt end, int steps) {
    auto it = begin;
    
    // 安全向前移动
    if (!mystl::safe_advance(it, begin, end, steps)) {
        throw std::out_of_range("Advance out of bounds");
    }
    
    // 处理元素
    process(*it);
    
    // 安全向后移动
    if (!mystl::safe_advance(it, begin, end, -steps / 2)) {
        throw std::out_of_range("Retreat out of bounds");
    }
}
```

### 示例 3：性能关键路径优化
```cpp
template<typename ContiguousIt>
void fast_copy(ContiguousIt first, ContiguousIt last, ContiguousIt result) {
    // 连续迭代器，使用内存操作优化
    if constexpr (is_contiguous_iterator_v<ContiguousIt>) {
        std::size_t bytes = mystl::distance(first, last) * sizeof(*first);
        std::memcpy(&*result, &*first, bytes);
    } else {
        // 通用实现
        while (first != last) {
            *result = *first;
            mystl::advance(first, 1);
            mystl::advance(result, 1);
        }
    }
}
```

### 示例 4：编译时优化
```cpp
template<typename RandomIt>
void rotate_by_four(RandomIt first, RandomIt last) {
    // 编译时已知距离为 4
    auto it = first;
    mystl::advance_const<4>(it);  // 完全展开，无循环
    
    // 旋转操作
    std::rotate(first, it, last);
}
```

## 最佳实践

### 1. 选择合适的 advance 版本

| 场景 | 推荐版本 | 理由 |
|------|---------|------|
| 通用算法，类型未知 | `advance()` | 标签分发，支持所有迭代器 |
| 已知迭代器类型 | `advance_optimized()` | 编译时优化，无运行时开销 |
| 小距离移动 (n < 16) | `advance_unrolled()` | 循环展开，减少开销 |
| 编译时常量距离 | `advance_const<n>()` | 完全展开，最优性能 |
| 需要边界检查 | `safe_advance()` | 安全第一，防止越界 |

### 2. 错误处理策略

```cpp
template<typename InputIt>
void safe_algorithm(InputIt first, InputIt last, int n) {
    // 静态断言确保迭代器类型
    static_assert(is_iterator_v<InputIt>, "Requires iterator type");
    
    // 运行时边界检查
    if (n < 0) {
        // 检查是否支持反向移动
        if constexpr (!is_bidirectional_iterator_v<InputIt>) {
            throw std::invalid_argument("Negative advance requires bidirectional iterator");
        }
    }
    
    // 使用安全版本
    auto it = first;
    if (!mystl::safe_advance(it, last, n)) {
        throw std::out_of_range("Advance out of bounds");
    }
    
    // 继续处理...
}
```

### 3. 性能调优建议

1. **分析迭代器使用模式**
   ```cpp
   // 分析常见距离
   if (common_distance == 1) {
       // 使用特化版本
       ++it;
   } else if (common_distance <= 8) {
       // 使用展开版本
       mystl::advance_unrolled(it, common_distance);
   } else {
       // 使用通用版本
       mystl::advance(it, common_distance);
   }
   ```

2. **利用编译时信息**
   ```cpp
   template<typename Iterator, int CommonDistance = 1>
   class OptimizedProcessor {
   public:
       void process(Iterator it) {
           // 编译时已知常见距离
           mystl::advance_const<CommonDistance>(it);
           // ... 处理
       }
   };
   ```

3. **避免不必要的边界检查**
   ```cpp
   // 在性能关键路径中，如果确定不会越界
   #ifdef NDEBUG
       mystl::advance_optimized(it, n);  // 无检查
   #else
       if (!mystl::safe_advance(it, end, n)) {
           // 调试版本检查
       }
   #endif
   ```

## 设计哲学

### 1. 零开销抽象
- 为随机访问迭代器生成最优代码（直接算术运算）
- 为其他迭代器提供合理实现
- 编译时决策，无运行时开销

### 2. 渐进式优化
- 基础版本：标签分发，支持所有场景
- 优化版本：if constexpr，已知类型优化
- 特化版本：循环展开，小距离优化
- 常量版本：完全展开，编译时常量优化

### 3. 类型安全
- 静态断言防止错误使用
- 编译时检查迭代器能力
- 安全版本提供运行时检查

### 4. 可扩展性
- 易于添加新的优化策略
- 支持自定义迭代器类型
- 兼容 C++ 标准迭代器概念

## 总结

`iterator_functions` 提供了完整、高效、安全的迭代器操作函数集：

1. **核心功能**：`advance`、`distance`、`next`、`prev` 等基本操作
2. **优化策略**：标签分发、编译时优化、循环展开、常量优化
3. **安全特性**：边界检查、类型安全、错误处理
4. **实用工具**：`iter_swap`、`iter_move`、`size`、`empty` 等辅助函数

通过合理选择不同版本的函数，可以在保证代码安全性的同时获得最佳性能。
