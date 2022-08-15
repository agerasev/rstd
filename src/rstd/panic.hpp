#pragma once

#include <rcore/panic.hpp>

namespace rstd::panic {
using rcore::panic::panic;
using rcore::panic::PanicHook;
using rcore::panic::PanicInfo;
using rcore::panic::set_hook;
} // namespace rstd::panic

#define rstd_panic(...) rcore_panic(__VA_ARGS__)
#define rstd_unimplemented(...) rcore_unimplemented(__VA_ARGS__)
#define rstd_unreachable(...) rcore_unreachable(__VA_ARGS__)
