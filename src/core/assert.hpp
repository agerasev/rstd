#pragma once

#include "panic.hpp"

#if defined(__GNUC__) && !defined(__clang__)
#define _core_unlikely_branch(expr) (expr) [[unlikely]]
#else
#define _core_unlikely_branch(expr) (__builtin_expect(!!(expr), 0))
#endif

#define core_assert(value) \
    do { \
        if _core_unlikely_branch (!(value)) { \
            core_panic("Assertion failed: {} is false", #value); \
        } \
    } while (0)

#define core_assert_eq(left, right) \
    do { \
        if _core_unlikely_branch (!((left) == (right))) { \
            core_panic("Assertion failed: expected {} == {}, but got {} != {}", #left, #right, (left), (right)); \
        } \
    } while (0)

#define core_assert_ne(left, right) \
    do { \
        if _core_unlikely_branch (!((left) != (right))) { \
            core_panic("Assertion failed: expected {} != {}, but got {} == {}", #left, #right, (left), (right)); \
        } \
    } while (0)
