#include "panic.hpp"

#include <atomic>

namespace rcore::panic {

/// NOTE: Must subject to [constant initialization](https://en.cppreference.com/w/cpp/language/constant_initialization).
using PanicHook = std::atomic<void (*)()>;

// NOLINTNEXTLINE(*-avoid-non-const-global-variables)
static PanicHook PANIC_HOOK;

void set_hook(void (*hook)()) {
    PANIC_HOOK.store(hook);
}

[[noreturn]] void panic(const PanicInfo &info) {
    auto hook = PANIC_HOOK.load();
    if (hook != nullptr) {
        hook();
    }
    rcore_panic_handler(info);
}

} // namespace rcore::panic
