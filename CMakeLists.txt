cmake_minimum_required(VERSION 3.12)
project(Integer VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Host-specific optimizations (optional)
option(ENABLE_HOST_OPT "Enable host-specific optimizations (e.g. -march=native)" ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra)
    if(ENABLE_HOST_OPT)
        add_compile_options(-march=native)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
    else()
        add_compile_options(-g -O0 -DENABLE_VALIDITY_CHECK)
    endif()
endif()

# 定义header-only库
add_library(Integer INTERFACE)
add_library(Integer::Integer ALIAS Integer)

# 设置包含目录
target_include_directories(Integer 
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# 设置编译器要求
target_compile_features(Integer INTERFACE cxx_std_14)

# 添加示例（可选）
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    # Optionally enable AVX2 for examples if supported
    option(ENABLE_EXAMPLE_AVX2 "Enable AVX2 for examples if supported" OFF)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
    if(ENABLE_EXAMPLE_AVX2 AND COMPILER_SUPPORTS_AVX2)
        set(EXAMPLE_COMPILE_FLAGS "-mavx2 -mfma")
    endif()
    add_subdirectory(examples)
endif()

# 添加测试（可选）
option(BUILD_TESTS "Build test programs" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装配置
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 安装头文件
install(DIRECTORY Integer/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Integer
    FILES_MATCHING PATTERN "*.h"
)

# 安装库目标
install(TARGETS Integer
    EXPORT IntegerTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 创建并安装包配置文件
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/IntegerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/IntegerConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Integer
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/IntegerConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/IntegerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/IntegerConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Integer
)

# 安装导出目标
install(EXPORT IntegerTargets
    FILE IntegerTargets.cmake
    NAMESPACE Integer::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Integer
)

# 显示配置信息
message(STATUS "Integer Library Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build Examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Host-specific optimizations: ${ENABLE_HOST_OPT}")