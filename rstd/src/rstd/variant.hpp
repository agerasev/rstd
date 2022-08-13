#pragma once

#include <type_traits>
#include <variant>
#include <utility>
#include <optional>
#include "templates.hpp"
#include "container.hpp"
#include "format.hpp"
#include "assert.hpp"
#include "tuple.hpp"


namespace rstd {

template <typename ...Elems>
class Variant final {
private:
    typedef std::variant<std::monostate, Elems...> Base;
    Base base;

    void assert_some() const {
        assert_(base.index() > 0);
    }
    template <size_t P>
    void assert_variant() const {
        assert_(base.index() == P + 1);
    }
    void assert_none() const {
        assert_(base.index() == 0);
    }

public:
    template <size_t P>
    using Elem = nth_type<P, Elems...>;

    Variant() = default;
    Variant(const Variant &) = default;
    Variant &operator=(const Variant &) = default;
    ~Variant() = default;

    Variant(Variant &&other) :
        base(std::move(other.base))
    {
        other.base = Base();
    }
    Variant &operator=(Variant &&other) {
        base = std::move(other.base);
        other.base = Base();
        return *this;
    }

    static constexpr size_t size() {
        return sizeof...(Elems);
    }
    // FIXME: Rename to maybe `index`
    size_t id() const {
        size_t idx = base.index();
        if (idx == 0) {
            return size();
        } else {
            return idx - 1;
        }
    }

    bool is_some() const {
        return base.index() > 0;
    }
    bool is_none() const {
        return base.index() == 0;
    }
    explicit operator bool() const {
        return this->is_some();
    }

    template <size_t P>
    void _put(Elem<P> &&x) {
        static_assert(P < size(), "Index is out of bounds");
        base = Base(std::in_place_index<P + 1>, std::move(x));
    }
    template <size_t P>
    void put(Elem<P> &&x) {
        assert_none();
        _put<P>(std::move(x));
    }
    template <size_t P, std::enable_if_t<is_copyable_v<Elem<P>>, int> = 0>
    void put(const Elem<P> &x) {
        Elem<P> cx(x);
        put<P>(std::move(cx));
    }

    template <size_t P>
    void set(Elem<P> &&x) {
        try_destroy();
        _put<P>(std::move(x));
    }
    template <size_t P, std::enable_if_t<is_copyable_v<Elem<P>>, int> = 0>
    void set(const Elem<P> &x) {
        Elem<P> cx(x);
        set<P>(std::move(cx));
    }

    template <size_t P>
    const Elem<P> &_get() const {
        return std::get<P + 1>(base);
    }
    template <size_t P>
    const Elem<P> &get() const {
        assert_variant<P>();
        return _get<P>();
    }
    template <size_t P>
    Elem<P> &_get() {
        return std::get<P + 1>(base);
    }
    template <size_t P>
    Elem<P> &get() {
        assert_variant<P>();
        return _get<P>();
    }

    template <size_t P>
    Elem<P> _take() {
        Elem<P> x(std::get<P + 1>(std::move(base)));
        destroy();
        return x;
    }
    template <size_t P>
    Elem<P> take() {
        assert_variant<P>();
        return _take<P>();
    }

    void try_destroy() {
        base = Base();
    }
    void destroy() {
        assert_some();
        try_destroy();
    }

