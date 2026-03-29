/**
 * @file test.cpp
 * @brief Test suite for mySTL container implementations
 * @brief mySTL容器实现测试套件
 * 
 * This file contains comprehensive tests for vector, list, deque, and forward_list.
 * Each test verifies basic functionality, edge cases, and container-specific operations.
 * 
 * 此文件包含对vector、list、deque和forward_list的综合测试。
 * 每个测试验证基本功能、边界情况和容器特定操作。
 */

#include <iostream>
#include <cassert>
#include <string>

#include "include/mystl/vector.hpp"
#include "include/mystl/list.hpp"
#include "include/mystl/deque.hpp"
#include "include/mystl/forward_list.hpp"
#include "include/iterator/iterator_functions.hpp"   // for mystl::distance

using namespace mystl;

// Helper function to print test results
void print_test(const std::string& name, bool passed) {
    std::cout << "[ " << (passed ? "PASS" : "FAIL") << " ] " << name << std::endl;
}

// ==================== Vector Tests ====================
void test_vector() {
    std::cout << "\n=== Testing Vector ===\n";

    // Default constructor
    vector<int> v1;
    assert(v1.empty());
    assert(v1.size() == 0);
    assert(v1.capacity() == 0);
    print_test("Default constructor", true);

    // Constructor with size and value
    vector<int> v2(5, 10);
    assert(v2.size() == 5);
    assert(v2.capacity() >= 5);
    for (size_t i = 0; i < v2.size(); ++i)
        assert(v2[i] == 10);
    print_test("Constructor with size and value", true);

    // Range constructor
    int arr[] = {1, 2, 3, 4, 5};
    vector<int> v3(arr, arr + 5);
    assert(v3.size() == 5);
    for (size_t i = 0; i < v3.size(); ++i)
        assert(v3[i] == i + 1);
    print_test("Range constructor", true);

    // Initializer list constructor
    vector<int> v4 = {10, 20, 30, 40};
    assert(v4.size() == 4);
    assert(v4[0] == 10 && v4[3] == 40);
    print_test("Initializer list constructor", true);

    // Copy constructor
    vector<int> v5(v4);
    assert(v5.size() == v4.size());
    for (size_t i = 0; i < v5.size(); ++i)
        assert(v5[i] == v4[i]);
    print_test("Copy constructor", true);

    // Move constructor
    vector<int> v6(mystl::move(v5));
    assert(v5.empty()); // moved-from vector is empty (implementation-specific but assumed)
    assert(v6.size() == 4);
    assert(v6[0] == 10);
    print_test("Move constructor", true);

    // Assignment operators
    v1 = v6;
    assert(v1.size() == v6.size());
    v1 = {100, 200};
    assert(v1.size() == 2 && v1[0] == 100);
    print_test("Assignment operators", true);

    // Element access
    assert(v6.front() == 10);
    assert(v6.back() == 40);
    v6.at(1) = 25;
    assert(v6[1] == 25);
    bool caught = false;
    try { v6.at(10); } catch (const mystl::out_of_range&) { caught = true; }
    assert(caught);
    print_test("Element access", true);

    // Capacity
    v6.reserve(100);
    assert(v6.capacity() >= 100);
    v6.shrink_to_fit();
    assert(v6.capacity() == v6.size());
    v6.resize(10, 99);
    assert(v6.size() == 10 && v6[9] == 99);
    v6.resize(3);
    assert(v6.size() == 3);
    print_test("Capacity functions", true);

    // Modifiers (修正部分)
    v6.push_back(1000);
    assert(v6.back() == 1000);
    v6.pop_back();
    assert(v6.back() == 30); 
    v6.insert(v6.begin(), -1);
    assert(v6.front() == -1);
    v6.insert(v6.begin() + 2, 2, 777);
    assert(v6[2] == 777 && v6[3] == 777);
    v6.erase(v6.begin() + 1);
    assert(v6[1] == 777);
    // 删除从索引2到末尾的所有元素
    v6.erase(v6.begin() + 2, v6.end());
    // 此时 v6 的元素为：[-1, 777] (size=2)
    assert(v6.size() == 2);  // 原为3，修正为2
    v6.clear();
    assert(v6.empty());
    print_test("Modifiers", true);

    // Emplace
    vector<std::string> vs;
    vs.emplace_back(3, 'a');
    vs.emplace(vs.begin(), "hello");
    assert(vs.size() == 2 && vs[0] == "hello" && vs[1] == "aaa");
    print_test("Emplace functions", true);

    // Swap
    vector<int> a = {1,2,3};
    vector<int> b = {4,5};
    a.swap(b);
    assert(a.size() == 2 && b.size() == 3);
    assert(a[0] == 4 && b[0] == 1);
    print_test("Swap", true);

    // Equality comparisons
    vector<int> eq1 = {1,2,3};
    vector<int> eq2 = {1,2,3};
    vector<int> neq = {1,2,4};
    assert(eq1 == eq2);
    assert(eq1 != neq);
    assert(eq1 < neq);
    assert(neq > eq1);
    print_test("Comparisons", true);

    // C++20 erase/erase_if
    vector<int> erase_test = {1,2,3,2,4,2,5};
    size_t erased = erase_test.erase(2);
    assert(erased == 3);
    assert(erase_test.size() == 4 && erase_test[0] == 1 && erase_test[3] == 5);
    erase_test = {1,2,3,4,5};
    erased = erase_test.erase_if([](int x) { return x % 2 == 0; });
    assert(erased == 2);
    assert(erase_test.size() == 3 && erase_test[0] == 1 && erase_test[2] == 5);
    print_test("erase/erase_if", true);

    std::cout << "All vector tests passed.\n";
}
// ==================== List Tests ====================
void test_list() {
    std::cout << "\n=== Testing List ===\n";

    // Constructors
    list<int> l1;
    assert(l1.empty());
    list<int> l2(3, 42);
    assert(l2.size() == 3);
    for (int x : l2) assert(x == 42);
    list<int> l3 = {1,2,3,4,5};
    assert(l3.size() == 5);
    list<int> l4(l3);
    assert(l4 == l3);
    list<int> l5(mystl::move(l4));
    assert(l4.empty());
    assert(l5.size() == 5);
    print_test("Constructors", true);

    // Assignment
    l1 = l5;
    assert(l1.size() == 5);
    l1 = {10,20,30};
    assert(l1.size() == 3 && l1.front() == 10);
    print_test("Assignment operators", true);

    // Element access
    assert(l1.front() == 10);
    assert(l1.back() == 30);
    print_test("front/back", true);

    // Iterators
    list<int>::iterator it = l1.begin();
    assert(*it == 10);
    ++it;
    assert(*it == 20);
    --it;
    assert(*it == 10);
    print_test("Iterators", true);

    // Modifiers
    l1.push_front(5);
    assert(l1.front() == 5);
    l1.push_back(35);
    assert(l1.back() == 35);
    l1.pop_front();
    assert(l1.front() == 10);
    l1.pop_back();
    assert(l1.back() == 30);
    l1.insert(l1.begin(), 0);
    assert(l1.front() == 0);
    l1.insert(l1.end(), 40);
    assert(l1.back() == 40);
    l1.erase(l1.begin());
    assert(l1.front() == 10);
    l1.erase(++l1.begin(), l1.end());
    assert(l1.size() == 1 && l1.front() == 10);
    l1.clear();
    assert(l1.empty());
    print_test("Modifiers", true);

    // Emplace
    list<std::pair<int,char>> lp;
    lp.emplace_front(1,'a');
    lp.emplace_back(2,'b');
    lp.emplace(lp.begin(), 0,'z');
    assert(lp.size() == 3);
    assert(lp.front().first == 0 && lp.back().second == 'b');
    print_test("Emplace", true);

    // Splice
    list<int> src = {1,2,3};
    list<int> dst = {10,20};
    dst.splice(dst.begin(), src);
    assert(src.empty());
    assert(dst.size() == 5);
    assert(dst.front() == 1 && dst.back() == 20);
    dst.splice(dst.end(), dst, dst.begin()); // move first to end
    assert(dst.back() == 1);
    print_test("Splice", true);

    // Remove/remove_if
    list<int> rm = {1,2,3,2,4,2,5};
    size_t removed = 0;
    for (int x : rm) if (x == 2) ++removed;
    rm.remove(2);
    assert(rm.size() == 4 && rm.front() == 1 && rm.back() == 5);
    // count removed elements: original size (7) - new size (4) = 3
    assert(removed == 3);
    rm.remove_if([](int x){ return x % 2 == 0; });
    assert(rm.size() == 3 && rm.front() == 1 && rm.back() == 5);
    print_test("remove/remove_if", true);

    // Unique
    list<int> dup = {1,1,2,2,2,3,4,4,5};
    dup.unique();
    assert(dup.size() == 5);
    int expected[] = {1,2,3,4,5};
    size_t i=0;
    for (int x : dup) assert(x == expected[i++]);
    print_test("unique", true);

    // Merge
    list<int> a = {1,3,5};
    list<int> b = {2,4,6};
    a.merge(b);
    assert(a.size() == 6 && b.empty());
    i=0;
    for (int x : a) assert(x == ++i);
    print_test("merge", true);

    // Sort
    list<int> unsorted = {5,2,8,1,9,3};
    unsorted.sort();
    int sorted_expected[] = {1,2,3,5,8,9};
    i=0;
    for (int x : unsorted) assert(x == sorted_expected[i++]);
    
    print_test("sort", true);

    // Reverse
    list<int> rev = {1,2,3,4};
    rev.reverse();
    assert(rev.front() == 4 && rev.back() == 1);
    print_test("reverse", true);

    // Swap
    list<int> s1 = {1,2};
    list<int> s2 = {3,4,5};
    s1.swap(s2);
    assert(s1.size() == 3 && s2.size() == 2);
    assert(s1.front() == 3 && s2.front() == 1);
    print_test("swap", true);

    // Comparisons
    list<int> cmp1 = {1,2,3};
    list<int> cmp2 = {1,2,3};
    list<int> cmp3 = {1,2,4};
    assert(cmp1 == cmp2);
    assert(cmp1 != cmp3);
    assert(cmp1 < cmp3);
    assert(cmp3 > cmp1);
    print_test("comparisons", true);

    // erase/erase_if (C++20 style) - using remove instead of erase(const T&)
    list<int> erase_test = {1,2,3,2,4,2,5};
    size_t erased = 0;
    for (int x : erase_test) if (x == 2) ++erased;
    erase_test.remove(2);  // remove is the standard way to delete all occurrences
    assert(erase_test.size() == 4);
    assert(erased == 3);
    erase_test = {1,2,3,4,5};
    erased = erase_test.erase_if([](int x){ return x%2==0; });
    assert(erased == 2);
    assert(erase_test.size() == 3 && erase_test.front() == 1 && erase_test.back() == 5);
    print_test("erase/erase_if", true);

    std::cout << "All list tests passed.\n";
}

