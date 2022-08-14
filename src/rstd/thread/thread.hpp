#pragma once

#include <functional>
#include <iostream>

#include <rcore/sync/lazy_static.hpp>

namespace rstd::thread {

class Thread final {
public:
    std::istream *in = nullptr;
    std::ostream *out = nullptr;
    std::ostream *err = nullptr;

    std::function<void(const std::string &)> panic_hook;

    bool is_main = true;
};

Thread &current();

} // namespace rstd::thread
