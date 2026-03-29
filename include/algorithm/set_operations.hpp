/**
 * @file set_operations.hpp
 * @brief Set operation algorithms for mySTL
 * @brief mySTL集合操作算法
 * 
 * This file implements set operations like union, intersection, difference, and symmetric difference.
 * These algorithms work on sorted ranges and produce sorted results.
 * 
 * 本文件实现集合操作，如并集、交集、差集和对称差集。
 * 这些算法在已排序的范围上工作，并产生已排序的结果。
 * 
 * @author shensjt
 * @date 2026-03-29
 */

#pragma once

#include "../iterator/iterator_traits.hpp"
#include "../utility/less.hpp"

namespace mystl {

/**
 * @brief Compute union of two sorted ranges
 * @brief 计算两个已排序范围的并集
 * 
 * Constructs a sorted range beginning at d_first consisting of all elements
 * present in one or both sorted ranges [first1, last1) and [first2, last2).
 * 
 * 构造一个从d_first开始的已排序范围，包含在已排序范围[first1, last1)和[first2, last2)
 * 中的一个或两个中出现的所有元素。
 * 
 * @tparam InputIt1 First input iterator type
 * @tparam InputIt1 第一个输入迭代器类型
 * @tparam InputIt2 Second input iterator type
 * @tparam InputIt2 第二个输入迭代器类型
 * @tparam OutputIt Output iterator type
 * @tparam OutputIt 输出迭代器类型
 * @tparam Compare Comparison function type
 * @tparam Compare 比较函数类型
 * @param first1 Iterator to the first element of the first range
 * @param first1 指向第一个范围第一个元素的迭代器
 * @param last1 Iterator to one past the last element of the first range
 * @param last1 指向第一个范围最后一个元素之后位置的迭代器
 * @param first2 Iterator to the first element of the second range
 * @param first2 指向第二个范围第一个元素的迭代器
 * @param last2 Iterator to one past the last element of the second range
 * @param last2 指向第二个范围最后一个元素之后位置的迭代器
 * @param d_first Output iterator to the beginning of the destination range
 * @param d_first 指向目标范围开始位置的输出迭代器
 * @param comp Comparison function object
 * @param comp 比较函数对象
 * @return OutputIt Iterator to one past the last element of the constructed range
 * @return OutputIt 指向构造范围最后一个元素之后位置的迭代器
 */
template<typename InputIt1, typename InputIt2, typename OutputIt,
         typename Compare = less<typename iterator_traits<InputIt1>::value_type>>
OutputIt set_union(InputIt1 first1, InputIt1 last1,
                   InputIt2 first2, InputIt2 last2,
                   OutputIt d_first, Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *d_first++ = *first1++;
        } else if (comp(*first2, *first1)) {
            *d_first++ = *first2++;
        } else {
            // Elements are equal, take one from first range
            *d_first++ = *first1++;
            ++first2;
        }
    }
    
    // Copy remaining elements from first range
    while (first1 != last1) {
        *d_first++ = *first1++;
    }
    
    // Copy remaining elements from second range
    while (first2 != last2) {
        *d_first++ = *first2++;
    }
    
    return d_first;
}

/**
 * @brief Compute intersection of two sorted ranges
 * @brief 计算两个已排序范围的交集
 * 
 * Constructs a sorted range beginning at d_first consisting of all elements
 * that are present in both sorted ranges [first1, last1) and [first2, last2).
 * 
 * 构造一个从d_first开始的已排序范围，包含在已排序范围[first1, last1)和[first2, last2)
 * 中都出现的所有元素。
 * 
 * @tparam InputIt1 First input iterator type
 * @tparam InputIt1 第一个输入迭代器类型
 * @tparam InputIt2 Second input iterator type
 * @tparam InputIt2 第二个输入迭代器类型
 * @tparam OutputIt Output iterator type
 * @tparam OutputIt 输出迭代器类型
 * @tparam Compare Comparison function type
 * @tparam Compare 比较函数类型
 * @param first1 Iterator to the first element of the first range
 * @param first1 指向第一个范围第一个元素的迭代器
 * @param last1 Iterator to one past the last element of the first range
 * @param last1 指向第一个范围最后一个元素之后位置的迭代器
 * @param first2 Iterator to the first element of the second range
 * @param first2 指向第二个范围第一个元素的迭代器
 * @param last2 Iterator to one past the last element of the second range
 * @param last2 指向第二个范围最后一个元素之后位置的迭代器
 * @param d_first Output iterator to the beginning of the destination range
 * @param d_first 指向目标范围开始位置的输出迭代器
 * @param comp Comparison function object
 * @param comp 比较函数对象
 * @return OutputIt Iterator to one past the last element of the constructed range
 * @return OutputIt 指向构造范围最后一个元素之后位置的迭代器
 */
