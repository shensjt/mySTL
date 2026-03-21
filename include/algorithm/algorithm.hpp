#pragma once

/**
 * @file algorithm.hpp
 * @brief Basic algorithm implementations for mySTL
 * 
 * Provides essential algorithms like max, min, copy, fill, etc.
 * These are simplified implementations for mySTL containers.
 */

#include <cstddef>
#include "../iterator/iterator_traits.hpp"
#include "../iterator/iterator_functions.hpp"
#include "../utility/type_traits.hpp"
#include "../utility/move.hpp"
#include "../utility/swap.hpp"

namespace mystl {

// 简单异常类 - 用于容器边界检查
class out_of_range {
private:
    const char* msg_;
public:
    explicit out_of_range(const char* msg) : msg_(msg) {}
    const char* what() const noexcept { return msg_; }
};

// max函数 - 返回两个值中的较大者
template<typename T>
constexpr const T& max(const T& a, const T& b) {
    return (a < b) ? b : a;
}

// min函数 - 返回两个值中的较小者
template<typename T>
constexpr const T& min(const T& a, const T& b) {
    return (a < b) ? a : b;
}

// equal算法 - 比较两个范围是否相等
template<typename InputIt1, typename InputIt2>
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return true;
}

// lexicographical_compare算法 - 字典序比较
template<typename InputIt1, typename InputIt2>
bool lexicographical_compare(InputIt1 first1, InputIt1 last1,
                             InputIt2 first2, InputIt2 last2) {
    for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
        if (*first1 < *first2) return true;
        if (*first2 < *first1) return false;
    }
    return (first1 == last1) && (first2 != last2);
}

// copy算法 - 复制范围
template<typename InputIt, typename OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
    for (; first != last; ++first, ++d_first) {
        *d_first = *first;
    }
    return d_first;
}

// copy_backward算法 - 向后复制范围
template<typename BidirIt1, typename BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last) {
    while (first != last) {
        *(--d_last) = *(--last);
    }
    return d_last;
}

// fill算法 - 用值填充范围
template<typename ForwardIt, typename T>
void fill(ForwardIt first, ForwardIt last, const T& value) {
    for (; first != last; ++first) {
        *first = value;
    }
}

// fill_n算法 - 用值填充前n个元素
template<typename OutputIt, typename Size, typename T>
OutputIt fill_n(OutputIt first, Size count, const T& value) {
    for (Size i = 0; i < count; ++i, ++first) {
        *first = value;
    }
    return first;
}

// move算法 - 移动范围
template<typename InputIt, typename OutputIt>
OutputIt move(InputIt first, InputIt last, OutputIt d_first) {
    for (; first != last; ++first, ++d_first) {
        *d_first = mystl::move(*first);
    }
    return d_first;
}

// move_backward算法 - 向后移动范围
template<typename BidirIt1, typename BidirIt2>
BidirIt2 move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last) {
    while (first != last) {
        *(--d_last) = mystl::move(*(--last));
    }
    return d_last;
}

// swap_ranges算法 - 交换两个范围
template<typename ForwardIt1, typename ForwardIt2>
ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        mystl::swap(*first1, *first2);
    }
    return first2;
}

// remove算法 - 移除等于指定值的元素
template<typename ForwardIt, typename T>
ForwardIt remove(ForwardIt first, ForwardIt last, const T& value) {
    ForwardIt result = first;
    for (; first != last; ++first) {
        if (!(*first == value)) {
            if (result != first) {
                *result = mystl::move(*first);
            }
            ++result;
        }
    }
    return result;
}

// remove_if算法 - 移除满足条件的元素
template<typename ForwardIt, typename UnaryPredicate>
ForwardIt remove_if(ForwardIt first, ForwardIt last, UnaryPredicate p) {
    ForwardIt result = first;
    for (; first != last; ++first) {
        if (!p(*first)) {
            if (result != first) {
                *result = mystl::move(*first);
            }
            ++result;
        }
    }
    return result;
}

} // namespace mystl
