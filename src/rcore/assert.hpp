#pragma once

#include <type_traits>

#include "panic.hpp"

#if defined(__GNUC__) && !defined(__clang__)
#define rcore_likely_branch(expr) (expr) [[likely]]
#define rcore_unlikely_branch(expr) (expr) [[unlikely]]
#else
#define rcore_likely_branch(expr) (__builtin_expect(!(expr), 0))
#define rcore_unlikely_branch(expr) (__builtin_expect(!!(expr), 0))
#endif

#define rcore_assert(value) \
    do { \
        if (::std::is_constant_evaluated()) { \
            assert(!!(value)); \
        } else { \
            if rcore_unlikely_branch (!(value)) { \
                rcore_panic("Assertion failed: {} is false", #value); \
            } \
        } \
    } while (false)

#define rcore_assert_eq(left, right) \
    do { \
        if (::std::is_constant_evaluated()) { \
            assert((left) == (right)); \
        } else { \
            if rcore_unlikely_branch (!((left) == (right))) { \
                rcore_panic("Assertion failed: expected {} == {}, but got {} != {}", #left, #right, (left), (right)); \
            } \
        } \
    } while (false)

#define rcore_assert_ne(left, right) \
    do { \
        if (::std::is_constant_evaluated()) { \
            assert((left) != (right)); \
        } else { \
            if rcore_unlikely_branch (!((left) != (right))) { \
                rcore_panic("Assertion failed: expected {} != {}, but got {} == {}", #left, #right, (left), (right)); \
            } \
        } \
    } while (false)
