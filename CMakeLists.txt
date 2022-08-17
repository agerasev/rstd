cmake_minimum_required(VERSION 3.16)

project("rstd" VERSION 0.5.0)

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror=return-type")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++20 -fno-exceptions")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("src")

set(RCORE_SRC
    "src/rcore/fmt/display.hpp"
    "src/rcore/fmt/format.hpp"
    "src/rcore/sync/once.hpp"
    "src/rcore/sync/lazy_static.hpp"
    "src/rcore/mem/maybe_uninit.hpp"
    "src/rcore/assert.hpp"
    "src/rcore/panic.hpp"
    "src/rcore/panic.cpp"
    "src/rcore/option.hpp"
    "src/rcore/result.hpp"
)
set(RCORE_TEST_SRC
    "src/rcore/test.cpp"
)

set(RSTD_SRC
    "src/rstd/fmt/display.hpp"
    "src/rstd/fmt/format.hpp"
    "src/rstd/io/ansi_color.hpp"
    "src/rstd/io/error.hpp"
    "src/rstd/io/stream.hpp"
    "src/rstd/io/stream.cpp"
    "src/rstd/sync/mutex.hpp"
    "src/rstd/thread/local.hpp"
    "src/rstd/thread/handle.hpp"
    "src/rstd/thread/handle.cpp"
    "src/rstd/thread/thread.hpp"
    "src/rstd/thread/builder.hpp"
    "src/rstd/assert.hpp"
    "src/rstd/panic.hpp"
    "src/rstd/panic.cpp"
    "src/rstd/option.hpp"
    "src/rstd/result.hpp"
)
set(RSTD_TEST_SRC
    "src/rstd/test.cpp"
)

add_library("rcore" OBJECT ${RCORE_SRC})

add_library("rstd" OBJECT ${RSTD_SRC})
target_link_libraries("rstd" PUBLIC "rcore" PUBLIC "pthread")

enable_testing()

add_executable("rcore_test" ${RCORE_TEST_SRC})
target_compile_definitions("rcore_test" PRIVATE "RTEST")
target_link_libraries("rcore_test" "rcore")
add_test("rcore_test" "rcore_test")

add_executable("rstd_test" ${RSTD_TEST_SRC})
target_compile_definitions("rstd_test" PRIVATE "RTEST")
target_link_libraries("rstd_test" "rstd" "rcore")
add_test("rstd_test" "rstd_test")
