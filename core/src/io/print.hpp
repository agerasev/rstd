#pragma once

#include <iostream>

#include <core/fmt/format.hpp>

#define core_print(fmt_str, ...) core_write(std::cout, fmt_str __VA_OPT__(, )##__VA_ARGS__)
#define core_println(fmt_str, ...) core_write(std::cout, fmt_str __VA_OPT__(, )##__VA_ARGS__)
