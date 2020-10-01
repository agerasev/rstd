#pragma once

#include "tuple.hpp"
#include "variant.hpp"


namespace rstd {

template <typename T=Tuple<>>
class Option final {
private:
    Variant<Tuple<>, T> var;

public:
    Option() : Option(Variant<Tuple<>, T>::template create<0>(Tuple<>())) {}

    Option(const Option &) = default;
    Option &operator=(const Option &) = default;

    Option(Option &&other) : var(std::move(other.var)) {
        other.var.template put<0>(Tuple<>());
    }
    Option &operator=(Option &&other) {
        var.try_destroy();
        var = std::move(other.var);
        other.var.template put<0>(Tuple<>());
    }

    ~Option() = default;

private:
    Option(Variant<Tuple<>, T> &&v) :
        var(std::move(v))
    {}

    const Variant<Tuple<>, T> &as_variant() const {
        return var;
    }
    Variant<Tuple<>, T> &as_variant() {
        return var;
    }

public:
    static Option None() {
        return Option();
    }
    static Option Some(T &&x) {
        return Option(Variant<Tuple<>, T>::template create<1>(std::move(x)));
    }
    static Option Some(const T &x) {
        return Option(Variant<Tuple<>, T>::template create<1>(x));
    }

    bool is_none() const {
        return var.id() == 0;
    }
    bool is_some() const {
        return var.id() == 1;
    }

    T &get() {
        return this->var.template get<1>();
    }
    const T &get() const {
        return this->var.template get<1>();
    }

private:
    T take_some() {
#ifdef DEBUG
        assert_(this->is_some());
#endif // DEBUG
        T v(std::move(this->var.template take<1>()));
        this->var.template put<0>(Tuple<>());
        return v;
    }

public:
    Option take() {
        if (this->is_some()) {
            return Option<T>::Some(this->take_some());
        } else {
            return Option<T>::None();
        }
    }

    T unwrap() {
        if (!this->is_some()) {
            panic_("Option is None");
        }
        return this->take_some();
    }
    T expect(const std::string &message) {
        if (!this->is_some()) {
            panic_("Option expect Some:\n{}", message);
        }
        return this->take_some();
    }

    void unwrap_none() {
        if (!this->is_none()) {
            panic_("Option is Some");
        }
    }
    void expect_none(const std::string &message) {
        if (!this->is_none()) {
            panic_("Option expect None:\n{}", message);
        }
    }
};

template <typename T>
struct fmt::Display<Option<T>> {
public:
    static void fmt(const Option<T> &t, std::ostream &o) {
        if (t.is_some()) {
            o << "Some(";
            write_(o, t.get());
            o << ")";
        } else {
            o << "None";
        }
    }
};

} // namespace rstd
