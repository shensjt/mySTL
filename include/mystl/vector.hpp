#pragma once

/**
 * @file vector.hpp
 * @brief Dynamic array container implementation for mySTL
 * @brief 动态数组容器实现（mySTL）
 * 
 * Provides vector (dynamic array) container with random access,
 * contiguous storage, and automatic memory management.
 * This implementation follows the C++ standard vector specification,
 * providing amortized O(1) insertion at the end and O(1) random access.
 * 
 * 提供具有随机访问、连续存储和自动内存管理的动态数组容器。
 * 此实现遵循C++标准vector规范，提供摊还O(1)的尾部插入和O(1)随机访问。
 * 
 * @details
 * The vector is a sequence container that encapsulates dynamic size arrays.
 * Elements are stored contiguously in memory, which allows efficient
 * random access and cache locality. The container automatically handles
 * memory allocation and reallocation as elements are added or removed.
 * 
 * @details
 * vector是一个封装动态大小数组的序列容器。
 * 元素在内存中连续存储，这允许高效的随机访问和缓存局部性。
 * 容器在添加或删除元素时自动处理内存分配和重新分配。
 * 
 * @note Key features:
 * 1. Random access: O(1) access to any element via operator[]
 * 2. Contiguous storage: elements are stored in contiguous memory
 * 3. Dynamic resizing: automatic memory management with amortized O(1) push_back
 * 4. Three-pointer model: uses begin_, end_, capacity_ pointers for efficiency
 * 5. Exception safety: provides strong exception guarantee for most operations
 * 
 * @note 主要特性：
 * 1. 随机访问：通过operator[] O(1)访问任何元素
 * 2. 连续存储：元素在连续内存中存储
 * 3. 动态调整大小：自动内存管理，摊还O(1)的push_back
 * 4. 三指针模型：使用begin_、end_、capacity_指针提高效率
 * 5. 异常安全：为大多数操作提供强异常保证
 * 
 * @author shensjt
 * @date 2026-03-20
 * 
 * @see https://en.cppreference.com/w/cpp/container/vector
 * @see 相关参考：https://zh.cppreference.com/w/cpp/container/vector
 */

#include <cstddef>
#include <initializer_list>
#include "../memory/allocator.hpp"
#include "../memory/allocator_traits.hpp"
#include "../memory/uninitialized.hpp"
#include "../iterator/iterator_traits.hpp"
#include "../iterator/iterator_functions.hpp"
#include "../iterator/reverse_iterator.hpp"
#include "../utility/move.hpp"
#include "../utility/swap.hpp"
#include "../algorithm/algorithm.hpp"

// ==================== Namespace mystl ====================
// ==================== 命名空间 mystl ====================

namespace mystl {

// ==================== vector Class ====================
// ==================== vector 类 ====================

/**
 * @brief Dynamic array container
 * @brief 动态数组容器
 * 
 * @tparam T Type of elements stored in the vector
 * @tparam T 存储在vector中的元素类型
 * @tparam Allocator Allocator type for memory management
 * @tparam Allocator 用于内存管理的分配器类型
 * 
 * @details
 * The vector is a sequence container that encapsulates dynamic size arrays.
 * Elements are stored contiguously in memory, which allows efficient
 * random access and cache locality. The container automatically handles
 * memory allocation and reallocation as elements are added or removed.
 * 
 * @details
 * vector是一个封装动态大小数组的序列容器。
 * 元素在内存中连续存储，这允许高效的随机访问和缓存局部性。
 * 容器在添加或删除元素时自动处理内存分配和重新分配。
 * 
 * @note Key characteristics:
 * 1. Random access: O(1) access to any element via operator[]
 * 2. Contiguous storage: elements are stored in contiguous memory
 * 3. Dynamic resizing: automatic memory management with amortized O(1) push_back
 * 4. Three-pointer model: uses begin_, end_, capacity_ pointers for efficiency
 * 5. Exception safety: provides strong exception guarantee for most operations
 * 
 * @note 主要特性：
 * 1. 随机访问：通过operator[] O(1)访问任何元素
 * 2. 连续存储：元素在连续内存中存储
 * 3. 动态调整大小：自动内存管理，摊还O(1)的push_back
 * 4. 三指针模型：使用begin_、end_、capacity_指针提高效率
 * 5. 异常安全：为大多数操作提供强异常保证
 * 
 * @see https://en.cppreference.com/w/cpp/container/vector
 * @see 相关参考：https://zh.cppreference.com/w/cpp/container/vector
 */
template<typename T, typename Allocator = allocator<T>>
class vector {
public:
    // ==================== 类型定义 ====================
    // ==================== Type Definitions ====================
    
    /**
     * @brief Type of elements stored in the vector
     * @brief vector中存储的元素类型
     */
    using value_type = T;
    
    /**
     * @brief Type of the allocator used for memory management
     * @brief 用于内存管理的分配器类型
     */
    using allocator_type = Allocator;
    
    /**
     * @brief Unsigned integer type used for size and capacity
     * @brief 用于大小和容量的无符号整数类型
     */
    using size_type = std::size_t;
    
    /**
     * @brief Signed integer type used for differences between iterators
     * @brief 用于迭代器之间差值的有符号整数类型
     */
    using difference_type = std::ptrdiff_t;
    
    /**
     * @brief Reference type to elements
     * @brief 元素的引用类型
     */
    using reference = value_type&;
    
    /**
     * @brief Const reference type to elements
     * @brief 元素的常量引用类型
     */
    using const_reference = const value_type&;
    
    /**
     * @brief Pointer type to elements
     * @brief 元素的指针类型
     */
    using pointer = T*;
    
    /**
     * @brief Const pointer type to elements
     * @brief 元素的常量指针类型
     */
    using const_pointer = const T*;
    
    // ==================== 迭代器类型 ====================
    // ==================== Iterator Types ====================
    
    /**
     * @brief Random access iterator type
     * @brief 随机访问迭代器类型
     * 
     * @details In vector, iterator is simply a pointer to T.
     * This provides O(1) random access to elements.
     * 
     * @details 在vector中，迭代器就是指向T的指针。
     * 这提供了O(1)的随机访问元素能力。
     */
    using iterator = pointer;
    
    /**
     * @brief Const random access iterator type
     * @brief 常量随机访问迭代器类型
     */
    using const_iterator = const_pointer;
    
    /**
     * @brief Reverse iterator type
     * @brief 反向迭代器类型
     * 
     * @details Allows traversal of the vector in reverse order.
     * 
     * @details 允许以相反顺序遍历vector。
     */
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    
    /**
     * @brief Const reverse iterator type
     * @brief 常量反向迭代器类型
     */
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;
    
private:
    /**
     * @brief Pointer to the first element in the vector
     * @brief 指向vector中第一个元素的指针
     */
    pointer begin_;
    
    /**
     * @brief Pointer to one past the last element in the vector
     * @brief 指向vector中最后一个元素之后位置的指针
     * 
     * @details This pointer marks the end of the valid elements.
     * The range [begin_, end_) contains all elements in the vector.
     * 
     * @details 此指针标记有效元素的结束。
     * 范围 [begin_, end_) 包含vector中的所有元素。
     */
    pointer end_;
    
    /**
     * @brief Pointer to one past the allocated memory
     * @brief 指向分配内存之后位置的指针
     * 
     * @details This pointer marks the end of the allocated memory.
     * The range [begin_, capacity_) is the total allocated memory.
     * 
     * @details 此指针标记分配内存的结束。
     * 范围 [begin_, capacity_) 是总共分配的内存。
     */
    pointer capacity_;
    
    /**
     * @brief Allocator instance for memory management
     * @brief 用于内存管理的分配器实例
     */
    Allocator alloc_;
    
    /**
     * @brief Allocator traits type alias
     * @brief 分配器特性类型别名
     * 
     * @details Used to access allocator traits like allocate, deallocate,
     * construct, destroy, etc.
     * 
     * @details 用于访问分配器特性，如allocate、deallocate、
     * construct、destroy等。
     */
    using alloc_traits = allocator_traits<Allocator>;
    
public:
    // ==================== 构造函数和析构函数 ====================
    
    /**
     * @brief Default constructor
     * @brief 默认构造函数
     * 
     * @details Constructs an empty vector with no elements.
     * The allocator is default-constructed.
     * 
     * @details 构造一个没有元素的空vector。
     * 分配器被默认构造。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This constructor is noexcept, meaning it cannot throw exceptions.
     * 
     * @note 此构造函数是noexcept的，意味着它不能抛出异常。
     * 
     * @note The vector is initialized with null pointers for begin_, end_, and capacity_,
     * indicating that no memory is allocated initially.
     * 
     * @note vector使用begin_、end_和capacity_的空指针初始化，
     * 表示初始时没有分配内存。
     */
    vector() noexcept : begin_(nullptr), end_(nullptr), capacity_(nullptr) {}
    
    /**
     * @brief Constructor with allocator
     * @brief 带分配器的构造函数
     * 
     * @param alloc Allocator to use for memory management
     * @param alloc 用于内存管理的分配器
     * 
     * @details Constructs an empty vector with no elements, using the specified allocator.
     * The vector is initialized with null pointers for begin_, end_, and capacity_,
     * indicating that no memory is allocated initially.
     * 
     * @details 使用指定的分配器构造一个没有元素的空vector。
     * vector使用begin_、end_和capacity_的空指针初始化，
     * 表示初始时没有分配内存。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This constructor is noexcept, meaning it cannot throw exceptions.
     * 
     * @note 此构造函数是noexcept的，意味着它不能抛出异常。
     * 
     * @note This constructor is useful when you need to use a specific allocator
     * instead of the default one.
     * 
     * @note 当需要使用特定分配器而不是默认分配器时，此构造函数很有用。
     */
    explicit vector(const Allocator& alloc) noexcept 
        : begin_(nullptr), end_(nullptr), capacity_(nullptr), alloc_(alloc) {}
    
    /**
     * @brief Constructs the vector with specified size and value
     * @brief 使用指定大小和值构造vector
     * 
     * @param count Number of elements to initialize with
     * @param count 要初始化的元素数量
     * @param value Value to initialize each element with
     * @param value 用于初始化每个元素的值
     * @param alloc Allocator to use for memory management
     * @param alloc 用于内存管理的分配器
     * 
     * @details Constructs a vector with `count` copies of `value`.
     * If `count` is 0, no memory is allocated.
     * 
     * @details 构造一个包含`count`个`value`副本的vector。
     * 如果`count`为0，则不分配内存。
     * 
     * @note Time complexity: O(count)
     * @note 时间复杂度：O(count)
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     */
    explicit vector(size_type count, const T& value = T(), const Allocator& alloc = Allocator())
        : alloc_(alloc) {
        begin_ = alloc_traits::allocate(alloc_, count);
        end_ = begin_ + count;
        capacity_ = end_;
        
        try {
            mystl::uninitialized_fill_n(begin_, count, value);
        } catch (...) {
            alloc_traits::deallocate(alloc_, begin_, count);
            throw;
        }
    }
    
    /**
     * @brief Constructs the vector from a range of elements
     * @brief 从元素范围构造vector
     * 
     * @tparam InputIt Input iterator type
     * @tparam InputIt 输入迭代器类型
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to one past the last element in the range
     * @param last 指向范围中最后一个元素之后位置的迭代器
     * @param alloc Allocator to use for memory management
     * @param alloc 用于内存管理的分配器
     * 
     * @details Constructs a vector with copies of the elements in the range [first, last).
     * The range must be valid and InputIt must satisfy the InputIterator concept.
     * 
     * @details 构造一个包含范围[first, last)中元素副本的vector。
     * 范围必须有效，且InputIt必须满足InputIterator概念。
     * 
     * @note Time complexity: O(n) where n is the distance between first and last
     * @note 时间复杂度：O(n)，其中n是first和last之间的距离
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     */
    template<typename InputIt, typename = typename std::iterator_traits<InputIt>::iterator_category>
    vector(InputIt first, InputIt last, const Allocator& alloc = Allocator())
        : alloc_(alloc) {
        size_type count = static_cast<size_type>(mystl::distance(first, last));
        begin_ = alloc_traits::allocate(alloc_, count);
        end_ = begin_ + count;
        capacity_ = end_;
        
        try {
            mystl::uninitialized_copy(first, last, begin_);
        } catch (...) {
            alloc_traits::deallocate(alloc_, begin_, count);
            throw;
        }
    }
    
    /**
     * @brief Copy constructor
     * @brief 拷贝构造函数
     * 
     * @param other Another vector to copy from
     * @param other 要从中拷贝的另一个vector
     * 
     * @details Constructs a vector with a copy of each element in `other`.
     * The allocator is obtained by calling `select_on_container_copy_construction`
     * on `other`'s allocator, which may propagate the allocator.
     * 
     * @details 构造一个包含`other`中每个元素副本的vector。
     * 分配器通过调用`other`分配器的`select_on_container_copy_construction`获得，
     * 这可能会传播分配器。
     * 
     * @note Time complexity: O(n) where n is the size of other
     * @note 时间复杂度：O(n)，其中n是other的大小
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see allocator_traits::select_on_container_copy_construction
     * @see 分配器特性::select_on_container_copy_construction
     */
    vector(const vector& other)
        : alloc_(alloc_traits::select_on_container_copy_construction(other.alloc_)) {
        size_type count = other.size();
        if (count > 0) {
            begin_ = alloc_traits::allocate(alloc_, count);
            end_ = begin_ + count;
            capacity_ = end_;
            
            try {
                mystl::uninitialized_copy(other.begin_, other.end_, begin_);
            } catch (...) {
                alloc_traits::deallocate(alloc_, begin_, count);
                throw;
            }
        } else {
            begin_ = end_ = capacity_ = nullptr;
        }
    }
    
    /**
     * @brief Copy constructor with specified allocator
     * @brief 带指定分配器的拷贝构造函数
     * 
     * @param other Another vector to copy from
     * @param other 要从中拷贝的另一个vector
     * @param alloc Allocator to use for the new vector
     * @param alloc 用于新vector的分配器
     * 
     * @details Constructs a vector with a copy of each element in `other`,
     * using the specified allocator `alloc` instead of propagating from `other`.
     * 
     * @details 构造一个包含`other`中每个元素副本的vector，
     * 使用指定的分配器`alloc`而不是从`other`传播分配器。
     * 
     * @note Time complexity: O(n) where n is the size of other
     * @note 时间复杂度：O(n)，其中n是other的大小
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see vector(const vector&) for the version that propagates allocator
     * @see vector(const vector&) 用于传播分配器的版本
     */
    vector(const vector& other, const Allocator& alloc)
        : alloc_(alloc) {
        size_type count = other.size();
        if (count > 0) {
            begin_ = alloc_traits::allocate(alloc_, count);
            end_ = begin_ + count;
            capacity_ = end_;
            
            try {
                mystl::uninitialized_copy(other.begin_, other.end_, begin_);
            } catch (...) {
                alloc_traits::deallocate(alloc_, begin_, count);
                throw;
            }
        } else {
            begin_ = end_ = capacity_ = nullptr;
        }
    }
    
    /**
     * @brief Move constructor
     * @brief 移动构造函数
     * 
     * @param other Another vector to move from
     * @param other 要从中移动的另一个vector
     * 
     * @details Constructs a vector by moving resources from another vector.
     * The source vector `other` is left in a valid but unspecified state
     * (typically empty). This operation is noexcept and does not allocate
     * any memory or copy any elements.
     * 
     * @details 通过从另一个vector移动资源来构造vector。
     * 源vector `other` 被留在有效但未指定的状态（通常为空）。
     * 此操作是noexcept的，不分配任何内存或复制任何元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The allocator is moved using move semantics if the allocator
     * supports move construction. Otherwise, it is copied.
     * 
     * @note 如果分配器支持移动构造，则使用移动语义移动分配器。
     * 否则，分配器被复制。
     */
    vector(vector&& other) noexcept
        : begin_(other.begin_), end_(other.end_), capacity_(other.capacity_), 
          alloc_(mystl::move(other.alloc_)) {
        other.begin_ = other.end_ = other.capacity_ = nullptr;
    }
    
