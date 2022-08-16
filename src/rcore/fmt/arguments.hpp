#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <tuple>

#include "display.hpp"

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
    inline static void fmt(const Error &self, IFormatter &f) {
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

template <typename Tuple>
struct FormatChecker;

template <Displayable... Ts>
struct FormatChecker<std::tuple<Ts...>> final {
    static constexpr std::optional<Error> check(std::string_view str) {
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
};

template <Displayable... Ts>
struct NthArg {
    inline static void format(IFormatter &f, size_t) {
        __builtin_unreachable();
        f.write_str("{ERROR}");
    }
};

template <Displayable T, Displayable... Ts>
struct NthArg<T, Ts...> {
    static void format(IFormatter &f, size_t n, const T &arg, Ts &&...args) {
        if (n == 0) {
            Display<std::remove_cvref_t<decltype(arg)>>::fmt(arg, f);
        } else {
            NthArg<Ts...>::format(f, n - 1, std::forward<Ts>(args)...);
        }
    }
};

template <Displayable... Ts>
void format_nth(IFormatter &f, size_t n, Ts &&...args) {
    NthArg<Ts...>::format(f, n, std::forward<Ts>(args)...);
}

} // namespace _impl

template <typename Tuple>
constexpr std::optional<Error> check_format(std::string_view str) {
    return _impl::FormatChecker<Tuple>::check(str);
}

template <Displayable... Ts>
void write_unchecked(IFormatter &f, const std::string_view str, Ts &&...args) {
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
            assert(!opened && !closed);
            f.write_char(c);
        }
    }
    assert(!opened && !closed);
    assert(arg == sizeof...(Ts));
}

template <Displayable... Ts>
class Arguments final : public IDisplay {
public:
    using Tuple = std::tuple<Ts...>;

private:
    std::string_view fmt_str_;
    Tuple args_;

public:
    constexpr Arguments(std::string_view fmt_str, Ts &&...args) : fmt_str_(fmt_str), args_(std::forward<Ts>(args)...) {}

    void fmt(IFormatter &f) const override {
        std::apply(
            write_unchecked<Ts...>,
            std::tuple_cat(std::tuple<IFormatter &, std::string_view>(f, fmt_str_.data()), args_));
    }
};

template <Displayable... Ts>
Arguments<Ts...> format_args_unchecked(std::string_view fmt_str, Ts &&...args) {
    return Arguments<Ts...>(fmt_str, std::forward<Ts>(args)...);
}

} // namespace rcore::fmt

#define rcore_format_args(fmt_str, ...) \
    [&]() { \
        auto arguments = ::rcore::fmt::format_args_unchecked(fmt_str "" __VA_OPT__(, )##__VA_ARGS__); \
        static_assert(!::rcore::fmt::check_format<typename decltype(arguments)::Tuple>(fmt_str).has_value(), "Format error"); \
        return arguments; \
    }()
