# 构建说明

## 快速开始

这是一个header-only库，最简单的使用方法是直接包含头文件：

```cpp
#include "Integer/Integer.h"
```

## 使用CMake构建示例

### 基本构建

```bash
mkdir build && cd build
cmake ..
make
```

### 构建选项

- `BUILD_EXAMPLES=ON/OFF` - 构建示例程序（默认：ON）
- `BUILD_TESTS=ON/OFF` - 构建测试程序（默认：OFF）
- `CMAKE_BUILD_TYPE=Debug/Release` - 构建类型

```bash
# 发布版本构建
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# 调试版本构建（启用有效性检查）
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### 运行示例

```bash
# 运行基本用法示例
make run_basic_usage

# 运行高级功能演示
make run_advanced_demo

# 运行所有示例
make run_all_examples
```

## 在其他项目中使用

### 方法1: 直接复制

将 `Integer/` 目录复制到你的项目中，然后包含头文件。

### 方法2: 使用CMake的find_package

安装库：
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make install
```

在你的CMakeLists.txt中：
```cmake
find_package(Integer REQUIRED)
target_link_libraries(your_target Integer::Integer)
```

### 方法3: 作为CMake子项目

将此项目添加为子模块或子目录：
```cmake
add_subdirectory(third_party/Integer)
target_link_libraries(your_target Integer::Integer)
```

## 编译器要求

- **C++14** 或更高版本
- 支持 **AVX2** 指令集的处理器（可选，用于性能优化）
- 支持的编译器：
  - GCC 5.0+
  - Clang 3.4+
  - MSVC 2015+

## 编译标志

推荐的编译标志：

### GCC/Clang
```bash
g++ -std=c++14 -O3 -mavx2 -mfma -DNDEBUG your_program.cpp
```

### MSVC
```cmd
cl /std:c++14 /O2 /arch:AVX2 your_program.cpp
```

## 调试模式

在调试时，可以启用参数验证：
```cpp
#define ENABLE_VALIDITY_CHECK
#include "Integer/Integer.h"
```

或者在编译时定义：
```bash
g++ -DENABLE_VALIDITY_CHECK your_program.cpp
```
