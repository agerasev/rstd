#pragma once

#include <iostream>
#include <functional>
#include "io.hpp"


namespace core {

class Thread {
public:
    core::StdIo stdio;
    std::function<void(const std::string &)> panic_hook;
};

namespace thread {

Thread &current();

} // namespace thread

} // namespace core
