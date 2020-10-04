#pragma once

#include "macros.hpp"
#include "format.hpp"


namespace rstd {

inline void _assert(
    const char *expr, bool value
) {
    if (!value) {
        panic(
            "Assertion failed: {}", expr
        );
    }
}

template <typename T0, typename T1>
inline void _assert_eq(
    const char *e0, const char *e1,
    const T0 &v0, const T1 &v1
) {
    if (!(v0 == v1)) {
        panic(
            "Assertion failed: {} == {}\n{} != {}",
            e0, e1, v0, v1
        );
    }
}

} // namespace rstd


#define assert_(x) ::rstd::_assert(#x, (x))
#define assert_eq_(a, b) ::rstd::_assert_eq(#a, #b, (a), (b))
