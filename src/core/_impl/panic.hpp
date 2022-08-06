#pragma once

namespace core::_impl {

void set_panic_hook(void (*hook)());

void print_backtrace();
[[noreturn]] void panic();

} // namespace core::_impl
