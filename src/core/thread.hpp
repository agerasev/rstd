#pragma once

#include <iostream>
#include <functional>
#include "io.hpp"


namespace core {

class Thread {
public:
    std::function<void(const std::string &)> panic_hook;
    core::StdIo stdio;
};

namespace thread {

Thread &current();

} // namespace thread

} // namespace core
