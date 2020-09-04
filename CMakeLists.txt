cmake_minimum_required(VERSION 3.0)
project(core VERSION 0.1.2 DESCRIPTION "Rust-like types and routines for C++17")

set(CMAKE_C_FLAGS "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=gnu++17") #-fno-exceptions
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

enable_testing()
set(TEST_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/format.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/assert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/traits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/tuple.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/union.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/variant.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/option.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/result.cpp"
)
add_executable("test_${PROJECT_NAME}"
    ${TEST_SOURCE}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp"
)
target_compile_definitions("test_${PROJECT_NAME}" PRIVATE "DEBUG")
add_test("test_${PROJECT_NAME}" "test_${PROJECT_NAME}")
