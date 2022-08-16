#pragma once

// TODO: Migrate to std::format when supported.

#include <cassert> // We use `cassert` it because `rcore_assert` depends on `format`.
#include <optional>
#include <string>
#include <tuple>

#include "arguments.hpp"
#include "display.hpp"

#define rcore_write(f, ...) (rcore_format_args(__VA_ARGS__).fmt(f))
#define rcore_writeln(f, fmt_str, ...) rcore_write(f, fmt_str "\n" __VA_OPT__(, )##__VA_ARGS__)