    /**
     * @brief Move constructor with specified allocator
     * @brief 带指定分配器的移动构造函数
     * 
     * @param other Another vector to move from
     * @param other 要从中移动的另一个vector
     * @param alloc Allocator to use for the new vector
     * @param alloc 用于新vector的分配器
     * 
     * @details Constructs a vector by moving resources from another vector,
     * using the specified allocator `alloc`. If the allocators compare equal,
     * the resources are moved efficiently. Otherwise, the elements are copied.
     * 
     * @details 通过从另一个vector移动资源来构造vector，
     * 使用指定的分配器`alloc`。如果分配器相等，则高效地移动资源。
     * 否则，元素被复制。
     * 
     * @note Time complexity: O(1) if allocators are equal, O(n) otherwise
     * @note 时间复杂度：如果分配器相等则为O(1)，否则为O(n)
     * 
     * @note This constructor is useful when you need to move a vector
     * but want to use a different allocator than the source vector.
     * 
     * @note 当需要移动vector但希望使用与源vector不同的分配器时，
     * 此构造函数很有用。
     * 
     * @exception std::bad_alloc If memory allocation fails (when allocators are not equal)
     * @exception std::bad_alloc 如果内存分配失败（当分配器不相等时）
     * @exception Any exception thrown by the copy constructor of T (when allocators are not equal)
     * @exception T的拷贝构造函数抛出的任何异常（当分配器不相等时）
     */
    vector(vector&& other, const Allocator& alloc)
        : alloc_(alloc) {
        if (alloc_ == other.alloc_) {
            // 如果分配器相等，可以移动资源
            begin_ = other.begin_;
            end_ = other.end_;
            capacity_ = other.capacity_;
            
            other.begin_ = other.end_ = other.capacity_ = nullptr;
        } else {
            // 分配器不相等，需要拷贝
            size_type count = other.size();
            if (count > 0) {
                begin_ = alloc_traits::allocate(alloc_, count);
                end_ = begin_ + count;
                capacity_ = end_;
                
                try {
                    mystl::uninitialized_copy(other.begin_, other.end_, begin_);
                } catch (...) {
                    alloc_traits::deallocate(alloc_, begin_, count);
                    throw;
                }
            } else {
                begin_ = end_ = capacity_ = nullptr;
            }
        }
    }
    
    /**
     * @brief Constructs the vector from an initializer list
     * @brief 从初始化列表构造vector
     * 
     * @param init Initializer list containing elements to initialize the vector with
     * @param init 包含用于初始化vector的元素的初始化列表
     * @param alloc Allocator to use for memory management
     * @param alloc 用于内存管理的分配器
     * 
     * @details Constructs a vector with copies of the elements in the initializer list `init`.
     * The elements are inserted in the same order as they appear in the initializer list.
     * 
     * @details 构造一个包含初始化列表`init`中元素副本的vector。
     * 元素按照它们在初始化列表中出现的顺序插入。
     * 
     * @note Time complexity: O(n) where n is the size of the initializer list
     * @note 时间复杂度：O(n)，其中n是初始化列表的大小
     * 
     * @note This constructor allows convenient initialization of vectors with literal values:
     * `vector<int> v = {1, 2, 3, 4, 5};`
     * 
     * @note 此构造函数允许使用字面值方便地初始化vector：
     * `vector<int> v = {1, 2, 3, 4, 5};`
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     */
    vector(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : vector(init.begin(), init.end(), alloc) {}
    
    /**
     * @brief Destructor
     * @brief 析构函数
     * 
     * @details Destroys the vector, deallocating all storage capacity.
     * All elements are destroyed and their memory is deallocated.
     * 
     * @details 销毁vector，释放所有存储容量。
     * 所有元素都被销毁，其内存被释放。
     * 
     * @note Time complexity: O(n) where n is the size of the vector
     * @note 时间复杂度：O(n)，其中n是vector的大小
     * 
     * @note The destructor is noexcept, ensuring that vector destruction
     * cannot throw exceptions.
     * 
     * @note 析构函数是noexcept的，确保vector销毁不会抛出异常。
     */
    ~vector() {
        if (begin_) {
            clear();
            alloc_traits::deallocate(alloc_, begin_, capacity());
        }
    }
    
    // ==================== 赋值操作符 ====================
    
    /**
     * @brief Copy assignment operator
     * @brief 拷贝赋值操作符
     * 
     * @param other Another vector to copy from
     * @param other 要从中拷贝的另一个vector
     * @return Reference to this vector
     * @return 此vector的引用
     * 
     * @details Replaces the contents of this vector with a copy of the contents of `other`.
     * Uses the copy-and-swap idiom to provide strong exception safety guarantee.
     * The allocator is replaced only if `alloc_traits::propagate_on_container_copy_assignment`
     * is true_type.
     * 
     * @details 用`other`内容的副本替换此vector的内容。
     * 使用拷贝并交换技术提供强异常安全保证。
     * 仅当`alloc_traits::propagate_on_container_copy_assignment`为true_type时才替换分配器。
     * 
     * @note Time complexity: O(n) where n is the size of other
     * @note 时间复杂度：O(n)，其中n是other的大小
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see vector(const vector&) for copy constructor
     * @see vector(const vector&) 用于拷贝构造函数
     */
    vector& operator=(const vector& other) {
        if (this != &other) {
            vector temp(other);
            swap(temp);
        }
        return *this;
    }
    
    /**
     * @brief Move assignment operator
     * @brief 移动赋值操作符
     * 
     * @param other Another vector to move from
     * @param other 要从中移动的另一个vector
     * @return Reference to this vector
     * @return 此vector的引用
     * 
     * @details Replaces the contents of this vector with those of `other` using move semantics.
     * After the move, `other` is in a valid but unspecified state (typically empty).
     * This operation is noexcept and does not allocate any memory or copy any elements.
     * 
     * @details 使用移动语义用`other`的内容替换此vector的内容。
     * 移动后，`other`处于有效但未指定的状态（通常为空）。
     * 此操作是noexcept的，不分配任何内存或复制任何元素。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The allocator is moved only if `alloc_traits::propagate_on_container_move_assignment`
     * is true_type. Otherwise, the original allocator is kept.
     * 
     * @note 仅当`alloc_traits::propagate_on_container_move_assignment`为true_type时才移动分配器。
     * 否则，保留原始分配器。
     */
    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            clear();
            alloc_traits::deallocate(alloc_, begin_, capacity());
            
            begin_ = other.begin_;
            end_ = other.end_;
            capacity_ = other.capacity_;
            alloc_ = mystl::move(other.alloc_);
            
            other.begin_ = other.end_ = other.capacity_ = nullptr;
        }
        return *this;
    }
    
    /**
     * @brief Initializer list assignment operator
     * @brief 初始化列表赋值操作符
     * 
     * @param ilist Initializer list to assign
     * @param ilist 要赋值的初始化列表
     * @return Reference to this vector
     * @return 此vector的引用
     * 
     * @details Replaces the contents of the vector with the elements from the initializer list `ilist`.
     * Uses the copy-and-swap idiom to provide strong exception safety guarantee.
     * 
     * @details 用初始化列表`ilist`中的元素替换vector的内容。
     * 使用拷贝并交换技术提供强异常安全保证。
     * 
     * @note Time complexity: O(n) where n is the size of the initializer list
     * @note 时间复杂度：O(n)，其中n是初始化列表的大小
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see vector(std::initializer_list<T>) for initializer list constructor
     * @see vector(std::initializer_list<T>) 用于初始化列表构造函数
     */
    vector& operator=(std::initializer_list<T> ilist) {
        vector temp(ilist);
        swap(temp);
        return *this;
    }
    
    // ==================== 容量相关函数 ====================
    
    /**
     * @brief Checks whether the vector is empty
     * @brief 检查vector是否为空
     * 
     * @return true if the vector is empty, false otherwise
     * @return 如果vector为空则返回true，否则返回false
     * 
     * @details Checks if the vector has no elements, i.e., whether size() == 0.
     * This function does not modify the vector in any way.
     * 
     * @details 检查vector是否没有元素，即size() == 0。
     * 此函数不以任何方式修改vector。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not allocate or deallocate memory.
     * 
     * @note 此函数是noexcept的，不分配或释放内存。
     */
    bool empty() const noexcept {
        return begin_ == end_;
    }
    
    /**
     * @brief Returns the number of elements in the vector
     * @brief 返回vector中的元素数量
     * 
     * @return Number of elements in the vector
     * @return vector中的元素数量
     * 
     * @details Returns the number of elements currently stored in the vector.
     * This is the actual number of elements, not the allocated capacity.
     * 
     * @details 返回当前存储在vector中的元素数量。
     * 这是实际元素数量，不是分配的容量。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The size is calculated as the difference between the end pointer
     * and the begin pointer, which is efficient and constant-time.
     * 
     * @note 大小计算为end指针和begin指针之间的差值，这是高效且常数时间的。
     */
    size_type size() const noexcept {
        return static_cast<size_type>(end_ - begin_);
    }
    
    /**
     * @brief Returns the maximum possible number of elements
     * @brief 返回可能的最大元素数量
     * 
     * @return Maximum number of elements the vector can hold
     * @return vector可以容纳的最大元素数量
     * 
     * @details Returns the maximum number of elements the vector can hold
     * due to system or library implementation limitations.
     * This value typically reflects the theoretical limit on the size
     * of the largest possible container.
     * 
     * @details 返回由于系统或库实现限制，vector可以容纳的最大元素数量。
     * 此值通常反映可能的最大容器的理论限制。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This value is usually the maximum value of size_type
     * divided by the size of the element type, but may be further
     * limited by the allocator.
     * 
     * @note 此值通常是size_type的最大值除以元素类型的大小，
     * 但可能受到分配器的进一步限制。
     */
    size_type max_size() const noexcept {
        return alloc_traits::max_size(alloc_);
    }
    
    /**
     * @brief Returns the number of elements that can be held in currently allocated storage
     * @brief 返回当前分配存储中可以容纳的元素数量
     * 
     * @return Capacity of the currently allocated storage
     * @return 当前分配存储的容量
     * 
     * @details Returns the number of elements that the vector can hold without
     * needing to reallocate memory. This is always greater than or equal to size().
     * 
     * @details 返回vector在不需重新分配内存的情况下可以容纳的元素数量。
     * 此值始终大于或等于size()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The capacity is calculated as the difference between the capacity pointer
     * and the begin pointer. When size() == capacity(), the next insertion will
     * trigger a reallocation.
     * 
     * @note 容量计算为capacity指针和begin指针之间的差值。
     * 当size() == capacity()时，下一次插入将触发重新分配。
     */
    size_type capacity() const noexcept {
        return static_cast<size_type>(capacity_ - begin_);
    }
    
    /**
     * @brief Increase the capacity of the vector to a value that's greater or equal to new_capacity
     * @brief 增加vector的容量到大于或等于new_capacity的值
     * 
     * @param new_capacity New capacity of the vector
     * @param new_capacity vector的新容量
     * 
     * @details If new_capacity is greater than the current capacity(),
     * new storage is allocated, otherwise the function does nothing.
     * 
     * @details 如果new_capacity大于当前capacity()，
     * 则分配新的存储空间，否则函数不执行任何操作。
     * 
     * @note Time complexity: O(n) where n is the size of the vector
     * @note 时间复杂度：O(n)，其中n是vector的大小
     * 
     * @note reserve() does not change the size of the vector.
     * If new_capacity is greater than the current capacity(),
     * a reallocation happens, and all iterators, pointers and references
     * to the elements are invalidated.
     * 
     * @note reserve()不改变vector的大小。
     * 如果new_capacity大于当前capacity()，
     * 会发生重新分配，所有指向元素的迭代器、指针和引用都会失效。
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     * 
     * @see capacity() for current capacity
     * @see capacity() 用于获取当前容量
     * @see shrink_to_fit() for reducing capacity to fit size
     * @see shrink_to_fit() 用于减少容量以适应大小
     */
    void reserve(size_type new_capacity) {
        if (new_capacity > capacity()) {
            reallocate(new_capacity);
        }
    }
    
    /**
     * @brief Requests the removal of unused capacity
     * @brief 请求移除未使用的容量
     * 
     * @details Requests the container to reduce its capacity to fit its size.
     * This is a non-binding request to reduce capacity() to size().
     * It depends on the implementation whether the request is fulfilled.
     * 
     * @details 请求容器减少其容量以适应其大小。
     * 这是一个非绑定请求，将capacity()减少到size()。
     * 请求是否被满足取决于实现。
     * 
     * @note Time complexity: O(n) where n is the size of the vector
     * @note 时间复杂度：O(n)，其中n是vector的大小
     * 
     * @note This function may cause a reallocation, which invalidates
     * all iterators, pointers and references to elements in the vector.
     * 
     * @note 此函数可能导致重新分配，这将使所有指向vector中元素的
     * 迭代器、指针和引用失效。
     * 
     * @note The request is non-binding to allow latitude for
     * implementation-specific optimizations.
     * 
     * @note 请求是非绑定的，以便为特定于实现的优化留出余地。
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     * 
     * @see capacity() for current capacity
     * @see capacity() 用于获取当前容量
     * @see reserve() for increasing capacity
     * @see reserve() 用于增加容量
     */
    void shrink_to_fit() {
        if (size() < capacity()) {
            reallocate(size());
        }
    }
    
    /**
     * @brief Resizes the container to contain count elements
     * @brief 调整容器大小以包含count个元素
     * 
     * @param new_size New size of the container
     * @param new_size 容器的新大小
     * 
     * @details If the current size is greater than new_size, the container is reduced
     * to its first new_size elements. If the current size is less than new_size,
     * additional default-inserted elements are appended.
     * 
     * @details 如果当前大小大于new_size，容器被缩减为其前new_size个元素。
     * 如果当前大小小于new_size，则追加额外的默认插入元素。
     * 
     * @note Time complexity: O(|new_size - size()|)
     * @note 时间复杂度：O(|new_size - size()|)
     * 
     * @note If new_size is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, only the iterators, pointers and references to the removed elements are invalidated.
     * 
     * @note 如果new_size大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，只有指向被移除元素的迭代器、指针和引用会失效。
     * 
     * @exception std::bad_alloc If memory allocation fails (when increasing size)
     * @exception std::bad_alloc 如果内存分配失败（当增加大小时）
     * @exception Any exception thrown by the default constructor of T (when increasing size)
     * @exception T的默认构造函数抛出的任何异常（当增加大小时）
     * 
     * @see resize(size_type, const T&) for version with specified value
     * @see resize(size_type, const T&) 用于带指定值的版本
     */
    void resize(size_type new_size) {
        resize(new_size, T());
    }
    
