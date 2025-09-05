# 添加基本用法示例
add_executable(basic_usage basic_usage.cpp)
target_link_libraries(basic_usage PRIVATE Integer::Integer)
if(DEFINED EXAMPLE_COMPILE_FLAGS)
    target_compile_options(basic_usage PRIVATE ${EXAMPLE_COMPILE_FLAGS})
endif()

# 添加高级功能演示
add_executable(advanced_demo advanced_demo.cpp)
target_link_libraries(advanced_demo PRIVATE Integer::Integer)
if(DEFINED EXAMPLE_COMPILE_FLAGS)
    target_compile_options(advanced_demo PRIVATE ${EXAMPLE_COMPILE_FLAGS})
endif()

# 线程安全探测示例
add_executable(thread_safety thread_safety.cpp)
target_link_libraries(thread_safety PRIVATE Integer::Integer)
if(DEFINED EXAMPLE_COMPILE_FLAGS)
    target_compile_options(thread_safety PRIVATE ${EXAMPLE_COMPILE_FLAGS})
endif()

# 安装示例程序（可选）
option(INSTALL_EXAMPLES "Install example programs" OFF)
if(INSTALL_EXAMPLES)
    set(_EXAMPLE_TARGETS basic_usage advanced_demo thread_safety)
    if(TARGET bench_integer_vs_gmp)
        list(APPEND _EXAMPLE_TARGETS bench_integer_vs_gmp)
    endif()
    install(TARGETS ${_EXAMPLE_TARGETS}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples)
    install(FILES basic_usage.cpp advanced_demo.cpp thread_safety.cpp bench_integer_vs_gmp.cpp
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/Integer/examples)
endif()

# 输出配置信息
message(STATUS "Configured examples:")
message(STATUS "  - basic_usage")
message(STATUS "  - advanced_demo")
message(STATUS "  - thread_safety")
message(STATUS "  - Install examples: ${INSTALL_EXAMPLES}")

# 可选：GMP 对比示例（若未找到GMP则跳过）
find_path(GMP_INCLUDE_DIR NAMES gmpxx.h)
find_library(GMPXX_LIBRARY NAMES gmpxx)
find_library(GMP_LIBRARY NAMES gmp)

if(GMP_INCLUDE_DIR AND GMPXX_LIBRARY AND GMP_LIBRARY)
    add_executable(bench_integer_vs_gmp bench_integer_vs_gmp.cpp)
    target_link_libraries(bench_integer_vs_gmp PRIVATE Integer::Integer ${GMPXX_LIBRARY} ${GMP_LIBRARY})
    target_include_directories(bench_integer_vs_gmp PRIVATE ${GMP_INCLUDE_DIR})
    if(DEFINED EXAMPLE_COMPILE_FLAGS)
        target_compile_options(bench_integer_vs_gmp PRIVATE ${EXAMPLE_COMPILE_FLAGS})
    endif()
    message(STATUS "  - bench_integer_vs_gmp (GMP found)")
    add_custom_target(examples_all ALL DEPENDS basic_usage advanced_demo thread_safety bench_integer_vs_gmp)
else()
    message(STATUS "  - bench_integer_vs_gmp (skipped: GMP not found)")
    add_custom_target(examples_all ALL DEPENDS basic_usage advanced_demo thread_safety)
endif()
