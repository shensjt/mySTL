# type_traits 系统详解

## 概述

`type_traits` 是 C++ 模板元编程的核心组件，提供编译时类型内省、类型分类和类型转换功能。它是实现通用、高效、类型安全的 C++ 代码的基础。

## 核心功能分类

### 1. 基础工具类

#### integral_constant
```cpp
template<typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant<T, v>;
    
    constexpr operator value_type() const noexcept;
    constexpr value_type operator()() const noexcept;
};
```
- 所有类型特性的基类
- 提供编译时常量值和类型信息
- 支持隐式转换和函数调用

#### true_type / false_type
```cpp
using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;
```
- 布尔类型特性的基础
- 用于 SFINAE 和编译时条件判断

### 2. 类型转换（基本）

#### 移除限定符
- `remove_const` / `remove_const_t` - 移除 const 限定符
- `remove_volatile` / `remove_volatile_t` - 移除 volatile 限定符
- `remove_cv` / `remove_cv_t` - 同时移除 const 和 volatile 限定符
- `remove_reference` / `remove_reference_t` - 移除引用

#### 添加限定符
- `add_const` / `add_const_t` - 添加 const 限定符
- `add_volatile` / `add_volatile_t` - 添加 volatile 限定符
- `add_cv` / `add_cv_t` - 同时添加 const 和 volatile 限定符
- `add_pointer` / `add_pointer_t` - 添加指针
- `add_lvalue_reference` / `add_lvalue_reference_t` - 添加左值引用
- `add_rvalue_reference` / `add_rvalue_reference_t` - 添加右值引用

### 3. 类型检查

#### 基本类型检查
- `is_same` / `is_same_v` - 检查两个类型是否相同
- `is_const` / `is_const_v` - 检查是否 const 限定
- `is_volatile` / `is_volatile_v` - 检查是否 volatile 限定
- `is_pointer` / `is_pointer_v` - 检查是否为指针
- `is_reference` / `is_reference_v` - 检查是否为引用
- `is_lvalue_reference` / `is_lvalue_reference_v` - 检查是否为左值引用
- `is_rvalue_reference` / `is_rvalue_reference_v` - 检查是否为右值引用
- `is_void` / `is_void_v` - 检查是否为 void
- `is_null_pointer` / `is_null_pointer_v` - 检查是否为 nullptr_t

#### 数值类型检查
- `is_integral` / `is_integral_v` - 检查是否为整数类型
- `is_floating_point` / `is_floating_point_v` - 检查是否为浮点类型
- `is_arithmetic` / `is_arithmetic_v` - 检查是否为算术类型
- `is_signed` / `is_signed_v` - 检查是否为有符号类型
- `is_unsigned` / `is_unsigned_v` - 检查是否为无符号类型

#### 复合类型检查
- `is_array` / `is_array_v` - 检查是否为数组
- `is_function` / `is_function_v` - 检查是否为函数
- `is_class` / `is_class_v` - 检查是否为类/结构体

### 4. 条件选择

#### conditional
```cpp
template<bool B, typename T, typename F>
struct conditional {
    using type = T;  // B 为真时选择 T
};

template<typename T, typename F>
struct conditional<false, T, F> {
    using type = F;  // B 为假时选择 F
};

template<bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;
```
- 编译时的三元运算符
- 根据布尔条件选择类型

#### 逻辑运算
- `conjunction` / `conjunction_v` - 逻辑与（所有条件为真）
- `disjunction` / `disjunction_v` - 逻辑或（任一条件为真）
- `negation` / `negation_v` - 逻辑非

### 5. SFINAE 支持

#### enable_if
```cpp
template<bool B, typename T = void>
struct enable_if {};

template<typename T>
struct enable_if<true, T> {
    using type = T;
};

template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;
```
- SFINAE（替换失败不是错误）的核心工具
- 根据条件启用/禁用模板特化

#### void_t
```cpp
template<typename...>
using void_t = void;
```
- 将任意类型列表映射到 void
- 用于检测类型是否具有特定成员

