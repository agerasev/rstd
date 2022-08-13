#include "panic.hpp"

#include <atomic>
#include <iostream>
#include <regex>
#include <string>
#include <type_traits>

#include <cstdlib>
#include <cstring>
#include <execinfo.h>
#include <unistd.h>

#include <cxxabi.h>

namespace core {

/// NOTE: Must subject to [constant initialization](https://en.cppreference.com/w/cpp/language/constant_initialization).
using PanicHook = std::atomic<void (*)()>;
// NOLINTNEXTLINE(*-avoid-non-const-global-variables)
static PanicHook PANIC_HOOK;

void set_panic_hook(void (*hook)()) {
    PANIC_HOOK.store(hook);
}

namespace _impl {

void print_backtrace() {
    static const size_t MAX_SIZE = 64;
    void *array[MAX_SIZE];
    const size_t size = backtrace(array, MAX_SIZE);
    std::cout << "BACKTRACE (" << size << "):" << std::endl;

    char **symbols = backtrace_symbols(array, size);
    if (symbols == nullptr) {
        std::cout << "Error obtaining symbols" << std::endl;
        return;
    }
    const std::regex regex("(.*)\\s*\\((.*)\\+(0x[0-9a-fA-F]+)\\)\\s*\\[(.*)\\]");
    for (size_t i = 0; i < size; ++i) {
        const std::string symbol(symbols[i]);

        std::smatch match;
        if (!std::regex_match(symbol, match, regex)) {
            std::cout << symbol << ": match failed" << std::endl;
            continue;
        }
        const std::string loc = match[1], shift = match[3], addr = match[4];

        std::string name = match[2];
        if (name.size() > 0) {
            int status = -4;
            char *name_ptr = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
            if (status == 0 && name_ptr != nullptr) {
                name = std::string(name_ptr);
                free(name_ptr);
            } else {
                std::cout << "demangle status: " << status << std::endl;
                name = match[2].str();
            }
        }

        std::cout << "    " << loc << " (" << name << " + " << shift << ") [" << addr << "]" << std::endl;
    }
    free(symbols);
}

} // namespace _impl

[[noreturn]] void panic() {
    auto hook = PANIC_HOOK.load();
    if (hook != nullptr) {
        hook();
    } else {
        std::abort();
    }
}

} // namespace core
