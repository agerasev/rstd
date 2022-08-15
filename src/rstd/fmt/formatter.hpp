#pragma once

#include <iostream>
#include <sstream>

#include <rcore/fmt/formatter.hpp>

namespace rstd::fmt {

using rcore::fmt::IFormatter;

class OstreamFormatter final : public IFormatter {
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

class StringFormatter final : public IFormatter {
private:
    std::stringstream sstream;

public:
    void write_str(std::string_view s) override {
        sstream << s;
    }
    void write_char(char c) override {
        sstream << c;
    }

    std::string copy_string() const {
        return sstream.str();
    }
    void clear() {
        sstream.clear();
    }
};

} // namespace rstd::fmt
