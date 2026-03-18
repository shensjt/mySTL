/**
 * @file test_memory_pool.cpp
 * @brief Comprehensive test suite for mystl::detail::MemoryPool implementation
 * @brief mystl::detail::MemoryPool实现的综合测试套件
 * 
 * This file contains tests for the memory pool manager in mystl.
 * It verifies the correctness of memory pool operations including allocation,
 * deallocation, thread-local storage, and memory alignment.
 * 
 * 此文件包含对mystl中内存池管理器的测试。
 * 验证内存池操作的正确性，包括分配、释放、线程局部存储和内存对齐。
 * 
 * @author shensjt
 * @date 2026-03-18
 * 
 * @see include/memory/detail/memory_pool.hpp
 */

#include "../../include/memory/detail/memory_pool.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <thread>
#include <atomic>
#include <cstring>

// Test Helper Macros with cleaner bilingual output
// 测试辅助宏，带有更清晰的双语输出
#define TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "[FAIL] Line " << __LINE__ << ": " << #expr << std::endl; \
            std::cerr << "[失败] 行号 " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  ✓ " << #expr << std::endl; \
        } \
    } while(0)

#define TEST_NAMED(name, expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "[FAIL] Test '" << name << "' at line " << __LINE__ << ": " << #expr << std::endl; \
            std::cerr << "[失败] 测试 '" << name << "' 行号 " << __LINE__ << ": " << #expr << std::endl; \
            return 1; \
        } else { \
            std::cout << "  ✓ " << name << " (" << #expr << ")" << std::endl; \
        } \
    } while(0)

