#pragma once

#include "tuple.hpp"
#include "variant.hpp"


namespace core {

template <typename T=Tuple<>>
class Option final {
private:
    Variant<Tuple<>, T> var;

public:
    Option() = default;
    Option(Variant<Tuple<>, T> &&v) :
        var(std::move(v))
    {}

    Option(const Option &) = default;
    Option &operator=(const Option &) = default;

    Option(Option &&) = default;
    Option &operator=(Option &&) = default;

    ~Option() = default;

    const Variant<Tuple<>, T> &as_variant() const {
        return var;
    }
    Variant<Tuple<>, T> &as_variant() {
        return var;
    }

    static Option None() {
        return Option(Variant<Tuple<>, T>::template create<0>(Tuple<>()));
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

    T unwrap() {
        if (!this->is_some()) {
            panic_("Option unwrap error");
        }
        return this->var.template take<1>();
    }
    T expect(const std::string &message) {
        if (!this->is_some()) {
            panic_("Option expect none:\n{}", message);
        }
        return this->var.template take<1>();
    }

    explicit operator bool() const {
        return bool(var);
    }
};

template <typename T>
struct fmt::Display<Option<T>> {
public:
    static void fmt(const Option<T> &t, std::ostream &o) {
        assert_(bool(t));
        if (t.is_some()) {
            o << "Some(";
            write_(o, t.get());
            o << ")";
        } else {
            o << "None";
        }
    }
};

} // namespace core
