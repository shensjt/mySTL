/**
 * @file test_iterator_tags.cpp
 * @brief Test for iterator tag system
 * @brief 迭代器标签系统测试
 * 
 * @details This file contains comprehensive tests for the iterator tag system
 * defined in include/iterator/iterator_tags.hpp. It verifies the correctness
 * of iterator tag hierarchy, inheritance relationships, type traits, and
 * extended functionality.
 * 
 * @details 本文件包含对 include/iterator/iterator_tags.hpp 中定义的迭代器标签系统的
 * 全面测试。它验证了迭代器标签层次结构、继承关系、类型特性和扩展功能的正确性。
 * 
 * @note Tests cover all iterator tag types and their relationships.
 * 
 * @note 测试涵盖所有迭代器标签类型及其关系。
 * 
 * @author shensjt
 * @date 2026-03-17
 * 
 * @see include/iterator/iterator_tags.hpp
 * @see include/utility/type_traits.hpp
 */

#include "../include/iterator/iterator_tags.hpp"
#include "../include/utility/type_traits.hpp"
#include <iostream>
#include <string>

// ==================== Test Helper Functions ====================
// ==================== 测试辅助函数 ====================

/**
 * @brief Global test counter
 * @brief 全局测试计数器
 */
int test_count = 0;

/**
 * @brief Global failure counter
 * @brief 全局失败计数器
 */
int fail_count = 0;

/**
 * @brief Test assertion macro
 * @brief 测试断言宏
 * 
 * @details Checks an expression and updates test statistics.
 * Prints success/failure messages to appropriate output streams.
 * 
 * @details 检查表达式并更新测试统计信息。
 * 将成功/失败消息打印到适当的输出流。
 * 
 * @param expr Expression to check
 * @param expr 要检查的表达式
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

// ==================== Test Iterator Tags ====================
// ==================== 测试迭代器标签 ====================

/**
 * @brief Test iterator tags
 * @brief 测试迭代器标签
 * 
 * @details Tests iterator tag hierarchy, inheritance relationships,
 * and type aliases.
 * 
 * @details 测试迭代器标签层次结构、继承关系和类型别名。
 */
void test_iterator_tags() {
    std::cout << "\n=== Testing iterator tags ===" << std::endl;
    std::cout << "\n=== 测试迭代器标签 ===" << std::endl;
    
    // Test tag inheritance relationships / 测试标签继承关系
    CHECK((mystl::is_base_of_v<mystl::input_iterator_tag, mystl::forward_iterator_tag>));
    CHECK((mystl::is_base_of_v<mystl::forward_iterator_tag, mystl::bidirectional_iterator_tag>));
    CHECK((mystl::is_base_of_v<mystl::bidirectional_iterator_tag, mystl::random_access_iterator_tag>));
    CHECK((mystl::is_base_of_v<mystl::random_access_iterator_tag, mystl::contiguous_iterator_tag>));
    
    // Test output iterator does not participate in inheritance hierarchy / 测试输出迭代器不参与继承层次
    CHECK((!mystl::is_base_of_v<mystl::input_iterator_tag, mystl::output_iterator_tag>));
    CHECK((!mystl::is_base_of_v<mystl::output_iterator_tag, mystl::input_iterator_tag>));
    
    // Test type aliases / 测试类型别名
    CHECK((mystl::is_same_v<mystl::input_iterator, mystl::input_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::random_access_iterator, mystl::random_access_iterator_tag>));
    
    std::cout << "All iterator tags tests passed! / 所有迭代器标签测试通过！" << std::endl;
}

// ==================== Test is_iterator_tag Type Trait ====================
// ==================== 测试 is_iterator_tag 类型特征 ====================

/**
 * @brief Test is_iterator_tag type trait
 * @brief 测试 is_iterator_tag 类型特征
 * 
 * @details Tests identification of iterator tags and non-iterator tag types.
 * 
 * @details 测试迭代器标签的识别和非迭代器标签类型。
 */
void test_is_iterator_tag() {
    std::cout << "\n=== Testing is_iterator_tag type trait ===" << std::endl;
    std::cout << "\n=== 测试 is_iterator_tag 类型特征 ===" << std::endl;
    
    // Test iterator tag identification / 测试迭代器标签识别
    CHECK((mystl::is_iterator_tag_v<mystl::input_iterator_tag>));
    CHECK((mystl::is_iterator_tag_v<mystl::output_iterator_tag>));
    CHECK((mystl::is_iterator_tag_v<mystl::forward_iterator_tag>));
    CHECK((mystl::is_iterator_tag_v<mystl::bidirectional_iterator_tag>));
    CHECK((mystl::is_iterator_tag_v<mystl::random_access_iterator_tag>));
    CHECK((mystl::is_iterator_tag_v<mystl::contiguous_iterator_tag>));
    
    // Test non-iterator tag types / 测试非迭代器标签类型
    CHECK((!mystl::is_iterator_tag_v<int>));
    CHECK((!mystl::is_iterator_tag_v<float>));
    CHECK((!mystl::is_iterator_tag_v<void*>));
    
    std::cout << "All is_iterator_tag tests passed! / 所有 is_iterator_tag 测试通过！" << std::endl;
}

// ==================== Test Extended Features ====================
// ==================== 测试扩展功能 ====================

/**
 * @brief Test extended iterator tag features
 * @brief 测试扩展迭代器标签功能
 * 
 * @details Tests iterator tag conversion relationships, common tag calculation,
 * tag capabilities analysis, tag names, and tag upgrade/downgrade operations.
 * 
 * @details 测试迭代器标签转换关系、公共标签计算、标签能力分析、
 * 标签名称和标签升级/降级操作。
 */