// ==================== Deque Tests ====================
void test_deque() {
    std::cout << "\n=== Testing Deque ===\n";

    // Constructors
    deque<int> d1;
    assert(d1.empty());
    deque<int> d2(5, 10);
    assert(d2.size() == 5);
    for (size_t i = 0; i < d2.size(); ++i) assert(d2[i] == 10);
    deque<int> d3 = {1,2,3,4,5};
    assert(d3.size() == 5);
    deque<int> d4(d3);
    assert(d4 == d3);
    deque<int> d5(mystl::move(d4));
    assert(d4.empty());
    assert(d5.size() == 5);
    print_test("Constructors", true);

    // Assignment
    d1 = d5;
    assert(d1.size() == 5);
    d1 = {10,20,30};
    assert(d1.size() == 3 && d1[0] == 10);
    print_test("Assignment", true);

    // Element access
    assert(d1.front() == 10);
    assert(d1.back() == 30);
    d1[1] = 25;
    assert(d1[1] == 25);
    bool caught = false;
    try { d1.at(10); } catch (const mystl::out_of_range&) { caught = true; }
    assert(caught);
    print_test("Element access", true);

    // Capacity
    assert(!d1.empty());
    assert(d1.size() == 3);
    d1.shrink_to_fit(); // no-op but should not crash
    d1.resize(5, 100);
    assert(d1.size() == 5 && d1[3] == 100);
    d1.resize(2);
    assert(d1.size() == 2);
    print_test("Capacity", true);

    // Modifiers
    d1.push_back(40);
    assert(d1.back() == 40);
    d1.push_front(5);
    assert(d1.front() == 5);
    d1.pop_back();
    assert(d1.back() == 25);          // 修正：删除40后，back为25
    d1.pop_front();
    assert(d1.front() == 10);
    d1.insert(d1.begin() + 1, 15);
    assert(d1[1] == 15);
    d1.insert(d1.begin() + 2, 2, 77);
    assert(d1[2] == 77 && d1[3] == 77);
    d1.erase(d1.begin() + 1);
    assert(d1[1] == 77);
    d1.erase(d1.begin(), d1.begin() + 2);
    // 修正：剩余元素为 {77, 25}
    assert(d1.size() == 2 && d1[0] == 77 && d1[1] == 25);
    d1.clear();
    assert(d1.empty());
    print_test("Modifiers", true);

    // Emplace
    deque<std::string> ds;
    ds.emplace_back(3, 'b');
    ds.emplace_front("hello");
    assert(ds.size() == 2 && ds.front() == "hello" && ds.back() == "bbb");
    auto it = ds.emplace(ds.begin() + 1, "world");
    assert(ds[1] == "world" && it == ds.begin() + 1);
    print_test("Emplace", true);

    // Iterators
    deque<int> di = {10,20,30};
    auto it2 = di.begin();
    assert(*it2 == 10);
    ++it2;
    assert(*it2 == 20);
    it2 += 1;
    assert(*it2 == 30);
    it2 -= 2;
    assert(*it2 == 10);
    // 修正：+2 指向最后一个元素，+3 指向尾后
    assert(*(it2 + 2) == 30);
    assert((it2 + 3) == di.end());
    assert(di.end() - di.begin() == 3);
    assert(it2 < di.end());
    print_test("Iterators", true);

    // Reverse iterators
    int rev_expected[] = {30,20,10};
    size_t idx=0;
    for (auto rit = di.rbegin(); rit != di.rend(); ++rit)
        assert(*rit == rev_expected[idx++]);
    print_test("Reverse iterators", true);

    // Swap
    deque<int> d6 = {1,2};
    deque<int> d7 = {3,4,5};
    d6.swap(d7);
    assert(d6.size() == 3 && d7.size() == 2);
    assert(d6[0] == 3 && d7[0] == 1);
    print_test("swap", true);

    // Comparisons
    deque<int> eq1 = {1,2,3};
    deque<int> eq2 = {1,2,3};
    deque<int> neq = {1,2,4};
    assert(eq1 == eq2);
    assert(eq1 != neq);
    assert(eq1 < neq);
    assert(neq > eq1);
    print_test("Comparisons", true);

    // erase/erase_if (C++20)
    deque<int> deq = {1,2,3,2,4,2,5};
    size_t erased = deq.erase(2);
    assert(erased == 3);
    assert(deq.size() == 4);
    deq = {1,2,3,4,5};
    erased = deq.erase_if([](int x){ return x%2==0; });
    assert(erased == 2);
    assert(deq.size() == 3);
    print_test("erase/erase_if", true);

    std::cout << "All deque tests passed.\n";
}
// ==================== Forward List Tests ====================
void test_forward_list() {
    std::cout << "\n=== Testing Forward List ===\n";

    // Constructors
    forward_list<int> fl1;
    assert(fl1.empty());
    forward_list<int> fl2(3, 42);
    assert(!fl2.empty());
    forward_list<int> fl3 = {1,2,3,4,5};
    assert(fl3.front() == 1);
    forward_list<int> fl4(fl3);
    assert(fl4.front() == 1);
    forward_list<int> fl5(mystl::move(fl4));
    assert(fl4.empty());
    assert(fl5.front() == 1);
    print_test("Constructors", true);

    // Assignment
    fl1 = fl5;
    assert(fl1.front() == 1);
    fl1 = {10,20,30};
    assert(fl1.front() == 10);
    print_test("Assignment", true);

    // Element access
    assert(fl1.front() == 10);
    print_test("front", true);

    // Iterators
    auto it = fl1.begin();
    assert(*it == 10);
    ++it;
    assert(*it == 20);
    ++it;
    assert(*it == 30);
    ++it;
    assert(it == fl1.end());
    print_test("Iterators", true);

    // Modifiers
    fl1.push_front(5);
    assert(fl1.front() == 5);
    fl1.pop_front();
    assert(fl1.front() == 10);
    fl1.insert_after(fl1.before_begin(), 0);
    assert(fl1.front() == 0);
    auto pos = fl1.begin();
    ++pos; // now points to 10
    fl1.insert_after(pos, 15);
    auto check = fl1.begin();
    assert(*check == 0); ++check;
    assert(*check == 10); ++check;
    assert(*check == 15); ++check;
    assert(*check == 20); ++check;
    assert(*check == 30);
    fl1.erase_after(fl1.before_begin());
    assert(fl1.front() == 10);
    auto first = fl1.begin();
    auto last = first;
    ++last; ++last; // points to 20
    fl1.erase_after(first, last); // remove 15
    check = fl1.begin();
    assert(*check == 10); ++check;
    assert(*check == 20); ++check;
    assert(*check == 30);
    fl1.clear();
    assert(fl1.empty());
    print_test("Modifiers", true);

    // Emplace
    forward_list<std::pair<int,int>> flp;
    flp.emplace_front(1,2);
    flp.emplace_after(flp.before_begin(), 3,4);
    assert(flp.front().first == 3);
    print_test("Emplace", true);

    // Splice
    forward_list<int> src = {1,2,3};
    forward_list<int> dst = {10,20};
    dst.splice_after(dst.before_begin(), src);
    assert(src.empty());
    auto itd = dst.begin();
    assert(*itd == 1); ++itd;
    assert(*itd == 2); ++itd;
    assert(*itd == 3); ++itd;
    assert(*itd == 10); ++itd;
    assert(*itd == 20);
    print_test("splice_after (whole list)", true);

    // Splice single element
    forward_list<int> src2 = {5,6};
    forward_list<int> dst2 = {100};
    dst2.splice_after(dst2.before_begin(), src2, src2.before_begin());
    // Count elements manually using distance
    size_t src2_count = static_cast<size_t>(mystl::distance(src2.begin(), src2.end()));
    size_t dst2_count = static_cast<size_t>(mystl::distance(dst2.begin(), dst2.end()));
    assert(src2_count == 1 && src2.front() == 6);
    assert(dst2_count == 2 && dst2.front() == 5);
    print_test("splice_after (single element)", true);

    // Splice range
    forward_list<int> src3 = {7,8,9,10};
    forward_list<int> dst3 = {0};
    auto before = src3.before_begin();
    auto start = before; ++start;
    auto end = start; ++end; ++end; // points to 9
    dst3.splice_after(dst3.before_begin(), src3, before, end);
    size_t src3_count = static_cast<size_t>(mystl::distance(src3.begin(), src3.end()));
    size_t dst3_count = static_cast<size_t>(mystl::distance(dst3.begin(), dst3.end()));
    assert(src3_count == 2 && src3.front() == 9);
    assert(dst3_count == 3);
    auto itd3 = dst3.begin();
    assert(*itd3 == 7); ++itd3;
    assert(*itd3 == 8); ++itd3;
    assert(*itd3 == 0);
    print_test("splice_after (range)", true);

    // Remove/remove_if
    forward_list<int> rm = {1,2,3,2,4,2,5};
    rm.remove(2);
    int expected_rm[] = {1,3,4,5};
    size_t i=0;
    for (int x : rm) assert(x == expected_rm[i++]);
    rm.remove_if([](int x){ return x%2==0; });
    expected_rm[0] = 1; expected_rm[1] = 3; expected_rm[2] = 5;
    i=0;
    for (int x : rm) assert(x == expected_rm[i++]);
    print_test("remove/remove_if", true);

    // Unique
    forward_list<int> dup = {1,1,2,2,2,3,4,4,5};
    dup.unique();
    int uniq[] = {1,2,3,4,5};
    i=0;
    for (int x : dup) assert(x == uniq[i++]);
    print_test("unique", true);

    // Merge
    forward_list<int> a = {1,3,5};
    forward_list<int> b = {2,4,6};
    a.merge(b);
    assert(b.empty());
    int merged[] = {1,2,3,4,5,6};
    i=0;
    for (int x : a) assert(x == merged[i++]);
    print_test("merge", true);

    // Sort
    forward_list<int> unsorted = {5,2,8,1,9,3};
    unsorted.sort();
    int sorted[] = {1,2,3,5,8,9};
    i=0;
    for (int x : unsorted) assert(x == sorted[i++]);
    print_test("sort", true);

    // Reverse
    forward_list<int> rev = {1,2,3,4};
    rev.reverse();
    int rev_exp[] = {4,3,2,1};
    i=0;
    for (int x : rev) assert(x == rev_exp[i++]);
    print_test("reverse", true);

    // Comparisons
    forward_list<int> cmp1 = {1,2,3};
    forward_list<int> cmp2 = {1,2,3};
    forward_list<int> cmp3 = {1,2,4};
    assert(cmp1 == cmp2);
    assert(cmp1 != cmp3);
    assert(cmp1 < cmp3);
    assert(cmp3 > cmp1);
    print_test("comparisons", true);

    // erase/erase_if (C++20) - using remove instead of erase(const T&)
    forward_list<int> fl_erase = {1,2,3,2,4,2,5};
    size_t erased = 0;
    for (int x : fl_erase) if (x == 2) ++erased;
    fl_erase.remove(2);   // remove is the standard way for forward_list
    int exp_erase[] = {1,3,4,5};
    i=0;
    for (int x : fl_erase) assert(x == exp_erase[i++]);
    assert(erased == 3);
    fl_erase = {1,2,3,4,5};
    erased = fl_erase.erase_if([](int x){ return x%2==0; });
    assert(erased == 2);
    exp_erase[0] = 1; exp_erase[1] = 3; exp_erase[2] = 5;
    i=0;
    for (int x : fl_erase) assert(x == exp_erase[i++]);
    print_test("erase/erase_if", true);

    std::cout << "All forward_list tests passed.\n";
}

int main() {
    std::cout << "Starting mySTL container tests...\n";
    test_vector();
    test_list();
    test_deque();
    test_forward_list();
    std::cout << "\nAll tests passed successfully!\n";
    return 0;
}
