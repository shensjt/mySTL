/**
 * @file test_stream_iterators.cpp
 * @brief Comprehensive test suite for mystl stream iterators implementation
 * @brief mystl 流迭代器实现的全面测试套件
 * 
 * This file contains extensive tests for the stream iterators in mystl:
 * - istream_iterator: reads values from an input stream
 * - ostream_iterator: writes values to an output stream
 * 
 * 本文件包含 mystl 中流迭代器的全面测试：
 * - istream_iterator：从输入流读取值
 * - ostream_iterator：向输出流写入值
 * 
 * The tests verify correctness of stream iterator operations including:
 * - Basic construction and usage
 * - Integration with standard algorithms
 * - Edge cases and error handling
 * - Integration between input and output iterators
 * 
 * 测试验证流迭代器操作的正确性，包括：
 * - 基本构造和使用
 * - 与标准算法的集成
 * - 边界情况和错误处理
 * - 输入和输出迭代器之间的集成
 * 
 * @author shensjt
 * @date 2026-03-19
 * 
 * @see istream_iterator.hpp
 * @see ostream_iterator.hpp
 * @see https://en.cppreference.com/w/cpp/iterator/istream_iterator
 * @see https://en.cppreference.com/w/cpp/iterator/ostream_iterator
 */

#include "../../include/iterator/istream_iterator.hpp"
#include "../../include/iterator/ostream_iterator.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

int test_count = 0;      ///< Global test counter / 全局测试计数器
int fail_count = 0;      ///< Global failure counter / 全局失败计数器

/**
 * @brief Test assertion macro
 * @brief 测试断言宏
 * 
 * @param expr Expression to evaluate
 * @param expr 要评估的表达式
 * 
 * @details Checks if the expression is true. If false, logs the failure
 * with line number and expression. Increments test_count and fail_count accordingly.
 * 
 * @details 检查表达式是否为真。如果为假，记录失败信息（包括行号和表达式）。
 * 相应地递增 test_count 和 fail_count。
 */
#define CHECK(expr) \
    do { \
        ++test_count; \
        if (!(expr)) { \
            std::cerr << "Test failed at line " << __LINE__ << ": " << #expr << std::endl; \
            ++fail_count; \
        } else { \
            std::cout << "Test passed: " << #expr << std::endl; \
        } \
    } while(0)

void test_istream_iterator_basic() {
    std::cout << "\n=== Testing Basic istream_iterator ===" << std::endl;
    
    // Test with string stream
    {
        std::istringstream iss("10 20 30 40 50");
        mystl::istream_iterator<int> it(iss);
        mystl::istream_iterator<int> end;
        
        CHECK(*it == 10);
        ++it;
        CHECK(*it == 20);
        ++it;
        CHECK(*it == 30);
        ++it;
        CHECK(*it == 40);
        ++it;
        CHECK(*it == 50);
        ++it;
        CHECK(it == end);
    }
    
    // Test with empty stream
    {
        std::istringstream iss("");
        mystl::istream_iterator<int> it(iss);
        mystl::istream_iterator<int> end;
        
        CHECK(it == end);
    }
    
    // Test with single value
    {
        std::istringstream iss("42");
        mystl::istream_iterator<int> it(iss);
        
        CHECK(*it == 42);
        ++it;
        CHECK(it == mystl::istream_iterator<int>());
    }
    
    // Test copy constructor and assignment
    {
        std::istringstream iss("100 200");
        mystl::istream_iterator<int> it1(iss);
        mystl::istream_iterator<int> it2 = it1;
        
        CHECK(*it1 == 100);
        CHECK(*it2 == 100);
        CHECK(it1 == it2);
        
        ++it1;
        CHECK(*it1 == 200);
        CHECK(*it2 == 100);  // it2 should still point to 100
        
        mystl::istream_iterator<int> it3;
        it3 = it1;
        CHECK(*it3 == 200);
    }
}

