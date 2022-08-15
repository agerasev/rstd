#pragma once

#include <type_traits>
#include <variant>

#include <rcore/fmt/display.hpp>
//#include <rcore/panic.hpp>

namespace rcore {

template <typename T>
struct Ok final {
    T value;

    constexpr explicit Ok(const T &t) : value(t) {}
    constexpr explicit Ok(T &&t) : value(std::move(t)) {}
};

template <typename E>
struct Err final {
    E value;

    constexpr explicit Err(const E &e) : value(e) {}
    constexpr explicit Err(E &&e) : value(std::move(e)) {}
};

template <typename T, typename E>
struct [[nodiscard]] Result final {
private:
    std::variant<E, T> variant_;

public:
    constexpr Result() = default;
    constexpr ~Result() = default;

    constexpr Result(const Ok<T> &t) : variant_(std::in_place_index<1>, std::move(t.value)) {}
    constexpr Result(const Err<E> &e) : variant_(std::in_place_index<0>, std::move(e.value)) {}
    constexpr Result(Ok<T> &&t) : variant_(std::in_place_index<1>, std::move(t.value)) {}
    constexpr Result(Err<E> &&e) : variant_(std::in_place_index<0>, std::move(e.value)) {}

    constexpr Result(const Result &) = default;
    constexpr Result(Result &&) = default;
    constexpr Result &operator=(const Result &) = default;
    constexpr Result &operator=(Result &&) = default;

    [[nodiscard]] constexpr bool is_ok() const {
        return this->variant_.index() == 1;
    }
    [[nodiscard]] constexpr bool is_err() const {
        return this->variant_.index() == 0;
    }

    [[nodiscard]] constexpr const T &ok() const {
        return std::get<1>(this->variant_);
    }
    [[nodiscard]] constexpr const E &err() const {
        return std::get<0>(this->variant_);
    }
    constexpr T &ok() {
        return std::get<1>(this->variant_);
    }
    constexpr E &err() {
        return std::get<0>(this->variant_);
    }

    /*
    T unwrap() {
        if (this->is_err()) {
            if constexpr (fmt::Displayable<E>) {
                core_panic("Result is Err({})", this->err());
            } else {
                core_panic("Result is Err");
            }
        }
        return std::move(this->ok());
    }
    E unwrap_err() {
        if (this->is_ok()) {
            if constexpr (fmt::Displayable<T>) {
                core_panic("Result is Ok({})", this->ok());
            } else {
                core_panic("Result is Ok");
            }
        }
        return std::move(this->err());
    }
    */

    constexpr bool operator==(const Result &other) const {
        return this->variant_ == other.variant_;
    }
    constexpr bool operator==(const Ok<T> &other) const {
        return this->is_ok() && this->ok() == other.value;
    }
    constexpr bool operator==(const Err<E> &other) const {
        return this->is_err() && this->err() == other.value;
    }
    constexpr bool operator!=(const Result &other) const {
        return this->variant_ != other.variant_;
    }
    constexpr bool operator!=(const Ok<T> &other) const {
        return !this->is_ok() || this->ok() != other.value;
    }
    constexpr bool operator!=(const Err<E> &other) const {
        return !this->is_err() || this->err() != other.value;
    }
};

namespace fmt {

template <typename T>
struct Display<Ok<T>> {
    static void fmt(const Ok<T> &self, IFormatter &f) {
        f.write_str("Ok)(");
        if constexpr (Displayable<T>) {
            Display<T>::fmt(self.value, f);
        }
        f.write_str(")");
    }
};

template <typename E>
struct Display<Err<E>> {
    static void fmt(const Err<E> &self, IFormatter &f) {
        f.write_str("Er)r(");
        if constexpr (Displayable<E>) {
            Display<E>::fmt(self.value, f);
        }
        f.write_str(")");
    }
};

template <typename T, typename E>
struct Display<Result<T, E>> {
    static void fmt(const Result<T, E> &self, IFormatter &f) {
        f.write_str("Result::");
        if (self.is_ok()) {
            f.write_str("Ok(");
            if constexpr (Displayable<T>) {
                Display<T>::fmt(self.ok(), f);
            }
        } else {
            f.write_str("Err(");
            if constexpr (Displayable<E>) {
                Display<E>::fmt(self.err(), f);
            }
        }
        f.write_str(")");
    }
};

} // namespace fmt

} // namespace rcore
