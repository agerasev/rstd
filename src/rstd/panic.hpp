#pragma once

#include <rcore/panic.hpp>

#define rstd_panic(...) rcore_panic(__VA_ARGS__)
#define rstd_unimplemented(...) rcore_unimplemented(__VA_ARGS__)
#define rstd_unreachable(...) rcore_unreachable(__VA_ARGS__)
