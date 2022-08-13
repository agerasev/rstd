#pragma once

#include <string>

#include <core/fmt/display.hpp>

namespace core {

namespace io {

enum class ErrorKind {
    NotFound,
    InvalidData,
    UnexpectedEof,
    TimedOut,
    Other,
};

struct Error final {
    ErrorKind kind;
    std::string message;

    inline Error(ErrorKind kind, std::string message = "") : kind(kind), message(message) {}
};

} // namespace io

namespace fmt {

template <>
struct Display<io::ErrorKind> {
    inline static void print(std::ostream &o, const io::ErrorKind &ek) {
        switch (ek) {
        case io::ErrorKind::NotFound:
            o << "Not Found";
            break;
        case io::ErrorKind::UnexpectedEof:
            o << "Unexpected Eof";
            break;
        case io::ErrorKind::InvalidData:
            o << "Invalid Data";
            break;
        case io::ErrorKind::TimedOut:
            o << "Timed Out";
            break;
        case io::ErrorKind::Other:
            o << "Other";
            break;
        }
    }
};

template <>
struct Display<io::Error> {
    inline static void print(std::ostream &o, const io::Error &e) {
        core_print_stream(o, "io::Error({}: {})", e.kind, e.message);
    }
};

} // namespace fmt

} // namespace core
