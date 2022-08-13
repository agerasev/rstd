#pragma once

#include <concepts>
#include <cstdint>
#include <iostream>

namespace core {

namespace _impl {

template <typename T>
concept IsStdPrintable = requires(std::ostream &stream, const T &value) {
    { stream << value } -> std::same_as<std::ostream &>;
};

} // namespace _impl

template <typename T>
struct Display {};

template <typename T>
concept IsDisplay = requires(std::ostream &stream, const T &value) {
    Display<T>::write(stream, value);
};


template <typename T>
    requires _impl::IsStdPrintable<T>
struct Display<T> {
    static void write(std::ostream &stream, const T &value) {
        stream << value;
    }
};

template <>
struct Display<uint8_t> {
    static void write(std::ostream &stream, uint8_t value) {
        stream << uint32_t(value);
    }
};
template <>
struct Display<int8_t> {
    static void write(std::ostream &stream, int8_t value) {
        stream << int32_t(value);
    }
};
template <>
struct Display<bool> {
    static void write(std::ostream &stream, bool value) {
        stream << (value ? "true" : "false");
    }
};

} // namespace core