void test_istream_iterator_with_algorithms() {
    std::cout << "\n=== Testing istream_iterator with Algorithms ===" << std::endl;
    
    // Test reading into vector
    {
        std::istringstream iss("1 2 3 4 5 6 7 8 9 10");
        std::vector<int> vec;
        
        mystl::istream_iterator<int> it(iss);
        mystl::istream_iterator<int> end;
        
        std::copy(it, end, std::back_inserter(vec));
        
        CHECK(vec.size() == 10);
        for (int i = 0; i < 10; ++i) {
            CHECK(vec[i] == i + 1);
        }
    }
    
    // Test with accumulate
    {
        std::istringstream iss("1.1 2.2 3.3 4.4");
        mystl::istream_iterator<double> it(iss);
        mystl::istream_iterator<double> end;
        
        double sum = 0;
        while (it != end) {
            sum += *it;
            ++it;
        }
        
        CHECK(std::abs(sum - 11.0) < 0.0001);  // 1.1 + 2.2 + 3.3 + 4.4 = 11.0
    }
}

void test_istream_iterator_edge_cases() {
    std::cout << "\n=== Testing istream_iterator Edge Cases ===" << std::endl;
    
    // Test with invalid input
    {
        std::istringstream iss("abc 123");
        mystl::istream_iterator<int> it(iss);
        mystl::istream_iterator<int> end;
        
        // Should fail to read "abc", so iterator should be at end
        CHECK(it == end);
    }
    
    // Test mixed valid and invalid input
    {
        std::istringstream iss("100 abc 200");
        mystl::istream_iterator<int> it(iss);
        
        CHECK(*it == 100);
        ++it;
        // Should fail on "abc", so iterator should be at end
        CHECK(it == mystl::istream_iterator<int>());
    }
    
    // Test with sentinel
    {
        std::istringstream iss("10 20 30");
        mystl::istream_iterator<int> it(iss);
        mystl::istream_iterator_sentinel<int> sentinel;
        
        int count = 0;
        int sum = 0;
        while (it != sentinel) {
            sum += *it;
            ++count;
            ++it;
        }
        
        CHECK(count == 3);
        CHECK(sum == 60);
    }
    
    // Test arrow operator with built-in type
    {
        std::istringstream iss("42");
        mystl::istream_iterator<int> it(iss);
        
        CHECK(*it == 42);
        CHECK(it.operator->() != nullptr);
        CHECK(*(it.operator->()) == 42);
    }
}

void test_ostream_iterator_basic() {
    std::cout << "\n=== Testing Basic ostream_iterator ===" << std::endl;
    
    // Test without delimiter
    {
        std::ostringstream oss;
        mystl::ostream_iterator<int> it(oss);
        
        *it = 10;
        ++it;
        *it = 20;
        ++it;
        *it = 30;
        
        CHECK(oss.str() == "102030");
    }
    
    // Test with delimiter
    {
        std::ostringstream oss;
        mystl::ostream_iterator<int> it(oss, ", ");
        
        *it = 1;
        ++it;
        *it = 2;
        ++it;
        *it = 3;
        
        // Standard ostream_iterator adds delimiter after each value
        CHECK(oss.str() == "1, 2, 3, ");
    }
    
    // Test with empty delimiter
    {
        std::ostringstream oss;
        mystl::ostream_iterator<int> it(oss, "");
        
        *it = 100;
        *it = 200;
        *it = 300;
        
        CHECK(oss.str() == "100200300");
    }
    
    // Test copy constructor
    {
        std::ostringstream oss1, oss2;
        mystl::ostream_iterator<int> it1(oss1, "|");
        mystl::ostream_iterator<int> it2 = it1;
        
        // Both should write to the same stream
        *it1 = 10;
        *it2 = 20;
        
        CHECK(oss1.str() == "10|20|");
    }
}

void test_ostream_iterator_with_algorithms() {
    std::cout << "\n=== Testing ostream_iterator with Algorithms ===" << std::endl;
    
    // Test writing vector to stream
    {
        std::ostringstream oss;
        std::vector<int> vec = {1, 2, 3, 4, 5};
        
        mystl::ostream_iterator<int> it(oss, " ");
        std::copy(vec.begin(), vec.end(), it);
        
        CHECK(oss.str() == "1 2 3 4 5 ");
    }
    
    // Test with transform
    {
        std::ostringstream oss;
        std::vector<int> vec = {1, 2, 3, 4, 5};
        
        mystl::ostream_iterator<int> it(oss, ", ");
        std::transform(vec.begin(), vec.end(), it,
                      [](int x) { return x * 2; });
        
        CHECK(oss.str() == "2, 4, 6, 8, 10, ");
    }
    
    // Test with string values
    {
        std::ostringstream oss;
        std::vector<std::string> vec = {"Hello", "World", "Test"};
        
        mystl::ostream_iterator<std::string> it(oss, "\n");
        std::copy(vec.begin(), vec.end(), it);
        
        CHECK(oss.str() == "Hello\nWorld\nTest\n");
    }
}