    /**
     * @brief Resizes the container to contain count elements with specified value
     * @brief 调整容器大小以包含count个具有指定值的元素
     * 
     * @param new_size New size of the container
     * @param new_size 容器的新大小
     * @param value The value to initialize the new elements with
     * @param value 用于初始化新元素的值
     * 
     * @details If the current size is greater than new_size, the container is reduced
     * to its first new_size elements. If the current size is less than new_size,
     * additional copies of value are appended.
     * 
     * @details 如果当前大小大于new_size，容器被缩减为其前new_size个元素。
     * 如果当前大小小于new_size，则追加value的额外副本。
     * 
     * @note Time complexity: O(|new_size - size()|)
     * @note 时间复杂度：O(|new_size - size()|)
     * 
     * @note If new_size is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, only the iterators, pointers and references to the removed elements are invalidated.
     * 
     * @note 如果new_size大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，只有指向被移除元素的迭代器、指针和引用会失效。
     * 
     * @exception std::bad_alloc If memory allocation fails (when increasing size)
     * @exception std::bad_alloc 如果内存分配失败（当增加大小时）
     * @exception Any exception thrown by the copy constructor of T (when increasing size)
     * @exception T的拷贝构造函数抛出的任何异常（当增加大小时）
     * 
     * @see resize(size_type) for version with default value
     * @see resize(size_type) 用于带默认值的版本
     */
    void resize(size_type new_size, const T& value) {
        size_type old_size = size();
        if (new_size > old_size) {
            // 增大容器
            reserve(new_size);
            mystl::uninitialized_fill_n(end_, new_size - old_size, value);
            end_ = begin_ + new_size;
        } else if (new_size < old_size) {
            // 缩小容器
            mystl::destroy(begin_ + new_size, end_);
            end_ = begin_ + new_size;
        }
        // 如果new_size == old_size，什么都不做
    }
    
    // ==================== 元素访问 ====================
    
    /**
     * @brief Access specified element without bounds checking
     * @brief 访问指定元素（无边界检查）
     * 
     * @param pos Position of the element to access
     * @param pos 要访问的元素位置
     * @return Reference to the requested element
     * @return 对请求元素的引用
     * 
     * @details Returns a reference to the element at specified location `pos`.
     * No bounds checking is performed. If `pos` is not within the range of
     * the container, the behavior is undefined.
     * 
     * @details 返回指定位置`pos`处元素的引用。
     * 不执行边界检查。如果`pos`不在容器范围内，行为未定义。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operator provides fast, unchecked access to elements.
     * Use `at()` for bounds-checked access.
     * 
     * @note 此操作符提供快速、无检查的元素访问。
     * 使用`at()`进行边界检查的访问。
     * 
     * @warning Calling this function with an out-of-range index causes
     * undefined behavior. Use `at()` for bounds-checked access.
     * 
     * @warning 使用超出范围的索引调用此函数会导致未定义行为。
     * 使用`at()`进行边界检查的访问。
     * 
     * @see at() for bounds-checked access
     * @see at() 用于边界检查的访问
     */
    reference operator[](size_type pos) {
        return begin_[pos];
    }
    
    /**
     * @brief Access specified element without bounds checking (const version)
     * @brief 访问指定元素（无边界检查，const版本）
     * 
     * @param pos Position of the element to access
     * @param pos 要访问的元素位置
     * @return Const reference to the requested element
     * @return 对请求元素的常量引用
     * 
     * @details Returns a const reference to the element at specified location `pos`.
     * No bounds checking is performed. If `pos` is not within the range of
     * the container, the behavior is undefined.
     * 
     * @details 返回指定位置`pos`处元素的常量引用。
     * 不执行边界检查。如果`pos`不在容器范围内，行为未定义。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This operator provides fast, unchecked access to elements.
     * Use `at()` for bounds-checked access.
     * 
     * @note 此操作符提供快速、无检查的元素访问。
     * 使用`at()`进行边界检查的访问。
     * 
     * @warning Calling this function with an out-of-range index causes
     * undefined behavior. Use `at()` for bounds-checked access.
     * 
     * @warning 使用超出范围的索引调用此函数会导致未定义行为。
     * 使用`at()`进行边界检查的访问。
     * 
     * @see at() for bounds-checked access
     * @see at() 用于边界检查的访问
     */
    const_reference operator[](size_type pos) const {
        return begin_[pos];
    }
    
    /**
     * @brief Access specified element with bounds checking
     * @brief 访问指定元素（带边界检查）
     * 
     * @param pos Position of the element to access
     * @param pos 要访问的元素位置
     * @return Reference to the requested element
     * @return 对请求元素的引用
     * 
     * @details Returns a reference to the element at specified location `pos`,
     * with bounds checking. If `pos` is not within the range of the container,
     * an exception of type `std::out_of_range` is thrown.
     * 
     * @details 返回指定位置`pos`处元素的引用，带边界检查。
     * 如果`pos`不在容器范围内，抛出`std::out_of_range`类型的异常。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function provides safe, bounds-checked access to elements.
     * Use `operator[]` for faster, unchecked access when you are certain
     * the index is valid.
     * 
     * @note 此函数提供安全、带边界检查的元素访问。
     * 当确定索引有效时，使用`operator[]`进行更快、无检查的访问。
     * 
     * @exception std::out_of_range If pos is out of range (pos >= size())
     * @exception std::out_of_range 如果pos超出范围（pos >= size()）
     * 
     * @see operator[] for unchecked access
     * @see operator[] 用于无检查的访问
     */
    reference at(size_type pos) {
        if (pos >= size()) {
            throw mystl::out_of_range("vector::at");
        }
        return begin_[pos];
    }
    
