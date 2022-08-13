#pragma once

// TODO: Migrate to std::format when supported.

#include <cassert>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "display.hpp"

namespace core {
// We cannot include <core/panic.hpp> here because it produces circular dependency.
[[noreturn]] void panic();
} // namespace core

#if defined(__GNUC__) && !defined(__clang__)
#define CORE_FORMAT_STATIC_CHECK
#endif

namespace core::fmt {

template <size_t N>
struct Literal {
    static_assert(N > 0);
    char data[N];

    constexpr Literal() = default;
    constexpr Literal(const char (&str)[N]) {
        std::copy_n(str, N, data);
    }

    constexpr std::string_view view() const {
        return std::string_view(data, N - 1);
    }
    constexpr size_t size() const {
        return N - 1;
    }

    template <size_t M>
    constexpr Literal<N + M - 1> append(Literal<M> other) const {
        Literal<N + M - 1> result;
        std::copy_n(this->data, N - 1, result.data);
        std::copy_n(other.data, M - 1, result.data + N - 1);
        return result;
    }
    template <size_t M>
    constexpr Literal<N + M - 1> append(const char (&str)[M]) const {
        return append(Literal<M>(str));
    }
};

enum class ErrorKind {
    TooManyArgs,
    TooFewArgs,
    UnpairedBrace,
};

struct Error {
    ErrorKind kind;
    size_t pos;
};

inline std::string get_error_string(const Error &err) {
    std::string_view str;
    switch (err.kind) {
    case ErrorKind::TooManyArgs:
        str = "Too many args";
        break;
    case ErrorKind::TooFewArgs:
        str = "Too few args";
        break;
    case ErrorKind::UnpairedBrace:
        str = "Unpaired brace";
        break;
    }
    std::stringstream ss;
    ss << "Format string error at char " << err.pos << ": " << str;
    return ss.str();
}


template <typename... Ts>
constexpr std::optional<Error> check_format_str(const std::string_view str) {
    constexpr size_t total_args = sizeof...(Ts);
    size_t arg = 0;
    bool opened = false;
    bool closed = false;
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (c == '{') {
            if (opened) {
                opened = false;
            } else {
                opened = true;
            }
        } else if (c == '}') {
            if (opened) {
                if (arg >= total_args) {
                    return Error{ErrorKind::TooManyArgs, i};
                }
                arg += 1;
                opened = false;
            } else if (closed) {
                closed = false;
            } else {
                closed = true;
            }
        } else {
            if (opened || closed) {
                return Error{ErrorKind::UnpairedBrace, i};
            }
        }
    }
    if (opened || closed) {
        return Error{ErrorKind::UnpairedBrace, str.size()};
    }
    if (arg != total_args) {
        return Error{ErrorKind::TooFewArgs, str.size()};
    }
    return std::nullopt;
}

template <typename... Ts>
void write_unchecked(std::ostream &stream, const std::string_view str, Ts &&...args) {
    [[maybe_unused]] auto arg_to_string = [](auto &&arg) {
        std::stringstream ss;
        Print<std::remove_cvref_t<decltype(arg)>>::print(ss, arg);
        return ss.str();
    };
    std::vector<std::string> printed_args{arg_to_string(std::forward<Ts>(args))...};
    size_t arg = 0;
    bool opened = false;
    bool closed = false;
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (c == '{') {
            if (opened) {
                stream << '{';
                opened = false;
            } else {
                opened = true;
            }
        } else if (c == '}') {
            if (opened) {
                assert(arg < printed_args.size());
                stream << printed_args[arg];
                arg += 1;
                opened = false;
            } else if (closed) {
                stream << '}';
                closed = false;
            } else {
                closed = true;
            }
        } else {
            assert(!opened && !closed);
            stream << c;
        }
    }
    assert(!opened && !closed);
    assert(arg == printed_args.size());
}


template <typename... Ts>
std::string format_unchecked(const std::string_view str, Ts &&...args) {
    std::stringstream stream;
    write_unchecked(stream, str, std::forward<Ts>(args)...);
    return stream.str();
}

template <typename... Ts>
void write_dynamic(std::ostream &stream, const std::string_view str, Ts &&...args) {
    const auto error = check_format_str<Ts...>(str);
    if (error.has_value()) {
        std::cout << get_error_string(error.value()) << std::endl;
        _impl::panic();
    }
    write_unchecked(stream, str, std::forward<Ts>(args)...);
}

template <typename... Ts>
std::string format_dynamic(const std::string_view str, Ts &&...args) {
    std::stringstream stream;
    write_dynamic(stream, str, std::forward<Ts>(args)...);
    return stream.str();
}

#if defined(CORE_FORMAT_STATIC_CHECK)

template <Literal FMT_STR, typename... Ts>
void write_static(std::ostream &stream, Ts &&...args) {
    constexpr auto error = check_format_str<Ts...>(FMT_STR.view());
    static_assert(!error.has_value(), "Format error");
    write_unchecked(stream, FMT_STR.view(), std::forward<Ts>(args)...);
}

template <Literal FMT_STR, typename... Ts>
std::string format_static(Ts &&...args) {
    std::stringstream stream;
    write_static<FMT_STR>(stream, std::forward<Ts>(args)...);
    return stream.str();
}

#endif

} // namespace core::fmt

#if defined(CORE_FORMAT_STATIC_CHECK)

#define core_write(stream, fmt_str, ...) ::core::fmt::write_static<fmt_str>(stream __VA_OPT__(, )##__VA_ARGS__)
#define core_format(fmt_str, ...) ::core::fmt::format_static<fmt_str>(__VA_ARGS__)

#else

#define core_write(stream, fmt_str, ...) ::core::fmt::write_dynamic(stream, fmt_str __VA_OPT__(, )##__VA_ARGS__)
#define core_format(fmt_str, ...) ::core::fmt::format_dynamic(fmt_str __VA_OPT__(, ) __VA_ARGS__)

#endif
