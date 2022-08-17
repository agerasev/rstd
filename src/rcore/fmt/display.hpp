#pragma once

#include <concepts>
#include <string>

namespace rcore::fmt {

class IFormatter {
public:
    virtual void write_str(std::string_view s) = 0;
    virtual void write_char(char c) = 0;
};

template <typename Self>
struct Display;

template <typename Self>
concept Displayable = requires(const Self &self, IFormatter &f) {
    Display<std::remove_cvref_t<Self>>::fmt(self, f);
};

class IDisplay {
public:
    virtual void fmt(IFormatter &f) const = 0;
};

template <std::derived_from<IDisplay> Self>
struct Display<Self> {
    static void fmt(const Self &self, IFormatter &f) {
        self.fmt(f);
    }
};

template <typename Self>
struct Display<const Self &> {
    static void fmt(const Self &self, IFormatter &f) {
        self.fmt(f);
    }
};

namespace _impl {

template <typename Self>
concept ToString = requires(Self self) {
    std::to_string(self);
};

} // namespace _impl

template <std::convertible_to<std::string_view> Self>
struct Display<Self> {
    static void fmt(const Self &self, IFormatter &f) {
        f.write_str(std::string_view(self));
    }
};

template <_impl::ToString Self>
struct Display<Self> {
    static void fmt(const Self &self, IFormatter &f) {
        // FIXME: Stringify manually without string allocation.
        const auto str = std::to_string(self);
        f.write_str(str);
    }
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