    /**
     * @brief Access specified element with bounds checking (const version)
     * @brief 访问指定元素（带边界检查，const版本）
     * 
     * @param pos Position of the element to access
     * @param pos 要访问的元素位置
     * @return Const reference to the requested element
     * @return 对请求元素的常量引用
     * 
     * @details Returns a const reference to the element at specified location `pos`,
     * with bounds checking. If `pos` is not within the range of the container,
     * an exception of type `std::out_of_range` is thrown.
     * 
     * @details 返回指定位置`pos`处元素的常量引用，带边界检查。
     * 如果`pos`不在容器范围内，抛出`std::out_of_range`类型的异常。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function provides safe, bounds-checked access to elements.
     * Use `operator[]` for faster, unchecked access when you are certain
     * the index is valid.
     * 
     * @note 此函数提供安全、带边界检查的元素访问。
     * 当确定索引有效时，使用`operator[]`进行更快、无检查的访问。
     * 
     * @exception std::out_of_range If pos is out of range (pos >= size())
     * @exception std::out_of_range 如果pos超出范围（pos >= size()）
     * 
     * @see operator[] for unchecked access
     * @see operator[] 用于无检查的访问
     */
    const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw mystl::out_of_range("vector::at");
        }
        return begin_[pos];
    }
    
    /**
     * @brief Access the first element
     * @brief 访问第一个元素
     * 
     * @return Reference to the first element in the vector
     * @return vector中第一个元素的引用
     * 
     * @details Returns a reference to the first element in the vector.
     * Calling front() on an empty container is undefined behavior.
     * 
     * @details 返回vector中第一个元素的引用。
     * 在空容器上调用front()是未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note For a non-empty vector, front() is equivalent to *begin().
     * 
     * @note 对于非空vector，front()等价于*begin()。
     * 
     * @warning Calling front() on an empty vector causes undefined behavior.
     * 
     * @warning 在空vector上调用front()会导致未定义行为。
     * 
     * @see back() for accessing the last element
     * @see back() 用于访问最后一个元素
     * @see begin() for getting an iterator to the first element
     * @see begin() 用于获取指向第一个元素的迭代器
     */
    reference front() {
        return *begin_;
    }
    
    /**
     * @brief Access the first element (const version)
     * @brief 访问第一个元素（const版本）
     * 
     * @return Const reference to the first element in the vector
     * @return vector中第一个元素的常量引用
     * 
     * @details Returns a const reference to the first element in the vector.
     * Calling front() on an empty container is undefined behavior.
     * 
     * @details 返回vector中第一个元素的常量引用。
     * 在空容器上调用front()是未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note For a non-empty vector, front() is equivalent to *cbegin().
     * 
     * @note 对于非空vector，front()等价于*cbegin()。
     * 
     * @warning Calling front() on an empty vector causes undefined behavior.
     * 
     * @warning 在空vector上调用front()会导致未定义行为。
     * 
     * @see back() for accessing the last element
     * @see back() 用于访问最后一个元素
     * @see cbegin() for getting a const iterator to the first element
     * @see cbegin() 用于获取指向第一个元素的常量迭代器
     */
    const_reference front() const {
        return *begin_;
    }
    
    /**
     * @brief Access the last element
     * @brief 访问最后一个元素
     * 
     * @return Reference to the last element in the vector
     * @return vector中最后一个元素的引用
     * 
     * @details Returns a reference to the last element in the vector.
     * Calling back() on an empty container is undefined behavior.
     * 
     * @details 返回vector中最后一个元素的引用。
     * 在空容器上调用back()是未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note For a non-empty vector, back() is equivalent to *(end() - 1).
     * 
     * @note 对于非空vector，back()等价于*(end() - 1)。
     * 
     * @warning Calling back() on an empty vector causes undefined behavior.
     * 
     * @warning 在空vector上调用back()会导致未定义行为。
     * 
     * @see front() for accessing the first element
     * @see front() 用于访问第一个元素
     * @see end() for getting an iterator to one past the last element
     * @see end() 用于获取指向最后一个元素之后位置的迭代器
     */
    reference back() {
        return *(end_ - 1);
    }
    
    /**
     * @brief Access the last element (const version)
     * @brief 访问最后一个元素（const版本）
     * 
     * @return Const reference to the last element in the vector
     * @return vector中最后一个元素的常量引用
     * 
     * @details Returns a const reference to the last element in the vector.
     * Calling back() on an empty container is undefined behavior.
     * 
     * @details 返回vector中最后一个元素的常量引用。
     * 在空容器上调用back()是未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note For a non-empty vector, back() is equivalent to *(cend() - 1).
     * 
     * @note 对于非空vector，back()等价于*(cend() - 1)。
     * 
     * @warning Calling back() on an empty vector causes undefined behavior.
     * 
     * @warning 在空vector上调用back()会导致未定义行为。
     * 
     * @see front() for accessing the first element
     * @see front() 用于访问第一个元素
     * @see cend() for getting a const iterator to one past the last element
     * @see cend() 用于获取指向最后一个元素之后位置的常量迭代器
     */
    const_reference back() const {
        return *(end_ - 1);
    }
    
    /**
     * @brief Direct access to the underlying array
     * @brief 直接访问底层数组
     * 
     * @return Pointer to the underlying element storage
     * @return 指向底层元素存储的指针
     * 
     * @details Returns a pointer to the underlying array serving as element storage.
     * The pointer is such that range [data(); data() + size()) is always a valid range,
     * even if the container is empty (data() is not dereferenceable in that case).
     * 
     * @details 返回指向作为元素存储的底层数组的指针。
     * 指针使得范围[data(); data() + size())始终是有效范围，
     * 即使容器为空（在这种情况下data()不可解引用）。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The pointer may be invalidated by any operation that causes
     * reallocation (e.g., reserve(), resize(), push_back(), etc.).
     * 
     * @note 指针可能被任何导致重新分配的操作（如reserve()、resize()、push_back()等）失效。
     * 
     * @note This function is useful for interoperability with C-style APIs
     * that expect raw pointers to arrays.
     * 
     * @note 此函数对于与期望数组原始指针的C风格API互操作很有用。
     * 
     * @see begin() for getting an iterator to the first element
     * @see begin() 用于获取指向第一个元素的迭代器
     * @see operator[] for accessing elements by index
     * @see operator[] 用于通过索引访问元素
     */
    pointer data() noexcept {
        return begin_;
    }
    
    /**
     * @brief Direct access to the underlying array (const version)
     * @brief 直接访问底层数组（const版本）
     * 
     * @return Const pointer to the underlying element storage
     * @return 指向底层元素存储的常量指针
     * 
     * @details Returns a const pointer to the underlying array serving as element storage.
     * The pointer is such that range [data(); data() + size()) is always a valid range,
     * even if the container is empty (data() is not dereferenceable in that case).
     * 
     * @details 返回指向作为元素存储的底层数组的常量指针。
     * 指针使得范围[data(); data() + size())始终是有效范围，
     * 即使容器为空（在这种情况下data()不可解引用）。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The pointer may be invalidated by any operation that causes
     * reallocation (e.g., reserve(), resize(), push_back(), etc.).
     * 
     * @note 指针可能被任何导致重新分配的操作（如reserve()、resize()、push_back()等）失效。
     * 
     * @note This function is useful for interoperability with C-style APIs
     * that expect raw pointers to arrays.
     * 
     * @note 此函数对于与期望数组原始指针的C风格API互操作很有用。
     * 
     * @see cbegin() for getting a const iterator to the first element
     * @see cbegin() 用于获取指向第一个元素的常量迭代器
     * @see operator[] for accessing elements by index
     * @see operator[] 用于通过索引访问元素
     */
    const_pointer data() const noexcept {
        return begin_;
    }
    
    // ==================== 迭代器 ====================
    
    /**
     * @brief Returns an iterator to the first element
     * @brief 返回指向第一个元素的迭代器
     * 
     * @return Iterator to the first element
     * @return 指向第一个元素的迭代器
     * 
     * @details Returns an iterator pointing to the first element in the vector.
     * If the vector is empty, the returned iterator will be equal to end().
     * 
     * @details 返回指向vector中第一个元素的迭代器。
     * 如果vector为空，返回的迭代器将等于end()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The iterator is a random access iterator, allowing O(1) access
     * to any element in the vector.
     * 
     * @note 迭代器是随机访问迭代器，允许O(1)访问vector中的任何元素。
     * 
     * @see end() for getting an iterator to one past the last element
     * @see end() 用于获取指向最后一个元素之后位置的迭代器
     * @see cbegin() for getting a const iterator to the first element
     * @see cbegin() 用于获取指向第一个元素的常量迭代器
     */
    iterator begin() noexcept {
        return begin_;
    }
    
    /**
     * @brief Returns a const iterator to the first element
     * @brief 返回指向第一个元素的常量迭代器
     * 
     * @return Const iterator to the first element
     * @return 指向第一个元素的常量迭代器
     * 
     * @details Returns a const iterator pointing to the first element in the vector.
     * If the vector is empty, the returned iterator will be equal to end().
     * 
     * @details 返回指向vector中第一个元素的常量迭代器。
     * 如果vector为空，返回的迭代器将等于end()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The iterator is a random access iterator, allowing O(1) access
     * to any element in the vector, but does not allow modification.
     * 
     * @note 迭代器是随机访问迭代器，允许O(1)访问vector中的任何元素，但不允许修改。
     * 
     * @see end() for getting a const iterator to one past the last element
     * @see end() 用于获取指向最后一个元素之后位置的常量迭代器
     * @see cbegin() for getting a const iterator to the first element
     * @see cbegin() 用于获取指向第一个元素的常量迭代器
     */
    const_iterator begin() const noexcept {
        return begin_;
    }
    
    /**
     * @brief Returns a const iterator to the first element
     * @brief 返回指向第一个元素的常量迭代器
     * 
     * @return Const iterator to the first element
     * @return 指向第一个元素的常量迭代器
     * 
     * @details Returns a const iterator pointing to the first element in the vector.
     * If the vector is empty, the returned iterator will be equal to cend().
     * 
     * @details 返回指向vector中第一个元素的常量迭代器。
     * 如果vector为空，返回的迭代器将等于cend()。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is provided for consistency with C++11's cbegin().
     * It returns the same as begin() const.
     * 
     * @note 此函数为与C++11的cbegin()保持一致而提供。
     * 它返回与begin() const相同的内容。
     * 
     * @see cend() for getting a const iterator to one past the last element
     * @see cend() 用于获取指向最后一个元素之后位置的常量迭代器
     * @see begin() for getting a non-const iterator to the first element
     * @see begin() 用于获取指向第一个元素的非常量迭代器
     */
    const_iterator cbegin() const noexcept {
        return begin_;
    }
    
    /**
     * @brief Returns an iterator to one past the last element
     * @brief 返回指向最后一个元素之后位置的迭代器
     * 
     * @return Iterator to one past the last element
     * @return 指向最后一个元素之后位置的迭代器
     * 
     * @details Returns an iterator pointing to one past the last element in the vector.
     * This iterator acts as a placeholder; attempting to access it results in undefined behavior.
     * 
     * @details 返回指向vector中最后一个元素之后位置的迭代器。
     * 此迭代器充当占位符；尝试访问它会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The end() iterator is used as a sentinel value to indicate the end of a range.
     * It should not be dereferenced.
     * 
     * @note end()迭代器用作哨兵值，表示范围的结束。
     * 不应解引用它。
     * 
     * @see begin() for getting an iterator to the first element
     * @see begin() 用于获取指向第一个元素的迭代器
     * @see cend() for getting a const iterator to one past the last element
     * @see cend() 用于获取指向最后一个元素之后位置的常量迭代器
     */
    iterator end() noexcept {
        return end_;
    }
    
    /**
     * @brief Returns a const iterator to one past the last element
     * @brief 返回指向最后一个元素之后位置的常量迭代器
     * 
     * @return Const iterator to one past the last element
     * @return 指向最后一个元素之后位置的常量迭代器
     * 
     * @details Returns a const iterator pointing to one past the last element in the vector.
     * This iterator acts as a placeholder; attempting to access it results in undefined behavior.
     * 
     * @details 返回指向vector中最后一个元素之后位置的常量迭代器。
     * 此迭代器充当占位符；尝试访问它会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note The end() iterator is used as a sentinel value to indicate the end of a range.
     * It should not be dereferenced.
     * 
     * @note end()迭代器用作哨兵值，表示范围的结束。
     * 不应解引用它。
     * 
     * @see begin() for getting a const iterator to the first element
     * @see begin() 用于获取指向第一个元素的常量迭代器
     * @see cend() for getting a const iterator to one past the last element
     * @see cend() 用于获取指向最后一个元素之后位置的常量迭代器
     */
    const_iterator end() const noexcept {
        return end_;
    }
    
    /**
     * @brief Returns a const iterator to one past the last element
     * @brief 返回指向最后一个元素之后位置的常量迭代器
     * 
     * @return Const iterator to one past the last element
     * @return 指向最后一个元素之后位置的常量迭代器
     * 
     * @details Returns a const iterator pointing to one past the last element in the vector.
     * This iterator acts as a placeholder; attempting to access it results in undefined behavior.
     * 
     * @details 返回指向vector中最后一个元素之后位置的常量迭代器。
     * 此迭代器充当占位符；尝试访问它会导致未定义行为。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is provided for consistency with C++11's cend().
     * It returns the same as end() const.
     * 
     * @note 此函数为与C++11的cend()保持一致而提供。
     * 它返回与end() const相同的内容。
     * 
     * @see cbegin() for getting a const iterator to the first element
     * @see cbegin() 用于获取指向第一个元素的常量迭代器
     * @see end() for getting a non-const iterator to one past the last element
     * @see end() 用于获取指向最后一个元素之后位置的非常量迭代器
     */
    const_iterator cend() const noexcept {
        return end_;
    }
    
    /**
     * @brief Returns a reverse iterator to the first element of the reversed vector
     * @brief 返回指向反转vector的第一个元素的反向迭代器
     * 
     * @return Reverse iterator to the first element of the reversed vector
     * @return 指向反转vector的第一个元素的反向迭代器
     * 
     * @details Returns a reverse iterator pointing to the last element of the non-reversed vector.
     * That is, rbegin() returns an iterator pointing to the element preceding the one
     * that end() would point to.
     * 
     * @details 返回指向非反转vector的最后一个元素的反向迭代器。
     * 也就是说，rbegin()返回指向end()将指向的元素的前一个元素的迭代器。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note Reverse iterators iterate from the end to the beginning of the vector.
     * Incrementing a reverse iterator moves it toward the beginning of the vector.
     * 
     * @note 反向迭代器从vector的末尾向开头迭代。
     * 递增反向迭代器会将其移向vector的开头。
     * 
     * @see rend() for getting a reverse iterator to one past the last element of the reversed vector
     * @see rend() 用于获取指向反转vector的最后一个元素之后位置的反向迭代器
     * @see crbegin() for getting a const reverse iterator to the first element of the reversed vector
     * @see crbegin() 用于获取指向反转vector的第一个元素的常量反向迭代器
     */
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end_);
    }
    
    /**
     * @brief Returns a const reverse iterator to the first element of the reversed vector
     * @brief 返回指向反转vector的第一个元素的常量反向迭代器
     * 
     * @return Const reverse iterator to the first element of the reversed vector
     * @return 指向反转vector的第一个元素的常量反向迭代器
     * 
     * @details Returns a const reverse iterator pointing to the last element of the non-reversed vector.
     * That is, rbegin() returns an iterator pointing to the element preceding the one
     * that end() would point to.
     * 
     * @details 返回指向非反转vector的最后一个元素的常量反向迭代器。
     * 也就是说，rbegin()返回指向end()将指向的元素的前一个元素的迭代器。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note Reverse iterators iterate from the end to the beginning of the vector.
     * Incrementing a reverse iterator moves it toward the beginning of the vector.
     * 
     * @note 反向迭代器从vector的末尾向开头迭代。
     * 递增反向迭代器会将其移向vector的开头。
     * 
     * @see rend() for getting a const reverse iterator to one past the last element of the reversed vector
     * @see rend() 用于获取指向反转vector的最后一个元素之后位置的常量反向迭代器
     * @see crbegin() for getting a const reverse iterator to the first element of the reversed vector
     * @see crbegin() 用于获取指向反转vector的第一个元素的常量反向迭代器
     */
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end_);
    }
    
    /**
     * @brief Returns a const reverse iterator to the first element of the reversed vector
     * @brief 返回指向反转vector的第一个元素的常量反向迭代器
     * 
     * @return Const reverse iterator to the first element of the reversed vector
     * @return 指向反转vector的第一个元素的常量反向迭代器
     * 
     * @details Returns a const reverse iterator pointing to the last element of the non-reversed vector.
     * That is, crbegin() returns an iterator pointing to the element preceding the one
     * that cend() would point to.
     * 
     * @details 返回指向非反转vector的最后一个元素的常量反向迭代器。
     * 也就是说，crbegin()返回指向cend()将指向的元素的前一个元素的迭代器。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is provided for consistency with C++11's crbegin().
     * It returns the same as rbegin() const.
     * 
     * @note 此函数为与C++11的crbegin()保持一致而提供。
     * 它返回与rbegin() const相同的内容。
     * 
     * @see crend() for getting a const reverse iterator to one past the last element of the reversed vector
     * @see crend() 用于获取指向反转vector的最后一个元素之后位置的常量反向迭代器
     * @see rbegin() for getting a non-const reverse iterator to the first element of the reversed vector
     * @see rbegin() 用于获取指向反转vector的第一个元素的非常量反向迭代器
     */
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end_);
    }
    
    /**
     * @brief Returns a reverse iterator to one past the last element of the reversed vector
     * @brief 返回指向反转vector的最后一个元素之后位置的反向迭代器
     * 
     * @return Reverse iterator to one past the last element of the reversed vector
     * @return 指向反转vector的最后一个元素之后位置的反向迭代器
     * 
     * @details Returns a reverse iterator pointing to one past the last element of the reversed vector.
     * That is, rend() returns an iterator pointing to the element preceding the one
     * that begin() would point to.
     * 
     * @details 返回指向反转vector的最后一个元素之后位置的反向迭代器。
     * 也就是说，rend()返回指向begin()将指向的元素的前一个元素的迭代器。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note Reverse iterators iterate from the end to the beginning of the vector.
     * rend() is the sentinel value for reverse iteration.
     * 
     * @note 反向迭代器从vector的末尾向开头迭代。
     * rend()是反向迭代的哨兵值。
     * 
     * @see rbegin() for getting a reverse iterator to the first element of the reversed vector
     * @see rbegin() 用于获取指向反转vector的第一个元素的反向迭代器
     * @see crend() for getting a const reverse iterator to one past the last element of the reversed vector
     * @see crend() 用于获取指向反转vector的最后一个元素之后位置的常量反向迭代器
     */
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin_);
    }
    
    /**
     * @brief Returns a const reverse iterator to one past the last element of the reversed vector
     * @brief 返回指向反转vector的最后一个元素之后位置的常量反向迭代器
     * 
     * @return Const reverse iterator to one past the last element of the reversed vector
     * @return 指向反转vector的最后一个元素之后位置的常量反向迭代器
     * 
     * @details Returns a const reverse iterator pointing to one past the last element of the reversed vector.
     * That is, rend() returns an iterator pointing to the element preceding the one
     * that begin() would point to.
     * 
     * @details 返回指向反转vector的最后一个元素之后位置的常量反向迭代器。
     * 也就是说，rend()返回指向begin()将指向的元素的前一个元素的迭代器。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note Reverse iterators iterate from the end to the beginning of the vector.
     * rend() is the sentinel value for reverse iteration.
     * 
     * @note 反向迭代器从vector的末尾向开头迭代。
     * rend()是反向迭代的哨兵值。
     * 
     * @see rbegin() for getting a const reverse iterator to the first element of the reversed vector
     * @see rbegin() 用于获取指向反转vector的第一个元素的常量反向迭代器
     * @see crend() for getting a const reverse iterator to one past the last element of the reversed vector
     * @see crend() 用于获取指向反转vector的最后一个元素之后位置的常量反向迭代器
     */
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin_);
    }
    
    /**
     * @brief Returns a const reverse iterator to one past the last element of the reversed vector
     * @brief 返回指向反转vector的最后一个元素之后位置的常量反向迭代器
     * 
     * @return Const reverse iterator to one past the last element of the reversed vector
     * @return 指向反转vector的最后一个元素之后位置的常量反向迭代器
     * 
     * @details Returns a const reverse iterator pointing to one past the last element of the reversed vector.
     * That is, crend() returns an iterator pointing to the element preceding the one
     * that cbegin() would point to.
     * 
     * @details 返回指向反转vector的最后一个元素之后位置的常量反向迭代器。
     * 也就是说，crend()返回指向cbegin()将指向的元素的前一个元素的迭代器。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is provided for consistency with C++11's crend().
     * It returns the same as rend() const.
     * 
     * @note 此函数为与C++11的crend()保持一致而提供。
     * 它返回与rend() const相同的内容。
     * 
     * @see crbegin() for getting a const reverse iterator to the first element of the reversed vector
     * @see crbegin() 用于获取指向反转vector的第一个元素的常量反向迭代器
     * @see rend() for getting a non-const reverse iterator to one past the last element of the reversed vector
     * @see rend() 用于获取指向反转vector的最后一个元素之后位置的非常量反向迭代器
     */
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin_);
    }
    
    // ==================== 修改操作 ====================
    
    /**
     * @brief Removes all elements from the vector
     * @brief 从vector中移除所有元素
     * 
     * @details Erases all elements from the container. After this call, size() returns zero.
     * Leaves the capacity() of the vector unchanged (note: the standard's specification
     * on capacity after clear() is implementation-defined).
     * 
     * @details 从容器中擦除所有元素。调用此函数后，size()返回零。
     * 保持vector的capacity()不变（注意：标准对clear()后容量的规定是实现定义的）。
     * 
     * @note Time complexity: O(n) where n is the size of the vector
     * @note 时间复杂度：O(n)，其中n是vector的大小
     * 
     * @note This function does not deallocate memory. The capacity remains unchanged,
     * allowing efficient reuse of the allocated memory for future insertions.
     * 
     * @note 此函数不释放内存。容量保持不变，允许高效地重用已分配的内存用于未来的插入。
     * 
     * @note All iterators, pointers and references to elements are invalidated.
     * 
     * @note 所有指向元素的迭代器、指针和引用都会失效。
     * 
     * @note This function is noexcept, meaning it cannot throw exceptions.
     * 
     * @note 此函数是noexcept的，意味着它不能抛出异常。
     * 
     * @see erase() for removing specific elements
     * @see erase() 用于移除特定元素
     * @see shrink_to_fit() for reducing capacity after clearing
     * @see shrink_to_fit() 用于在清空后减少容量
     */
    void clear() noexcept {
        if (begin_) {
            mystl::destroy(begin_, end_);
            end_ = begin_;
        }
    }
    
    /**
     * @brief Adds an element to the end of the vector (copy version)
     * @brief 在vector末尾添加元素（拷贝版本）
     * 
     * @param value The value of the element to append
     * @param value 要追加的元素的值
     * 
     * @details Appends the given element `value` to the end of the vector.
     * The new element is initialized as a copy of `value`.
     * 
     * @details 将给定元素`value`追加到vector的末尾。
     * 新元素被初始化为`value`的副本。
     * 
     * @note Time complexity: Amortized O(1)
     * @note 时间复杂度：摊还O(1)
     * 
     * @note If the new size() is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, only the end() iterator is invalidated.
     * 
     * @note 如果新的size()大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，只有end()迭代器会失效。
     * 
     * @note This function provides the strong exception guarantee:
     * if an exception is thrown, the vector remains unchanged.
     * 
     * @note 此函数提供强异常保证：如果抛出异常，vector保持不变。
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see emplace_back() for constructing elements in-place
     * @see emplace_back() 用于原位构造元素
     * @see pop_back() for removing the last element
     * @see pop_back() 用于移除最后一个元素
     */
    void push_back(const T& value) {
        check_grow();
        alloc_traits::construct(alloc_, end_, value);
        ++end_;
    }
    
    /**
     * @brief Adds an element to the end of the vector (move version)
     * @brief 在vector末尾添加元素（移动版本）
     * 
     * @param value The value of the element to append
     * @param value 要追加的元素的值
     * 
     * @details Appends the given element `value` to the end of the vector.
     * The new element is initialized by moving `value` into it.
     * 
     * @details 将给定元素`value`追加到vector的末尾。
     * 新元素通过将`value`移动其中来初始化。
     * 
     * @note Time complexity: Amortized O(1)
     * @note 时间复杂度：摊还O(1)
     * 
     * @note If the new size() is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, only the end() iterator is invalidated.
     * 
     * @note 如果新的size()大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，只有end()迭代器会失效。
     * 
     * @note This function provides the strong exception guarantee:
     * if an exception is thrown, the vector remains unchanged.
     * 
     * @note 此函数提供强异常保证：如果抛出异常，vector保持不变。
     * 
     * @note After this call, `value` is in a valid but unspecified state.
     * 
     * @note 调用此函数后，`value`处于有效但未指定的状态。
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the move constructor of T
     * @exception T的移动构造函数抛出的任何异常
     * 
     * @see emplace_back() for constructing elements in-place
     * @see emplace_back() 用于原位构造元素
     * @see pop_back() for removing the last element
     * @see pop_back() 用于移除最后一个元素
     */
    void push_back(T&& value) {
        check_grow();
        alloc_traits::construct(alloc_, end_, mystl::move(value));
        ++end_;
    }
    
    /**
     * @brief Constructs an element in-place at the end of the vector
     * @brief 在vector末尾原位构造元素
     * 
     * @tparam Args Types of arguments to forward to the element's constructor
     * @tparam Args 要转发给元素构造函数的参数类型
     * @param args Arguments to forward to the element's constructor
     * @param args 要转发给元素构造函数的参数
     * @return Reference to the newly constructed element
     * @return 对新构造元素的引用
     * 
     * @details Constructs a new element at the end of the vector using the provided arguments.
     * The element is constructed in-place, which avoids unnecessary copies or moves.
     * This is more efficient than push_back() when constructing complex objects.
     * 
     * @details 使用提供的参数在vector末尾构造一个新元素。
     * 元素在原位构造，避免了不必要的拷贝或移动。
     * 当构造复杂对象时，这比push_back()更高效。
     * 
     * @note Time complexity: Amortized O(1)
     * @note 时间复杂度：摊还O(1)
     * 
     * @note If the new size() is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, only the end() iterator is invalidated.
     * 
     * @note 如果新的size()大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，只有end()迭代器会失效。
     * 
     * @note This function provides the strong exception guarantee:
     * if an exception is thrown, the vector remains unchanged.
     * 
     * @note 此函数提供强异常保证：如果抛出异常，vector保持不变。
     * 
     * @note This is a C++11 feature that allows perfect forwarding of arguments
     * to the element's constructor.
     * 
     * @note 这是C++11特性，允许将参数完美转发给元素的构造函数。
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the constructor of T
     * @exception T的构造函数抛出的任何异常
     * 
     * @see push_back() for adding existing elements
     * @see push_back() 用于添加现有元素
     * @see emplace() for constructing elements at arbitrary positions
     * @see emplace() 用于在任意位置构造元素
     */
    template<typename... Args>
    reference emplace_back(Args&&... args) {
        check_grow();
        alloc_traits::construct(alloc_, end_, mystl::forward<Args>(args)...);
        ++end_;
        return back();
    }
    
    /**
     * @brief Constructs an element in-place at the specified position
     * @brief 在指定位置原位构造元素
     * 
     * @tparam Args Types of arguments to forward to the element's constructor
     * @tparam Args 要转发给元素构造函数的参数类型
     * @param pos Iterator to the position before which the new element will be constructed
     * @param pos 指向新元素将在其前构造的位置的迭代器
     * @param args Arguments to forward to the element's constructor
     * @param args 要转发给元素构造函数的参数
     * @return Iterator pointing to the newly constructed element
     * @return 指向新构造元素的迭代器
     * 
     * @details Constructs a new element in-place at the specified position `pos`
     * using the provided arguments. The element is constructed directly in the vector's
     * storage, avoiding unnecessary copies or moves.
     * 
     * @details 使用提供的参数在指定位置`pos`处原位构造新元素。
     * 元素直接在vector的存储中构造，避免了不必要的拷贝或移动。
     * 
     * @note Time complexity: O(n) where n is the number of elements after the insertion point
     * @note 时间复杂度：O(n)，其中n是插入点之后的元素数量
     * 
     * @note If the new size() is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, all iterators, pointers and references to elements at or after the
     * insertion point are invalidated.
     * 
     * @note 如果新的size()大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，所有指向插入点或之后元素的迭代器、指针和引用都会失效。
     * 
     * @note This function provides the strong exception guarantee:
     * if an exception is thrown, the vector remains unchanged.
     * 
     * @note 此函数提供强异常保证：如果抛出异常，vector保持不变。
     * 
     * @note This is a C++11 feature that allows perfect forwarding of arguments
     * to the element's constructor, enabling efficient construction of complex objects.
     * 
     * @note 这是C++11特性，允许将参数完美转发给元素的构造函数，
     * 从而实现对复杂对象的高效构造。
     * 
     * @note The implementation uses a three-step process:
     * 1. Check if reallocation is needed and grow if necessary
     * 2. Move elements after the insertion point to make space
     * 3. Construct the new element in-place using perfect forwarding
     * 
     * @note 实现使用三步过程：
     * 1. 检查是否需要重新分配，必要时扩容
     * 2. 移动插入点之后的元素以腾出空间
     * 3. 使用完美转发原位构造新元素
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the constructor of T
     * @exception T的构造函数抛出的任何异常
     * 
     * @see emplace_back() for constructing elements at the end
     * @see emplace_back() 用于在末尾构造元素
     * @see insert() for inserting existing elements
     * @see insert() 用于插入现有元素
     */
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        size_type index = pos - begin_;
        check_grow();
        
        pointer p = begin_ + index;
        if (p != end_) {
            // 移动尾部元素（构造最后一个元素到新位置）
            alloc_traits::construct(alloc_, end_, mystl::move(*(end_ - 1)));
            // 向后移动中间元素
            for (pointer it = end_ - 1; it > p; --it) {
                *it = mystl::move(*(it - 1));
            }
            // 销毁原位置元素（已经被移动走了）
            alloc_traits::destroy(alloc_, p);
        }
        
        alloc_traits::construct(alloc_, p, mystl::forward<Args>(args)...);
        ++end_;
        return p;
    }
    
    /**
     * @brief Removes the last element from the vector
     * @brief 从vector中移除最后一个元素
     * 
     * @details Removes the last element from the vector, effectively reducing
     * the container size by one. The element is destroyed and its memory is deallocated.
     * 
     * @details 从vector中移除最后一个元素，有效地将容器大小减少一。
     * 元素被销毁，其内存被释放。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note Calling pop_back() on an empty container results in undefined behavior.
     * Always check if the vector is not empty before calling pop_back().
     * 
     * @note 在空容器上调用pop_back()会导致未定义行为。
     * 在调用pop_back()之前，始终检查vector是否非空。
     * 
     * @note This function invalidates only the end() iterator and the iterator,
     * pointer and reference to the removed element.
     * 
     * @note 此函数仅使end()迭代器以及指向被移除元素的迭代器、指针和引用失效。
     * 
     * @note This function is noexcept, meaning it cannot throw exceptions.
     * 
     * @note 此函数是noexcept的，意味着它不能抛出异常。
     * 
     * @warning Calling pop_back() on an empty vector causes undefined behavior.
     * 
     * @warning 在空vector上调用pop_back()会导致未定义行为。
     * 
     * @see push_back() for adding elements to the end
     * @see push_back() 用于在末尾添加元素
     * @see back() for accessing the last element
     * @see back() 用于访问最后一个元素
     * @see clear() for removing all elements
     * @see clear() 用于移除所有元素
     */
    void pop_back() {
        if (begin_ != end_) {
            --end_;
            alloc_traits::destroy(alloc_, end_);
        }
    }
    
    /**
     * @brief Inserts a single element at the specified position (copy version)
     * @brief 在指定位置插入单个元素（拷贝版本）
     * 
     * @param pos Iterator to the position before which the element will be inserted
     * @param pos 指向元素将在其前插入的位置的迭代器
     * @param value Element value to insert
     * @param value 要插入的元素值
     * @return Iterator pointing to the inserted element
     * @return 指向插入元素的迭代器
     * 
     * @details Inserts a copy of `value` at the specified position `pos`.
     * The element is inserted before the element at the specified position.
     * 
     * @details 在指定位置`pos`处插入`value`的副本。
     * 元素插入在指定位置的元素之前。
     * 
     * @note Time complexity: O(n) where n is the number of elements after the insertion point
     * @note 时间复杂度：O(n)，其中n是插入点之后的元素数量
     * 
     * @note If the new size() is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, all iterators, pointers and references to elements at or after the
     * insertion point are invalidated.
     * 
     * @note 如果新的size()大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，所有指向插入点或之后元素的迭代器、指针和引用都会失效。
     * 
     * @note This function provides the strong exception guarantee:
     * if an exception is thrown, the vector remains unchanged.
     * 
     * @note 此函数提供强异常保证：如果抛出异常，vector保持不变。
     * 
     * @note The implementation delegates to `insert_impl()` with count = 1.
     * 
     * @note 实现委托给`insert_impl()`，count = 1。
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see insert(const_iterator, size_type, const T&) for inserting multiple copies
     * @see insert(const_iterator, size_type, const T&) 用于插入多个副本
     * @see insert(const_iterator, T&&) for moving an element into the vector
     * @see insert(const_iterator, T&&) 用于将元素移动到vector中
     * @see emplace() for constructing elements in-place
     * @see emplace() 用于原位构造元素
     */
    iterator insert(const_iterator pos, const T& value) {
        return insert_impl(pos, 1, value);
    }
    
    /**
     * @brief Inserts multiple copies of value at the specified position
     * @brief 在指定位置插入value的多个副本
     * 
     * @param pos Iterator to the position before which the elements will be inserted
     * @param pos 指向元素将在其前插入的位置的迭代器
     * @param count Number of elements to insert
     * @param count 要插入的元素数量
     * @param value Value to initialize the inserted elements with
     * @param value 用于初始化插入元素的值
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details Inserts `count` copies of `value` at the specified position `pos`.
     * The elements are inserted before the element at the specified position.
     * 
     * @details 在指定位置`pos`处插入`count`个`value`的副本。
     * 元素插入在指定位置的元素之前。
     * 
     * @note Time complexity: O(n + count) where n is the number of elements after the insertion point
     * @note 时间复杂度：O(n + count)，其中n是插入点之后的元素数量
     * 
     * @note If the new size() is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, all iterators, pointers and references to elements at or after the
     * insertion point are invalidated.
     * 
     * @note 如果新的size()大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，所有指向插入点或之后元素的迭代器、指针和引用都会失效。
     * 
     * @note This function provides the strong exception guarantee.
     * 
     * @note 此函数提供强异常保证。
     * 
     * @note The implementation delegates to `insert_impl()` which handles both
     * in-place insertion and reallocation cases.
     * 
     * @note 实现委托给`insert_impl()`，它处理原地插入和重新分配两种情况。
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see insert(const_iterator, const T&) for inserting a single element
     * @see insert(const_iterator, const T&) 用于插入单个元素
     * @see insert(const_iterator, T&&) for moving an element into the vector
     * @see insert(const_iterator, T&&) 用于将元素移动到vector中
     * @see insert(const_iterator, std::initializer_list<T>) for inserting from an initializer list
     * @see insert(const_iterator, std::initializer_list<T>) 用于从初始化列表插入
     */
    iterator insert(const_iterator pos, size_type count, const T& value) {
        return insert_impl(pos, count, value);
    }
    
    /**
     * @brief Inserts a single element at the specified position (move version)
     * @brief 在指定位置插入单个元素（移动版本）
     * 
     * @param pos Iterator to the position before which the element will be inserted
     * @param pos 指向元素将在其前插入的位置的迭代器
     * @param value Element value to insert (will be moved from)
     * @param value 要插入的元素值（将从其移动）
     * @return Iterator pointing to the inserted element
     * @return 指向插入元素的迭代器
     * 
     * @details Inserts `value` at the specified position `pos` by moving it into the vector.
     * The element is inserted before the element at the specified position.
     * After the move, `value` is in a valid but unspecified state.
     * 
     * @details 通过将`value`移动到vector中，在指定位置`pos`处插入它。
     * 元素插入在指定位置的元素之前。
     * 移动后，`value`处于有效但未指定的状态。
     * 
     * @note Time complexity: O(n) where n is the number of elements after the insertion point
     * @note 时间复杂度：O(n)，其中n是插入点之后的元素数量
     * 
     * @note If the new size() is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, all iterators, pointers and references to elements at or after the
     * insertion point are invalidated.
     * 
     * @note 如果新的size()大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，所有指向插入点或之后元素的迭代器、指针和引用都会失效。
     * 
     * @note This function provides the strong exception guarantee:
     * if an exception is thrown, the vector remains unchanged.
     * 
     * @note 此函数提供强异常保证：如果抛出异常，vector保持不变。
     * 
     * @note The implementation handles both in-place insertion and reallocation cases.
     * When reallocation is needed, it uses the standard growth strategy (doubling capacity).
     * 
     * @note 实现处理原地插入和重新分配两种情况。
     * 当需要重新分配时，它使用标准增长策略（容量翻倍）。
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the move constructor of T
     * @exception T的移动构造函数抛出的任何异常
     * 
     * @see insert(const_iterator, const T&) for inserting a copy of an element
     * @see insert(const_iterator, const T&) 用于插入元素的副本
     * @see insert(const_iterator, size_type, const T&) for inserting multiple copies
     * @see insert(const_iterator, size_type, const T&) 用于插入多个副本
     * @see emplace() for constructing elements in-place with perfect forwarding
     * @see emplace() 用于使用完美转发原位构造元素
     */
    iterator insert(const_iterator pos, T&& value) {
        size_type index = pos - begin_;
        check_grow();
        
        pointer p = begin_ + index;
        if (p != end_) {
            // 移动尾部元素（构造最后一个元素到新位置）
            alloc_traits::construct(alloc_, end_, mystl::move(*(end_ - 1)));
            // 向后移动中间元素
            for (pointer it = end_ - 1; it > p; --it) {
                *it = mystl::move(*(it - 1));
            }
            // 销毁原位置元素（已经被移动走了）
            alloc_traits::destroy(alloc_, p);
        }
        
        alloc_traits::construct(alloc_, p, mystl::move(value));
        ++end_;
        return p;
    }
    
    /**
     * @brief Inserts elements from an initializer list at the specified position
     * @brief 在指定位置插入来自初始化列表的元素
     * 
     * @param pos Iterator to the position before which the elements will be inserted
     * @param pos 指向元素将在其前插入的位置的迭代器
     * @param ilist Initializer list containing elements to insert
     * @param ilist 包含要插入的元素的初始化列表
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details Inserts copies of the elements from the initializer list `ilist`
     * at the specified position `pos`. The elements are inserted in the same order
     * as they appear in the initializer list, before the element at the specified position.
     * 
     * @details 在指定位置`pos`处插入来自初始化列表`ilist`的元素副本。
     * 元素按照它们在初始化列表中出现的顺序插入，在指定位置的元素之前。
     * 
     * @note Time complexity: O(n + m) where n is the number of elements after the insertion point
     * and m is the size of the initializer list
     * @note 时间复杂度：O(n + m)，其中n是插入点之后的元素数量，m是初始化列表的大小
     * 
     * @note If the new size() is greater than capacity(), a reallocation takes place,
     * in which case all iterators, pointers and references to the elements are invalidated.
     * Otherwise, all iterators, pointers and references to elements at or after the
     * insertion point are invalidated.
     * 
     * @note 如果新的size()大于capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，所有指向插入点或之后元素的迭代器、指针和引用都会失效。
     * 
     * @note This function provides the strong exception guarantee.
     * 
     * @note 此函数提供强异常保证。
     * 
     * @note This function is convenient for inserting literal values into a vector:
     * `v.insert(v.begin() + 2, {10, 20, 30});`
     * 
     * @note 此函数便于将字面值插入vector：
     * `v.insert(v.begin() + 2, {10, 20, 30});`
     * 
     * @note The implementation delegates to `insert_range()` which handles both
     * in-place insertion and reallocation cases.
     * 
     * @note 实现委托给`insert_range()`，它处理原地插入和重新分配两种情况。
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see insert(const_iterator, const T&) for inserting a single element
     * @see insert(const_iterator, const T&) 用于插入单个元素
     * @see insert(const_iterator, size_type, const T&) for inserting multiple copies
     * @see insert(const_iterator, size_type, const T&) 用于插入多个副本
     * @see insert(const_iterator, T&&) for moving an element into the vector
     * @see insert(const_iterator, T&&) 用于将元素移动到vector中
     */
    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert_range(pos, ilist.begin(), ilist.end());
    }
    
    /**
     * @brief Erases the element at the specified position
     * @brief 删除指定位置的元素
     * 
     * @param pos Iterator to the element to erase
     * @param pos 指向要删除的元素的迭代器
     * @return Iterator following the last removed element
     * @return 指向最后一个被删除元素之后位置的迭代器
     * 
     * @details Erases the element at the specified position `pos`.
     * The element is destroyed and its memory is deallocated.
     * Elements after the erased element are shifted to fill the gap.
     * 
     * @details 删除指定位置`pos`处的元素。
     * 元素被销毁，其内存被释放。
     * 被删除元素之后的元素被移动以填补空缺。
     * 
     * @note Time complexity: O(n) where n is the number of elements after the erased element
     * @note 时间复杂度：O(n)，其中n是被删除元素之后的元素数量
     * 
     * @note This function invalidates iterators, pointers and references
     * to the erased element and all elements after it.
     * 
     * @note 此函数使指向被删除元素及其之后所有元素的迭代器、指针和引用失效。
     * 
     * @note This function is noexcept, meaning it cannot throw exceptions.
     * 
     * @note 此函数是noexcept的，意味着它不能抛出异常。
     * 
     * @note The implementation destroys the element at the specified position,
     * then moves the subsequent elements one position forward to fill the gap.
     * 
     * @note 实现销毁指定位置的元素，然后将后续元素向前移动一个位置以填补空缺。
     * 
     * @warning Calling erase() with an invalid iterator (e.g., end()) results in undefined behavior.
     * 
     * @warning 使用无效迭代器（如end()）调用erase()会导致未定义行为。
     * 
     * @see erase(const_iterator, const_iterator) for erasing a range of elements
     * @see erase(const_iterator, const_iterator) 用于删除元素范围
     * @see clear() for removing all elements
     * @see clear() 用于移除所有元素
     * @see pop_back() for removing the last element
     * @see pop_back() 用于移除最后一个元素
     */
    iterator erase(const_iterator pos) {
        pointer p = const_cast<pointer>(pos);
        alloc_traits::destroy(alloc_, p);
        
        if (p + 1 != end_) {
            // 向前移动元素
            for (pointer it = p; it + 1 < end_; ++it) {
                *it = mystl::move(*(it + 1));
            }
        }
        --end_;
        return p;
    }
    
    /**
     * @brief Erases a range of elements
     * @brief 删除元素范围
     * 
     * @param first Iterator to the first element in the range to erase
     * @param first 指向要删除范围中第一个元素的迭代器
     * @param last Iterator to one past the last element in the range to erase
     * @param last 指向要删除范围中最后一个元素之后位置的迭代器
     * @return Iterator following the last removed element
     * @return 指向最后一个被删除元素之后位置的迭代器
     * 
     * @details Erases the elements in the range [first, last).
     * The elements are destroyed and their memory is deallocated.
     * Elements after the erased range are shifted to fill the gap.
     * 
     * @details 删除范围[first, last)中的元素。
     * 元素被销毁，其内存被释放。
     * 被删除范围之后的元素被移动以填补空缺。
     * 
     * @note Time complexity: O(n) where n is the number of elements after the erased range
     * @note 时间复杂度：O(n)，其中n是被删除范围之后的元素数量
     * 
     * @note This function invalidates iterators, pointers and references
     * to the erased elements and all elements after them.
     * 
     * @note 此函数使指向被删除元素及其之后所有元素的迭代器、指针和引用失效。
     * 
     * @note This function is noexcept, meaning it cannot throw exceptions.
     * 
     * @note 此函数是noexcept的，意味着它不能抛出异常。
     * 
     * @note If `first == last`, the function does nothing and returns `first`.
     * 
     * @note 如果`first == last`，函数不执行任何操作并返回`first`。
     * 
     * @note The implementation destroys all elements in the range,
     * then moves the subsequent elements to fill the gap.
     * 
     * @note 实现销毁范围内的所有元素，然后将后续元素移动以填补空缺。
     * 
     * @warning Calling erase() with an invalid range (e.g., where `last` comes before `first`)
     * results in undefined behavior.
     * 
     * @warning 使用无效范围（如`last`在`first`之前）调用erase()会导致未定义行为。
     * 
     * @see erase(const_iterator) for erasing a single element
     * @see erase(const_iterator) 用于删除单个元素
     * @see clear() for removing all elements
     * @see clear() 用于移除所有元素
     */
    iterator erase(const_iterator first, const_iterator last) {
        if (first == last) return const_cast<pointer>(first);
        
        pointer f = const_cast<pointer>(first);
        pointer l = const_cast<pointer>(last);
        size_type count = l - f;
        
        // 销毁范围内的元素
        mystl::destroy(f, l);
        
        if (l != end_) {
            // 向前移动尾部元素
            for (pointer src = l, dst = f; src < end_; ++src, ++dst) {
                *dst = mystl::move(*src);
            }
        }
        end_ -= count;
        return f;
    }
    
    /**
     * @brief Exchanges the contents of the vector with those of another vector
     * @brief 将此vector的内容与另一个vector交换
     * 
     * @param other Another vector to exchange contents with
     * @param other 要与之交换内容的另一个vector
     * 
     * @details Exchanges the contents of the vector with those of `other`.
     * Does not invoke any move, copy, or swap operations on individual elements.
     * All iterators and references remain valid. The past-the-end iterator is invalidated.
     * 
     * @details 将此vector的内容与`other`的内容交换。
     * 不对单个元素调用任何移动、拷贝或交换操作。
     * 所有迭代器和引用保持有效。尾后迭代器失效。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept, meaning it cannot throw exceptions.
     * It simply swaps the internal pointers and allocator.
     * 
     * @note 此函数是noexcept的，意味着它不能抛出异常。
     * 它只是交换内部指针和分配器。
     * 
     * @note After the swap, the allocators are swapped only if
     * `alloc_traits::propagate_on_container_swap` is true_type.
     * Otherwise, the allocators are not swapped.
     * 
     * @note 交换后，仅当`alloc_traits::propagate_on_container_swap`为true_type时
     * 才交换分配器。否则，分配器不交换。
     * 
     * @note This operation is very efficient as it only swaps pointers
     * and does not copy or move any elements.
     * 
     * @note 此操作非常高效，因为它只交换指针，不复制或移动任何元素。
     * 
     * @see global swap() function for ADL-enabled swapping
     * @see 全局swap()函数用于支持ADL的交换
     */
    void swap(vector& other) noexcept {
        using mystl::swap;
        swap(begin_, other.begin_);
        swap(end_, other.end_);
        swap(capacity_, other.capacity_);
        swap(alloc_, other.alloc_);
    }
    
    /**
     * @brief Replaces the contents of the vector with count copies of value
     * @brief 用count个value的副本替换vector的内容
     * 
     * @param count New size of the container
     * @param count 容器的新大小
     * @param value The value to initialize elements with
     * @param value 用于初始化元素的值
     * 
     * @details Replaces the contents of the vector with `count` copies of `value`.
     * If `count` is greater than the current capacity(), a reallocation takes place.
     * Otherwise, the existing memory is reused.
     * 
     * @details 用`count`个`value`的副本替换vector的内容。
     * 如果`count`大于当前capacity()，会发生重新分配。
     * 否则，重用现有内存。
     * 
     * @note Time complexity: O(count)
     * @note 时间复杂度：O(count)
     * 
     * @note If a reallocation happens, all iterators, pointers and references
     * to the elements are invalidated. Otherwise, only the iterators, pointers
     * and references to the removed elements are invalidated.
     * 
     * @note 如果发生重新分配，所有指向元素的迭代器、指针和引用都会失效。
     * 否则，只有指向被移除元素的迭代器、指针和引用会失效。
     * 
     * @note This function provides the strong exception guarantee.
     * 
     * @note 此函数提供强异常保证。
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see assign(InputIt, InputIt) for assigning from a range
     * @see assign(InputIt, InputIt) 用于从范围赋值
     * @see assign(std::initializer_list<T>) for assigning from an initializer list
     * @see assign(std::initializer_list<T>) 用于从初始化列表赋值
     */
    void assign(size_type count, const T& value) {
        // 如果count大于当前容量，需要重新分配
        if (count > capacity()) {
            vector temp(count, value, alloc_);
            swap(temp);
        } else {
            // 重用现有内存
            clear();
            if (count > 0) {
                mystl::uninitialized_fill_n(begin_, count, value);
                end_ = begin_ + count;
            }
        }
    }
    
    /**
     * @brief Replaces the contents of the vector with elements from a range
     * @brief 用来自范围的元素替换vector的内容
     * 
     * @tparam InputIt Input iterator type
     * @tparam InputIt 输入迭代器类型
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to one past the last element in the range
     * @param last 指向范围中最后一个元素之后位置的迭代器
     * 
     * @details Replaces the contents of the vector with copies of the elements
     * in the range [first, last). The range must be valid and InputIt must
     * satisfy the InputIterator concept.
     * 
     * @details 用范围[first, last)中元素的副本替换vector的内容。
     * 范围必须有效，且InputIt必须满足InputIterator概念。
     * 
     * @note Time complexity: O(n) where n is the distance between first and last
     * @note 时间复杂度：O(n)，其中n是first和last之间的距离
     * 
     * @note If the new size is greater than the current capacity(), a reallocation
     * takes place, in which case all iterators, pointers and references to the
     * elements are invalidated. Otherwise, only the iterators, pointers and
     * references to the removed elements are invalidated.
     * 
     * @note 如果新大小大于当前capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，只有指向被移除元素的迭代器、指针和引用会失效。
     * 
     * @note This function provides the strong exception guarantee.
     * 
     * @note 此函数提供强异常保证。
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see assign(size_type, const T&) for assigning multiple copies of a value
     * @see assign(size_type, const T&) 用于分配一个值的多个副本
     * @see assign(std::initializer_list<T>) for assigning from an initializer list
     * @see assign(std::initializer_list<T>) 用于从初始化列表分配
     */
    template<typename InputIt, typename = typename std::iterator_traits<InputIt>::iterator_category>
    void assign(InputIt first, InputIt last) {
        size_type count = static_cast<size_type>(mystl::distance(first, last));
        
        // 如果count大于当前容量，需要重新分配
        if (count > capacity()) {
            vector temp(first, last, alloc_);
            swap(temp);
        } else {
            // 重用现有内存
            clear();
            if (count > 0) {
                mystl::uninitialized_copy(first, last, begin_);
                end_ = begin_ + count;
            }
        }
    }
    
    /**
     * @brief Replaces the contents of the vector with elements from an initializer list
     * @brief 用来自初始化列表的元素替换vector的内容
     * 
     * @param ilist Initializer list containing elements to assign
     * @param ilist 包含要分配的元素的初始化列表
     * 
     * @details Replaces the contents of the vector with copies of the elements
     * in the initializer list `ilist`. The elements are inserted in the same order
     * as they appear in the initializer list.
     * 
     * @details 用初始化列表`ilist`中元素的副本替换vector的内容。
     * 元素按照它们在初始化列表中出现的顺序插入。
     * 
     * @note Time complexity: O(n) where n is the size of the initializer list
     * @note 时间复杂度：O(n)，其中n是初始化列表的大小
     * 
     * @note If the new size is greater than the current capacity(), a reallocation
     * takes place, in which case all iterators, pointers and references to the
     * elements are invalidated. Otherwise, only the iterators, pointers and
     * references to the removed elements are invalidated.
     * 
     * @note 如果新大小大于当前capacity()，会发生重新分配，
     * 这种情况下所有指向元素的迭代器、指针和引用都会失效。
     * 否则，只有指向被移除元素的迭代器、指针和引用会失效。
     * 
     * @note This function provides the strong exception guarantee.
     * 
     * @note 此函数提供强异常保证。
     * 
     * @note This function is convenient for assigning literal values to a vector:
     * `v.assign({1, 2, 3, 4, 5});`
     * 
     * @note 此函数便于将字面值分配给vector：
     * `v.assign({1, 2, 3, 4, 5});`
     * 
     * @exception std::bad_alloc If memory allocation fails (when reallocation is needed)
     * @exception std::bad_alloc 如果内存分配失败（当需要重新分配时）
     * @exception Any exception thrown by the copy constructor of T
     * @exception T的拷贝构造函数抛出的任何异常
     * 
     * @see assign(size_type, const T&) for assigning multiple copies of a value
     * @see assign(size_type, const T&) 用于分配一个值的多个副本
     * @see assign(InputIt, InputIt) for assigning from a range
     * @see assign(InputIt, InputIt) 用于从范围分配
     */
    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }
    
    /**
     * @brief Erases all elements that compare equal to value (C++20 feature)
     * @brief 删除所有等于value的元素（C++20特性）
     * 
     * @param value Value to compare the elements to
     * @param value 用于与元素比较的值
     * @return Number of elements erased
     * @return 删除的元素数量
     * 
     * @details Erases all elements that compare equal to `value` using `operator==`.
     * This is a C++20 feature that provides a more convenient way to remove
     * all occurrences of a specific value from the vector.
     * 
     * @details 使用`operator==`删除所有等于`value`的元素。
     * 这是C++20特性，提供了一种更便捷的方式从vector中删除特定值的所有出现。
     * 
     * @note Time complexity: O(n) where n is the size of the vector
     * @note 时间复杂度：O(n)，其中n是vector的大小
     * 
     * @note This function uses the `remove` algorithm to move the elements
     * to be removed to the end of the vector, then erases them.
     * 
     * @note 此函数使用`remove`算法将要删除的元素移动到vector的末尾，然后删除它们。
     * 
     * @note All iterators, pointers and references to the removed elements
     * are invalidated. Iterators, pointers and references to elements
     * before the first removed element remain valid.
     * 
     * @note 所有指向被删除元素的迭代器、指针和引用都会失效。
     * 指向第一个被删除元素之前元素的迭代器、指针和引用保持有效。
     * 
     * @note This function provides the strong exception guarantee.
     * 
     * @note 此函数提供强异常保证。
     * 
     * @note This is a C++20 feature. For C++17 and earlier, use the
     * erase-remove idiom: `v.erase(std::remove(v.begin(), v.end(), value), v.end());`
     * 
     * @note 这是C++20特性。对于C++17及更早版本，使用erase-remove惯用法：
     * `v.erase(std::remove(v.begin(), v.end(), value), v.end());`
     * 
     * @exception Any exception thrown by the comparison operator of T
     * @exception T的比较运算符抛出的任何异常
     * 
     * @see erase_if() for erasing elements based on a predicate
     * @see erase_if() 用于基于谓词删除元素
     * @see erase(const_iterator) for erasing a single element at a specific position
     * @see erase(const_iterator) 用于删除特定位置的单个元素
     */
    size_type erase(const T& value) {
        size_type old_size = size();
        iterator new_end = mystl::remove(begin(), end(), value);
        size_type erased_count = old_size - size_type(new_end - begin());
        erase(new_end, end());
        return erased_count;
    }
    
    /**
     * @brief Erases all elements that satisfy the predicate (C++20 feature)
     * @brief 删除所有满足谓词的元素（C++20特性）
     * 
     * @tparam Predicate Type of the predicate function
     * @tparam Predicate 谓词函数的类型
     * @param pred Unary predicate which returns true for elements to be removed
     * @param pred 一元谓词，对于要删除的元素返回true
     * @return Number of elements erased
     * @return 删除的元素数量
     * 
     * @details Erases all elements for which the predicate `pred` returns true.
     * This is a C++20 feature that provides a convenient way to remove elements
     * based on a condition.
     * 
     * @details 删除所有谓词`pred`返回true的元素。
     * 这是C++20特性，提供了一种基于条件删除元素的便捷方式。
     * 
     * @note Time complexity: O(n) where n is the size of the vector
     * @note 时间复杂度：O(n)，其中n是vector的大小
     * 
     * @note This function uses the `remove_if` algorithm to move the elements
     * to be removed to the end of the vector, then erases them.
     * 
     * @note 此函数使用`remove_if`算法将要删除的元素移动到vector的末尾，然后删除它们。
     * 
     * @note All iterators, pointers and references to the removed elements
     * are invalidated. Iterators, pointers and references to elements
     * before the first removed element remain valid.
     * 
     * @note 所有指向被删除元素的迭代器、指针和引用都会失效。
     * 指向第一个被删除元素之前元素的迭代器、指针和引用保持有效。
     * 
     * @note This function provides the strong exception guarantee.
     * 
     * @note 此函数提供强异常保证。
     * 
     * @note This is a C++20 feature. For C++17 and earlier, use the
     * erase-remove idiom: `v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());`
     * 
     * @note 这是C++20特性。对于C++17及更早版本，使用erase-remove惯用法：
     * `v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());`
     * 
     * @note The predicate must not invalidate iterators or references within the range,
     * nor modify the elements of the sequence.
     * 
     * @note 谓词不得使范围内的迭代器或引用失效，也不得修改序列的元素。
     * 
     * @exception Any exception thrown by the predicate
     * @exception 谓词抛出的任何异常
     * 
     * @see erase(const T&) for erasing elements equal to a specific value
     * @see erase(const T&) 用于删除等于特定值的元素
     * @see erase(const_iterator) for erasing a single element at a specific position
     * @see erase(const_iterator) 用于删除特定位置的单个元素
     */
    template<typename Predicate>
    size_type erase_if(Predicate pred) {
        size_type old_size = size();
        iterator new_end = mystl::remove_if(begin(), end(), pred);
        size_type erased_count = old_size - size_type(new_end - begin());
        erase(new_end, end());
        return erased_count;
    }
    
    /**
     * @brief Returns the allocator associated with the vector
     * @brief 返回与vector关联的分配器
     * 
     * @return The allocator used by the vector
     * @return vector使用的分配器
     * 
     * @details Returns a copy of the allocator associated with the vector.
     * This function is useful when you need to allocate memory using the same
     * allocator as the vector, or when you need to pass the allocator to
     * another container or algorithm.
     * 
     * @details 返回与vector关联的分配器的副本。
     * 当需要使用与vector相同的分配器分配内存，或将分配器传递给
     * 另一个容器或算法时，此函数很有用。
     * 
     * @note Time complexity: O(1)
     * @note 时间复杂度：O(1)
     * 
     * @note This function is noexcept and does not allocate or deallocate memory.
     * 
     * @note 此函数是noexcept的，不分配或释放内存。
     * 
     * @note The returned allocator is a copy of the internal allocator.
     * Modifying the returned allocator does not affect the vector's allocator.
     * 
     * @note 返回的分配器是内部分配器的副本。
     * 修改返回的分配器不会影响vector的分配器。
     * 
     * @note This function is particularly useful when implementing custom
     * algorithms that need to allocate memory with the same allocator as
     * the container they operate on.
     * 
     * @note 此函数在实现需要以与操作容器相同的分配器分配内存的
     * 自定义算法时特别有用。
     * 
     * @see allocator_type for the type of the allocator
     * @see allocator_type 用于分配器类型
     * @see vector(const Allocator&) for constructing a vector with a specific allocator
     * @see vector(const Allocator&) 用于使用特定分配器构造vector
     */
    allocator_type get_allocator() const noexcept {
        return alloc_;
    }
    
