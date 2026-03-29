# map/multimap 实现任务清单

## 概述
基于已完成的红黑树实现，实现map和multimap关联容器。map存储键值对，键唯一；multimap允许重复键。

## 依赖关系
- ✅ 红黑树实现（rb_tree.hpp）
- ✅ pair实现（pair.hpp）
- ✅ 内存分配器系统
- ✅ 迭代器系统
- ✅ 类型特征系统

## 任务分解

### 第一阶段：基础结构设计（预计时间：1天）

#### 1.1 键提取器实现
- [✅] 实现 `select1st` 函数对象，从pair中提取第一个元素（键）
- [✅] 文件：`include/mystl/detail/select1st.hpp`
- [✅] 功能：从 `pair<const Key, T>` 中提取 `const Key&`

#### 1.2 map基础结构
- [✅] 设计map类模板参数
- [✅] 确定内部红黑树类型：`rb_tree<Key, pair<const Key, T>, select1st, Compare, Alloc, false>`
- [✅] 文件：`include/mystl/map.hpp`

#### 1.3 multimap基础结构
- [✅] 设计multimap类模板参数
- [✅] 确定内部红黑树类型：`rb_tree<Key, pair<const Key, T>, select1st, Compare, Alloc, true>`
- [✅] 文件：`include/mystl/multimap.hpp`

### 第二阶段：map实现（预计时间：3天）

#### 2.1 类型定义和成员变量
- [✅] 定义所有标准类型别名（key_type, mapped_type, value_type等）
- [✅] 定义内部红黑树类型
- [✅] 声明私有成员：内部红黑树实例

#### 2.2 构造函数和析构函数
- [✅] 默认构造函数
- [✅] 带比较函数和分配器的构造函数
- [✅] 范围构造函数
- [✅] 拷贝构造函数
- [✅] 移动构造函数
- [✅] 初始化列表构造函数
- [✅] 析构函数

#### 2.3 赋值操作符
- [✅] 拷贝赋值操作符
- [✅] 移动赋值操作符
- [✅] 初始化列表赋值操作符

#### 2.4 元素访问
- [✅] `operator[]` - 下标操作符（map特有）
- [✅] `at()` - 带边界检查的访问
- [✅] 注意：`operator[]` 在键不存在时插入默认值

#### 2.5 容量操作
- [✅] `empty()`, `size()`, `max_size()`
- [✅] 委托给内部红黑树

#### 2.6 迭代器
- [✅] `begin()`, `end()`, `cbegin()`, `cend()`
- [✅] `rbegin()`, `rend()`, `crbegin()`, `crend()`
- [✅] 委托给内部红黑树

#### 2.7 修改操作
- [✅] `insert()` - 插入键值对
- [✅] `emplace()` - 原位构造
- [✅] `emplace_hint()` - 带提示的原位构造
- [✅] `erase()` - 删除元素
- [✅] `clear()` - 清空容器
- [✅] `swap()` - 交换内容

#### 2.8 查找操作
- [✅] `find()` - 查找键
- [✅] `count()` - 计数键（map中为0或1）
- [✅] `lower_bound()` - 下界
- [✅] `upper_bound()` - 上界
- [✅] `equal_range()` - 相等范围

#### 2.9 观察器
- [✅] `key_comp()` - 键比较函数
- [✅] `value_comp()` - 值比较函数

#### 2.10 比较操作符
- [✅] `operator==`, `operator!=`
- [✅] `operator<`, `operator<=`, `operator>`, `operator>=`

### 第三阶段：multimap实现（预计时间：2天）

#### 3.1 复用map实现
- [✅] 基于map实现，修改模板参数允许重复键
- [✅] 注意：multimap没有 `operator[]` 和 `at()`

#### 3.2 特定修改
- [✅] `insert()` - 总是插入，不检查重复
- [✅] `count()` - 可能返回大于1的值
- [✅] `equal_range()` - 返回所有相等键的范围

### 第四阶段：测试实现（预计时间：2天）

#### 4.1 map测试文件
- [✅] 创建 `test/mystl/test_map.cpp`
- [✅] 测试所有map功能
- [✅] 测试边界情况和异常安全

