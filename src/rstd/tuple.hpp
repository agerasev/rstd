#pragma once

#include <tuple>
#include <utility>
#include "templates.hpp"
#include "format.hpp"
#include "container.hpp"


namespace rstd {

template <typename ...Elems>
class Tuple final {
private:
    typedef std::tuple<Elems...> Base;
    Base base;

public:
    template <size_t P>
    using Elem = std::tuple_element_t<P, Base>;

    explicit Tuple(Elems &&...args) :
        base(std::forward<Elems>(args)...)
    {}
    
    Tuple(const Tuple &t) = default;
    Tuple &operator=(const Tuple &t) = default;

    Tuple(Tuple &&t) = default;
    Tuple &operator=(Tuple &&t) = default;

    ~Tuple() = default;

    static constexpr size_t size() {
        return std::tuple_size_v<Base>;
    }
    
    template <size_t P>
    const Elem<P> &get() const {
        return std::get<P>(base);
    }
    template <size_t P>
    Elem<P> &get() {
        return std::get<P>(base);
    }
};

template<size_t P, typename ...Elems>
nth_type<P, Elems...> &get(Tuple<Elems...> &t) {
    return t.template get<P>();
}
template<size_t P, typename ...Elems>
const nth_type<P, Elems...> &get(const Tuple<Elems...> &t) {
    return t.template get<P>();
}

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
private:
    template <size_t ...I>
    static void print(std::ostream &o, const Tuple<Elems...> &t, std::index_sequence<I...>) {
        (..., write_(o, "{}{}", (I == 0 ? "" : ", "), t.template get<I>()));
    }

public:
    static void fmt(const Tuple<Elems...> &t, std::ostream &o) {
        o << "(";
        print(o, t, std::make_index_sequence<Tuple<Elems...>::size()>());
        o << ")";
    }
};

} // namespace rstd