private:
    /**
     * @brief Helper function for inserting multiple copies of a value
     * @brief 插入多个相同值的辅助函数
     * 
     * @param pos Iterator to the insertion position
     * @param pos 指向插入位置的迭代器
     * @param count Number of elements to insert
     * @param count 要插入的元素数量
     * @param value Value to initialize the inserted elements with
     * @param value 用于初始化插入元素的值
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details This is the core implementation for inserting multiple copies of a value.
     * It handles both in-place insertion (when there's enough capacity) and
     * reallocation (when capacity needs to be increased).
     * 
     * @details 这是插入多个相同值的核心实现。
     * 它处理原地插入（当有足够容量时）和重新分配（当需要增加容量时）两种情况。
     * 
     * @note Time complexity: O(n + count) where n is the number of elements after the insertion point
     * @note 时间复杂度：O(n + count)，其中n是插入点之后的元素数量
     * 
     * @note The algorithm uses a three-step process:
     * 1. Check if reallocation is needed
     * 2. Move existing elements to make space
     * 3. Construct new elements in the created space
     * 
     * @note 算法使用三步过程：
     * 1. 检查是否需要重新分配
     * 2. 移动现有元素以腾出空间
     * 3. 在创建的空间中构造新元素
     * 
     * @note This function provides the strong exception guarantee.
     * 
     * @note 此函数提供强异常保证。
     */
    iterator insert_impl(const_iterator pos, size_type count, const T& value) {
        size_type index = pos - begin_;
        
        if (count == 0) return begin_ + index;
        
        // 确保有足够容量：如果当前容量不足，需要重新分配内存
        if (size() + count > capacity()) {
            size_type new_capacity = size() + count;
            size_type doubled = capacity() * 2;
            if (doubled > new_capacity) {
                new_capacity = doubled;
            }
            reallocate_with_insert(new_capacity, index, count, value);
            return begin_ + index;
        }
        
        pointer p = begin_ + index;
        
        if (p != end_) {
            // 移动尾部元素：为插入腾出空间
            mystl::uninitialized_move(end_ - count, end_, end_);
            // 向后移动中间元素：将插入点之后的元素向后移动
            for (pointer it = end_ - 1; it >= p + count; --it) {
                *it = mystl::move(*(it - count));
            }
            // 销毁原位置元素（已经被移动走了）
            mystl::destroy(p, p + count);
        }
        
        // 构造新元素：在腾出的空间中构造count个value的副本
        mystl::uninitialized_fill_n(p, count, value);
        end_ += count;
        return p;
    }
    
    /**
     * @brief Reallocates memory and inserts elements during the process
     * @brief 重新分配内存并在过程中插入元素
     * 
     * @param new_capacity New capacity after reallocation
     * @param new_capacity 重新分配后的新容量
     * @param index Index where elements should be inserted
     * @param index 元素应插入的索引位置
     * @param count Number of elements to insert
     * @param count 要插入的元素数量
     * @param value Value to initialize the inserted elements with
     * @param value 用于初始化插入元素的值
     * 
     * @details This function is called when the vector needs to reallocate memory
     * while inserting elements. It allocates new memory, moves existing elements
     * to the new memory while inserting new elements at the specified position.
     * 
     * @details 当vector在插入元素时需要重新分配内存时调用此函数。
     * 它分配新内存，将现有元素移动到新内存中，同时在指定位置插入新元素。
     * 
     * @note Time complexity: O(n + count) where n is the size of the vector
     * @note 时间复杂度：O(n + count)，其中n是vector的大小
     * 
     * @note This function provides the strong exception guarantee:
     * if an exception is thrown during element construction, all already
     * constructed elements are destroyed and the memory is deallocated.
     * @note 此函数提供强异常保证：如果在元素构造期间抛出异常，
     * 所有已构造的元素都会被销毁，内存会被释放。
     * 
     * @note The function uses a three-step process:
     * 1. Allocate new memory with the specified capacity
     * 2. Move existing elements before the insertion point to the new memory
     * 3. Construct new elements at the insertion point
     * 4. Move existing elements after the insertion point to the new memory
     * 
     * @note 函数使用三步过程：
     * 1. 分配具有指定容量的新内存
     * 2. 将插入点之前的现有元素移动到新内存
     * 3. 在插入点构造新元素
     * 4. 将插入点之后的现有元素移动到新内存
     * 
     * @note This is a low-level helper function that handles the most complex
     * case of insertion with reallocation.
     * 
     * @note 这是一个低级辅助函数，处理重新分配插入的最复杂情况。
     */
    void reallocate_with_insert(size_type new_capacity, size_type index, size_type count, const T& value) {
        pointer new_begin = alloc_traits::allocate(alloc_, new_capacity);
        pointer new_end = new_begin;
        
        try {
            // 拷贝插入点之前的元素：将原vector中插入点之前的元素移动到新内存
            new_end = mystl::uninitialized_move(begin_, begin_ + index, new_begin);
            
            // 构造新插入的元素：在插入点构造count个value的副本
            new_end = mystl::uninitialized_fill_n(new_end, count, value);
            
            // 拷贝插入点之后的元素：将原vector中插入点之后的元素移动到新内存
            new_end = mystl::uninitialized_move(begin_ + index, end_, new_end);
        } catch (...) {
            mystl::destroy(new_begin, new_end);
            alloc_traits::deallocate(alloc_, new_begin, new_capacity);
            throw;
        }
        
        // 销毁旧元素：销毁原vector中的所有元素
        mystl::destroy(begin_, end_);
        
        // 释放旧内存：释放原vector分配的内存
        size_type old_capacity = capacity();
        alloc_traits::deallocate(alloc_, begin_, old_capacity);
        
        // 更新指针：更新vector的三指针模型
        begin_ = new_begin;
        end_ = new_end;
        capacity_ = begin_ + new_capacity;
    }
    
    /**
     * @brief Helper function for inserting a range of elements
     * @brief 插入元素范围的辅助函数
     * 
     * @tparam InputIt Input iterator type that satisfies the InputIterator concept.
     * Must support dereferencing (*it) to get the element value and incrementing (++it)
     * to move to the next element.
     * @tparam InputIt 满足InputIterator概念的输入迭代器类型。
     * 必须支持解引用（*it）以获取元素值和递增（++it）以移动到下一个元素。
     * @param pos Iterator to the insertion position
     * @param pos 指向插入位置的迭代器
     * @param first Iterator to the first element in the range
     * @param first 指向范围中第一个元素的迭代器
     * @param last Iterator to one past the last element in the range
     * @param last 指向范围中最后一个元素之后位置的迭代器
     * @return Iterator pointing to the first inserted element
     * @return 指向第一个插入元素的迭代器
     * 
     * @details This function inserts a range of elements [first, last) at the
     * specified position. It handles both in-place insertion and reallocation.
     * 
     * @details 此函数在指定位置插入元素范围[first, last)。
     * 它处理原地插入和重新分配两种情况。
     * 
     * @note Time complexity: O(n + m) where n is the number of elements after
     * the insertion point and m is the size of the range
     * @note 时间复杂度：O(n + m)，其中n是插入点之后的元素数量，m是范围的大小
     * 
     * @note This function is used by range constructors, range assignment,
     * and range insertion operations.
     * 
     * @note 此函数被范围构造函数、范围赋值和范围插入操作使用。
     * 
     * @note The implementation is similar to insert_impl() but works with
     * an iterator range instead of a single value.
     * 
     * @note 实现类似于insert_impl()，但使用迭代器范围而不是单个值。
     * 
     * @note This function provides the strong exception guarantee.
     * 
     * @note 此函数提供强异常保证。
     */
    template<typename InputIt>
    iterator insert_range(const_iterator pos, InputIt first, InputIt last) {
        size_type count = static_cast<size_type>(mystl::distance(first, last));
        size_type index = pos - begin_;
        
        if (count == 0) return begin_ + index;
        
        // 确保有足够容量：如果当前容量不足，需要重新分配内存
        if (size() + count > capacity()) {
            size_type new_capacity = size() + count;
            size_type doubled = capacity() * 2;
            if (doubled > new_capacity) {
                new_capacity = doubled;
            }
            
            // 重新分配内存并插入元素
            pointer new_begin = alloc_traits::allocate(alloc_, new_capacity);
            pointer new_end = new_begin;
            
            try {
                // 拷贝插入点之前的元素
                new_end = mystl::uninitialized_move(begin_, begin_ + index, new_begin);
                
                // 构造新插入的元素
                new_end = mystl::uninitialized_copy(first, last, new_end);
                
                // 拷贝插入点之后的元素
                new_end = mystl::uninitialized_move(begin_ + index, end_, new_end);
            } catch (...) {
                mystl::destroy(new_begin, new_end);
                alloc_traits::deallocate(alloc_, new_begin, new_capacity);
                throw;
            }
            
            // 销毁旧元素
            mystl::destroy(begin_, end_);
            
            // 释放旧内存
            size_type old_capacity = capacity();
            alloc_traits::deallocate(alloc_, begin_, old_capacity);
            
            // 更新指针
            begin_ = new_begin;
            end_ = new_end;
            capacity_ = begin_ + new_capacity;
            
            return begin_ + index;
        }
        
        pointer p = begin_ + index;
        
        if (p != end_) {
            // 移动尾部元素
            mystl::uninitialized_move(end_ - count, end_, end_);
            // 向后移动中间元素
            for (pointer it = end_ - 1; it >= p + count; --it) {
                *it = mystl::move(*(it - count));
            }
            // 销毁原位置元素（已经被移动走了）
            mystl::destroy(p, p + count);
        }
        
        // 构造新元素
        mystl::uninitialized_copy(first, last, p);
        end_ += count;
        return p;
    }
    
