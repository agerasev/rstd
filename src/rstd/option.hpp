#pragma once

#include "tuple.hpp"
#include "variant.hpp"


namespace rstd {

template <typename T=Tuple<>>
class Option final {
private:
    Variant<T> var;

public:
    Option() : Option(Variant<T>()) {}

    Option(const Option &) = default;
    Option &operator=(const Option &) = default;

    Option(Option &&other) = default;
    Option &operator=(Option &&other) = default;

    ~Option() = default;

private:
    Option(Variant<T> &&v) :
        var(std::move(v))
    {}

    const Variant<T> &as_variant() const {
        return var;
    }
    Variant<T> &as_variant() {
        return var;
    }

public:
    static Option None() {
        return Option();
    }
    static Option Some(T &&x) {
        return Option(Variant<T>::template create<0>(std::move(x)));
    }
    static Option Some(const T &x) {
        return Option(Variant<T>::template create<0>(x));
    }

    bool is_some() const {
        return var.id() == 0;
    }
    bool is_none() const {
        return var.id() == 1;
    }

    T &get() {
        return this->var.template get<0>();
    }
    const T &get() const {
        return this->var.template get<0>();
    }

private:
    T take_some() {
#ifdef DEBUG
        assert_(this->is_some());
#endif // DEBUG
        return this->var.template take<0>();
    }

public:
    Option take() {
        return Option<T>(std::move(var));
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
