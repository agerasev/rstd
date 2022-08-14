cmake_minimum_required(VERSION 3.16)

project("rstd" VERSION 0.5.0)

set(CMAKE_C_FLAGS "-Wall -Wextra -Werror=return-type")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++20 -fno-exceptions")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("src")

set(RCORE_SRC
    "src/rcore/mem/maybe_uninit.hpp"
    "src/rcore/mem/lazy_static.hpp"
    "src/rcore/sync/once.hpp"
    "src/rcore/option.hpp"
    "src/rcore/result.hpp"
)
set(RCORE_TEST_SRC
    "src/rcore/test.cpp"
)

set(RSTD_SRC
    "src/rstd/io/ansi_color.hpp"
    "src/rstd/io/error.hpp"
    "src/rstd/io/stream.hpp"
    "src/rstd/io/stream.cpp"
    "src/rstd/sync/mutex.hpp"
    "src/rstd/sync/option.hpp"
    "src/rstd/sync/result.hpp"
)
set(RSTD_TEST_SRC
    "src/rstd/test.cpp"
)

add_library("rcore" OBJECT ${RCORE_SRC})

add_library("rstd" OBJECT ${RCORE_SRC})
target_link_libraries("rstd" PUBLIC "rcore" PUBLIC "pthread")

enable_testing()
add_executable("rcore_test" ${RCORE_TEST_SRC})
target_compile_definitions("rcore_test" PRIVATE "RTEST")
target_link_libraries("rcore_test" "rcore")
add_test("rcore_test" "rcore_test")

add_executable("rstd_test" ${RCORE_TEST_SRC})
target_compile_definitions("rstd_test" PRIVATE "RTEST")
target_link_libraries("rstd_test" "rstd")
add_test("rstd_test" "rstd_test")