private:
    /**
     * @brief Reallocates memory to a new capacity
     * @brief 重新分配内存到新容量
     * 
     * @param new_capacity New capacity to allocate
     * @param new_capacity 要分配的新容量
     * 
     * @details This function reallocates the vector's memory to the specified new capacity.
     * It allocates new memory, moves existing elements to the new memory using
     * exception-safe move operations, then deallocates the old memory.
     * 
     * @details 此函数将vector的内存重新分配到指定的新容量。
     * 它分配新内存，使用异常安全移动操作将现有元素移动到新内存，
     * 然后释放旧内存。
     * 
     * @note Time complexity: O(n) where n is the size of the vector
     * @note 时间复杂度：O(n)，其中n是vector的大小
     * 
     * @note This function uses `uninitialized_move_if_noexcept()` to move elements,
     * which uses move construction if the move constructor is noexcept,
     * otherwise falls back to copy construction to maintain exception safety.
     * 
     * @note 此函数使用`uninitialized_move_if_noexcept()`移动元素，
     * 如果移动构造函数是noexcept的，则使用移动构造，
     * 否则回退到拷贝构造以保持异常安全。
     * 
     * @note If `new_capacity` is 0, it is set to 1 to ensure valid memory allocation.
     * 
     * @note 如果`new_capacity`为0，它被设置为1以确保有效的内存分配。
     * 
     * @note This function provides the strong exception guarantee:
     * if an exception is thrown, the vector remains unchanged.
     * 
     * @note 此函数提供强异常保证：如果抛出异常，vector保持不变。
     * 
     * @exception std::bad_alloc If memory allocation fails
     * @exception std::bad_alloc 如果内存分配失败
     * @exception Any exception thrown by the move or copy constructor of T
     * @exception T的移动或拷贝构造函数抛出的任何异常
     */
    void reallocate(size_type new_capacity) {
        if (new_capacity == 0) {
            new_capacity = 1;
        }
        
        pointer new_begin = alloc_traits::allocate(alloc_, new_capacity);
        
        try {
            // 使用安全移动函数：如果移动构造函数是noexcept的则移动，否则拷贝
            mystl::uninitialized_move_if_noexcept(begin_, end_, new_begin);
        } catch (...) {
            alloc_traits::deallocate(alloc_, new_begin, new_capacity);
            throw;
        }
        
        // 销毁旧元素：销毁原vector中的所有元素
        mystl::destroy(begin_, end_);
        
        // 释放旧内存：释放原vector分配的内存
        size_type old_capacity = capacity();
        alloc_traits::deallocate(alloc_, begin_, old_capacity);
        
        // 更新指针：更新vector的三指针模型
        size_type old_size = size();
        begin_ = new_begin;
        end_ = begin_ + old_size;
        capacity_ = begin_ + new_capacity;
    }
    
    /**
     * @brief Checks if the vector needs to grow and grows it if necessary
     * @brief 检查vector是否需要扩容，必要时进行扩容
     * 
     * @details This function checks if the vector has reached its capacity.
     * If `end_ == capacity_`, it means the vector is full and needs to grow.
     * The growth strategy is to double the current capacity, or set it to 1
     * if the current capacity is 0.
     * 
     * @details 此函数检查vector是否已达到其容量。
     * 如果`end_ == capacity_`，意味着vector已满需要扩容。
     * 增长策略是将当前容量翻倍，如果当前容量为0则设置为1。
     * 
     * @note Time complexity: O(1) for checking, O(n) for reallocation if needed
     * @note 时间复杂度：检查为O(1)，如果需要重新分配则为O(n)
     * 
     * @note This function implements the standard vector growth strategy:
     * - If capacity is 0, new capacity is 1
     * - Otherwise, new capacity is capacity * 2
     * This provides amortized O(1) time complexity for push_back operations.
     * 
     * @note 此函数实现标准vector增长策略：
     * - 如果容量为0，新容量为1
     * - 否则，新容量为容量 * 2
     * 这为push_back操作提供摊还O(1)时间复杂度。
     * 
     * @note The doubling growth strategy ensures that the amortized cost
     * of insertions at the end is constant, making vector efficient for
     * sequential insertions.
     * 
     * @note 翻倍增长策略确保尾部插入的摊还成本是常数，
     * 使得vector对于顺序插入是高效的。
     * 
     * @note This function is called by `push_back()`, `emplace_back()`,
     * and other insertion operations that may require more capacity.
     * 
     * @note 此函数被`push_back()`、`emplace_back()`和其他可能需要更多容量的插入操作调用。
     */
    void check_grow() {
        if (end_ == capacity_) {
            // 翻倍增长策略：如果当前容量为0，则新容量为1；否则新容量为当前容量的两倍
            // 这种策略确保摊还O(1)的尾部插入时间复杂度
            // Doubling growth strategy: if current capacity is 0, new capacity is 1;
            // otherwise new capacity is twice the current capacity.
            // This strategy ensures amortized O(1) time complexity for push_back operations.
            size_type new_capacity = capacity() == 0 ? 1 : capacity() * 2;
            reallocate(new_capacity);
        }
    }
};

