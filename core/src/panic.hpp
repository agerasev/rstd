#pragma once

#include <core/io/print.hpp>

namespace core {

void set_panic_hook(void (*hook)());

namespace _impl {
void print_backtrace();
} // namespace _impl

[[noreturn]] void panic();

} // namespace core

#define core_panic(...) \
    do { \
        ::core::_impl::print_backtrace(); \
        core_println("Thread panicked in {}, at {}:{}", __FUNCTION__, __FILE__, __LINE__); \
        core_println(__VA_ARGS__); \
        ::core::panic(); \
    } while (false)

#define core_unimplemented(...) core_panic("Unimplemented. " __VA_ARGS__)

#define core_unreachable(...) \
    do { \
        __builtin_unreachable(); \
        core_panic("Unreachable code reached. " __VA_ARGS__); \
    } while (false)
