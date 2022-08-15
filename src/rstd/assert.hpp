#pragma once

#include <rcore/assert.hpp>

#define rstd_assert(...) rcore_assert(__VA_ARGS__)
#define rstd_assert_eq(...) rcore_assert_eq(__VA_ARGS__)
#define rstd_assert_ne(...) rcore_assert_ne(__VA_ARGS__)
