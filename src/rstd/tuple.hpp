#pragma once

#include <tuple>
#include <cassert>
#include "templates.hpp"
#include "format.hpp"
#include "container.hpp"


namespace rstd {

template <typename ...Elems>
class Tuple {
public:
    static constexpr int size() {
        return 0;
    }
};

template <size_t P, typename T, typename ...Elems>
struct _TupleGetter;

template <typename T, typename ...Elems>
class Tuple<T, Elems...> {
private:
    template <size_t P, typename T_, typename ...Elems_>
    friend struct _TupleGetter;
    template <size_t P>
    using Getter = _TupleGetter<P, T, Elems...>;

    T value_;
    Tuple<Elems...> tail_;

public:
    Tuple() = default;
    
    explicit Tuple(const T &v, Elems &&...args) :
        value_(v),
        tail_(std::forward<Elems>(args)...)
    {}
    explicit Tuple(T &&v, Elems &&...args) :
        value_(std::move(v)),
        tail_(std::forward<Elems>(args)...)
    {}

    Tuple(const Tuple &t) = default;
    Tuple &operator=(const Tuple &t) = default;

    Tuple(Tuple &&t) = default;
    Tuple &operator=(Tuple &&t) = default;

    ~Tuple() = default;

    static constexpr size_t size() {
        return 1 + sizeof...(Elems);
    }

    Tuple<Elems...> &tail() {
        return tail_;
    }
    const Tuple<Elems...> &tail() const {
        return tail_;
    }
    
    template <size_t P>
    const nth_type<P, T, Elems...> &get() const {
        return Getter<P>::get(*this);
    }
    template <size_t P>
    nth_type<P, T, Elems...> &get() {
        return Getter<P>::get(*this);
    }
};

template <size_t P, typename T, typename ...Elems>
struct _TupleGetter {
    static const nth_type<P - 1, Elems...> &get(const Tuple<T, Elems...> &t) {
        return _TupleGetter<P - 1, Elems...>::get(t.tail_);
    }
    static nth_type<P - 1, Elems...> &get(Tuple<T, Elems...> &t) {
        return _TupleGetter<P - 1, Elems...>::get(t.tail_);
    }
};
template <typename T, typename ...Elems>
struct _TupleGetter<0, T, Elems...> {
    static const T &get(const Tuple<T, Elems...> &t) {
        return t.value_;
    }
    static T &get(Tuple<T, Elems...> &t) {
        return t.value_;
    }
};


template <typename ...Elems>
struct _TuplePrinter {
    static void print(std::ostream &, const Tuple<Elems...> &) {}
};
template <typename T, typename S, typename ...Elems>
struct _TuplePrinter<T, S, Elems...> {
    static void print(std::ostream &o, const Tuple<T, S, Elems...> &t) {
        write_(o, "{}, ", t.template get<0>());
        _TuplePrinter<S, Elems...>::print(o, t.tail());
    }
};
template <typename T>
struct _TuplePrinter<T> {
    static void print(std::ostream &o, const Tuple<T> &t) {
        write_(o, t.template get<0>());
    }
};

template <typename ...Elems>
struct fmt::Display<Tuple<Elems...>> {
public:
    static void fmt(const Tuple<Elems...> &t, std::ostream &o) {
        o << "(";
        _TuplePrinter<Elems...>::print(o, t);
        o << ")";
    }
};

} // namespace rstd
