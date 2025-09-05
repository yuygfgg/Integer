find_package(Python3 COMPONENTS Interpreter REQUIRED)

# 将 Python 测试脚本纳入 CTest
set(TEST_RUNNER ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.py)

add_test(NAME integer_run_tests
    COMMAND ${Python3_EXECUTABLE} ${TEST_RUNNER}
)

if(APPLE)
    set(_TEST_CLANG "clang++")
else()
    set(_TEST_CLANG "${CMAKE_CXX_COMPILER}")
endif()

set_tests_properties(integer_run_tests PROPERTIES
    ENVIRONMENT "CLANG=${_TEST_CLANG}"
    TIMEOUT 900
)

message(STATUS "Configured tests: integer_run_tests (Python-driven)")


