cmake_minimum_required(VERSION 3.16)

project("core_test")

add_subdirectory(".." "core")


set(CMAKE_BUILD_TYPE "Debug" CACHE INTERNAL "")

set(CMAKE_C_FLAGS "-Wall -Wextra -Werror=return-type" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++20 -fno-exceptions" CACHE INTERNAL "")

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*" CACHE INTERNAL "")


set(SRC
    "src/main.cpp"
    "src/convert.cpp"
    "src/format.cpp"
    "src/numeric.cpp"
    "src/collections/vec.cpp"
    "src/collections/vec_deque.cpp"
)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS)

enable_testing()
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE "core" ${CONAN_LIBS})
add_test(${PROJECT_NAME} ${PROJECT_NAME})