template<typename InputIt1, typename InputIt2, typename OutputIt,
         typename Compare = less<typename iterator_traits<InputIt1>::value_type>>
OutputIt set_intersection(InputIt1 first1, InputIt1 last1,
                          InputIt2 first2, InputIt2 last2,
                          OutputIt d_first, Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            ++first1;
        } else if (comp(*first2, *first1)) {
            ++first2;
        } else {
            // Elements are equal, copy to output
            *d_first++ = *first1++;
            ++first2;
        }
    }
    
    return d_first;
}

/**
 * @brief Compute difference of two sorted ranges
 * @brief 计算两个已排序范围的差集
 * 
 * Constructs a sorted range beginning at d_first consisting of all elements
 * that are present in the first range [first1, last1) but not in the second
 * range [first2, last2).
 * 
 * 构造一个从d_first开始的已排序范围，包含在第一个范围[first1, last1)中出现但不在
 * 第二个范围[first2, last2)中出现的所有元素。
 * 
 * @tparam InputIt1 First input iterator type
 * @tparam InputIt1 第一个输入迭代器类型
 * @tparam InputIt2 Second input iterator type
 * @tparam InputIt2 第二个输入迭代器类型
 * @tparam OutputIt Output iterator type
 * @tparam OutputIt 输出迭代器类型
 * @tparam Compare Comparison function type
 * @tparam Compare 比较函数类型
 * @param first1 Iterator to the first element of the first range
 * @param first1 指向第一个范围第一个元素的迭代器
 * @param last1 Iterator to one past the last element of the first range
 * @param last1 指向第一个范围最后一个元素之后位置的迭代器
 * @param first2 Iterator to the first element of the second range
 * @param first2 指向第二个范围第一个元素的迭代器
 * @param last2 Iterator to one past the last element of the second range
 * @param last2 指向第二个范围最后一个元素之后位置的迭代器
 * @param d_first Output iterator to the beginning of the destination range
 * @param d_first 指向目标范围开始位置的输出迭代器
 * @param comp Comparison function object
 * @param comp 比较函数对象
 * @return OutputIt Iterator to one past the last element of the constructed range
 * @return OutputIt 指向构造范围最后一个元素之后位置的迭代器
 */
template<typename InputIt1, typename InputIt2, typename OutputIt,
         typename Compare = less<typename iterator_traits<InputIt1>::value_type>>
OutputIt set_difference(InputIt1 first1, InputIt1 last1,
                        InputIt2 first2, InputIt2 last2,
                        OutputIt d_first, Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *d_first++ = *first1++;
        } else if (comp(*first2, *first1)) {
            ++first2;
        } else {
            // Elements are equal, skip both
            ++first1;
            ++first2;
        }
    }
    
    // Copy remaining elements from first range
    while (first1 != last1) {
        *d_first++ = *first1++;
    }
    
    return d_first;
}

