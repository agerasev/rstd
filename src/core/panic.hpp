#pragma once

#include <iostream>
#include <cstdlib>

namespace core {

// FIXME: Print call stack trace
[[ noreturn ]] void _panic(
    const char *loc, int ln,
    const std::string &message=""
) {
    std::cerr
        << "Thread panicked: " << std::endl
        << loc << ":" << ln  << ": " << std::endl
        << message << std::endl;
    abort();
}

} // namespace core
