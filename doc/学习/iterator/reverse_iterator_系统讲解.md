# reverse_iterator 系统详解

## 概述

`reverse_iterator` 是 C++ 标准库中最重要的迭代器适配器之一，它允许以相反的顺序遍历容器。通过包装一个正向迭代器，`reverse_iterator` 提供了一种简洁、高效的方式来实现反向遍历，而无需修改底层容器或算法。

## 核心功能

### 1. 反向遍历
将正向迭代器的方向反转，使得 `begin()` 对应原容器的 `end()-1`，`end()` 对应原容器的 `begin()-1`。

### 2. 透明适配
- 保持底层迭代器的所有特性（随机访问、双向等）
- 自动处理边界条件和空范围
- 提供与正向迭代器相同的接口

### 3. 类型保持
通过 `iterator_traits` 提取并保持底层迭代器的所有类型信息：
- `iterator_category` - 迭代器类别
- `value_type` - 元素类型
- `difference_type` - 距离类型
- `pointer` - 指针类型
- `reference` - 引用类型

## 实现架构

### 类定义
```cpp
template<typename Iterator>
class reverse_iterator {
private:
    Iterator current_;  // 底层正向迭代器

public:
    using iterator_type = Iterator;
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = typename iterator_traits<Iterator>::pointer;
    using reference = typename iterator_traits<Iterator>::reference;
    
    // 构造函数
    reverse_iterator() = default;
    explicit reverse_iterator(Iterator x);
    
    // 转换构造函数
    template<typename Iter>
    reverse_iterator(const reverse_iterator<Iter>& other);
    
    // 获取底层迭代器
    Iterator base() const;
    
    // 解引用操作
    reference operator*() const;
    pointer operator->() const;
    reference operator[](difference_type n) const;
    
    // 递增/递减
    reverse_iterator& operator++();
    reverse_iterator operator++(int);
    reverse_iterator& operator--();
    reverse_iterator operator--(int);
    
    // 算术运算
    reverse_iterator operator+(difference_type n) const;
    reverse_iterator& operator+=(difference_type n);
    reverse_iterator operator-(difference_type n) const;
    reverse_iterator& operator-=(difference_type n);
};
```

### 关键实现细节

#### 1. 解引用操作
```cpp
reference operator*() const {
    Iterator tmp = current_;
    return *--tmp;  // 返回前一个元素
}

pointer operator->() const {
    Iterator tmp = current_;
    --tmp;
    return &*tmp;  // 返回指向前一个元素的指针
}
```

#### 2. 关系运算符
```cpp
template<typename Iterator1, typename Iterator2>
bool operator==(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
    return lhs.base() == rhs.base();  // 比较底层迭代器
}

template<typename Iterator1, typename Iterator2>
bool operator<(const reverse_iterator<Iterator1>& lhs,
               const reverse_iterator<Iterator2>& rhs) {
    return lhs.base() > rhs.base();  // 反向比较
}
```

## 设计模式

### 1. 适配器模式
`reverse_iterator` 是典型的适配器模式应用：
- **目标接口**：标准迭代器接口
- **被适配者**：正向迭代器
- **适配器**：`reverse_iterator` 类

### 2. 代理模式
通过重载 `operator*` 和 `operator->`，`reverse_iterator` 充当了底层元素的代理，透明地处理方向反转。

### 3. 类型擦除
通过模板和 `iterator_traits`，`reverse_iterator` 可以适配任何符合迭代器概念的类型，包括：
- 自定义迭代器类
- 原始指针
- 标准库迭代器

## 使用示例

### 示例 1：基本反向遍历
```cpp
#include <vector>
#include <iostream>
#include "include/iterator/reverse_iterator.hpp"

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 使用 reverse_iterator 反向遍历
    for (auto rit = vec.rbegin(); rit != vec.rend(); ++rit) {
        std::cout << *rit << " ";  // 输出: 5 4 3 2 1
    }
    
    // 等价于
    for (auto it = vec.end(); it != vec.begin(); ) {
        std::cout << *--it << " ";  // 输出: 5 4 3 2 1
    }
    
    return 0;
}
```

### 示例 2：算法中的反向迭代器
```cpp
#include <algorithm>
#include <vector>
#include "include/iterator/reverse_iterator.hpp"

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 反向查找
    auto rit = std::find(vec.rbegin(), vec.rend(), 3);
    if (rit != vec.rend()) {
        std::cout << "Found 3 at reverse position " 
                  << (rit - vec.rbegin()) << std::endl;
    }
    
    // 反向排序
    std::sort(vec.rbegin(), vec.rend());  // 降序排序
    
    // 反向复制
    std::vector<int> reversed(vec.size());
    std::copy(vec.rbegin(), vec.rend(), reversed.begin());
    
    return 0;
}
```

### 示例 3：自定义容器的反向迭代器
```cpp
template<typename T>
class MyContainer {
private:
    T* data_;
    size_t size_;
    
public:
    // 正向迭代器
    class iterator {
        // ... 实现正向迭代器
    };
    
    // 反向迭代器
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    
    // const 版本
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;
    
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
    
    const_reverse_iterator crbegin() const {
        return rbegin();
    }
    
    const_reverse_iterator crend() const {
        return rend();
    }
};
```

## 设计要点

### 1. 边界处理
`reverse_iterator` 的关键设计挑战是正确处理边界：
- `rbegin()` 对应 `end()`，解引用时返回 `end()-1`
- `rend()` 对应 `begin()`，解引用时返回 `begin()-1`（无效）
- 递增操作对应底层迭代器的递减
- 递减操作对应底层迭代器的递增

