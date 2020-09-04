#pragma once

#include "tuple.hpp"
#include "variant.hpp"


namespace core {

template <typename T, typename E>
class Result final {
private:
    Variant<T, E> var;

public:
    Result() = default;
    Result(Variant<T, E> &&v) :
        var(std::move(v))
    {}

    Result(const Result &) = default;
    Result &operator=(const Result &) = default;

    Result(Result &&) = default;
    Result &operator=(Result &&) = default;

    ~Result() = default;

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

    T &get() {
        return this->var.template get<0>();
    }
    const T &get() const {
        return this->var.template get<0>();
    }
    E &get_err() {
        return this->var.template get<1>();
    }
    const E &get_err() const {
        return this->var.template get<1>();
    }

    T unwrap() {
        if (!this->is_ok()) {
            panic_("Result unwrap error:\n{}", this->get_err());
        }
        return this->var.template take<0>();
    }
    E unwrap_err() {
        if (!this->is_err()) {
            panic_("Result unwrap_err failed");
        }
        return this->var.template take<1>();
    }

    operator bool() const {
        return bool(var);
    }
};

template <typename T, typename E>
struct fmt::Display<Result<T, E>> {
public:
    static void fmt(const Result<T, E> &t, std::ostream &o) {
        assert_(t);
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

} // namespace core
