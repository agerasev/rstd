#pragma once

#include <optional>

#include <rcore/fmt/display.hpp>
//#include <core/panic.hpp>

namespace rcore {

template <typename T>
struct Some final {
    T value;

    constexpr explicit Some(const T &t) : value(t) {}
    constexpr explicit Some(T &&t) : value(std::move(t)) {}
};

struct None final {};

template <typename T>
struct Option final {
private:
    std::optional<T> optional_;

public:
    constexpr Option() = default;
    constexpr ~Option() = default;

    constexpr Option(const Some<T> &t) : optional_(std::move(t.value)) {}
    constexpr Option(Some<T> &&t) : optional_(std::move(t.value)) {}
    constexpr Option(None) : optional_(std::nullopt) {}

    constexpr Option(const Option &) = default;
    constexpr Option(Option &&) = default;
    constexpr Option &operator=(const Option &) = default;
    constexpr Option &operator=(Option &&) = default;

    constexpr Option(const std::optional<T> &opt) : optional_(opt){};
    constexpr Option(std::optional<T> &&opt) : optional_(opt){};

    [[nodiscard]] constexpr bool is_some() const {
        return this->optional_.has_value();
    }
    [[nodiscard]] constexpr bool is_none() const {
        return !this->optional_.has_value();
    }

    constexpr const T &some() const {
        return this->optional_.value();
    }
    constexpr T &some() {
        return this->optional_.value();
    }
    /*
    T unwrap() {
        if (this->is_none()) {
            core_panic("Option is None");
        }
        return std::move(this->some());
    }
    void unwrap_none() {
        if (this->is_some()) {
            if constexpr (fmt::Displayable<T>) {
                core_panic("Option is Some({})", this->some());
            } else {
                core_panic("Option is Some");
            }
        }
    }
    */
    constexpr bool operator==(const Option &other) const {
        return this->optional_ == other.optional_;
    }
    constexpr bool operator==(const Some<T> &other) const {
        return this->is_some() && this->some() == other.value;
    }
    constexpr bool operator==(None) const {
        return this->is_none();
    }
    constexpr bool operator!=(const Option &other) const {
        return this->optional_ != other.optional_;
    }
    constexpr bool operator!=(const Some<T> &other) const {
        return !this->is_some() || this->some() != other.value;
    }
    constexpr bool operator!=(None) const {
        return !this->is_none();
    }
};

namespace fmt {

template <typename T>
struct Display<Some<T>> {
    static void fmt(const Some<T> &self, Formatter &f) {
        f.write_str("Some(");
        if constexpr (Displayable<T>) {
            Display<T>::fmt(self.value, f);
        }
        f.write_str(")");
    }
};

template <>
struct Display<None> {
    inline static void fmt(None, Formatter &f) {
        f.write_str("None");
    }
};

template <typename T>
struct Display<Option<T>> {
    static void fmt(const Option<T> &self, Formatter &f) {
        f.write_str("Option::");
        if (self.is_some()) {
            f.write_str("Some(");
            if constexpr (Displayable<T>) {
                Display<T>::fmt(self.some(), f);
            }
            f.write_str(")");
        } else {
            f.write_str("None");
        }
    }
};

} // namespace fmt

} // namespace rcore
