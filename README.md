# Integer - 高性能C++任意精度整数库

- [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
- [![C++](https://img.shields.io/badge/C%2B%2B-11%2B-blue.svg)](https://en.cppreference.com/)
- [![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)]()
- [![Compiler](https://img.shields.io/badge/Compiler-GCC-green.svg)](https://gcc.gnu.org/)

> **目前最高效的十进制高精度整数库** - 在 Library Checker 上打破多项性能记录的 Header-Only C++ 库

一个现代化、高性能的 C++ 任意精度整数算术库，专为追求极致性能而设计。支持无符号和有符号大整数的完整运算，并在权威测试平台上刷新性能记录。

# 目录

- [Integer - 高性能C++任意精度整数库](#integer---高性能c任意精度整数库)
- [目录](#目录)
- [特色功能](#特色功能)
- [快速开始](#快速开始)
- [使用方法](#使用方法)
- [效率展示](#效率展示)
- [完整功能](#完整功能)
  - [`UnsignedInteger`](#unsignedinteger)
  - [`SignedInteger`](#signedinteger)
- [项目维护](#项目维护)
  - [许可证](#许可证)
  - [贡献指南](#贡献指南)
    - [报告问题](#报告问题)
    - [提交代码](#提交代码)
    - [代码规范](#代码规范)
  - [版本历史](#版本历史)
    - [V1 (2025-8-24)](#v1-2025-8-24)
  - [问题反馈](#问题反馈)
    - [报告 Bug](#报告-bug)
    - [功能请求](#功能请求)
    - [性能问题](#性能问题)
  - [社区支持](#社区支持)
  - [常见问题](#常见问题)

# 特色功能

- **使用方法简单**：这是一个 Header-Only 的库，你只需要包含头文件即可使用。
- **效率极其优秀**：在发布时（2025-9-2），本模板是 [Library Checker](https://judge.yosupo.jp/) 中所有十进制高精度整数模板的最优解。
- **覆盖功能多样**：本模板实现了几乎所有会用到的类型转换和输入输出（兼容 `iostream`）以及全体基础运算（加减乘除模以及比较）。
- **支持动态长度**：和部分高效率高精度模板不同，本模板的效率并不依赖静态内存。

# 快速开始

详细的示例可以参考[基础使用示例](./basic_usage.cpp)和[进阶使用示例](./advanced_demo.cpp)。

```cpp
#include "Integer.h"

int main() {
    // 创建大整数
    UnsignedInteger a = "123456789012345678901234567890"_UI;
    UnsignedInteger b = "987654321098765432109876543210"_UI;
    
    // 基本运算
    std::cout << "a + b = " << a + b << std::endl;
    std::cout << "a * b = " << a * b << std::endl;
    
    // 有符号运算
    SignedInteger x = -"123456789"_SI;
    SignedInteger y = "987654321"_SI;
    std::cout << "x + y = " << x + y << std::endl;
    
    return 0;
}
```

# 使用方法

本模板库是一个 Header-Only 的库，使用方法较为简单，遵循如下步骤即可：

1. 下载或复制 `Integer.h` 的内容到本地。
2. 检查编译器配置：
  1. 建议在编译参数中启用 `-march=native`，以自动使用所在平台可用的 SIMD 指令集（x86_64 上为 AVX2，AArch64 上为 NEON）获得最佳性能；但这不是必须条件，未启用时库会自动选择可用实现（NEON 或纯标量）。同时确保 C++ 版本在 C++11 及以上。
  2. 使用 GCC 编译器。
3. 在需要该库的头文件 `#include "Integer.h"` 即可。

# 效率展示

本模板效率极其优秀。截至目前：

- 加法：对两个长度为 $2\cdot10^6$ 的高精度整数进行输入、加法、输出用时仅 $29\text{ ms}$，是 [Library Checker 最优解](https://judge.yosupo.jp/submission/309899)。
- 乘法：对两个长度为 $2\cdot10^6$ 的高精度整数进行输入、乘法、输出用时仅 $37\text{ ms}$，是 [Library Checker 最优解](https://judge.yosupo.jp/submission/309889)。
- 除法：对两个长度为 $2\cdot10^6$ 的高精度整数进行输入、除法、取模、输出用时仅 $143\text{ ms}$，是 [Library Checker 最优解](https://judge.yosupo.jp/submission/309781)。

由此看来本模板的效率相当优秀，确实配得上“目前最高效”的称号。尤其是除法取模的效率非常惊人，比第二名快了大约 $34\%$！

# 完整功能

`namespace detail` 中的内容均为辅助类，除非你知道你在做什么，否则不要动它们。它们的功能不做介绍。

本模板主要实现了两个类：`UnsignedInteger` 和 `SignedInteger`。下面是它们的功能表格，先说一些约定：

- $x$ 为 `*this` 所代表的值。
- $y$ 为 `other` 所代表的值。
- $v$ 为 `value` 所代表的值。
- $n$ 为 `*this` 的长度，也就是 $\lceil\lg|x|\rceil$。
- $m$ 为 `other` 的长度，也就是 $\lceil\lg|y|\rceil$。
- $L$ 为快速傅里叶变换长度上限，此处为 $4194304$。
- $T$ 为算法切换阈值，此处为 $64$。

合法检查仅当宏 `ENABLE_VALIDITY_CHECK` 被定义时执行。

## `UnsignedInteger`

| 函数签名 | 功能概述 | 合法检查 | 时间复杂度 | 备注 |
|:-:|:-:|:-:|:-:|:-:|
| `UnsignedInteger()` | $x\leftarrow0$ | 无 | $O(1)$ | 默认构造函数 |
| `UnsignedInteger(const UnsignedInteger& other)` | $x\leftarrow y$ | 无 | $O(m)$ | 复制构造函数 |
| `UnsignedInteger(UnsignedInteger&& other) noexcept` | $x\leftarrow y,y\leftarrow0$ | 无 | $O(1)$ | 移动构造函数 |
| `UnsignedInteger(const SignedInteger& other)` | $x\leftarrow y$ | $y\ge0$ | $O(m)$ | 无 |
| `UnsignedInteger(unsignedIntegral value)` | $x\leftarrow v$ | 无 | $O(\log v)$ | 对全体无符号整数启用 |
| `UnsignedInteger(signedIntegral value)` | $x\leftarrow v$ | $v\ge0$ | $O(\log v)$ | 对全体有符号整数启用 |
| `UnsignedInteger(floatingPoint value)` | $x\leftarrow\lfloor v\rfloor$ | $v\ge0$ | $O(\log v)$ | 对全体浮点数启用 |
| `UnsignedInteger(const char* value)` | $x\leftarrow v$ | $v$ 不是 `nullptr`，$v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 无 |
| `UnsignedInteger(const std::string& value)` | $x\leftarrow v$ | $v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 无 |
| `~UnsignedInteger() noexcept` | 解分配内存 | 无 | $O(1)$ | 析构函数 |
| `UnsignedInteger& operator=(const UnsignedInteger& other)` | $x\leftarrow y$ | 无 | $O(m)$ | 复制赋值运算符 |
| `UnsignedInteger& operator=(UnsignedInteger&& other) noexcept` | $x\leftarrow y,y\leftarrow0$ | 无 | $O(1)$ | 移动赋值运算符 |
| `UnsignedInteger& operator=(const SignedInteger& other)` | $x\leftarrow y$ | $y\ge0$ | $O(m)$ | 无 |
| `UnsignedInteger& operator=(unsignedIntegral value)` | $x\leftarrow v$ | 无 | $O(\log v)$ | 对全体无符号整数启用 |
| `UnsignedInteger& operator=(signedIntegral value)` | $x\leftarrow v$ | $v\ge0$ | $O(\log v)$ | 对全体有符号整数启用 |
| `UnsignedInteger& operator=(floatingPoint value)` | $x\leftarrow\lfloor v\rfloor$ | $v\ge0$ | $O(\log v)$ | 对全体浮点数启用 |
| `UnsignedInteger& operator=(const char* value)` | $x\leftarrow v$ | $v$ 不是 `nullptr`，$v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 无 |
| `UnsignedInteger& operator=(const std::string& value)` | $x\leftarrow v$ | $v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 无 |
| `friend std::istream& operator>>(std::istream& stream, UnsignedInteger& destination)` | 从 `stream` 读入 `destination` | $v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 流式读入运算符 |
| `friend std::ostream& operator<<(std::ostream& stream, const UnsignedInteger& source)` | 向 `stream` 输出 `source` | 无 | $O(n)$ | 流式输出运算符 |
| `operator unsignedIntegral() const` | 返回 $x$ 的 `unsignedIntegral` 形式 | 无 | $O(n)$ | 类型转换运算符，对全体无符号整数启用 |
| `operator signedIntegral() const` | 返回 $x$ 的 `signedIntegral` 形式 | 无 | $O(n)$ | 类型转换运算符，对全体有符号整数启用 |
| `operator floatingPoint() const` | 返回 $x$ 的 `floatingPoint` 形式 | 无 | $O(n)$ | 类型转换运算符，对全体浮点数启用 |
| `operator const char*() const` | 返回 $x$ 的 `const char*` 形式 | 无 | $O(n)$ | 类型转换运算符 |
| `operator std::string() const` | 返回 $x$ 的 `std::string` 形式 | 无 | $O(n)$ | 类型转换运算符 |
| `operator bool() const noexcept` | 判断 $x$ 是否非 $0$ | 无 | $O(1)$ | 类型转换运算符 |
| `std::strong_ordering operator<=>(const UnsignedInteger& other) const` | 判断 $x$ 与 $y$ 的大小关系 | 无 | $O(n)$ | 三路比较运算符，仅在版本在 C++20 及以上启用 |
| `bool operator==(const UnsignedInteger& other) const` | 判断是否 $x=y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator!=(const UnsignedInteger& other) const` | 判断是否 $x\ne y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator<(const UnsignedInteger& other) const` | 判断是否 $x<y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator>(const UnsignedInteger& other) const` | 判断是否 $x>y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator<=(const UnsignedInteger& other) const` | 判断是否 $x\le y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator>=(const UnsignedInteger& other) const` | 判断是否 $x\ge y$ | 无 | $O(n)$ | 比较运算符 |
| `UnsignedInteger& operator+=(const UnsignedInteger& other)` | $x\leftarrow x+y$ | 无 | $O(\max(n,m))$ | 加法赋值运算符 |
| `UnsignedInteger operator+(const UnsignedInteger& other) const` | 返回 $x+y$ | 无 | $O(\max(n,m))$ | 加法运算符 |
| `UnsignedInteger& operator++()` | $x\leftarrow x+1$ | 无 | $O(n)$ | 前置自增运算符 |
| `UnsignedInteger operator++(int)` | $x\leftarrow x+1$ | 无 | $O(n)$ | 后置自增运算符 |
| `UnsignedInteger& operator-=(const UnsignedInteger& other)` | $x\leftarrow x-y$ | $x\ge y$ | $O(\max(n,m))$ | 减法赋值运算符 |
| `UnsignedInteger operator-(const UnsignedInteger& other) const` | 返回 $x-y$ | $x\ge y$ | $O(\max(n,m))$ | 减法运算符 |
| `UnsignedInteger& operator--()` | $x\leftarrow x-1$ | $x\ne0$ | $O(n)$ | 前置自减运算符 |
| `UnsignedInteger operator--(int)` | $x\leftarrow x-1$ | $x\ne0$ | $O(n)$ | 后置自减运算符 |
| `UnsignedInteger& operator*=(const UnsignedInteger& other)` | $x\leftarrow x\cdot y$ | $n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 乘法赋值运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `UnsignedInteger operator*(const UnsignedInteger& other) const` | 返回 $x\cdot y$ | $n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 乘法运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `UnsignedInteger& operator/=(const UnsignedInteger& other)` | $x\leftarrow\lfloor\frac xy\rfloor$ | $y\ne0\land n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 除法赋值运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `UnsignedInteger operator/(const UnsignedInteger& other) const` | 返回 $\lfloor\frac xy\rfloor$ | $y\ne0\land n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 除法运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `UnsignedInteger& operator%=(const UnsignedInteger& other)` | $x\leftarrow x\bmod y$ | $y\ne0\land n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 模赋值运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `UnsignedInteger operator%(const UnsignedInteger& other) const` | 返回 $x\bmod y$ | $y\ne0\land n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 模运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `UnsignedInteger operator""_UI(const char* literal, std::size_t)` | 返回 `literal` 的 `UnsignedInteger` 形式 | `literal` 是非空数字串 | $O(n)$ | 字符串字面量 |

## `SignedInteger`

| 函数签名 | 功能概述 | 合法检查 | 时间复杂度 | 备注 |
|:-:|:-:|:-:|:-:|:-:|
| `SignedInteger()` | $x\leftarrow0$ | 无 | $O(1)$ | 默认构造函数 |
| `SignedInteger(const SignedInteger& other)` | $x\leftarrow y$ | 无 | $O(m)$ | 复制构造函数 |
| `SignedInteger(SignedInteger&& other) noexcept` | $x\leftarrow y,y\leftarrow0$ | 无 | $O(1)$ | 移动构造函数 |
| `SignedInteger(const UnsignedInteger& other)` | $x\leftarrow y$ | 无 | $O(m)$ | 无 |
| `SignedInteger(unsignedIntegral value)` | $x\leftarrow v$ | 无 | $O(\log v)$ | 对全体无符号整数启用 |
| `SignedInteger(signedIntegral value)` | $x\leftarrow v$ | 无 | $O(\log v)$ | 对全体有符号整数启用 |
| `SignedInteger(floatingPoint value)` | $x\leftarrow\lfloor v\rfloor$ | 无 | $O(\log v)$ | 对全体浮点数启用 |
| `SignedInteger(const char* value)` | $x\leftarrow v$ | $v$ 不是 `nullptr`，$v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 无 |
| `SignedInteger(const std::string& value)` | $x\leftarrow v$ | $v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 无 |
| `~SignedInteger()` | 解分配内存 | 无 | $O(1)$ | 析构函数 |
| `SignedInteger& operator=(const SignedInteger& other)` | $x\leftarrow y$ | 无 | $O(m)$ | 复制赋值运算符 |
| `SignedInteger& operator=(SignedInteger&& other) noexcept` | $x\leftarrow y,y\leftarrow0$ | 无 | $O(1)$ | 移动赋值运算符 |
| `SignedInteger& operator=(const UnsignedInteger& other)` | $x\leftarrow y$ | 无 | $O(m)$ | 无 |
| `SignedInteger& operator=(unsignedIntegral value)` | $x\leftarrow v$ | 无 | $O(\log v)$ | 对全体无符号整数启用 |
| `SignedInteger& operator=(signedIntegral value)` | $x\leftarrow v$ | 无 | $O(\log v)$ | 对全体有符号整数启用 |
| `SignedInteger& operator=(floatingPoint value)` | $x\leftarrow\lfloor v\rfloor$ | 无 | $O(\log v)$ | 对全体浮点数启用 |
| `SignedInteger& operator=(const char* value)` | $x\leftarrow v$ | $v$ 不是 `nullptr`，$v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 无 |
| `SignedInteger& operator=(const std::string& value)` | $x\leftarrow v$ | $v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 无 |
| `friend std::istream& operator>>(std::istream& stream, SignedInteger& destination)` | 从 `stream` 读入 `destination` | $v$ 非空，$v$ 是数字串 | $O(\lg v)$ | 流式读入运算符 |
| `friend std::ostream& operator<<(std::ostream& stream, const SignedInteger& source)` | 向 `stream` 输出 `source` | 无 | $O(n)$ | 流式输出运算符 |
| `operator unsignedIntegral() const` | 返回 $x$ 的 `unsignedIntegral` 形式 | $x\ge0$ | $O(n)$ | 类型转换运算符，对全体无符号整数启用 |
| `operator signedIntegral() const` | 返回 $x$ 的 `signedIntegral` 形式 | 无 | $O(n)$ | 类型转换运算符，对全体有符号整数启用 |
| `operator floatingPoint() const` | 返回 $x$ 的 `floatingPoint` 形式 | 无 | $O(n)$ | 类型转换运算符，对全体浮点数启用 |
| `operator const char*() const` | 返回 $x$ 的 `const char*` 形式 | 无 | $O(n)$ | 类型转换运算符 |
| `operator std::string() const` | 返回 $x$ 的 `std::string` 形式 | 无 | $O(n)$ | 类型转换运算符 |
| `operator bool() const noexcept` | 判断 $x$ 是否非 $0$ | 无 | $O(1)$ | 类型转换运算符 |
| `std::strong_ordering operator<=>(const SignedInteger& other) const` | 判断 $x$ 与 $y$ 的大小关系 | 无 | $O(n)$ | 三路比较运算符，仅在版本在 C++20 及以上启用 |
| `bool operator==(const SignedInteger& other) const` | 判断是否 $x=y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator!=(const SignedInteger& other) const` | 判断是否 $x\ne y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator<(const SignedInteger& other) const` | 判断是否 $x<y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator>(const SignedInteger& other) const` | 判断是否 $x>y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator<=(const SignedInteger& other) const` | 判断是否 $x\le y$ | 无 | $O(n)$ | 比较运算符 |
| `bool operator>=(const SignedInteger& other) const` | 判断是否 $x\ge y$ | 无 | $O(n)$ | 比较运算符 |
| `SignedInteger& operator+=(const SignedInteger& other)` | $x\leftarrow x+y$ | 无 | $O(\max(n,m))$ | 加法赋值运算符 |
| `SignedInteger operator+(const SignedInteger& other) const` | 返回 $x+y$ | 无 | $O(\max(n,m))$ | 加法运算符 |
| `SignedInteger& operator++()` | $x\leftarrow x+1$ | 无 | $O(n)$ | 前置自增运算符 |
| `SignedInteger operator++(int)` | $x\leftarrow x+1$ | 无 | $O(n)$ | 后置自增运算符 |
| `SignedInteger& operator-=(const SignedInteger& other)` | $x\leftarrow x-y$ | 无 | $O(\max(n,m))$ | 减法赋值运算符 |
| `SignedInteger operator-(const SignedInteger& other) const` | 返回 $x-y$ | 无 | $O(\max(n,m))$ | 减法运算符 |
| `SignedInteger& operator--()` | $x\leftarrow x-1$ | 无 | $O(n)$ | 前置自减运算符 |
| `SignedInteger operator--(int)` | $x\leftarrow x-1$ | 无 | $O(n)$ | 后置自减运算符 |
| `SignedInteger& operator*=(const SignedInteger& other)` | $x\leftarrow x\cdot y$ | $n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 乘法赋值运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `SignedInteger operator*(const SignedInteger& other) const` | 返回 $x\cdot y$ | $n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 乘法运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `SignedInteger& operator/=(const SignedInteger& other)` | $x\leftarrow\lfloor\frac xy\rfloor$ | $y\ne0\land n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 除法赋值运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `SignedInteger operator/(const SignedInteger& other) const` | 返回 $\lfloor\frac xy\rfloor$ | $y\ne0\land n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 除法运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `SignedInteger& operator%=(const SignedInteger& other)` | $x\leftarrow x\bmod y$ | $y\ne0\land n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 模赋值运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `SignedInteger operator%(const SignedInteger& other) const` | 返回 $x\bmod y$ | $y\ne0\land n\le L\land m\le L$ | $O(nm),O((n+m)\log(n+m))$ | 模运算符，当 $n\le T\lor m\le T$ 时使用暴力算法 |
| `SignedInteger operator""_SI(const char* literal, std::size_t)` | 返回 `literal` 的 `SignedInteger` 形式 | `literal` 是非空数字串 | $O(n)$ | 字符串字面量 |

**特别注意，`SignedInteger` 的取模和除法的结果是和 C++ 一致的**。也就是说：

- 除法结果向 $0$ 取整。
- 取模结果的符号为左运算数的符号。

# 项目维护

## 许可证

本项目采用 [MIT License](LICENSE) 开源许可证。

```
MIT License

Copyright (c) 2024 masonxiong

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

## 贡献指南

欢迎任何形式的贡献！如果你想为这个项目贡献代码，请遵循以下步骤：

### 报告问题
- 在提交问题前，请先查看现有的 [Issues](../../issues) 是否有相似问题
- 清楚描述问题的复现步骤、期望行为和实际行为
- 提供必要的环境信息（操作系统、编译器版本等）

### 提交代码
1. Fork 本仓库到你的 GitHub 账户
2. 创建一个新的特性分支 (`git checkout -b feature/your-feature`)
3. 提交你的修改 (`git commit -am 'Add some feature'`)
4. 推送到分支 (`git push origin feature/your-feature`)
5. 创建一个 Pull Request

### 代码规范
- 遵循现有的代码风格和命名约定
- 为新功能添加适当的测试用例
- 更新相关文档
- 确保代码通过所有现有测试

## 版本历史

### V1 (2025-8-24)

- 首次发布
  - Incoming：
    - 开根、最大公约数、最小公倍数、阶乘、质数判断、二进制位运算。
    - 另一个版本的高精度整数库，拥有更好的可移植性和线程安全性以及 `constexpr` 支持，但是可能失去部分性能。

## 问题反馈

如果你在使用过程中遇到问题或有功能建议：

### 报告 Bug

请在 [GitHub Issues](../../issues) 中创建新问题，并包含：

- 问题的详细描述
- 最小化的复现代码
- 编译器和系统环境信息
- 错误输出或异常信息

### 功能请求

我们欢迎新功能建议！请在 Issues 中描述：

- 期望的功能详细说明（最好不要和最新版本的 Incoming 部分重复）
- 使用场景和理由
- 可能的实现方案（如果有的话）

### 性能问题

如果发现性能问题，请提供：

- 具体的性能测试代码
- 与其他库的对比结果
- 运行环境和数据规模信息

## 社区支持

- **GitHub Issues**: [项目问题讨论](../../issues)
- **洛谷主页**: [masonxiong](https://www.luogu.com.cn/user/446979)
- **Library Checker**: [性能记录查看](https://judge.yosupo.jp/)

## 常见问题

- 我的编程环境非常老，看你的代码一堆不认识的语法，真能过编吗？

本模板对语言环境的要求较为宽松。你只需要一个支持 C++11 的 GCC 编译器即可通过编译。为获得最佳性能，建议添加 `-march=native` 以启用可用的 SIMD（x86_64 上为 AVX2，AArch64 上为 NEON）；但这不是必须，未启用时库会自动退回 NEON 或纯标量实现。

- 编译时报错 `inlining failed in call to 'always_inline' '__m128d _mm_fmaddsub_pd(__m128d, __m128d, __m128d)': target specific option mismatch` 是怎么回事？

这通常是未启用对应 SIMD 指令集导致（例如在 x86_64 上未启用 AVX2）。你可以在编译参数中添加 `-march=native` 或针对性开关（如 `-mavx2`）以获得更高性能；也可以不启用，库会自动使用标量实现，但性能会有所下降。

- 你的模板怎么不支持 `divmod`？

诶其实是支持的，在 `protected` 函数里面存在一个 `divisionAndModulus` 函数，它实现了 `divmod` 的功能，时间复杂度与 `operator/` 一致。但是因为把它放到 `public` 函数里面会显得结构很丑，于是就把它扔到 `protected` 里面了（

- 在我启用调试的前提下，程序崩溃且没有错误信息，如何调试？

注意，为了效率，本模板并不线程安全。请检查你是否使用了多线程。若没有或你确信不是多线程的问题，那你很可能发现了一个 Bug！请参考前文 Bug 反馈相关内容进行反馈。