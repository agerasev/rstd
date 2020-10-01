#pragma once

#include <iostream>
#include <cstdlib>

namespace rstd {

// FIXME: Print call stack trace
[[ noreturn ]] inline void panic(const std::string &message="") {
    std::cerr
        << "Thread panicked: " << std::endl
        << message << std::endl;
    abort();
}

} // namespace rstd