void test_extended_features() {
    std::cout << "\n=== Testing extended features ===" << std::endl;
    std::cout << "\n=== 测试扩展功能 ===" << std::endl;
    
    // Test iterator tag conversion relationships / 测试迭代器标签转换关系
    CHECK((mystl::is_iterator_tag_convertible_v<mystl::random_access_iterator_tag, 
                                               mystl::input_iterator_tag>));
    CHECK((mystl::is_iterator_tag_convertible_v<mystl::bidirectional_iterator_tag, 
                                               mystl::forward_iterator_tag>));
    CHECK((!mystl::is_iterator_tag_convertible_v<mystl::input_iterator_tag, 
                                                mystl::random_access_iterator_tag>));
    CHECK((mystl::is_iterator_tag_convertible_v<mystl::output_iterator_tag, 
                                               mystl::output_iterator_tag>));
    CHECK((!mystl::is_iterator_tag_convertible_v<mystl::input_iterator_tag, 
                                                mystl::output_iterator_tag>));
    
    // Test common tag calculation / 测试公共标签计算
    CHECK((mystl::is_same_v<mystl::common_iterator_tag_t<mystl::random_access_iterator_tag,
                                                      mystl::bidirectional_iterator_tag>,
                         mystl::bidirectional_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::common_iterator_tag_t<mystl::forward_iterator_tag,
                                                      mystl::input_iterator_tag>,
                         mystl::input_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::common_iterator_tag_t<mystl::output_iterator_tag,
                                                      mystl::random_access_iterator_tag>,
                         mystl::output_iterator_tag>));
    
    // Test iterator tag capabilities analysis / 测试迭代器标签能力分析
    using capabilities = mystl::iterator_tag_capabilities<mystl::random_access_iterator_tag>;
    
    CHECK((capabilities::is_input));
    CHECK((capabilities::is_forward));
    CHECK((capabilities::is_bidirectional));
    CHECK((capabilities::is_random_access));
    CHECK((capabilities::can_read));
    CHECK((capabilities::can_jump));
    CHECK((capabilities::has_constant_time_access));
    
    // Test output iterator capabilities / 测试输出迭代器能力
    using output_capabilities = mystl::iterator_tag_capabilities<mystl::output_iterator_tag>;
    
    CHECK((output_capabilities::is_output));
    CHECK((output_capabilities::can_write));
    CHECK((!output_capabilities::can_read));
    
    // Test iterator tag names / 测试迭代器标签名称
    {
        const char* input_name = mystl::iterator_tag_name<mystl::input_iterator_tag>();
        const char* random_name = mystl::iterator_tag_name<mystl::random_access_iterator_tag>();
        
        std::string input_str(input_name);
        std::string random_str(random_name);
        
        CHECK(input_str == "input_iterator");
        CHECK(random_str == "random_access_iterator");
    }
    
    // Test iterator tag upgrade/downgrade / 测试迭代器标签升级/降级
    CHECK((mystl::is_same_v<mystl::upgrade_iterator_tag_t<mystl::forward_iterator_tag,
                                                       mystl::random_access_iterator_tag>,
                         mystl::random_access_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::downgrade_iterator_tag_t<mystl::random_access_iterator_tag,
                                                         mystl::forward_iterator_tag>,
                         mystl::forward_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::upgrade_iterator_tag_t<mystl::forward_iterator_tag,
                                                       mystl::bidirectional_iterator_tag>,
                         mystl::bidirectional_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::upgrade_iterator_tag_t<mystl::input_iterator_tag,
                                                       mystl::forward_iterator_tag>,
                         mystl::forward_iterator_tag>));
    CHECK((mystl::is_same_v<mystl::upgrade_iterator_tag_t<mystl::forward_iterator_tag,
                                                       mystl::input_iterator_tag>,
                         mystl::forward_iterator_tag>));
    
    std::cout << "All extended features tests passed! / 所有扩展功能测试通过！" << std::endl;
}

// ==================== Main Function ====================
// ==================== 主函数 ====================

/**
 * @brief Main test function
 * @brief 主测试函数
 * 
 * @details Runs all test suites and reports results.
 * 
 * @details 运行所有测试套件并报告结果。
 * 
 * @return 0 if all tests pass, 1 otherwise
 * @return 如果所有测试通过则返回 0，否则返回 1
 */
int main() {
    std::cout << "Starting iterator tag system tests..." << std::endl;
    std::cout << "开始测试迭代器标签系统..." << std::endl;
    std::cout << "================================" << std::endl;
    
    test_count = 0;
    fail_count = 0;
    
    try {
        test_iterator_tags();
        test_is_iterator_tag();
        test_extended_features();
        
        std::cout << "\n================================" << std::endl;
        std::cout << "Test statistics: / 测试统计：" << std::endl;
        std::cout << "Total tests: " << test_count << std::endl;
        std::cout << "Failures: " << fail_count << std::endl;
        std::cout << "Passes: " << (test_count - fail_count) << std::endl;
        
        if (fail_count == 0) {
            std::cout << "All iterator tag tests passed! / 所有迭代器标签测试通过！" << std::endl;
            return 0;
        } else {
            std::cout << fail_count << " test(s) failed! / " << fail_count << " 个测试失败！" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Test exception: " << e.what() << std::endl;
        std::cerr << "测试异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown test exception / 未知测试异常" << std::endl;
        return 1;
    }
}
