/**
 * @file advanced_demo.cpp
 * @brief 高级功能演示
 * 
 * 展示Integer库在复杂数学计算中的应用，
 * 包括阶乘、斐波那契数列、大数幂运算等
 */

#include <iostream>
#include <chrono>
#include <vector>
#include "../Integer.h"

// 计算阶乘
UnsignedInteger factorial(int n) {
    UnsignedInteger result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// 计算斐波那契数列第n项
UnsignedInteger fibonacci(int n) {
    if (n <= 1) return n;
    
    UnsignedInteger prev = 0, curr = 1;
    for (int i = 2; i <= n; ++i) {
        UnsignedInteger next = prev + curr;
        prev = curr;
        curr = next;
    }
    return curr;
}

// 快速幂运算（模拟，因为库没有内置幂运算）
UnsignedInteger power(const UnsignedInteger& base, int exponent) {
    UnsignedInteger result = 1;
    UnsignedInteger b = base;
    
    while (exponent > 0) {
        if (exponent & 1) {
            result *= b;
        }
        b *= b;
        exponent >>= 1;
    }
    return result;
}

// 计算组合数 C(n, k)
UnsignedInteger combination(int n, int k) {
    if (k > n || k < 0) return 0;
    if (k == 0 || k == n) return 1;
    
    // 使用 C(n,k) = n! / (k! * (n-k)!)
    // 但为了避免大数运算，我们使用递推公式
    UnsignedInteger result = 1;
    for (int i = 0; i < k; ++i) {
        result *= (n - i);
        result /= (i + 1);
    }
    return result;
}

// 性能测试函数
template<typename Func>
void benchmark(const std::string& name, Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << name << ": " << result << std::endl;
    std::cout << "计算时间: " << duration.count() << "ms" << std::endl;
    std::cout << "位数: " << static_cast<std::string>(result).length() << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Integer库高级功能演示 ===" << std::endl;
    
    // 1. 大数阶乘计算
    std::cout << "\n1. 大数阶乘计算:" << std::endl;
    benchmark("100! ", [](){ return factorial(100); });
    benchmark("200! ", [](){ return factorial(200); });
    
    // 2. 斐波那契数列
    std::cout << "\n2. 斐波那契数列:" << std::endl;
    benchmark("Fib(1000)", [](){ return fibonacci(1000); });
    benchmark("Fib(5000)", [](){ return fibonacci(5000); });
    
    // 3. 大数幂运算
    std::cout << "\n3. 大数幂运算:" << std::endl;
    benchmark("2^1000", [](){ return power(2, 1000); });
    benchmark("123^50", [](){ return power(123, 50); });
    
    // 4. 组合数计算
    std::cout << "\n4. 组合数计算:" << std::endl;
    benchmark("C(100, 50)", [](){ return combination(100, 50); });
    benchmark("C(200, 100)", [](){ return combination(200, 100); });
    
    // 5. 大数除法和取模
    std::cout << "\n5. 大数除法和取模:" << std::endl;
    UnsignedInteger large_dividend = factorial(50);
    UnsignedInteger large_divisor = factorial(25);
    
    std::cout << "被除数: 50! = " << large_dividend << std::endl;
    std::cout << "除数: 25! = " << large_divisor << std::endl;
    
    benchmark("50! / 25!", [&](){ return large_dividend / large_divisor; });
    benchmark("50! % 25!", [&](){ return large_dividend % large_divisor; });
    
    // 6. 字符串输入输出性能
    std::cout << "\n6. 字符串转换性能:" << std::endl;
    UnsignedInteger very_large = factorial(500);
    
    benchmark("转换为字符串", [&](){ return static_cast<std::string>(very_large); });
    
    std::string large_str = static_cast<std::string>(very_large);
    benchmark("从字符串构造", [&](){ return UnsignedInteger(large_str); });
    
    // 7. 有符号运算
    std::cout << "\n7. 有符号大数运算:" << std::endl;
    SignedInteger pos_big("123456789012345678901234567890");
    SignedInteger neg_big("-987654321098765432109876543210");
    
    std::cout << "正数: " << pos_big << std::endl;
    std::cout << "负数: " << neg_big << std::endl;
    std::cout << "相加: " << pos_big + neg_big << std::endl;
    std::cout << "相乘: " << pos_big * neg_big << std::endl;
    
    // 8. 内存使用演示
    std::cout << "\n8. 大量大数操作:" << std::endl;
    std::vector<UnsignedInteger> big_numbers;
    big_numbers.reserve(100);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= 100; ++i) {
        big_numbers.push_back(factorial(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "计算1!到100!用时: " << duration.count() << "ms" << std::endl;
    std::cout << "100!的位数: " << static_cast<std::string>(big_numbers.back()).length() << std::endl;
    
    std::cout << "\n=== 演示结束 ===" << std::endl;
    return 0;
}