/**
 * @brief Compute symmetric difference of two sorted ranges
 * @brief 计算两个已排序范围的对称差集
 * 
 * Constructs a sorted range beginning at d_first consisting of all elements
 * that are present in either of the sorted ranges [first1, last1) and
 * [first2, last2), but not in both.
 * 
 * 构造一个从d_first开始的已排序范围，包含在已排序范围[first1, last1)和[first2, last2)
 * 中的一个中出现但不在两个中都出现的所有元素。
 * 
 * @tparam InputIt1 First input iterator type
 * @tparam InputIt1 第一个输入迭代器类型
 * @tparam InputIt2 Second input iterator type
 * @tparam InputIt2 第二个输入迭代器类型
 * @tparam OutputIt Output iterator type
 * @tparam OutputIt 输出迭代器类型
 * @tparam Compare Comparison function type
 * @tparam Compare 比较函数类型
 * @param first1 Iterator to the first element of the first range
 * @param first1 指向第一个范围第一个元素的迭代器
 * @param last1 Iterator to one past the last element of the first range
 * @param last1 指向第一个范围最后一个元素之后位置的迭代器
 * @param first2 Iterator to the first element of the second range
 * @param first2 指向第二个范围第一个元素的迭代器
 * @param last2 Iterator to one past the last element of the second range
 * @param last2 指向第二个范围最后一个元素之后位置的迭代器
 * @param d_first Output iterator to the beginning of the destination range
 * @param d_first 指向目标范围开始位置的输出迭代器
 * @param comp Comparison function object
 * @param comp 比较函数对象
 * @return OutputIt Iterator to one past the last element of the constructed range
 * @return OutputIt 指向构造范围最后一个元素之后位置的迭代器
 */
template<typename InputIt1, typename InputIt2, typename OutputIt,
         typename Compare = less<typename iterator_traits<InputIt1>::value_type>>
OutputIt set_symmetric_difference(InputIt1 first1, InputIt1 last1,
                                  InputIt2 first2, InputIt2 last2,
                                  OutputIt d_first, Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *d_first++ = *first1++;
        } else if (comp(*first2, *first1)) {
            *d_first++ = *first2++;
        } else {
            // Elements are equal, skip both
            ++first1;
            ++first2;
        }
    }
    
    // Copy remaining elements from first range
    while (first1 != last1) {
        *d_first++ = *first1++;
    }
    
    // Copy remaining elements from second range
    while (first2 != last2) {
        *d_first++ = *first2++;
    }
    
    return d_first;
}

/**
 * @brief Check if one sorted range is a subset of another
 * @brief 检查一个已排序范围是否是另一个的子集
 * 
 * Returns true if every element from the first sorted range [first1, last1)
 * is found within the second sorted range [first2, last2).
 * 
 * 如果第一个已排序范围[first1, last1)中的每个元素都在第二个已排序范围[first2, last2)中
 * 找到，则返回true。
 * 
 * @tparam InputIt1 First input iterator type
 * @tparam InputIt1 第一个输入迭代器类型
 * @tparam InputIt2 Second input iterator type
 * @tparam InputIt2 第二个输入迭代器类型
 * @tparam Compare Comparison function type
 * @tparam Compare 比较函数类型
 * @param first1 Iterator to the first element of the first range
 * @param first1 指向第一个范围第一个元素的迭代器
 * @param last1 Iterator to one past the last element of the first range
 * @param last1 指向第一个范围最后一个元素之后位置的迭代器
 * @param first2 Iterator to the first element of the second range
 * @param first2 指向第二个范围第一个元素的迭代器
 * @param last2 Iterator to one past the last element of the second range
 * @param last2 指向第二个范围最后一个元素之后位置的迭代器
 * @param comp Comparison function object
 * @param comp 比较函数对象
 * @return true if [first1, last1) is a subset of [first2, last2), false otherwise
 * @return 如果[first1, last1)是[first2, last2)的子集返回true，否则返回false
 */
template<typename InputIt1, typename InputIt2,
         typename Compare = less<typename iterator_traits<InputIt1>::value_type>>
bool includes(InputIt1 first1, InputIt1 last1,
              InputIt2 first2, InputIt2 last2,
              Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1)) {
            // Element in second range not found in first range
            return false;
        } else if (comp(*first1, *first2)) {
            ++first1;
        } else {
            // Elements are equal
            ++first1;
            ++first2;
        }
    }
    
    // If we've exhausted second range, it's a subset
    return first2 == last2;
}

} // namespace mystl
