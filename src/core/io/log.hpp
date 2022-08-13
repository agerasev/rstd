#pragma once

#include <array>
#include <string>

#include <core/io/print.hpp>

namespace core::io {

enum class LogLevel {
    Fatal = 0,
    Error,
    Warning,
    Info,
    Debug,
    Trace,
};

namespace _impl {

constexpr std::array<const char *, size_t(LogLevel::Trace) + 1> LEVEL_NAMES = {
    "fatal",
    "error",
    "warning",
    "info",
    "debug",
    "trace",
};

template <typename... Ts>
void log_dynamic(LogLevel level, std::string_view fmt_str, Ts &&...args) {
    const auto log_fmt_str = std::string("[app:{}] ").append(fmt_str).append("\n");
    print_dynamic(std::cout, log_fmt_str, LEVEL_NAMES[size_t(level)], std::forward<Ts>(args)...);
}

#if defined(CORE_FORMAT_STATIC_CHECK)
template <LogLevel LEVEL, format::Literal FMT_STR, typename... Ts>
void log_static(Ts &&...args) {
    constexpr auto LOG_FMT_STR = format::Literal("[app:{}] ").append(FMT_STR).append("\n");
    print_static<LOG_FMT_STR>(std::cout, LEVEL_NAMES[size_t(LEVEL)], std::forward<Ts>(args)...);
}
#endif

} // namespace _impl
} // namespace core::io

#if defined(CORE_FORMAT_STATIC_CHECK)
#define core_log(level, fmt_str, ...) ::core::_impl::log_static<level, fmt_str>(__VA_ARGS__)
#else
#define core_log(level, fmt_str, ...) ::core::_impl::log_dynamic(level, fmt_str __VA_OPT__(, ) __VA_ARGS__)
#endif

/* clang-format off */
#define core_log_fatal(  fmt, ...) core_log(::core::LogLevel::Fatal,   fmt __VA_OPT__(,) ##__VA_ARGS__)
#define core_log_error(  fmt, ...) core_log(::core::LogLevel::Error,   fmt __VA_OPT__(,) ##__VA_ARGS__)
#define core_log_warning(fmt, ...) core_log(::core::LogLevel::Warning, fmt __VA_OPT__(,) ##__VA_ARGS__)
#define core_log_info(   fmt, ...) core_log(::core::LogLevel::Info,    fmt __VA_OPT__(,) ##__VA_ARGS__)
#define core_log_debug(  fmt, ...) core_log(::core::LogLevel::Debug,   fmt __VA_OPT__(,) ##__VA_ARGS__)
#define core_log_trace(  fmt, ...) core_log(::core::LogLevel::Trace,   fmt __VA_OPT__(,) ##__VA_ARGS__)
/* clang-format on */
