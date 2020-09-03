cmake_minimum_required(VERSION 3.0)
project(core VERSION 0.1.0 DESCRIPTION "Rust-like types and routines for C++17")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
set(SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/container/base.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/container/tuple.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/container/union.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/container/variant.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/traits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/option.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/result.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/prelude.cpp"
)
add_library(${PROJECT_NAME} OBJECT ${SOURCE})

enable_testing()
add_executable("test_${PROJECT_NAME}" ${SOURCE} "${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp")
target_compile_definitions("test_${PROJECT_NAME}" PRIVATE "-DDEBUG" "-DTEST_CORE")
add_test("test_${PROJECT_NAME}" "test_${PROJECT_NAME}")
