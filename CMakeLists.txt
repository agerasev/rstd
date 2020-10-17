if(USE_PCH)
    cmake_minimum_required(VERSION 3.16)
else()
    cmake_minimum_required(VERSION 3.0)
endif()

project(rstd VERSION 0.3.2 DESCRIPTION "Rust-like types and routines for C++17")

set(CMAKE_C_FLAGS "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=gnu++17 -fno-exceptions") # -std=c++17 -pedantic
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
set(HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/once.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/thread.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/io.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/panic.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/prelude.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/macros.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/format.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/assert.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/functions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/templates.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/container.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/tuple.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/union.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/variant.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/option.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/result.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/hash.hpp"
    #"${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/box.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/rc.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/thread.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/mutex.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/prelude.hpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rtest/test.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rtest/prelude.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rtest.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/lazy_static.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ansi_color.hpp"
)
set(SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/io.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rcore/panic.cpp"
)
set(TEST_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/format.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/assert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/functions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/templates.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/tuple.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/union.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/variant.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/option.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/result.cpp"
    #"${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/iterator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/box.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rstd/mutex.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/lazy_static.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/rtest/test.cpp"
)

add_library(${PROJECT_NAME} OBJECT ${SOURCE})
if (USE_PCH)
    target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADERS}) 
endif()
target_link_libraries(${PROJECT_NAME} "pthread")

enable_testing()
set(PROJECT_TEST "${PROJECT_NAME}_test")
add_executable("${PROJECT_TEST}"
    ${SOURCE}
    ${TEST_SOURCE}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/self_test.cpp"
)
target_compile_definitions("${PROJECT_TEST}" PRIVATE "DEBUG")
if (USE_PCH)
    target_precompile_headers(${PROJECT_TEST} PRIVATE ${HEADERS})
endif()
target_link_libraries("${PROJECT_TEST}" PRIVATE "pthread")
add_test("${PROJECT_TEST}" "${PROJECT_TEST}")
