#include <iostream>
#include "include/mystl/set.hpp"

int main() {
    mystl::set<int> s;
    s.insert(1);
    s.insert(2);
    s.insert(3);
    
    std::cout << "Set size: " << s.size() << std::endl;
    
    // 测试正向迭代
    std::cout << "Forward iteration: ";
    int forward_count = 0;
    for (auto it = s.begin(); it != s.end() && forward_count < 10; ++it) {
        std::cout << *it << " ";
        ++forward_count;
    }
    std::cout << std::endl;
    
    // 测试反向迭代
    std::cout << "Reverse iteration: ";
    int reverse_count = 0;
    for (auto rit = s.rbegin(); rit != s.rend() && reverse_count < 10; ++rit) {
        std::cout << *rit << " ";
        ++reverse_count;
    }
    std::cout << std::endl;
    
    if (reverse_count >= 10) {
        std::cout << "WARNING: Reverse iteration may be infinite!" << std::endl;
    }
    
    // 检查rbegin()和rend()
    std::cout << "\nrbegin() value: ";
    auto rb = s.rbegin();
    if (rb != s.rend()) {
        std::cout << *rb;
    } else {
        std::cout << "rend";
    }
    std::cout << std::endl;
    
    std::cout << "rend() == rbegin(): " << (s.rend() == s.rbegin()) << std::endl;
    
    return 0;
}
