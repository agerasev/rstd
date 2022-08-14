#pragma once

#include <concepts>
#include <cstdint>
#include <iostream>

#include "formatter.hpp"

namespace rcore::fmt {

template <typename Self>
struct Display;

template <typename Self>
concept IsDisplay = requires(const Self &self, Formatter &f) {
    Display<Self>::fmt(f, self);
};
/*
template <>
struct Display<uint8_t> {
    static void fmt(std::ostream &stream, uint8_t value) {
        stream << uint32_t(value);
    }
};
template <>
struct Display<int8_t> {
    static void fmt(std::ostream &stream, int8_t value) {
        stream << int32_t(value);
    }
};
template <>
struct Display<bool> {
    static void fmt(std::ostream &stream, bool value) {
        stream << (value ? "true" : "false");
    }
};
*/
} // namespace rcore::fmt
