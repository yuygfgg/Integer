/**
 * @file basic_usage.cpp
 * @brief 基本用法示例
 * 
 * 演示Integer库的基本功能，包括构造、运算和类型转换
 */

#include <iostream>
#include <string>
#include "../Integer.h"

int main() {
    std::cout << "=== Integer库基本用法示例 ===" << std::endl;
    
    // 1. 创建无符号大整数
    std::cout << "\n1. 创建无符号大整数:" << std::endl;
    UnsignedInteger a = 123456789;
    UnsignedInteger b("987654321123456789012345");
    UnsignedInteger c = "42"_UI;  // 使用字面量
    
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "c = " << c << std::endl;
    
    // 2. 基本算术运算
    std::cout << "\n2. 基本算术运算:" << std::endl;
    auto sum = a + b;
    auto difference = b - a;
    auto product = a * c;
    auto quotient = b / a;
    auto remainder = b % a;
    
    std::cout << "a + b = " << sum << std::endl;
    std::cout << "b - a = " << difference << std::endl;
    std::cout << "a * c = " << product << std::endl;
    std::cout << "b / a = " << quotient << std::endl;
    std::cout << "b % a = " << remainder << std::endl;
    
    // 3. 比较运算
    std::cout << "\n3. 比较运算:" << std::endl;
    std::cout << "a < b: " << std::boolalpha << (a < b) << std::endl;
    std::cout << "a == c: " << (a == c) << std::endl;
    std::cout << "b > a: " << (b > a) << std::endl;
    
    // 4. 自增自减
    std::cout << "\n4. 自增自减:" << std::endl;
    UnsignedInteger d = 100;
    std::cout << "d = " << d << std::endl;
    std::cout << "++d = " << ++d << std::endl;
    std::cout << "d++ = " << d++ << std::endl;
    std::cout << "d = " << d << std::endl;
    std::cout << "--d = " << --d << std::endl;
    
    // 5. 有符号大整数
    std::cout << "\n5. 有符号大整数:" << std::endl;
    SignedInteger signed_a = -123456789;
    SignedInteger signed_b("987654321");
    SignedInteger signed_c("-42");
    
    std::cout << "signed_a = " << signed_a << std::endl;
    std::cout << "signed_b = " << signed_b << std::endl;
    std::cout << "signed_c = " << signed_c << std::endl;
    
    auto signed_sum = signed_a + signed_b;
    auto signed_product = signed_a * signed_c;
    
    std::cout << "signed_a + signed_b = " << signed_sum << std::endl;
    std::cout << "signed_a * signed_c = " << signed_product << std::endl;
    
    // 6. 类型转换
    std::cout << "\n6. 类型转换:" << std::endl;
    UnsignedInteger big_num("123456789012345");
    
    // 转换为标准类型（注意：大数可能会溢出）
    try {
        long long as_long = static_cast<long long>(UnsignedInteger(123456));
        double as_double = static_cast<double>(big_num);
        std::string as_string = static_cast<std::string>(big_num);
        
        std::cout << "作为 long long: " << as_long << std::endl;
        std::cout << "作为 double: " << as_double << std::endl;
        std::cout << "作为 string: " << as_string << std::endl;
    } catch (const std::exception& e) {
        std::cout << "转换错误: " << e.what() << std::endl;
    }
    
    // 7. 复合赋值运算
    std::cout << "\n7. 复合赋值运算:" << std::endl;
    UnsignedInteger e = 1000;
    std::cout << "e = " << e << std::endl;
    
    e += 500;
    std::cout << "e += 500: " << e << std::endl;
    
    e *= 2;
    std::cout << "e *= 2: " << e << std::endl;
    
    e /= 3;
    std::cout << "e /= 3: " << e << std::endl;
    
    std::cout << "\n=== 示例结束 ===" << std::endl;
    return 0;
}
