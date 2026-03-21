#pragma once

// allocator_traits - 提供分配器的统一接口
// 参考：C++11标准，GCC libstdc++ bits/alloc_traits.h

#include <cstddef>
#include <new>
#include "../utility/type_traits.hpp"
#include "../utility/move.hpp"

namespace mystl {

namespace detail {
    // SFINAE检测类型是否存在
    template<typename T, typename = void>
    struct has_pointer : false_type {};
    
    template<typename T>
    struct has_pointer<T, void_t<typename T::pointer>> : true_type {};
    
    template<typename T, typename = void>
    struct has_const_pointer : false_type {};
    
    template<typename T>
    struct has_const_pointer<T, void_t<typename T::const_pointer>> : true_type {};
    
    template<typename T, typename = void>
    struct has_void_pointer : false_type {};
    
    template<typename T>
    struct has_void_pointer<T, void_t<typename T::void_pointer>> : true_type {};
    
    template<typename T, typename = void>
    struct has_const_void_pointer : false_type {};
    
    template<typename T>
    struct has_const_void_pointer<T, void_t<typename T::const_void_pointer>> : true_type {};
    
    template<typename T, typename = void>
    struct has_size_type : false_type {};
    
    template<typename T>
    struct has_size_type<T, void_t<typename T::size_type>> : true_type {};
    
    template<typename T, typename = void>
    struct has_difference_type : false_type {};
    
    template<typename T>
    struct has_difference_type<T, void_t<typename T::difference_type>> : true_type {};
}

// allocator_traits主模板
template<typename Alloc>
struct allocator_traits {
    // 类型定义
    using allocator_type = Alloc;
    using value_type = typename Alloc::value_type;
    
    // 指针类型，如果分配器定义了则使用，否则使用默认
    using pointer = typename conditional<
        detail::has_pointer<Alloc>::value,
        typename Alloc::pointer,
        value_type*
    >::type;
    
    using const_pointer = typename conditional<
        detail::has_const_pointer<Alloc>::value,
        typename Alloc::const_pointer,
        const value_type*
    >::type;
    
    using void_pointer = typename conditional<
        detail::has_void_pointer<Alloc>::value,
        typename Alloc::void_pointer,
        void*
    >::type;
    
    using const_void_pointer = typename conditional<
        detail::has_const_void_pointer<Alloc>::value,
        typename Alloc::const_void_pointer,
        const void*
    >::type;
    
    // 大小和差异类型
    using size_type = typename conditional<
        detail::has_size_type<Alloc>::value,
        typename Alloc::size_type,
        std::size_t
    >::type;
    
    using difference_type = typename conditional<
        detail::has_difference_type<Alloc>::value,
        typename Alloc::difference_type,
        std::ptrdiff_t
    >::type;
    
    // 传播特性（默认为false_type）
    using propagate_on_container_copy_assignment = 
        typename Alloc::propagate_on_container_copy_assignment;
    
    using propagate_on_container_move_assignment = 
        typename Alloc::propagate_on_container_move_assignment;
    
    using propagate_on_container_swap = 
        typename Alloc::propagate_on_container_swap;
    
    using is_always_equal = typename Alloc::is_always_equal;
    
    // 静态成员函数
    
    // 分配内存
    [[nodiscard]] static pointer allocate(Alloc& a, size_type n) {
        return a.allocate(n);
    }
    
    // 带提示的分配
    [[nodiscard]] static pointer allocate(Alloc& a, size_type n, const_void_pointer hint) {
        return a.allocate(n, hint);
    }
    
    // 释放内存
    static void deallocate(Alloc& a, pointer p, size_type n) {
        a.deallocate(p, n);
    }
    
    // 最大大小
    static size_type max_size(const Alloc& a) noexcept {
        return a.max_size();
    }
    
    // 构造对象
    template<typename T, typename... Args>
    static void construct(Alloc& a, T* p, Args&&... args) {
        a.construct(p, mystl::forward<Args>(args)...);
    }
    
    // 销毁对象
    template<typename T>
    static void destroy(Alloc& a, T* p) {
        a.destroy(p);
    }
    
    // 容器复制构造选择
    static Alloc select_on_container_copy_construction(const Alloc& a) {
        return a.select_on_container_copy_construction();
    }
};

} // namespace mystl