void test_ostream_iterator_edge_cases() {
    std::cout << "\n=== Testing ostream_iterator Edge Cases ===" << std::endl;
    
    // Test with null stream
    {
        // Can't really test null stream easily since constructor requires stream
        std::cout << "Note: Null stream test not implemented" << std::endl;
    }
    
    // Test with special characters in delimiter
    {
        std::ostringstream oss;
        mystl::ostream_iterator<int> it(oss, "\t");
        
        *it = 1;
        *it = 2;
        *it = 3;
        
        CHECK(oss.str() == "1\t2\t3\t");
    }
    
    // Test multiple assignments without increment
    {
        std::ostringstream oss;
        mystl::ostream_iterator<int> it(oss, "-");
        
        *it = 10;
        *it = 20;  // Should still work
        *it = 30;
        
        CHECK(oss.str() == "10-20-30-");
    }
}

void test_stream_iterators_integration() {
    std::cout << "\n=== Testing Stream Iterators Integration ===" << std::endl;
    
    // Test read-process-write pipeline
    {
        std::istringstream input("1 2 3 4 5 6 7 8 9 10");
        std::ostringstream output;
        
        mystl::istream_iterator<int> in_it(input);
        mystl::istream_iterator<int> in_end;
        mystl::ostream_iterator<int> out_it(output, " ");
        
        // Filter even numbers and multiply by 10
        while (in_it != in_end) {
            int value = *in_it;
            if (value % 2 == 0) {
                *out_it = value * 10;
            }
            ++in_it;
        }
        
        CHECK(output.str() == "20 40 60 80 100 ");
    }
    
    // Test with string streams
    {
        std::string input_data = "apple banana cherry date";
        std::istringstream input(input_data);
        std::ostringstream output;
        
        mystl::istream_iterator<std::string> in_it(input);
        mystl::istream_iterator<std::string> in_end;
        mystl::ostream_iterator<std::string> out_it(output, "|");
        
        std::copy(in_it, in_end, out_it);
        
        CHECK(output.str() == "apple|banana|cherry|date|");
    }
}

/**
 * @brief Main test runner
 * @brief 主测试运行器
 * 
 * @return 0 if all tests pass, 1 if any test fails
 * @return 如果所有测试通过则返回 0，如果有任何测试失败则返回 1
 * 
 * @details Runs all test suites for stream iterators and reports statistics.
 * Catches and reports any exceptions that occur during testing.
 * 
 * @details 运行流迭代器的所有测试套件并报告统计信息。
 * 捕获并报告测试期间发生的任何异常。
 */
int main() {
    std::cout << "Starting stream_iterator tests..." << std::endl;
    std::cout << "==================================" << std::endl;
    
    test_count = 0;
    fail_count = 0;
    
    try {
        test_istream_iterator_basic();
        test_istream_iterator_with_algorithms();
        test_istream_iterator_edge_cases();
        test_ostream_iterator_basic();
        test_ostream_iterator_with_algorithms();
        test_ostream_iterator_edge_cases();
        test_stream_iterators_integration();
        
        std::cout << "\n==================================" << std::endl;
        std::cout << "Test statistics:" << std::endl;
        std::cout << "Total tests: " << test_count << std::endl;
        std::cout << "Failures: " << fail_count << std::endl;
        std::cout << "Passes: " << (test_count - fail_count) << std::endl;
        
        if (fail_count == 0) {
            std::cout << "All stream_iterator tests passed!" << std::endl;
            return 0;
        } else {
            std::cout << fail_count << " test(s) failed!" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Test exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown test exception" << std::endl;
        return 1;
    }
}