### 6. 构造和赋值特性

#### 构造检查
- `is_constructible` / `is_constructible_v` - 检查是否可从参数构造
- `is_default_constructible` / `is_default_constructible_v` - 检查是否可默认构造
- `is_copy_constructible` / `is_copy_constructible_v` - 检查是否可复制构造
- `is_move_constructible` / `is_move_constructible_v` - 检查是否可移动构造

#### 赋值检查
- `is_assignable` / `is_assignable_v` - 检查是否可从另一类型赋值
- `is_copy_assignable` / `is_copy_assignable_v` - 检查是否可复制赋值
- `is_move_assignable` / `is_move_assignable_v` - 检查是否可移动赋值

#### 析构检查
- `is_destructible` / `is_destructible_v` - 检查是否可析构

### 7. 类型属性

#### 平凡性
- `is_trivial` / `is_trivial_v` - 检查是否为平凡类型
- `is_trivially_copyable` / `is_trivially_copyable_v` - 检查是否为平凡可复制类型
- `is_trivially_default_constructible` / `is_trivially_default_constructible_v` - 检查是否为平凡默认构造

#### 布局和继承
- `is_standard_layout` / `is_standard_layout_v` - 检查是否为标准布局
- `is_polymorphic` / `is_polymorphic_v` - 检查是否为多态类型（有虚函数）
- `is_abstract` / `is_abstract_v` - 检查是否为抽象类
- `is_final` / `is_final_v` - 检查是否为 final 类
- `is_empty` / `is_empty_v` - 检查是否为空类

#### 枚举
- `is_enum` / `is_enum_v` - 检查是否为枚举类型
- `underlying_type` / `underlying_type_t` - 获取枚举的基础类型

### 8. 高级类型转换

#### 数值类型转换
- `make_signed` / `make_signed_t` - 转换为有符号类型
- `make_unsigned` / `make_unsigned_t` - 转换为无符号类型

#### 公共类型
- `common_type` / `common_type_t` - 查找一组类型的公共类型

#### 退化
- `decay` / `decay_t` - 应用按值传递时的类型转换
  - 数组 → 指针
  - 函数 → 函数指针
  - 移除 cv 限定符

### 9. 类型关系

#### 继承关系
- `is_base_of` / `is_base_of_v` - 检查是否为基类

#### 转换关系
- `is_convertible` / `is_convertible_v` - 检查是否可转换

## 关键设计模式

### 1. SFINAE 模式
```cpp
template<typename T, typename = void>
struct has_type_member : false_type {};

template<typename T>
struct has_type_member<T, void_t<typename T::type>> : true_type {};
```

### 2. 标签分发模式
```cpp
template<typename T>
void process_impl(T value, true_type) {
    // 针对有特定特性的类型
}

template<typename T>
void process_impl(T value, false_type) {
    // 针对没有特定特性的类型
}

template<typename T>
void process(T value) {
    process_impl(value, has_type_member<T>{});
}
```

### 3. 编译时优化
```cpp
template<typename Iterator>
void advance(Iterator& it, difference_type n) {
    if constexpr (is_random_access_iterator_v<Iterator>) {
        it += n;  // O(1)
    } else {
        while (n-- > 0) ++it;  // O(n)
    }
}
```

## 使用示例

### 示例 1：安全类型转换
```cpp
template<typename From, typename To>
To safe_cast(From value) {
    static_assert(is_convertible_v<From, To>,
                  "Cannot convert from From to To");
    return static_cast<To>(value);
}
```

### 示例 2：通用算法优化
```cpp
template<typename T>
void copy_optimized(const T* src, T* dest, size_t count) {
    if constexpr (is_trivially_copyable_v<T>) {
        // 使用 memcpy 优化
        std::memcpy(dest, src, count * sizeof(T));
    } else {
        // 通用实现
        for (size_t i = 0; i < count; ++i) {
            dest[i] = src[i];
        }
    }
}
```

