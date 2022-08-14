#pragma once

#include <iostream>

#include <rcore/fmt/formatter.hpp>

namespace rstd::fmt {

using rcore::fmt::Formatter;

class OstreamFormatter final : public Formatter {
private:
    std::ostream &ostream;

public:
    explicit OstreamFormatter(std::ostream &ostream) : ostream(ostream) {}

    void write_str(std::string_view s) override {
        ostream << s;
    }
    void write_char(char c) override {
        ostream << c;
    }
};

} // namespace rstd::fmt
