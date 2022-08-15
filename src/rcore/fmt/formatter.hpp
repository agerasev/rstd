#pragma once

#include <string>

namespace rcore::fmt {

class IFormatter {
public:
    virtual void write_str(std::string_view s) = 0;
    virtual void write_char(char c) = 0;
};

} // namespace rcore::fmt
