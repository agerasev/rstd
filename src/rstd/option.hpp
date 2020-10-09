#pragma once

#include <type_traits>
#include "tuple.hpp"
#include "variant.hpp"


namespace rstd {

template <typename T=Tuple<>>
class Some final {
private:
    T value;
public:
    Some(const Some &) = default;
    Some &operator=(const Some &) = default;
    Some(Some &&) = default;
    Some &operator=(Some &&) = default;

    explicit Some(const T &v) : value(v) {}
    explicit Some(T &&v) : value(std::move(v)) {}
    template <typename _T=T, typename X=std::enable_if_t<std::is_same_v<_T, Tuple<>>, void>>
    Some() : value(Tuple<>()) {}

    T &get() {
        return value;
    }
    const T &get() const {
        return value;
    }
    T take() {
        return std::move(value);
    }
};

class None final {
public:
    None() = default;
    None(const None &) = default;
    None &operator=(const None &) = default;
    None(None &&) = default;
    None &operator=(None &&) = default;
};

template <typename T=Tuple<>>
class Option final {
private:
    Variant<T> var;

public:
    Option() = default;
    explicit Option(Variant<T> &&v) : var(std::move(v)) {}
    explicit Option(T &&x) : Option(Variant<T>::template create<0>(std::move(x))) {}
    explicit Option(const T &x) : Option(Variant<T>::template create<0>(x)) {}

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

    Option(Some<T> &&some) : Option(some.take()) {}
    Option(const Some<T> &some) : Option(some.get()) {}
    Option(None none) : Option() {}

    Option &operator=(Some<T> &&some) { return *this = Option(some); }
    Option &operator=(const Some<T> &some) { return *this = Option(std::move(some)); }
    Option &operator=(None none) { return *this = Option(none); }

    static Option None() {
        return Option();
    }
    static Option Some(T &&x) {
        return Option(std::move(x));
    }
    static Option Some(const T &x) {
        return Option(x);
    }
    template <typename _T=T, typename X=std::enable_if_t<std::is_same_v<_T, Tuple<>>, void>>
    static Option Some() {
        return Option(Tuple<>());
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
        return this->_take_some();
    }
    T expect(const std::string &message) {
        if (!this->is_some()) {
            panic_("Option expect Some:\n{}", message);
        }
        return this->_take_some();
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

    template <
        typename FS,
        typename FN,
        typename US=std::invoke_result_t<FS, T &&>,
        typename UN=std::invoke_result_t<FN>
    >
    std::common_type_t<US, UN> match(FS fsome, FN fnone) {
        if (this->is_some()) {
            return fsome(this->_take_some());
        } else {
            return fnone();
        }
    }
    template <
        typename FS,
        typename FN,
        typename US=std::invoke_result_t<FS, T &>,
        typename UN=std::invoke_result_t<FN>
    >
    std::common_type_t<US, UN> match_ref(FS fsome, FN fnone) {
        if (this->is_some()) {
            return fsome(this->_get());
        } else {
            return fnone();
        }
    }
    template <
        typename FS,
        typename FN,
        typename US=std::invoke_result_t<FS, const T &>,
        typename UN=std::invoke_result_t<FN>
    >
    std::common_type_t<US, UN> match_ref(FS fsome, FN fnone) const {
        if (this->is_some()) {
            return fsome(this->_get());
        } else {
            return fnone();
        }
    }

    T unwrap_or(T &&d) {
        return this->match(
            [](T &&x) { return std::move(x); },
            [d]() { return std::move(d); }
        );
    }
    T unwrap_or(const T &d) {
        return this->unwrap_or(clone(d));
    }
    template <typename F>
    T unwrap_or_else(F f) {
        return this->match(
            [](T &&x) { return std::move(x); },
            [f]() { return f(); }
        );
    }

    template <
        typename F,
        typename U=std::invoke_result_t<F, T &&>
    >
    Option<U> map(F f) {
        return this->match(
            [f](T &&x) { return Option<U>::Some(f(std::move(x))); },
            []() { return Option<U>::None(); }
        );
    }
    template <
        typename F,
        typename D,
        typename U=std::invoke_result_t<F, T &&>
    >
    std::common_type<U, D> map_or(D &&d, F f) {
        return this->match(
            [f](T &&x) { return f(std::move(x)); },
            [d]() { return std::move(d); }
        );
    }
    template <typename F, typename D>
    decltype(auto) map_or(const D &d, F f) {
        return this->map_or(clone(d), f);
    }
    template <
        typename F,
        typename FD,
        typename U=std::invoke_result_t<F, T &&>,
        typename UD=std::invoke_result_t<FD>
    >
    std::common_type<U, UD> map_or_else(FD fd, F f) {
        return this->match(
            [f](T &&x) { return f(std::move(x)); },
            [fd]() { return fd(); }
        );
    }
    template <typename U>
    Option<U> and_(Option<U> &&opt) {
        return this->match(
            [&](T &&) { return std::move(opt); },
            [&]() {
                drop(opt);
                return Option<U>::None();
            }
        );
    }
    template <
        typename F,
        typename OU=std::invoke_result_t<F, T &&>
    >
    OU and_then(F f) {
        return this->match(
            [f](T &&x) { return f(std::move(x)); },
            []() { return OU::None(); }
        );
    }
    Option or_(Option &&opt) {
        return this->match(
            [&](T &&x) {
                drop(opt);
                return Option::Some(std::move(x));
            },
            [&]() { return std::move(opt); }
        );
    }
    template <typename F>
    Option or_else(F f) {
        return this->match(
            [](T &&x) { return Option::Some(std::move(x)); },
            [f]() { return f(); }
        );
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
