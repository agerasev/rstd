#pragma once

#include "tuple.hpp"
#include "variant.hpp"


namespace rstd {

template <typename T=Tuple<>>
class Option final {
private:
    Variant<T> var;

public:
    Option() = default;
    Option(Variant<T> &&v) : var(std::move(v)) {}
    Option(T &&x) : Option(Variant<T>::template create<0>(std::move(x))) {}
    Option(const T &x) : Option(Variant<T>::template create<0>(x)) {}

    Option(const Option &) = default;
    Option &operator=(const Option &) = default;

    Option(Option &&other) = default;
    Option &operator=(Option &&other) = default;

    ~Option() = default;

    const Variant<T> &as_variant() const {
        return var;
    }
    Variant<T> &as_variant() {
        return var;
    }

    static Option None() {
        return Option();
    }
    static Option Some(T &&x) {
        return Option(std::move(x));
    }
    static Option Some(const T &x) {
        return Option(x);
    }

    bool is_some() const {
        return var.is_some();
    }
    bool is_none() const {
        return var.is_none();
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

    T _take_some() {
        return this->var.template _take<0>();
    }
    T take_some() {
        return this->var.template take<0>();
    }

    Option take() {
        return Option(std::move(*this));
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

    template <typename FSome, typename FNone>
    decltype(auto) match(FSome fsome, FNone fnone) {
        if (this->is_some()) {
            return fsome(this->_get());
        } else {
            return fnone();
        }
    }
    template <typename FSome, typename FNone>
    decltype(auto) match(FSome fsome, FNone fnone) const {
        if (this->is_some()) {
            return fsome(this->_get());
        } else {
            return fnone();
        }
    }

    template <typename FSome, typename FNone>
    decltype(auto) match_take(FSome fsome, FNone fnone) {
        if (this->is_some()) {
            return fsome(this->_take_some());
        } else {
            return fnone();
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
