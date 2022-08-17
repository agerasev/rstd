#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <rstd/panic.hpp>

namespace rstd::thread {

struct Stdio {
    std::istream *in = &std::cin;
    std::ostream *out = &std::cout;
    std::ostream *err = &std::cerr;
};

using PanicHook = std::function<void(const panic::PanicInfo &)>;

struct Thread final {
    Stdio stdio;
    PanicHook panic_hook;
    bool is_main = true;

    std::shared_ptr<std::string> panic_message;
};

[[nodiscard]] Thread &current();

} // namespace rstd::thread
