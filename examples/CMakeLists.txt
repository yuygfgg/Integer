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

# 安装示例程序（可选）
option(INSTALL_EXAMPLES "Install example programs" OFF)
if(INSTALL_EXAMPLES)
    install(TARGETS basic_usage advanced_demo
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples)
    install(FILES basic_usage.cpp advanced_demo.cpp
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/Integer/examples)
endif()

# 输出配置信息
message(STATUS "Configured examples:")
message(STATUS "  - basic_usage")
message(STATUS "  - advanced_demo")
message(STATUS "  - Install examples: ${INSTALL_EXAMPLES}")
