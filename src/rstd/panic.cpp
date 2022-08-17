#include "panic.hpp"

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <string>
#include <type_traits>

#include <cxxabi.h>
#include <execinfo.h>
#include <pthread.h>
#include <unistd.h>

#include <rstd/fmt/format.hpp>
#include <rstd/thread/handle.hpp>

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
    auto &thread = rstd::thread::current();
    rstd::fmt::OstreamFormatter f(*thread.stdio.out);

    if (thread.is_main) {
        rstd_writeln(f, "");
        rstd::panic::write_backtrace(f);
        rstd_writeln(f, "");
        rstd_writeln(
            f,
            "Thread 'main' panicked in '{}', at '{}':{}",
            info.location.function,
            info.location.file,
            info.location.line //
        );
        rstd_writeln(f, "{}", info.message);
    } else {
        if (thread.panic_message != nullptr) {
            rstd::fmt::StringFormatter sf;
            rstd_write(sf, "{}", info.message);
            *thread.panic_message = sf.copy_string();
        }
    }

    if (thread.is_main) {
        std::abort();
    } else {
        pthread_exit(nullptr);
    }
}