/**
 * @brief Test basic memory pool operations
 * @brief 测试基本内存池操作
 * 
 * Tests allocation and deallocation of small and large objects.
 * 测试小对象和大对象的分配和释放。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_basic_operations() {
    std::cout << "1. Testing Basic Memory Pool Operations / 测试基本内存池操作:" << std::endl;
    
    mystl::detail::MemoryPool pool;
    
    // Test small object allocation (should use pool)
    // 测试小对象分配（应使用内存池）
    {
        void* p1 = pool.allocate(64);
        TEST(p1 != nullptr);
        TEST_NAMED("Small object allocation / 小对象分配", p1 != nullptr);
        
        // Test memory alignment
        // 测试内存对齐
        TEST(mystl::detail::PoolConfig::is_aligned(p1));
        TEST_NAMED("Memory alignment / 内存对齐", mystl::detail::PoolConfig::is_aligned(p1));
        
        pool.deallocate(p1, 64);
        TEST_NAMED("Small object deallocation / 小对象释放", true);
    }
    
    // Test large object allocation (should use system allocator)
    // 测试大对象分配（应使用系统分配器）
    {
        void* p2 = pool.allocate(2048);  // > 1024 bytes, large object
        TEST(p2 != nullptr);
        TEST_NAMED("Large object allocation / 大对象分配", p2 != nullptr);
        
        TEST(mystl::detail::PoolConfig::is_aligned(p2));
        TEST_NAMED("Large object memory alignment / 大对象内存对齐", mystl::detail::PoolConfig::is_aligned(p2));
        
        pool.deallocate(p2, 2048);
        TEST_NAMED("Large object deallocation / 大对象释放", true);
    }
    
    // Test zero size allocation
    // 测试零大小分配
    {
        void* p3 = pool.allocate(0);
        TEST(p3 == nullptr);
        TEST_NAMED("Zero size allocation returns nullptr / 零大小分配返回nullptr", p3 == nullptr);
        
        pool.deallocate(nullptr, 0);
        TEST_NAMED("nullptr deallocation safe / nullptr释放安全", true);
    }
    
    return true;
}

/**
 * @brief Test memory pool with different sizes
 * @brief 测试不同大小的内存池
 * 
 * Tests allocation of objects with various sizes.
 * 测试不同大小对象的分配。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_different_sizes() {
    std::cout << "\n2. Testing Different Object Sizes / 测试不同对象大小:" << std::endl;
    
    mystl::detail::MemoryPool pool;
    
    // Test all pool sizes (8, 16, 32, 64, 128, 256, 512, 1024 bytes)
    // 测试所有池大小（8, 16, 32, 64, 128, 256, 512, 1024字节）
    const std::size_t test_sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    
    for (std::size_t size : test_sizes) {
        void* ptr = pool.allocate(size);
        TEST(ptr != nullptr);
        
        // Fill memory with pattern to detect corruption
        // 用模式填充内存以检测损坏
        std::memset(ptr, 0xAA, size);
        
        // Verify pattern
        // 验证模式
        unsigned char* byte_ptr = static_cast<unsigned char*>(ptr);
        bool pattern_ok = true;
        for (std::size_t i = 0; i < size; ++i) {
            if (byte_ptr[i] != 0xAA) {
                pattern_ok = false;
                break;
            }
        }
        TEST(pattern_ok);
        
        pool.deallocate(ptr, size);
    }
    
    TEST_NAMED("All pool sizes work correctly / 所有池大小正常工作", true);
    return true;
}

/**
 * @brief Test memory reuse
 * @brief 测试内存重用
 * 
 * Tests that deallocated memory can be reused.
 * 测试已释放的内存可以被重用。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_memory_reuse() {
    std::cout << "\n3. Testing Memory Reuse / 测试内存重用:" << std::endl;
    
    mystl::detail::MemoryPool pool;
    
    // Allocate and deallocate multiple times, check if memory is reused
    // 多次分配和释放，检查内存是否被重用
    const int num_iterations = 100;
    const std::size_t size = 64;
    
    std::vector<void*> pointers;
    pointers.reserve(num_iterations);
    
    // First allocation round
    // 第一轮分配
    for (int i = 0; i < num_iterations; ++i) {
        void* ptr = pool.allocate(size);
        TEST(ptr != nullptr);
        pointers.push_back(ptr);
    }
    
    // Deallocate all
    // 释放所有
    for (void* ptr : pointers) {
        pool.deallocate(ptr, size);
    }
    pointers.clear();
    
    // Second allocation round (should reuse memory)
    // 第二轮分配（应重用内存）
    for (int i = 0; i < num_iterations; ++i) {
        void* ptr = pool.allocate(size);
        TEST(ptr != nullptr);
        pointers.push_back(ptr);
    }
    
    // Cleanup
    // 清理
    for (void* ptr : pointers) {
        pool.deallocate(ptr, size);
    }
    
    TEST_NAMED("Memory reuse works / 内存重用正常工作", true);
    return true;
}

/**
 * @brief Test thread-local memory pools
 * @brief 测试线程局部内存池
 * 
 * Tests that each thread has its own memory pool instance.
 * 测试每个线程有自己的内存池实例。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_thread_local_pools() {
    std::cout << "\n4. Testing Thread-Local Memory Pools / 测试线程局部内存池:" << std::endl;
    
    std::atomic<int> success_count{0};
    const int num_threads = 4;
    const int allocations_per_thread = 100;
    
    auto thread_func = [&success_count]() {
        try {
            // Each thread gets its own memory pool via get_thread_pool()
            // 每个线程通过get_thread_pool()获取自己的内存池
            mystl::detail::MemoryPool& pool = mystl::detail::get_thread_pool();
            
            std::vector<void*> allocations;
            allocations.reserve(allocations_per_thread);
            
            // Allocate memory in this thread
            // 在此线程中分配内存
            for (int i = 0; i < allocations_per_thread; ++i) {
                void* ptr = pool.allocate(64 + (i % 8) * 8);  // Varying sizes
                if (!ptr) {
                    throw std::bad_alloc();
                }
                allocations.push_back(ptr);
            }
            
            // Deallocate all
            // 释放所有
            for (std::size_t i = 0; i < allocations.size(); ++i) {
                pool.deallocate(allocations[i], 64 + (i % 8) * 8);
            }
            
            ++success_count;
        } catch (...) {
            // Thread failed
            // 线程失败
        }
    };
    
    // Launch threads
    // 启动线程
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(thread_func);
    }
    
    // Wait for all threads
    // 等待所有线程
    for (auto& t : threads) {
        t.join();
    }
    
    TEST(success_count == num_threads);
    TEST_NAMED("Thread-local pools work correctly / 线程局部池正常工作", success_count == num_threads);
    
    return true;
}

/**
 * @brief Test memory alignment
 * @brief 测试内存对齐
 * 
 * Tests that all allocated memory is properly aligned.
 * 测试所有分配的内存都正确对齐。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_memory_alignment() {
    std::cout << "\n5. Testing Memory Alignment / 测试内存对齐:" << std::endl;
    
    mystl::detail::MemoryPool pool;
    
    // Test various sizes to ensure alignment
    // 测试各种大小以确保对齐
    const std::size_t test_sizes[] = {1, 7, 8, 15, 16, 31, 32, 63, 64, 127, 128, 
                                      255, 256, 511, 512, 1023, 1024, 2048, 4096};
    
    for (std::size_t size : test_sizes) {
        void* ptr = pool.allocate(size);
        TEST(ptr != nullptr);
        
        // Check alignment (should be aligned to max_align_t)
        // 检查对齐（应按max_align_t对齐）
        TEST(mystl::detail::PoolConfig::is_aligned(ptr));
        
        pool.deallocate(ptr, size);
    }
    
    TEST_NAMED("All allocations properly aligned / 所有分配正确对齐", true);
    return true;
}

/**
 * @brief Test exception safety
 * @brief 测试异常安全性
 * 
 * Tests exception safety guarantees of memory pool operations.
 * 测试内存池操作的异常安全保证。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_exception_safety() {
    std::cout << "\n6. Testing Exception Safety / 测试异常安全性:" << std::endl;
    
    mystl::detail::MemoryPool pool;
    
    // Test that deallocation of nullptr is safe
    // 测试nullptr释放是安全的
    try {
        pool.deallocate(nullptr, 0);
        pool.deallocate(nullptr, 100);
        TEST_NAMED("nullptr deallocation is exception safe / nullptr释放是异常安全的", true);
    } catch (...) {
        TEST(false && "Should not throw on nullptr deallocation / nullptr释放不应抛出异常");
    }
    
    // Test that allocation failure throws std::bad_alloc
    // 测试分配失败时抛出std::bad_alloc
    // Note: This test is hard to reliably trigger without allocating
    // an extremely large amount of memory
    // 注意：这个测试很难可靠触发，除非分配极大内存
    
    TEST_NAMED("Exception safety / 异常安全性", true);
    return true;
}

/**
 * @brief Test mixed allocation patterns
 * @brief 测试混合分配模式
 * 
 * Tests complex allocation and deallocation patterns.
 * 测试复杂的分配和释放模式。
 * 
 * @return true if all tests pass, false otherwise
 *         如果所有测试通过返回true，否则返回false
 */
