#pragma once

#include "_impl/panic.hpp"
#include "format.hpp"

namespace core {

using _impl::print_backtrace;

} // namespace core

#define core_panic(...) \
    do { \
        ::core::_impl::print_backtrace(); \
        core_println("Thread panicked in {}, at {}:{}", __FUNCTION__, __FILE__, __LINE__); \
        core_println(__VA_ARGS__); \
        ::core::_impl::panic(); \
    } while (false)

#define core_unimplemented(...) core_panic("Unimplemented. " __VA_ARGS__)

#define core_unreachable(...) \
    do { \
        __builtin_unreachable(); \
        core_panic("Unreachable code reached. " __VA_ARGS__); \
    } while (false)
