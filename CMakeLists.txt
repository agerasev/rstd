cmake_minimum_required(VERSION 3.0)
project(rstd VERSION 0.3.0 DESCRIPTION "Rust-like types and routines for C++17")

set(CMAKE_C_FLAGS "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=gnu++17 -fno-exceptions")
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
set(SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/io.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/panic.cpp"
)
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
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/mutex.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/lazy_static.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/rtest/test.cpp"
)

add_library(${PROJECT_NAME} OBJECT ${SOURCE})
target_link_libraries(${PROJECT_NAME} "pthread")

enable_testing()
set(PROJECT_TEST "${PROJECT_NAME}_test")
add_executable("${PROJECT_TEST}"
    ${SOURCE}
    ${TEST_SOURCE}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/self_test.cpp"
)
target_compile_definitions("${PROJECT_TEST}" PRIVATE "DEBUG")
target_link_libraries("${PROJECT_TEST}" PRIVATE "pthread")
add_test("${PROJECT_TEST}" "${PROJECT_TEST}")
