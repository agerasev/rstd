#pragma once

#include <optional>

#include <rcore/assert.hpp>
#include <rcore/fmt/display.hpp>
#include <rcore/panic.hpp>

namespace rcore {

template <typename T>
struct Some final {
    T value;

    constexpr explicit Some(const T &t) : value(t) {}
    constexpr explicit Some(T &&t) : value(std::move(t)) {}
};

struct None final {};

/// Value that can be either `Some(...)` or `None`.
///
/// NOTE: `Option` sets the source to `None` on move.
///       This behavior is different from `std::optional`.
template <typename T>
struct Option final {
private:
    std::optional<T> optional_;

public:
    constexpr Option() = default;
    constexpr ~Option() = default;

    constexpr Option(const Option &) = default;
    constexpr Option &operator=(const Option &) = default;
    constexpr Option(Option &&other) : optional_(std::move(other.optional_)) {
        other.optional_.reset();
    }
    constexpr Option &operator=(Option &&other) {
        optional_ = std::move(other.optional_);
        other.optional_.reset();
        return *this;
    }

    constexpr Option(const Some<T> &t) : optional_(t.value) {}
    constexpr Option &operator=(const Some<T> &t) {
        optional_ = t.value;
        return *this;
    }
    constexpr Option(Some<T> &&t) : optional_(std::move(t.value)) {}
    constexpr Option &operator=(Some<T> &&t) {
        optional_ = std::move(t.value);
        return *this;
    }
    constexpr Option(None) : optional_(std::nullopt) {}
    constexpr Option &operator=(None) {
        optional_ = std::nullopt;
        return *this;
    }

    constexpr Option(const std::optional<T> &opt) : optional_(opt) {}
    constexpr Option &operator=(const std::optional<T> &opt) {
        optional_ = opt;
        return *this;
    }
    constexpr Option(std::optional<T> &&opt) : optional_(std::move(opt)) {
        opt.reset();
    }
    constexpr Option &operator=(std::optional<T> &&opt) {
        optional_ = std::move(opt);
        opt.reset();
        return *this;
    }
    constexpr Option(std::nullopt_t) : optional_(std::nullopt) {}
    constexpr Option &operator=(std::nullopt_t) {
        optional_ = std::nullopt;
        return *this;
    }

    [[nodiscard]] constexpr bool is_some() const {
        return this->optional_.has_value();
    }
    [[nodiscard]] constexpr bool is_none() const {
        return !this->optional_.has_value();
    }

    [[nodiscard]] constexpr const T &some() const {
        rcore_assert(this->is_some());
        return this->optional_.value();
    }
    [[nodiscard]] constexpr T &some() {
        rcore_assert(this->is_some());
        return this->optional_.value();
    }

    [[nodiscard]] constexpr Option take() {
        return std::move(*this);
    }
    [[nodiscard]] constexpr T take_some() {
        T value = std::move(this->some());
        this->optional_.reset();
        return value;
    }

    [[nodiscard]] constexpr T unwrap() {
        if (this->is_none()) {
            rcore_panic("Option is None");
        } else {
            return take_some();
        }
    }
    constexpr void unwrap_none() {
        if (this->is_some()) {
            if constexpr (fmt::Displayable<T>) {
                rcore_panic("Option is Some({})", this->some());
            } else {
                rcore_panic("Option is Some");
            }
        }
    }

    [[nodiscard]] constexpr bool operator==(const Option &other) const {
        return this->optional_ == other.optional_;
    }
    [[nodiscard]] constexpr bool operator==(const Some<T> &other) const {
        return this->is_some() && this->some() == other.value;
    }
    [[nodiscard]] constexpr bool operator==(None) const {
        return this->is_none();
    }
    [[nodiscard]] constexpr bool operator!=(const Option &other) const {
        return this->optional_ != other.optional_;
    }
    [[nodiscard]] constexpr bool operator!=(const Some<T> &other) const {
        return !this->is_some() || this->some() != other.value;
    }
    [[nodiscard]] constexpr bool operator!=(None) const {
        return !this->is_none();
    }
};

namespace fmt {

template <typename T>
struct Display<Some<T>> {
    static void fmt(const Some<T> &self, IFormatter &f) {
        f.write_str("Some(");
        if constexpr (Displayable<T>) {
            Display<T>::fmt(self.value, f);
        }
        f.write_str(")");
    }
};

template <>
struct Display<None> {
    inline static void fmt(None, IFormatter &f) {
        f.write_str("None");
    }
};

template <typename T>
struct Display<Option<T>> {
    static void fmt(const Option<T> &self, IFormatter &f) {
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