### 2. 类型安全
- 通过模板确保类型安全
- 支持不同但兼容的迭代器类型之间的比较和转换
- 保持 const 正确性

### 3. 性能考虑
- 零开销抽象：所有操作都是内联的
- 无额外内存分配
- 编译时类型检查

### 4. 与标准库的兼容性
- 接口与 `std::reverse_iterator` 完全兼容
- 支持所有标准算法
- 正确处理迭代器类别和特性

## 最佳实践

### 1. 正确实现容器接口
```cpp
template<typename T>
class MyVector {
public:
    // 提供完整的迭代器接口
    iterator begin() { /* ... */ }
    iterator end() { /* ... */ }
    
    // 提供反向迭代器
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    
    // const 版本
    const_reverse_iterator rbegin() const { 
        return const_reverse_iterator(end()); 
    }
    const_reverse_iterator rend() const { 
        return const_reverse_iterator(begin()); 
    }
    
    // C++11 风格
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }
};
```

### 2. 算法中的使用
```cpp
// 使用反向迭代器实现从后向前查找
template<typename Container, typename T>
auto find_last(const Container& c, const T& value) {
    auto rit = std::find(c.rbegin(), c.rend(), value);
    if (rit == c.rend()) {
        return c.end();
    }
    // 将反向迭代器转换为正向迭代器
    return --rit.base();
}

// 使用反向迭代器检查回文
template<typename Container>
bool is_palindrome(const Container& c) {
    return std::equal(c.begin(), c.begin() + c.size()/2, c.rbegin());
}
```

### 3. 性能优化
```cpp
// 对于随机访问迭代器，使用直接算术运算
template<typename RandomAccessIterator>
reverse_iterator<RandomAccessIterator>
operator+(typename reverse_iterator<RandomAccessIterator>::difference_type n,
          const reverse_iterator<RandomAccessIterator>& it) {
    return it + n;  // 利用底层迭代器的随机访问特性
}

// 对于双向迭代器，使用循环
template<typename BidirectionalIterator>
reverse_iterator<BidirectionalIterator>&
operator++(reverse_iterator<BidirectionalIterator>& it) {
    --it.base();  // 底层迭代器递减
    return it;
}
```

## 常见问题

### Q: reverse_iterator 和反向遍历有什么区别？
A: `reverse_iterator` 是一个适配器，它包装正向迭代器并提供反向遍历接口。与手动反向遍历相比：
- **更安全**：自动处理边界条件
- **更通用**：可与所有标准算法一起使用
- **更清晰**：代码意图更明确

### Q: 如何将 reverse_iterator 转换回正向迭代器？
A: 使用 `base()` 方法：
```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};
auto rit = vec.rbegin();  // 指向 5
auto it = rit.base();     // 指向 end()
// 注意：rit.base() 指向 rit 后面的元素
```

### Q: reverse_iterator 支持哪些迭代器类别？
A: 支持所有迭代器类别，但行为不同：
- **随机访问**：支持所有算术运算
- **双向**：支持递增/递减
- **前向**：仅支持单向遍历（实际很少用）
- **输入/输出**：理论上支持，但实际意义有限

### Q: 为什么 operator* 返回前一个元素？
A: 这是设计决策，使得：
- `rbegin()` 指向最后一个元素
- `rend()` 指向第一个元素之前
- 范围 `[rbegin(), rend())` 包含所有元素

## 测试要点

### 1. 功能测试
```cpp
// 测试基本反向遍历
TEST(reverse_iterator, basic_traversal) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    mystl::reverse_iterator<std::vector<int>::iterator> rit(vec.end());
    
    EXPECT_EQ(*rit, 5);
    ++rit;
    EXPECT_EQ(*rit, 4);
    --rit;
    EXPECT_EQ(*rit, 5);
}

// 测试算法兼容性
TEST(reverse_iterator, algorithm_compatibility) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto rit = std::find(vec.rbegin(), vec.rend(), 3);
    
    EXPECT_NE(rit, vec.rend());
    EXPECT_EQ(*rit, 3);
    EXPECT_EQ(rit.base(), vec.begin() + 3);
}
```

### 2. 边界测试
```cpp
// 测试空容器
TEST(reverse_iterator, empty_container) {
    std::vector<int> vec;
    EXPECT_EQ(vec.rbegin(), vec.rend());
}

// 测试单元素容器
TEST(reverse_iterator, single_element) {
    std::vector<int> vec = {42};
    auto rit = vec.rbegin();
    
    EXPECT_EQ(*rit, 42);
    ++rit;
    EXPECT_EQ(rit, vec.rend());
}
```

### 3. 性能测试
```cpp
// 测试反向遍历性能
TEST(reverse_iterator, performance) {
    const size_t N = 1000000;
    std::vector<int> vec(N);
    std::iota(vec.begin(), vec.end(), 0);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 反向求和
    int sum = 0;
    for (auto rit = vec.rbegin(); rit != vec.rend(); ++rit) {
        sum += *rit;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 10);  // 应在10ms内完成
}
```

## 总结

`reverse_iterator` 是 C++ 迭代器适配器的典范，它展示了：
1. **适配器模式的威力**：通过包装现有组件提供新功能
2. **零开销抽象**：在保持高性能的同时提供高级接口
3. **类型安全的泛型编程**：通过模板支持多种类型
4. **标准库兼容性**：无缝集成到 C++ 生态系统中

理解 `reverse_iterator` 不仅有助于使用标准库，还为设计自己的适配器提供了宝贵经验。它是学习迭代器系统、模板元编程和设计模式的重要案例。

---
*文档更新时间：2026-03-19*  
*基于 mySTL 项目实现*  
*参考：C++标准、GCC libstdc++、LLVM libc++*
