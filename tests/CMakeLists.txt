find_package(Python3 COMPONENTS Interpreter REQUIRED)

# 将 Python 测试脚本纳入 CTest
set(TEST_RUNNER ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.py)

add_test(NAME integer_run_tests
    COMMAND ${Python3_EXECUTABLE} ${TEST_RUNNER}
)

# 将 CMake 使用的 C++ 编译器传递给脚本（脚本默认使用 clang++，这里覆盖为 CMAKE_CXX_COMPILER）
set_tests_properties(integer_run_tests PROPERTIES
    ENVIRONMENT "CLANG=${CMAKE_CXX_COMPILER}"
    TIMEOUT 900
)

message(STATUS "Configured tests: integer_run_tests (Python-driven)")