    template <size_t P>
    static Variant create(Elem<P> &&x) {
        static_assert(P < Variant::size(), "Index is out of bounds");
        Variant v;
        v.template _put<P>(std::move(x));
        return v;
    }
    template <size_t P, std::enable_if_t<is_copyable_v<Elem<P>>, int> = 0>
    static Variant create(const Elem<P> &x) {
        Elem<P> cx(x);
        return create<P>(std::move(cx));
    }

private:
    template <typename F>
    struct Visitor {
        Variant *owner;
        F func;
        Visitor(Variant *o, F &&f) :
            owner(o),
            func(std::move(f))
        {}
        template <size_t P>
        void operator()() {
            if (owner->id() == P) {
                func.template operator()<P>(owner->template take<P>());
            }
        }
    };
    template <typename F>
    struct VisitorRef {
        Variant *owner;
        F func;
        VisitorRef(Variant *o, F &&f) :
            owner(o),
            func(std::move(f))
        {}
        template <size_t P>
        void operator()() {
            if (owner->id() == P) {
                func.template operator()<P>(owner->template get<P>());
            }
        }
    };
    template <typename F>
    struct VisitorRefConst {
        const Variant *owner;
        F func;
        VisitorRefConst(const Variant *o, F &&f) :
            owner(o),
            func(std::move(f))
        {}
        template <size_t P>
        void operator()() {
            if (owner->id() == P) {
                func.template operator()<P>(owner->template get<P>());
            }
        }
    };

public:
    template <typename F>
    void visit(F &&f) {
        assert_some();
        _Visit<size()>::visit(Visitor<F>(this, std::move(f)));
    }
    template <typename F>
    void visit_ref(F &&f) {
        assert_some();
        _Visit<size()>::visit(VisitorRef<F>(this, std::move(f)));
    }
    template <typename F>
    void visit_ref(F &&f) const {
        assert_some();
        _Visit<size()>::visit(VisitorRefConst<F>(this, std::move(f)));
    }

private:
    template <typename R, typename ...Fs>
    struct Matcher {
        Tuple<Fs...> funcs;
        std::optional<R> *ret;
        Matcher(std::optional<R> *r, Fs &&...fs) :
            funcs(std::forward<Fs>(fs)...), ret(r)
        {}
        template <size_t P>
        void operator()(nth_type<P, Elems...> &&v) {
            *ret = std::optional<R>(funcs.template get<P>()(std::move(v)));
        }
        static R visit(Variant *var, Fs &&...fs) {
            std::optional<R> ret;
            var->visit(Matcher(&ret, std::forward<Fs>(fs)...));
            return std::move(*ret);
        }
    };
    template <typename R, typename ...Fs>
    struct MatcherRef {
        Tuple<Fs...> funcs;
        std::optional<R> *ret;
        MatcherRef(std::optional<R> *r, Fs &&...fs) :
            funcs(std::forward<Fs>(fs)...), ret(r)
        {}
        template <size_t P>
        void operator()(nth_type<P, Elems...> &v) {
            *ret = std::optional<R>(funcs.template get<P>()(v));
        }
        static R visit(Variant *var, Fs &&...fs) {
            std::optional<R> ret;
            var->visit_ref(MatcherRef(&ret, std::forward<Fs>(fs)...));
            return std::move(*ret);
        }
    };
    template <typename R, typename ...Fs>
    struct MatcherRefConst {
        Tuple<Fs...> funcs;
        std::optional<R> *ret;
        MatcherRefConst(std::optional<R> *r, Fs &&...fs) :
            funcs(std::forward<Fs>(fs)...), ret(r)
        {}
        template <size_t P>
        void operator()(const nth_type<P, Elems...> &v) {
            *ret = std::optional<R>(funcs.template get<P>()(v));
        }
        static R visit(const Variant *var, Fs &&...fs) {
            std::optional<R> ret;
            var->visit_ref(MatcherRefConst(&ret, std::forward<Fs>(fs)...));
            return std::move(*ret);
        }
    };
    template <typename ...Fs>
    struct Matcher<void, Fs...> {
        Tuple<Fs...> funcs;
        Matcher(Fs &&...fs) : funcs(std::forward<Fs>(fs)...) {}
        template <size_t P>
        void operator()(nth_type<P, Elems...> &&v) {
            funcs.template get<P>()(std::move(v));
        }
        static void visit(Variant *var, Fs &&...fs) {
            var->visit(Matcher(std::forward<Fs>(fs)...));
        }
    };
    template <typename ...Fs>
    struct MatcherRef<void, Fs...> {
        Tuple<Fs...> funcs;
        MatcherRef(Fs &&...fs) : funcs(std::forward<Fs>(fs)...) {}
        template <size_t P>
        void operator()(nth_type<P, Elems...> &v) {
            funcs.template get<P>()(v);
        }
        static void visit(Variant *var, Fs &&...fs) {
            var->visit_ref(MatcherRef(std::forward<Fs>(fs)...));
        }
    };
    template <typename ...Fs>
    struct MatcherRefConst<void, Fs...> {
        Tuple<Fs...> funcs;
        MatcherRefConst(Fs &&...fs) : funcs(std::forward<Fs>(fs)...) {}
        template <size_t P>
        void operator()(const nth_type<P, Elems...> &v) {
            funcs.template get<P>()(v);
        }
        static void visit(const Variant *var, Fs &&...fs) {
            var->visit_ref(MatcherRefConst(std::forward<Fs>(fs)...));
        }
    };

public:
    template <typename ...Fs, typename R=std::common_type_t<std::invoke_result_t<Fs, Elems &&>...>>
    R match(Fs &&...fs) {
        return Matcher<R, Fs...>::visit(this, std::forward<Fs>(fs)...);
    }
    template <typename ...Fs, typename R=std::common_type_t<std::invoke_result_t<Fs, Elems &>...>>
    R match_ref(Fs &&...fs) {
        return MatcherRef<R, Fs...>::visit(this, std::forward<Fs>(fs)...);
    }
    template <typename ...Fs, typename R=std::common_type_t<std::invoke_result_t<Fs, const Elems &>...>>
    R match_ref(Fs &&...fs) const {
        return MatcherRefConst<R, Fs...>::visit(this, std::forward<Fs>(fs)...);
    }
};

template<size_t P, typename ...Elems>
nth_type<P, Elems...> &get(Variant<Elems...> &t) {
    return t.template get<P>();
}
template<size_t P, typename ...Elems>
const nth_type<P, Elems...> &get(const Variant<Elems...> &t) {
    return t.template get<P>();
}

template<size_t P, typename ...Elems>
nth_type<P, Elems...> take(Variant<Elems...> &&t) {
    return t.template take<P>();
}
template<size_t P, typename ...Elems>
const nth_type<P, Elems...> take(const Variant<Elems...> &&t) {
    return t.template take<P>();
}

template <typename ...Elems>
struct fmt::Display<Variant<Elems...>> {
private:
    struct Printer {
        std::ostream *o;
        template <size_t, typename T>
        void operator()(const T &v) {
            write_(*o, v);
        }
    };
public:
    static void fmt(const Variant<Elems...> &v, std::ostream &o) {
        assert_(v.is_some());
        o << "Variant<" << v.id() << ">(";
        v.visit_ref(Printer{&o});
        o << ")";
    }
};

} // namespace rstd
