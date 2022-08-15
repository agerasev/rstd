#include "panic.hpp"

#include <atomic>

namespace rcore::panic {

// NOLINTNEXTLINE(*-avoid-non-const-global-variables)
static constinit std::atomic<PanicHook> PANIC_HOOK;

void set_hook(PanicHook hook) {
    PANIC_HOOK.store(hook);
}

[[noreturn]] void panic(const PanicInfo &info) {
    auto hook = PANIC_HOOK.load();
    if (hook != nullptr) {
        hook(info);
    }
    rcore_panic_handler(info);
}

} // namespace rcore::panic
