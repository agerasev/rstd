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

    Tuple() = default;
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

private:
    template <typename F>
    struct Visitor {
        Tuple *owner;
        F func;
        Visitor(Tuple *o, F &&f) :
            owner(o),
            func(std::move(f))
        {}
        template <size_t P>
        void operator()() {
            func.template operator()<P>(std::move(owner->template get<P>()));
        }
    };
    template <typename F>
    struct VisitorRef {
        Tuple *owner;
        F func;
        VisitorRef(Tuple *o, F &&f) :
            owner(o),
            func(std::move(f))
        {}
        template <size_t P>
        void operator()() {
            func.template operator()<P>(owner->template get<P>());
        }
    };
    template <typename F>
    struct VisitorRefConst {
        const Tuple *owner;
        F func;
        VisitorRefConst(const Tuple *o, F &&f) :
            owner(o),
            func(std::move(f))
        {}
        template <size_t P>
        void operator()() {
            func.template operator()<P>(owner->template get<P>());
        }
    };

public:
    template <typename F>
    void visit(F &&f) {
        _Visit<size()>::visit(Visitor<F>(this, std::move(f)));
    }
    template <typename F>
    void visit_ref(F &&f) {
        _Visit<size()>::visit(VisitorRef<F>(this, std::move(f)));
    }
    template <typename F>
    void visit_ref(F &&f) const {
        _Visit<size()>::visit(VisitorRefConst<F>(this, std::move(f)));
    }

private:
    template <typename F, size_t S=size(), size_t Q=S>
    struct Unpacker {
        template <typename ...Args>
        static decltype(auto) unpack(Tuple *t, F &&f, Args &&...args) {
            return Unpacker<F, S, Q - 1>::unpack(t, std::move(f), std::move(t->template get<Q - 1>()), std::forward<Args>(args)...);
        }
    };
    template <typename F, size_t S>
    struct Unpacker<F, S, 0> {
        template <typename ...Args>
        static decltype(auto) unpack(Tuple *t, F &&f, Args &&...args) {
            return f(std::forward<Args>(args)...);
        }
    };
    template <typename F, size_t S=size(), size_t Q=S>
    struct UnpackerRef {
        template <typename ...Args>
        static decltype(auto) unpack(Tuple *t, F &&f, Args &&...args) {
            return UnpackerRef<F, S, Q - 1>::unpack(t, std::move(f), t->template get<Q - 1>(), std::forward<Args>(args)...);
        }
    };
    template <typename F, size_t S>
    struct UnpackerRef<F, S, 0> {
        template <typename ...Args>
        static decltype(auto) unpack(Tuple *t, F &&f, Args &&...args) {
            return f(std::forward<Args>(args)...);
        }
    };
    template <typename F, size_t S=size(), size_t Q=S>
    struct UnpackerRefConst {
        template <typename ...Args>
        static decltype(auto) unpack(const Tuple *t, F &&f, Args &&...args) {
            return UnpackerRefConst<F, S, Q - 1>::unpack(t, std::move(f), t->template get<Q - 1>(), std::forward<Args>(args)...);
        }
    };
    template <typename F, size_t S>
    struct UnpackerRefConst<F, S, 0> {
        template <typename ...Args>
        static decltype(auto) unpack(const Tuple *t, F &&f, Args &&...args) {
            return f(std::forward<Args>(args)...);
        }
    };
public:
    template <typename F>
    decltype(auto) unpack(F &&f) {
        return Unpacker<F>::unpack(this, std::move(f));
    }
    template <typename F>
    decltype(auto) unpack_ref(F &&f) {
        return UnpackerRef<F>::unpack(this, std::move(f));
    }
    template <typename F>
    decltype(auto) unpack_ref(F &&f) const {
        return UnpackerRefConst<F>::unpack(this, std::move(f));
    }
};

template <>
class Tuple<> final {
public:
    Tuple() = default;

    Tuple(const Tuple &t) = default;
    Tuple &operator=(const Tuple &t) = default;

    Tuple(Tuple &&t) = default;
    Tuple &operator=(Tuple &&t) = default;

    ~Tuple() = default;

    static constexpr size_t size() {
        return 0;
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
