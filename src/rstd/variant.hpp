#pragma once

#include <type_traits>
#include <variant>
#include <utility>
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
    void _put(nth_type<P, Elems...> &&x) {
        static_assert(P < size(), "Index is out of bounds");
        base = Base(std::in_place_index<P + 1>, std::move(x));
    }
    template <size_t P>
    void put(nth_type<P, Elems...> &&x) {
        assert_none();
        _put<P>(std::move(x));
    }
    template <size_t P, std::enable_if_t<is_copyable_v<nth_type<P, Elems...>>, int> = 0>
    void put(const nth_type<P, Elems...> &x) {
        nth_type<P, Elems...> cx(x);
        put<P>(std::move(cx));
    }

    template <size_t P>
    void set(nth_type<P, Elems...> &&x) {
        try_destroy();
        _put<P>(std::move(x));
    }
    template <size_t P, std::enable_if_t<is_copyable_v<nth_type<P, Elems...>>, int> = 0>
    void set(const nth_type<P, Elems...> &x) {
        nth_type<P, Elems...> cx(x);
        set<P>(std::move(cx));
    }

    template <size_t P>
    const nth_type<P, Elems...> &_get() const {
        return std::get<P + 1>(base);
    }
    template <size_t P>
    const nth_type<P, Elems...> &get() const {
        assert_variant<P>();
        return _get<P>();
    }
    template <size_t P>
    nth_type<P, Elems...> &_get() {
        return std::get<P + 1>(base);
    }
    template <size_t P>
    nth_type<P, Elems...> &get() {
        assert_variant<P>();
        return _get<P>();
    }

    template <size_t P>
    nth_type<P, Elems...> _take() {
        nth_type<P, Elems...> x(std::get<P + 1>(std::move(base)));
        destroy();
        return x;
    }
    template <size_t P>
    nth_type<P, Elems...> take() {
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
    static Variant create(nth_type<P, Elems...> &&x) {
        static_assert(P < Variant::size(), "Index is out of bounds");
        Variant v;
        v.template _put<P>(std::move(x));
        return v;
    }
    template <size_t P, std::enable_if_t<is_copyable_v<nth_type<P, Elems...>>, int> = 0>
    static Variant create(const nth_type<P, Elems...> &x) {
        nth_type<P, Elems...> cx(x);
        return create<P>(std::move(cx));
    }

private:
    template <typename O, typename F>
    struct Visitor {
        O owner;
        F func;
        Visitor(O o, F &&f) :
            owner(o),
            func(std::move(f))
        {}
        template <size_t P>
        decltype(auto) operator()() {
            return func.template operator()<P>(owner->template get<P>());
        }
    };

public:
    template <typename F>
    decltype(auto) visit(F &&f) {
        return _Visit<size()>::visit(id(), Visitor<Variant *, F>(this, std::move(f)));
    }
    template <typename F>
    decltype(auto) visit(F &&f) const {
        return _Visit<size()>::visit(id(), Visitor<const Variant *, F>(this, std::move(f)));
    }


private:
    template <typename ...Fs>
    struct Matcher {
        Variant *owner;
        Tuple<Fs...> funcs;
        Matcher(Variant *o, Fs &&...fs) :
            owner(o),
            funcs(std::forward<Fs>(fs)...)
        {}
        template <size_t P>
        decltype(auto) operator()() {
            return funcs.template get<P>()(owner->template take<P>());
        }
    };
    template <typename O, typename ...Fs>
    struct MatcherRef {
        O owner;
        Tuple<Fs...> funcs;
        MatcherRef(O o, Fs &&...fs) :
            owner(o),
            funcs(std::forward<Fs>(fs)...)
        {}
        template <size_t P>
        decltype(auto) operator()() {
            return funcs.template get<P>()(owner->template get<P>());
        }
    };

public:
    template <typename ...Fs, typename R=std::common_type_t<std::invoke_result_t<Fs, Elems &&>...>>
    R match(Fs &&...fs) {
        assert_some();
        return _Visit<size()>::visit(id(), Matcher<Fs...>(this, std::forward<Fs>(fs)...));
    }
    template <typename ...Fs, typename R=std::common_type_t<std::invoke_result_t<Fs, Elems &>...>>
    R match_ref(Fs &&...fs) {
        assert_some();
        return _Visit<size()>::visit(id(), MatcherRef<Variant *, Fs...>(this, std::forward<Fs>(fs)...));
    }
    template <typename ...Fs, typename R=std::common_type_t<std::invoke_result_t<Fs, const Elems &>...>>
    R match_ref(Fs &&...fs) const {
        assert_some();
        return _Visit<size()>::visit(id(), MatcherRef<const Variant *, Fs...>(this, std::forward<Fs>(fs)...));
    }
};

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
        v.visit(Printer{&o});
        o << ")";
    }
};

} // namespace rstd
