cmake_minimum_required(VERSION 3.16)

project("rstd" VERSION 0.5.0)

set(CMAKE_C_FLAGS "-Wall -Wextra -Werror=return-type")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++20 -fno-exceptions")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("src")

enable_testing()
add_executable("rcore_test" "src/rcore/test.cpp")
target_compile_definitions("rcore_test" PRIVATE "RSTD_TEST")
add_test("rcore_test" "rcore_test")
