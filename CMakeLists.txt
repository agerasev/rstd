cmake_minimum_required(VERSION 3.0)
project(core VERSION 0.1.0 DESCRIPTION "Rust-like types and routines for C++17")

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
set(SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/core/container/base.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/container/tuple.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/container/union.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/container/variant.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/traits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/option.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/result.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/prelude.cpp"
)
add_library(${PROJECT_NAME} OBJECT ${SOURCE})

enable_testing()
add_executable(test_core ${SOURCE} "${CMAKE_CURRENT_SOURCE_DIR}/core/test.cpp")

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++17") #-fno-exceptions
target_compile_definitions(test_core PRIVATE "-DDEBUG" "-DUNIT_TEST")
add_test(test_core test_core)