### 示例 3：类型安全的工厂函数
```cpp
template<typename T, typename... Args>
enable_if_t<is_constructible_v<T, Args...>, T*>
create(Args&&... args) {
    return new T(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
enable_if_t<!is_constructible_v<T, Args...>, T*>
create(Args&&... args) {
    // 提供默认构造或错误处理
    return new T();
}
```

### 示例 4：编译时类型选择
```cpp
template<typename T>
using storage_type = conditional_t<
    sizeof(T) <= sizeof(void*),
    T,           // 小类型：直接存储
    T*           // 大类型：存储指针
>;

template<typename T>
class Optional {
    storage_type<T> value_;
    bool has_value_;
    
public:
    // 根据存储类型选择不同的实现
    // ...
};
```

## 最佳实践

### 1. 优先使用变量模板
```cpp
// 好：简洁明了
if constexpr (is_integral_v<T>) { ... }

// 不好：冗长
if constexpr (is_integral<T>::value) { ... }
```

### 2. 合理使用 static_assert
```cpp
template<typename T>
class Container {
    static_assert(is_default_constructible_v<T>,
                  "T must be default constructible");
    static_assert(is_copy_constructible_v<T>,
                  "T must be copy constructible");
    // ...
};
```

### 3. 利用编译时优化
```cpp
template<typename T>
void process(T& obj) {
    // 根据类型特性选择不同算法
    if constexpr (is_trivial_v<T>) {
        process_trivial(obj);
    } else if constexpr (is_polymorphic_v<T>) {
        process_polymorphic(obj);
    } else {
        process_generic(obj);
    }
}
```

### 4. 避免过度使用 SFINAE
```cpp
// 好：使用 concepts（C++20）
template<typename T>
requires is_integral_v<T>
void process_integral(T value) { ... }

// 可以：使用 enable_if
template<typename T, typename = enable_if_t<is_integral_v<T>>>
void process_integral(T value) { ... }

// 不好：过度复杂的 SFINAE
template<typename T>
auto process_integral(T value) -> decltype(
    std::declval<T>() + 0, void()
) { ... }
```

## 性能考虑

### 1. 编译时开销
- 类型特性在编译时计算，不影响运行时性能
- 复杂的类型特性可能增加编译时间
- 合理使用可以显著减少代码膨胀

### 2. 运行时优化
```cpp
template<typename T>
void algorithm(T* data, size_t size) {
    // 编译时决策，无运行时开销
    if constexpr (is_trivially_copyable_v<T>) {
        std::memcpy(buffer, data, size * sizeof(T));
    } else {
        std::copy(data, data + size, buffer);
    }
}
```

### 3. 代码生成
- 类型特性帮助编译器生成更优化的代码
- 消除不必要的运行时检查
- 启用特定于类型的优化

## 常见问题

### Q: is_same 和 is_base_of 有什么区别？
A: 
- `is_same<T, U>`：检查 T 和 U 是否为完全相同类型
- `is_base_of<Base, Derived>`：检查 Base 是否为 Derived 的基类（或相同类型）

### Q: decay 的主要用途是什么？
A: `decay` 模拟按值传递时的类型转换：
- 数组 → 指针
- 函数 → 函数指针
- 移除 cv 限定符
常用于实现完美转发和类型推导。

### Q: 如何检查类型是否具有特定成员？
A: 使用 `void_t` 和 SFINAE：
```cpp
template<typename T, typename = void>
struct has_size_member : false_type {};

template<typename T>
struct has_size_member<T, void_t<decltype(std::declval<T>().size())>> 
    : true_type {};
```

### Q: 什么时候使用 enable_if？
A: 当需要根据类型特性有条件地启用模板时：
- 函数重载解析
- 类模板特化
- 防止不合理的模板实例化

## 总结

`type_traits` 是现代 C++ 编程的基石，它：

1. **提供类型安全**：编译时检查类型约束
2. **启用优化**：根据类型特性选择最优实现
3. **支持泛型编程**：编写适用于多种类型的通用代码
4. **实现元编程**：在编译时进行计算和决策

掌握 `type_traits` 对于编写高效、安全、可维护的 C++ 代码至关重要。
