#pragma once

#include <type_traits>
#include <optional>
#include "tuple.hpp"


namespace rstd {

template <typename T=Tuple<>>
class Option final {
private:
    std::optional<T> base;

public:
    Option() = default;
    explicit Option(T &&x) : base(std::move(x)) {}
    explicit Option(const T &x) : base(x) {}

    Option(const Option &) = default;
    Option &operator=(const Option &) = default;

    Option(Option &&other) :
        base(std::move(other.base))
    {
        other.base = std::nullopt;
    }
    Option &operator=(Option &&other) {
        base = std::move(other.base);
        other.base = std::nullopt;
        return *this;
    }

    Option(std::nullopt_t none) : base(none) {}
    Option &operator=(std::nullopt_t none) {
        base = none;
        return *this;
    }

    ~Option() = default;

    static Option None() { return Option(); }
    static Option Some(T &&x) { return Option(std::move(x)); }
    static Option Some(const T &x) { return Option(x); }
    template <typename _T=T, typename X=std::enable_if_t<std::is_same_v<_T, Tuple<>>, void>>
    static Option Some() { return Option(Tuple<>()); }

    bool is_some() const {
        return base.has_value();
    }
    bool is_none() const {
        return !base.has_value();
    }

    T &_get() {
        return base.value();
    }
    const T &_get() const {
        return base.value();
    }
    T &get() {
        assert_(is_some());
        return base.value();
    }
    const T &get() const {
        assert_(is_some());
        return base.value();
    }

    T _take_some() {
        T x(std::move(_get()));
        base = std::nullopt;
        return x;
    }
    T take_some() {
        assert_(is_some());
        return _take_some();
    }
    Option take() {
        return std::move(*this);
    }

    T unwrap() {
        if (!is_some()) {
            panic_("Option is None");
        }
        return _take_some();
    }
    T expect(const std::string &message) {
        if (!is_some()) {
            panic_("Option expect Some:\n{}", message);
        }
        return _take_some();
    }

    void unwrap_none() {
        if (!is_none()) {
            panic_("Option is Some");
        }
    }
    void expect_none(const std::string &message) {
        if (!is_none()) {
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
    // FIXME: Handle case when OU is not Option
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

template <typename T, typename X=std::enable_if_t<!std::is_pointer_v<T>, void>>
Option<T> Some(T &&t) {
    return Option<T>(std::move(t));
}
template <typename T>
Option<T> Some(const T &t) {
    return Option<T>(t);
}
inline Option<Tuple<>> Some() {
    return Option(Tuple<>());
}
inline std::nullopt_t None() {
    return std::nullopt;
}

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

} // namespace rstd2