/**
 * @brief Swaps the contents of two vectors
 * @brief 交换两个vector的内容
 * 
 * @tparam T Type of elements stored in the vectors
 * @tparam T 存储在vector中的元素类型
 * @tparam Alloc Allocator type used by the vectors
 * @tparam Alloc vector使用的分配器类型
 * @param lhs First vector to swap
 * @param lhs 要交换的第一个vector
 * @param rhs Second vector to swap
 * @param rhs 要交换的第二个vector
 * 
 * @details Exchanges the contents of `lhs` and `rhs` by calling `lhs.swap(rhs)`.
 * This function enables argument-dependent lookup (ADL) for swapping vectors,
 * allowing `swap(lhs, rhs)` to work properly in generic code.
 * 
 * @details 通过调用`lhs.swap(rhs)`交换`lhs`和`rhs`的内容。
 * 此函数为交换vector启用参数依赖查找（ADL），
 * 允许`swap(lhs, rhs)`在泛型代码中正常工作。
 * 
 * @note Time complexity: O(1)
 * @note 时间复杂度：O(1)
 * 
 * @note This function is noexcept, meaning it cannot throw exceptions.
 * It simply delegates to the member `swap()` function.
 * 
 * @note 此函数是noexcept的，意味着它不能抛出异常。
 * 它只是委托给成员`swap()`函数。
 * 
 * @note This function is found by ADL when using `swap(lhs, rhs)` in generic code.
 * It provides a non-member swap function that works with the standard library algorithms.
 * 
 * @note 在泛型代码中使用`swap(lhs, rhs)`时，此函数通过ADL找到。
 * 它提供了一个与标准库算法配合使用的非成员交换函数。
 * 
 * @see vector::swap() for the member swap function
 * @see vector::swap() 用于成员交换函数
 * @see std::swap() for the generic swap algorithm
 * @see std::swap() 用于通用交换算法
 */