#### 4.2 multimap测试文件
- [✅] 创建 `test/mystl/test_multimap.cpp`
- [✅] 测试所有multimap功能
- [✅] 测试重复键的情况

#### 4.3 集成测试
- [ ] 验证与set/multiset的一致性
- [ ] 验证与标准库的兼容性

### 第五阶段：文档和优化（预计时间：1天）

#### 5.1 代码注释
- [ ] 添加完整的中英文双语注释
- [ ] 遵循项目注释规范

#### 5.2 文档生成
- [ ] 更新任务清单状态
- [ ] 创建学习文档
- [ ] 创建实现文档

#### 5.3 性能优化
- [ ] 检查关键操作的性能
- [ ] 优化内存使用

## 技术要点

### 1. 键提取器设计
```cpp
template<typename Pair>
struct select1st {
    using first_type = typename Pair::first_type;
    using type = first_type;
    
    const first_type& operator()(const Pair& p) const {
        return p.first;
    }
};
```

### 2. map的operator[]实现
```cpp
mapped_type& operator[](const key_type& key) {
    auto result = insert(value_type(key, mapped_type()));
    return result.first->second;
}

mapped_type& operator[](key_type&& key) {
    auto result = insert(value_type(mystl::move(key), mapped_type()));
    return result.first->second;
}
```

### 3. 透明比较器支持
考虑支持C++14的透明比较器：
```cpp
template<typename K>
iterator find(const K& key);
```

## 实现顺序建议

1. **第一天**：完成第一阶段（基础结构设计）
2. **第二到四天**：完成第二阶段（map实现）
3. **第五到六天**：完成第三阶段（multimap实现）
4. **第七到八天**：完成第四阶段（测试实现）
5. **第九天**：完成第五阶段（文档和优化）

## 质量要求

### 代码质量
- 遵循项目编码规范
- 完整的错误处理
- 异常安全保证
- 内存安全

### 测试覆盖率
- 所有公有接口都有测试
- 边界情况测试
- 异常情况测试
- 性能基准测试

### 文档质量
- 中英文双语注释
- 清晰的API文档
- 设计决策说明
- 使用示例

## 风险与挑战

### 技术风险
1. **红黑树接口适配**：确保map正确使用红黑树接口
2. **迭代器类型**：map迭代器需要正确解引用为pair
3. **异常安全**：保证所有操作提供基本异常安全保证

### 时间风险
1. **复杂度估计**：实际实现可能比预计更复杂
2. **测试时间**：全面测试可能需要更多时间

### 缓解措施
1. **逐步实现**：先实现核心功能，再添加高级特性
2. **频繁测试**：每完成一个功能就进行测试
3. **代码审查**：定期检查代码质量

## 成功标准

### 功能完成
- [ ] map所有功能实现并通过测试
- [ ] multimap所有功能实现并通过测试
- [ ] 与标准库接口兼容

### 质量达标
- [ ] 所有测试通过
- [ ] 代码符合项目规范
- [ ] 文档完整

### 性能要求
- [ ] 关键操作时间复杂度符合标准
- [ ] 内存使用合理

## 更新日志

| 日期 | 版本 | 描述 |
|------|------|------|
| 2026-03-30 | 1.0.0 | 初始版本，基于项目需求创建 |

## 参考资源

### 项目内部参考
- `include/mystl/set.hpp` - set实现参考
- `include/mystl/multiset.hpp` - multiset实现参考
- `include/mystl/rb_tree.hpp` - 底层红黑树实现
- `include/utility/pair.hpp` - pair实现

### 外部参考
- C++标准：ISO/IEC 14882 §26.4.4 (map) 和 §26.4.5 (multimap)
- cppreference.com: std::map, std::multimap
- GCC libstdc++: `bits/stl_map.h`, `bits/stl_multimap.h`

## 备注

1. **教育优先**：理解map/multimap设计原理比完成功能更重要
2. **标准对齐**：尽量与C++标准库行为保持一致
3. **测试驱动**：先写测试用例，再实现功能
4. **文档同步**：代码和文档同步更新

---
*清单创建时间：2026-03-30*  
*基于mySTL项目当前状态和规划文档*
