#pragma once

#include "tuple.hpp"
#include "variant.hpp"
#include "option.hpp"


namespace rstd {

template <typename T=Tuple<>>
class Ok final {
private:
    T value;
public:
    Ok(const Ok &) = default;
    Ok &operator=(const Ok &) = default;
    Ok(Ok &&) = default;
    Ok &operator=(Ok &&) = default;

    explicit Ok(const T &v) : value(v) {}
    explicit Ok(T &&v) : value(std::move(v)) {}
    template <typename _T=T, typename X=std::enable_if_t<std::is_same_v<_T, Tuple<>>, void>>
    Ok() : value(Tuple<>()) {}

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

template <typename E=Tuple<>>
class Err final {
private:
    E value;
public:
    Err(const Err &) = default;
    Err &operator=(const Err &) = default;
    Err(Err &&) = default;
    Err &operator=(Err &&) = default;

    explicit Err(const E &v) : value(v) {}
    explicit Err(E &&v) : value(std::move(v)) {}
    template <typename _E=E, typename X=std::enable_if_t<std::is_same_v<_E, Tuple<>>, void>>
    Err() : value(Tuple<>()) {}

    E &get() {
        return value;
    }
    const E &get() const {
        return value;
    }
    E take() {
        return std::move(value);
    }
};

template <typename T=Tuple<>, typename E=Tuple<>>
class Result final {
private:
    Variant<T, E> var;

public:
    Result() = default;
    explicit Result(Variant<T, E> &&v) : var(std::move(v)) {}

    Result(const Result &) = default;
    Result &operator=(const Result &) = default;

    Result(Result &&) = default;
    Result &operator=(Result &&) = default;

    ~Result() {
#ifdef DEBUG
        if (this->is_some()) {
            panic_("Unhandled Result");
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

    Result(const Ok<T> &ok) : Result(Variant<T, E>::template create<0>(ok.get())) {}
    Result(Ok<T> &&ok) : Result(Variant<T, E>::template create<0>(ok.take())) {}
    Result(const Err<E> &err) : Result(Variant<T, E>::template create<1>(err.get())) {}
    Result(Err<E> &&err) : Result(Variant<T, E>::template create<1>(err.take())) {}

    Result &operator=(const Ok<T> &ok) { return *this = Result(ok); }
    Result &operator=(Ok<T> &&ok) { return *this = Result(std::move(ok)); }
    Result &operator=(const Err<E> &err) { return *this = Result(err); }
    Result &operator=(Err<E> &&err) { return *this = Result(std::move(err)); }

    static Result Ok(T &&x) {
        return rstd::Ok(std::move(x));
    }
    static Result Ok(const T &x) {
        return rstd::Ok(x);
    }
    template <typename _T=T, typename X=std::enable_if_t<std::is_same_v<_T, Tuple<>>, void>>
    static Result Ok() {
        return rstd::Ok(Tuple<>());
    }
    static Result Err(E &&x) {
        return rstd::Err(std::move(x));
    }
    static Result Err(const E &x) {
        return rstd::Err(x);
    }
    template <typename _E=E, typename X=std::enable_if_t<std::is_same_v<_E, Tuple<>>, void>>
    static Result Err() {
        return rstd::Err(Tuple<>());
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

    template <typename FT, typename FE>
    decltype(auto) match(FT fok, FE ferr) {
        return this->var.match(fok, ferr);
    }
    template <typename FT, typename FE>
    decltype(auto) match_ref(FT fok, FE ferr) {
        return this->var.match_ref(fok, ferr);
    }
    template <typename FT, typename FE>
    decltype(auto) match_ref(FT fok, FE ferr) const {
        return this->var.match_ref(fok, ferr);
    }

    Option<T> ok() {
        return this->match(
            [](T &&t) { return Option<T>::Some(std::move(t)); },
            [](E &&) { return Option<T>::None(); }
        );
    }
    Option<E> err() {
        return this->match(
            [](T &&) { return Option<E>::None(); },
            [](E &&e) { return Option<E>::Some(std::move(e)); }
        );
    }

    template <
        typename F,
        typename U=std::invoke_result_t<F, T &&>
    >
    Result<U, E> map(F f) {
        return this->match(
            [f](T &&x) { return Result<U, E>::Ok(f(std::move(x))); },
            [](E &&e) { return Result<U, E>::Err(std::move(e)); }
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
            [d](E &&) { return std::move(d); }
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
            [fd](E &&) { return fd(); }
        );
    }
    template <
        typename F,
        typename U=std::invoke_result_t<F, E &&>
    >
    Result<T, U> map_err(F f) {
        return this->match(
            [](T &&t) { return Result<T, U>::Ok(std::move(t)); },
            [f](E &&e) { return Result<T, U>::Err(f(std::move(e))); }
        );
    }

    template <typename U>
    Result<U, E> and_(Result<U, E> &&res) {
        return this->match(
            [&](T &&) { return std::move(res); },
            [&](E &&e) {
                res.clear();
                return Result<U, E>::Err(e);
            }
        );
    }
    template <
        typename F,
        typename RU=std::invoke_result_t<F, T &&>
    >
    RU and_then(F f) {
        return this->match(
            [f](T &&x) { return f(std::move(x)); },
            [](E &&e) { return RU::Err(std::move(e)); }
        );
    }
    Result or_(Result &&res) {
        return this->match(
            [&](T &&x) {
                res.clear();
                return Result::Ok(std::move(x)); 
            },
            [&](E &&) { return std::move(res); }
        );
    }
    template <typename F>
    Result or_else(F f) {
        return this->match(
            [](T &&x) { return Result::Ok(std::move(x)); },
            [f](E &&) { return f(); }
        );
    }

    T unwrap_or(T &&d) {
        return this->match(
            [](T &&x) { return std::move(x); },
            [d](E &&) { return std::move(d); }
        );
    }
    T unwrap_or(const T &d) {
        return this->unwrap_or(clone(d));
    }
    template <typename F>
    T unwrap_or_else(F f) {
        return this->match(
            [](T &&x) { return std::move(x); },
            [f](E &&) { return f(); }
        );
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

#define try_assign_(ok, res) do { \
    auto lres = ::rstd::move((res)); \
    if (lres.is_ok()) { \
        (ok) = lres._take_ok(); \
    } else { \
        return Err(lres._take_err()); \
    } \
} while(false)
