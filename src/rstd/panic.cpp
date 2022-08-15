#include "panic.hpp"

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <string>
#include <type_traits>

#include <cxxabi.h>
#include <execinfo.h>
#include <unistd.h>

#include <rstd/fmt/format.hpp>

namespace rstd::panic {

static void write_backtrace(fmt::IFormatter &f) {
    constexpr size_t MAX_SIZE = 64;
    void *array[MAX_SIZE];
    const size_t size = backtrace(array, MAX_SIZE);
    rstd_writeln(f, "Backtrace ({} items):", size);

    char **symbols = backtrace_symbols(array, size);
    if (symbols == nullptr) {
        rstd_writeln(f, "Error obtaining symbols");
        return;
    }
    const std::regex regex(R"((.*)\s*\((.*)\+([0-9a-fA-Fx]+)\)\s*\[(.*)\])");
    for (size_t i = 0; i < size; ++i) {
        const std::string symbol(symbols[i]);

        std::smatch match;
        if (!std::regex_match(symbol, match, regex)) {
            rstd_writeln(f, "{}: match failed {}", i, symbol);
            continue;
        }
        const std::string loc = match[1], shift = match[3], addr = match[4];

        std::string name = match[2].str();
        if (name.size() > 0) {
            int status = -4;
            char *name_ptr = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
            if (status == 0 && name_ptr != nullptr) {
                name = rstd_format("{} (demangled)", name_ptr);
                free(name_ptr);
            }
        } else {
            name = "(unnamed)";
        }

        rstd_writeln(f, "{}: {} + {}", i, loc, addr);
        rstd_writeln(f, "        {} + {}", name, shift);
    }
    free(symbols);
}

} // namespace rstd::panic

[[noreturn]] void rcore_panic_handler(const rcore::panic::PanicInfo &info) {
    rstd::fmt::OstreamFormatter f(std::cout);
    rstd_writeln(f, "");
    rstd::panic::write_backtrace(f);
    rstd_writeln(f, "");
    rstd_writeln(f, "Thread panicked in '{}', at '{}':{}", info.location.function, info.location.file, info.location.line);
    rstd_writeln(f, "{}", info.message);
    std::abort();
}