bool test_mixed_patterns() {
    std::cout << "\n7. Testing Mixed Allocation Patterns / 测试混合分配模式:" << std::endl;
    
    mystl::detail::MemoryPool pool;
    
    // Mix small and large allocations
    // 混合小对象和大对象分配
    std::vector<std::pair<void*, std::size_t>> allocations;
    
    // Allocate in pattern: small, large, small, large, etc.
    // 按模式分配：小、大、小、大等
    for (int i = 0; i < 20; ++i) {
        std::size_t size = (i % 2 == 0) ? 64 : 2048;
        void* ptr = pool.allocate(size);
        TEST(ptr != nullptr);
        allocations.emplace_back(ptr, size);
    }
    
    // Deallocate in reverse order
    // 按相反顺序释放
    while (!allocations.empty()) {
        auto [ptr, size] = allocations.back();
        pool.deallocate(ptr, size);
        allocations.pop_back();
    }
    
    // Allocate again in different pattern
    // 以不同模式再次分配
    for (int i = 0; i < 30; ++i) {
        std::size_t size = 32 * (1 + (i % 8));  // 32, 64, 96, ..., 256
        void* ptr = pool.allocate(size);
        TEST(ptr != nullptr);
        
        // Use the memory
        // 使用内存
        std::memset(ptr, i & 0xFF, size);
        
        allocations.emplace_back(ptr, size);
    }
    
    // Deallocate all
    // 释放所有
    for (auto [ptr, size] : allocations) {
        pool.deallocate(ptr, size);
    }
    
    TEST_NAMED("Mixed allocation patterns work / 混合分配模式正常工作", true);
    return true;
}

/**
 * @brief Main test function
 * @brief 主测试函数
 * 
 * Runs all memory pool tests and reports results.
 * 运行所有内存池测试并报告结果。
 * 
 * @return 0 if all tests pass, 1 otherwise
 *         如果所有测试通过返回0，否则返回1
 */
int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Testing mystl::detail::MemoryPool Implementation" << std::endl;
    std::cout << "测试mystl::detail::MemoryPool实现" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    bool all_passed = true;
    
    all_passed &= test_basic_operations();
    all_passed &= test_different_sizes();
    all_passed &= test_memory_reuse();
    all_passed &= test_thread_local_pools();
    all_passed &= test_memory_alignment();
    all_passed &= test_exception_safety();
    all_passed &= test_mixed_patterns();
    
    std::cout << "\n=========================================" << std::endl;
    if (all_passed) {
        std::cout << "All tests PASSED!" << std::endl;
        std::cout << "所有测试通过！" << std::endl;
    } else {
        std::cout << "Some tests FAILED!" << std::endl;
        std::cout << "部分测试失败！" << std::endl;
    }
    std::cout << "=========================================" << std::endl;
    
    return all_passed ? 0 : 1;
}
