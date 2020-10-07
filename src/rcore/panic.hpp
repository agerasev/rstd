#pragma once

#include <string>
#include <functional>


namespace rcore {

std::function<void(const std::string &)> panic_hook();

[[ noreturn ]] void panic(const std::string &message="");

} // namespace rcore
