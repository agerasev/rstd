#pragma once


#include <iostream>

namespace rcore {

class StdIo {
public:
    std::istream *in = nullptr;
    std::ostream *out = nullptr;
    std::ostream *err = nullptr;
};

std::istream &stdin_();
std::ostream &stdout_();
std::ostream &stderr_();

} // namespace rcore
