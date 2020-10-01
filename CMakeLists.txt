cmake_minimum_required(VERSION 3.0)
project(rstd VERSION 0.1.2 DESCRIPTION "Rust-like types and routines for C++17")

set(CMAKE_C_FLAGS "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=gnu++17") #-fno-exceptions
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

enable_testing()
set(TEST_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/format.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/assert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/traits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/tuple.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/union.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/variant.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/option.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/result.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/thread.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/lazy_static.cpp"
)
add_executable("test_${PROJECT_NAME}"
    ${TEST_SOURCE}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp"
)
target_compile_definitions("test_${PROJECT_NAME}" PRIVATE "DEBUG")
add_test("test_${PROJECT_NAME}" "test_${PROJECT_NAME}")

add_executable("self_test_${PROJECT_NAME}"
    #${TEST_SOURCE}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/self_test.cpp"
)
target_compile_definitions("self_test_${PROJECT_NAME}" PRIVATE "DEBUG")
target_link_libraries("self_test_${PROJECT_NAME}" PRIVATE "pthread")
add_test("self_test_${PROJECT_NAME}" "self_test_${PROJECT_NAME}")
