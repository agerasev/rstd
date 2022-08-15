#pragma once

#include <iostream>
#include <sstream>

#include "display.hpp"
#include "formatter.hpp"
#include <rcore/fmt/arguments.hpp>
#include <rcore/fmt/format.hpp>

namespace rstd::fmt {

using rcore::fmt::Arguments;
using rcore::fmt::write;

template <Displayable... Ts>
std::string format_unchecked(const std::string_view str, Ts &&...args) {
    StringFormatter f;
    write_unchecked(f, str, std::forward<Ts>(args)...);
    return f.copy_string();
}

template <Displayable... Ts>
void print_unchecked(const std::string_view str, Ts &&...args) {
    OstreamFormatter f(std::cout);
    write_unchecked(f, str, std::forward<Ts>(args)...);
}

template <const char *FMT_STR, Displayable... Ts>
std::string format(Ts &&...args) {
    StringFormatter f;
    write<FMT_STR>(f, std::forward<Ts>(args)...);
    return f.copy_string();
}

template <const char *FMT_STR, Displayable... Ts>
void print(Ts &&...args) {
    OstreamFormatter f(std::cout);
    write<FMT_STR>(f, std::forward<Ts>(args)...);
}

} // namespace rstd::fmt

#define rstd_write(...) rcore_write(__VA_ARGS__)
#define rstd_writeln(...) rcore_writeln(__VA_ARGS__)

#define rstd_format_args(lit, ...) rcore_format_args(lit __VA_OPT__(, )##__VA_ARGS__)

#define rstd_format(lit, ...) \
    [&]() -> ::std::string { \
        static constexpr const char fmt_str[] = (lit); \
        return ::rstd::fmt::format<fmt_str>(__VA_ARGS__); \
    }()

#define rstd_print(lit, ...) \
    [&]() { \
        static constexpr const char fmt_str[] = (lit); \
        ::rstd::fmt::print<fmt_str>(__VA_ARGS__); \
    }()

#define rstd_println(lit, ...) rstd_print(lit "\n" __VA_OPT__(, )##__VA_ARGS__)