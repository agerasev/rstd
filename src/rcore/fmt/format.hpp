#pragma once

// TODO: Migrate to std::format when supported.

#include <optional>
#include <string>
#include <tuple>

#include "display.hpp"
#include "formatter.hpp"

namespace rcore::fmt {

enum class ErrorKind {
    TooManyArgs,
    TooFewArgs,
    UnpairedBrace,
};

struct Error {
    ErrorKind kind;
    size_t pos;
};

template <>
struct Display<Error> {
    inline static void fmt(const Error &self, Formatter &f) {
        switch (self.kind) {
        case ErrorKind::TooManyArgs:
            f.write_str("Too many args");
            break;
        case ErrorKind::TooFewArgs:
            f.write_str("Too few args");
            break;
        case ErrorKind::UnpairedBrace:
            f.write_str("Unpaired brace");
            break;
        }
        f.write_str(" at char ");
        Display<size_t>::fmt(self.pos, f);
    }
};

namespace _impl {

template <Displayable... Ts>
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

template <Displayable... Ts>
struct FormatNth {
    inline static void apply(Formatter &, size_t) {
        // FIXME: rcore_assert_eq(n, 0);
    }
};

template <Displayable T, Displayable... Ts>
struct FormatNth<T, Ts...> {
    static void apply(Formatter &f, size_t n, T &&arg, Ts &&...args) {
        if (n == 0) {
            Display<std::remove_cvref_t<decltype(arg)>>::fmt(arg, f);
        } else {
            FormatNth<Ts...>::apply(f, n - 1, std::forward<Ts>(args)...);
        }
    }
};

template <Displayable... Ts>
void format_nth(Formatter &f, size_t n, Ts &&...args) {
    FormatNth<Ts...>::apply(f, n, std::forward<Ts>(args)...);
}

} // namespace _impl

template <Displayable... Ts>
void write_unchecked(Formatter &f, const std::string_view str, Ts &&...args) {
    // constexpr size_t total_args = sizeof...(Ts);
    size_t arg = 0;
    bool opened = false;
    bool closed = false;
    for (char c : str) {
        if (c == '{') {
            if (opened) {
                f.write_char('{');
                opened = false;
            } else {
                opened = true;
            }
        } else if (c == '}') {
            if (opened) {
                _impl::format_nth(f, arg, std::forward<Ts>(args)...);
                arg += 1;
                opened = false;
            } else if (closed) {
                f.write_char('}');
                closed = false;
            } else {
                closed = true;
            }
        } else {
            // rcore_assert(!opened && !closed);
            f.write_char(c);
        }
    }
    // rcore_assert(!opened && !closed);
    // rcore_assert_eq(arg, total_args);
}

template <const char *FMT_STR, Displayable... Ts>
void write(Formatter &f, Ts &&...args) {
    constexpr auto error = _impl::check_format_str<Ts...>(FMT_STR);
    static_assert(!error.has_value(), "Format error");
    write_unchecked(f, FMT_STR, std::forward<Ts>(args)...);
}

} // namespace rcore::fmt

#define rcore_write(f, lit, ...) \
    [&]() { \
        static constexpr const char fmt_str[] = (lit); \
        ::rcore::fmt::write<fmt_str>(f __VA_OPT__(, )##__VA_ARGS__); \
    }()

#define rcore_writeln(f, lit, ...) rcore_write(f, lit "\n" __VA_OPT__(, )##__VA_ARGS__)