template<typename T, typename Alloc>
void swap(vector<T, Alloc>& lhs, vector<T, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

/**
 * @brief Equality comparison operator for vectors
 * @brief vector的相等比较操作符
 * 
 * @tparam T Type of elements stored in the vectors
 * @tparam T 存储在vector中的元素类型
 * @tparam Alloc Allocator type used by the vectors
 * @tparam Alloc vector使用的分配器类型
 * @param lhs First vector to compare
 * @param lhs 要比较的第一个vector
 * @param rhs Second vector to compare
 * @param rhs 要比较的第二个vector
 * @return true if the vectors are equal, false otherwise
 * @return 如果vector相等则返回true，否则返回false
 * 
 * @details Compares two vectors for equality. Two vectors are equal if:
 * 1. They have the same size
 * 2. Each element in `lhs` compares equal with the element in `rhs` at the same position
 * 
 * @details 比较两个vector是否相等。两个vector相等当且仅当：
 * 1. 它们具有相同的大小
 * 2. `lhs`中的每个元素与`rhs`中相同位置的元素比较相等
 * 
 * @note Time complexity: O(n) where n is the size of the vectors
 * @note 时间复杂度：O(n)，其中n是vector的大小
 * 
 * @note This operator uses `mystl::equal()` to compare elements, which stops
 * at the first mismatch. If the vectors have different sizes, it returns false
 * immediately without comparing any elements.
 * 
 * @note 此操作符使用`mystl::equal()`比较元素，它在第一个不匹配处停止。
 * 如果vector具有不同的大小，它会立即返回false而不比较任何元素。
 * 
 * @note The comparison uses `operator==` for the element type T.
 * 
 * @note 比较使用元素类型T的`operator==`。
 * 
 * @exception Any exception thrown by the element type's `operator==`
 * @exception 元素类型的`operator==`抛出的任何异常
 * 
 * @see operator!= for inequality comparison
 * @see operator!= 用于不等比较
 * @see mystl::equal() for the underlying comparison algorithm
 * @see mystl::equal() 用于底层比较算法
 */
template<typename T, typename Alloc>
bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/**
 * @brief Inequality comparison operator for vectors
 * @brief vector的不等比较操作符
 * 
 * @tparam T Type of elements stored in the vectors
 * @tparam T 存储在vector中的元素类型
 * @tparam Alloc Allocator type used by the vectors
 * @tparam Alloc vector使用的分配器类型
 * @param lhs First vector to compare
 * @param lhs 要比较的第一个vector
 * @param rhs Second vector to compare
 * @param rhs 要比较的第二个vector
 * @return true if the vectors are not equal, false otherwise
 * @return 如果vector不相等则返回true，否则返回false
 * 
 * @details Compares two vectors for inequality. Returns the logical negation
 * of the equality comparison: `!(lhs == rhs)`.
 * 
 * @details 比较两个vector是否不相等。返回相等比较的逻辑否定：
 * `!(lhs == rhs)`。
 * 
 * @note Time complexity: O(n) where n is the size of the vectors
 * @note 时间复杂度：O(n)，其中n是vector的大小
 * 
 * @note This operator is implemented in terms of `operator==`, ensuring
 * consistency between equality and inequality comparisons.
 * 
 * @note 此操作符基于`operator==`实现，确保相等和不等比较之间的一致性。
 * 
 * @note The comparison uses `operator==` for the element type T.
 * 
 * @note 比较使用元素类型T的`operator==`。
 * 
 * @exception Any exception thrown by the element type's `operator==`
 * @exception 元素类型的`operator==`抛出的任何异常
 * 
 * @see operator== for equality comparison
 * @see operator== 用于相等比较
 */
template<typename T, typename Alloc>
bool operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief Less-than comparison operator for vectors
 * @brief vector的小于比较操作符
 * 
 * @tparam T Type of elements stored in the vectors
 * @tparam T 存储在vector中的元素类型
 * @tparam Alloc Allocator type used by the vectors
 * @tparam Alloc vector使用的分配器类型
 * @param lhs First vector to compare
 * @param lhs 要比较的第一个vector
 * @param rhs Second vector to compare
 * @param rhs 要比较的第二个vector
 * @return true if lhs is lexicographically less than rhs, false otherwise
 * @return 如果lhs在字典序上小于rhs则返回true，否则返回false
 * 
 * @details Performs lexicographical comparison of two vectors. Compares the
 * elements sequentially using `operator<` for the element type T, stopping
 * at the first mismatch. If one vector is a prefix of the other, the shorter
 * vector is considered less.
 * 
 * @details 对两个vector执行字典序比较。使用元素类型T的`operator<`
 * 顺序比较元素，在第一个不匹配处停止。如果一个vector是另一个的前缀，
 * 则较短的vector被认为较小。
 * 
 * @note Time complexity: O(min(n, m)) where n and m are the sizes of the vectors
 * @note 时间复杂度：O(min(n, m))，其中n和m是vector的大小
 * 
 * @note This operator uses `mystl::lexicographical_compare()` to perform
 * the comparison, which provides the standard lexicographical ordering.
 * 
 * @note 此操作符使用`mystl::lexicographical_compare()`执行比较，
 * 它提供标准的字典序排序。
 * 
 * @note The comparison uses `operator<` for the element type T.
 * 
 * @note 比较使用元素类型T的`operator<`。
 * 
 * @exception Any exception thrown by the element type's `operator<`
 * @exception 元素类型的`operator<`抛出的任何异常
 * 
 * @see operator<= for less-than-or-equal comparison
 * @see operator<= 用于小于等于比较
 * @see mystl::lexicographical_compare() for the underlying comparison algorithm
 * @see mystl::lexicographical_compare() 用于底层比较算法
 */
template<typename T, typename Alloc>
bool operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return mystl::lexicographical_compare(lhs.begin(), lhs.end(),
                                         rhs.begin(), rhs.end());
}

/**
 * @brief Less-than-or-equal comparison operator for vectors
 * @brief vector的小于等于比较操作符
 * 
 * @tparam T Type of elements stored in the vectors
 * @tparam T 存储在vector中的元素类型
 * @tparam Alloc Allocator type used by the vectors
 * @tparam Alloc vector使用的分配器类型
 * @param lhs First vector to compare
 * @param lhs 要比较的第一个vector
 * @param rhs Second vector to compare
 * @param rhs 要比较的第二个vector
 * @return true if lhs is lexicographically less than or equal to rhs, false otherwise
 * @return 如果lhs在字典序上小于等于rhs则返回true，否则返回false
 * 
 * @details Performs lexicographical comparison of two vectors. Returns true
 * if `lhs` is not greater than `rhs`, i.e., `!(rhs < lhs)`.
 * 
 * @details 对两个vector执行字典序比较。如果`lhs`不大于`rhs`，
 * 即`!(rhs < lhs)`，则返回true。
 * 
 * @note Time complexity: O(min(n, m)) where n and m are the sizes of the vectors
 * @note 时间复杂度：O(min(n, m))，其中n和m是vector的大小
 * 
 * @note This operator is implemented in terms of `operator<`, ensuring
 * consistency between all relational operators.
 * 
 * @note 此操作符基于`operator<`实现，确保所有关系操作符之间的一致性。
 * 
 * @note The comparison uses `operator<` for the element type T.
 * 
 * @note 比较使用元素类型T的`operator<`。
 * 
 * @exception Any exception thrown by the element type's `operator<`
 * @exception 元素类型的`operator<`抛出的任何异常
 * 
 * @see operator< for less-than comparison
 * @see operator< 用于小于比较
 * @see operator>= for greater-than-or-equal comparison
 * @see operator>= 用于大于等于比较
 */
template<typename T, typename Alloc>
bool operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief Greater-than comparison operator for vectors
 * @brief vector的大于比较操作符
 * 
 * @tparam T Type of elements stored in the vectors
 * @tparam T 存储在vector中的元素类型
 * @tparam Alloc Allocator type used by the vectors
 * @tparam Alloc vector使用的分配器类型
 * @param lhs First vector to compare
 * @param lhs 要比较的第一个vector
 * @param rhs Second vector to compare
 * @param rhs 要比较的第二个vector
 * @return true if lhs is lexicographically greater than rhs, false otherwise
 * @return 如果lhs在字典序上大于rhs则返回true，否则返回false
 * 
 * @details Performs lexicographical comparison of two vectors. Returns true
 * if `rhs` is less than `lhs`, i.e., `rhs < lhs`.
 * 
 * @details 对两个vector执行字典序比较。如果`rhs`小于`lhs`，
 * 即`rhs < lhs`，则返回true。
 * 
 * @note Time complexity: O(min(n, m)) where n and m are the sizes of the vectors
 * @note 时间复杂度：O(min(n, m))，其中n和m是vector的大小
 * 
 * @note This operator is implemented in terms of `operator<`, ensuring
 * consistency between all relational operators.
 * 
 * @note 此操作符基于`operator<`实现，确保所有关系操作符之间的一致性。
 * 
 * @note The comparison uses `operator<` for the element type T.
 * 
 * @note 比较使用元素类型T的`operator<`。
 * 
 * @exception Any exception thrown by the element type's `operator<`
 * @exception 元素类型的`operator<`抛出的任何异常
 * 
 * @see operator< for less-than comparison
 * @see operator< 用于小于比较
 * @see operator>= for greater-than-or-equal comparison
 * @see operator>= 用于大于等于比较
 */
template<typename T, typename Alloc>
bool operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return rhs < lhs;
}

/**
 * @brief Greater-than-or-equal comparison operator for vectors
 * @brief vector的大于等于比较操作符
 * 
 * @tparam T Type of elements stored in the vectors
 * @tparam T 存储在vector中的元素类型
 * @tparam Alloc Allocator type used by the vectors
 * @tparam Alloc vector使用的分配器类型
 * @param lhs First vector to compare
 * @param lhs 要比较的第一个vector
 * @param rhs Second vector to compare
 * @param rhs 要比较的第二个vector
 * @return true if lhs is lexicographically greater than or equal to rhs, false otherwise
 * @return 如果lhs在字典序上大于等于rhs则返回true，否则返回false
 * 
 * @details Performs lexicographical comparison of two vectors. Returns true
 * if `lhs` is not less than `rhs`, i.e., `!(lhs < rhs)`.
 * 
 * @details 对两个vector执行字典序比较。如果`lhs`不小于`rhs`，
 * 即`!(lhs < rhs)`，则返回true。
 * 
 * @note Time complexity: O(min(n, m)) where n and m are the sizes of the vectors
 * @note 时间复杂度：O(min(n, m))，其中n和m是vector的大小
 * 
 * @note This operator is implemented in terms of `operator<`, ensuring
 * consistency between all relational operators.
 * 
 * @note 此操作符基于`operator<`实现，确保所有关系操作符之间的一致性。
 * 
 * @note The comparison uses `operator<` for the element type T.
 * 
 * @note 比较使用元素类型T的`operator<`。
 * 
 * @exception Any exception thrown by the element type's `operator<`
 * @exception 元素类型的`operator<`抛出的任何异常
 * 
 * @see operator< for less-than comparison
 * @see operator< 用于小于比较
 * @see operator<= for less-than-or-equal comparison
 * @see operator<= 用于小于等于比较
 */
template<typename T, typename Alloc>
bool operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs < rhs);
}

} // namespace mystl
