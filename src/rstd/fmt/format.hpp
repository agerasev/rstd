#pragma once

#include <iostream>
#include <sstream>

#include "display.hpp"
#include <rcore/fmt/arguments.hpp>
#include <rcore/fmt/format.hpp>

namespace rstd::fmt {

using rcore::fmt::Arguments;

} // namespace rstd::fmt

#define rstd_write(...) rcore_write(__VA_ARGS__)
#define rstd_writeln(...) rcore_writeln(__VA_ARGS__)

#define rstd_format_args(...) rcore_format_args(__VA_ARGS__)

#define rstd_format(...) \
    [&]() -> ::std::string { \
        ::rstd::fmt::StringFormatter f; \
        rstd_write(f, __VA_ARGS__); \
        return f.copy_string(); \
    }()

#define rstd_print(...) \
    [&]() { \
        ::rstd::fmt::OstreamFormatter f(std::cout); \
        rstd_write(f, __VA_ARGS__); \
    }()

#define rstd_println(...) \
    [&]() { \
        ::rstd::fmt::OstreamFormatter f(std::cout); \
        rstd_writeln(f, __VA_ARGS__); \
    }()
