#pragma once

#include "tuple.hpp"
#include "variant.hpp"


namespace rstd {

template <typename T=Tuple<>, typename E=Tuple<>>
class Result final {
private:
    Variant<T, E> var;

public:
    Result() = default;
    Result(Variant<T, E> &&v) : var(std::move(v)) {}

    Result(const Result &) = default;
    Result &operator=(const Result &) = default;

    Result(Result &&) = default;
    Result &operator=(Result &&) = default;

    ~Result() {
#ifdef DEBUG
        if (this->is_some()) {
            panic_("Unhandled result");
        }
#endif // DEBUG
    }
    void clear() {
        drop(var);
    }

    const Variant<T, E> &as_variant() const {
        return var;
    }
    Variant<T, E> &as_variant() {
        return var;
    }

    static Result Ok(T &&x) {
        return Result(Variant<T, E>::template create<0>(std::move(x)));
    }
    static Result Ok(const T &x) {
        return Result(Variant<T, E>::template create<0>(x));
    }
    static Result Err(E &&x) {
        return Result(Variant<T, E>::template create<1>(std::move(x)));
    }
    static Result Err(const E &x) {
        return Result(Variant<T, E>::template create<1>(x));
    }

    bool is_ok() const {
        return var.id() == 0;
    }
    bool is_err() const {
        return var.id() == 1;
    }
    bool is_some() const {
        return var.is_some();
    }
    bool is_none() const {
        return var.is_none();
    }
    explicit operator bool() const {
        return this->is_some();
    }

    T &_get() {
        return this->var.template _get<0>();
    }
    const T &_get() const {
        return this->var.template _get<0>();
    }
    T &get() {
        return this->var.template get<0>();
    }
    const T &get() const {
        return this->var.template get<0>();
    }
    E &_get_err() {
        return this->var.template _get<1>();
    }
    const E &_get_err() const {
        return this->var.template _get<1>();
    }
    E &get_err() {
        return this->var.template get<1>();
    }
    const E &get_err() const {
        return this->var.template get<1>();
    }

    T _take_ok() {
        return this->var.template _take<0>();
    }
    T take_ok() {
        return this->var.template take<0>();
    }
    E _take_err() {
        return this->var.template _take<1>();
    }
    E take_err() {
        return this->var.template take<1>();
    }

    Result take() {
        return Result(std::move(*this));
    }

    T unwrap() {
        if (!this->is_ok()) {
            panic_("Result unwrap:\n{}", this->get_err());
        }
        return this->var.template take<0>();
    }
    E unwrap_err() {
        if (!this->is_err()) {
            panic_("Result unwrap_err failed");
        }
        return this->var.template take<1>();
    }
    T expect(const std::string &message) {
        if (!this->is_ok()) {
            panic_("Result expect:\n{}\n{}", this->get_err(), message);
        }
        return this->var.template take<0>();
    }
    T expect_err(const std::string &message) {
        if (!this->is_ok()) {
            panic_("Result expect_err failed:\n{}", message);
        }
        return this->var.template take<0>();
    }
};

template <typename T, typename E>
struct fmt::Display<Result<T, E>> {
public:
    static void fmt(const Result<T, E> &t, std::ostream &o) {
        assert_(t.is_some());
        if (t.is_ok()) {
            o << "Ok(";
            write_(o, t.get());
            o << ")";
        } else {
            o << "Err(";
            write_(o, t.get_err());
            o << ")";
        }
    }
};

} // namespace rstd
