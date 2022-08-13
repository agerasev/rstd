cmake_minimum_required(VERSION 3.16)

project("core")

set(CMAKE_C_FLAGS "-Wall -Wextra -Werror=return-type")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++20 -fno-exceptions")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC
    "src/core/_impl/panic.hpp"
    "src/core/_impl/panic.cpp"
    "src/core/panic.hpp"
    "src/core/assert.hpp"
    "src/core/convert.hpp"
    "src/core/numeric.hpp"
    "src/core/io.hpp"
    "src/core/print.hpp"
    "src/core/format.hpp"
    "src/core/log.hpp"
    "src/core/stream.hpp"
    "src/core/stream.cpp"
    "src/core/lazy_static.hpp"
    "src/core/maybe_uninit.hpp"
    "src/core/match.hpp"
    "src/core/mutex.hpp"
    "src/core/option.hpp"
    "src/core/result.hpp"
    "src/core/collections/slice.hpp"
    "src/core/collections/vec.hpp"
    "src/core/collections/vec_deque.hpp"
    "src/core/collections/vec_deque.hxx"
)

add_library(${PROJECT_NAME} OBJECT ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC "src")

list(TRANSFORM SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/ OUTPUT_VARIABLE SRC_ABS)
add_custom_target(
    ${PROJECT_NAME}_tidy
    COMMAND clang-tidy
    ${SRC_ABS}
    --header-filter=${CMAKE_CURRENT_SOURCE_DIR}/src
    -p=${CMAKE_BINARY_DIR}
)
