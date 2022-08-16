#pragma once

#include <type_traits>

#include <rcore/fmt/arguments.hpp>
#include <rcore/fmt/format.hpp>

namespace rcore::panic {

struct Location {
    std::string_view function;
    std::string_view file;
    size_t line;
};

struct PanicInfo {
    const fmt::IDisplay &message;
    panic::Location location;
};

[[noreturn]] void panic(const PanicInfo &info);

using PanicHook = void (*)(const PanicInfo &);

void set_hook(PanicHook hook);

} // namespace rcore::panic

extern "C" {
// Without rstd you need to implement this function manually.
[[noreturn]] void rcore_panic_handler(const rcore::panic::PanicInfo &info);
}

#define rcore_panic_location() (::rcore::panic::Location{__FUNCTION__, __FILE__, __LINE__})

#define rcore_panic(...) \
    do { \
        if (::std::is_constant_evaluated()) { \
            assert(false); \
        } else { \
            const auto message = rcore_format_args("" __VA_ARGS__); \
            const ::rcore::panic::PanicInfo info{ \
                message, \
                rcore_panic_location(), \
            }; \
            ::rcore::panic::panic(info); \
        } \
    } while (false)

#define rcore_unimplemented(...) rcore_panic("Unimplemented. " __VA_ARGS__)

#define rcore_unreachable(...) \
    do { \
        __builtin_unreachable(); \
        rcore_panic("Unreachable code reached. " __VA_ARGS__); \
    } while (false)
