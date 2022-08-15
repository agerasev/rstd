#pragma once

#include <string>
#include <tuple>

#include "display.hpp"
#include "format.hpp"
#include "formatter.hpp"

namespace rcore::fmt {

template <Displayable... Ts>
class Arguments final : public IDisplay {
private:
    std::string_view fmt_str;
    std::tuple<Ts...> args;

public:
    Arguments(std::string_view fmt_str, Ts &&...args) : fmt_str(fmt_str), args(std::forward<Ts>(args)...) {}

    void fmt(IFormatter &f) const override {
        std::apply(write_unchecked<Ts...>, std::tuple_cat(std::tuple<IFormatter &, std::string_view>(f, fmt_str), args));
    }
};

template <const char *FMT_STR, Displayable... Ts>
Arguments<Ts...> format_args(Ts &&...args) {
    constexpr std::string_view fmt_str(FMT_STR);
    constexpr auto error = check_format_str<Ts...>(fmt_str);
    static_assert(!error.has_value(), "Format error");
    return Arguments<Ts...>(fmt_str, std::forward<Ts>(args)...);
}

} // namespace rcore::fmt

#define rcore_format_args(lit, ...) \
    [&]() { \
        static constexpr const char fmt_str[] = (lit); \
        return ::rcore::fmt::format_args<fmt_str>(__VA_ARGS__); \
    }()
