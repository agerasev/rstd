#pragma once

#include <type_traits>
#include <variant>
#include <utility>
#include "templates.hpp"
#include "container.hpp"
#include "format.hpp"
#include "assert.hpp"


namespace rstd2 {

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
    void _put(rstd::nth_type<P, Elems...> &&x) {
        static_assert(P < size(), "Index is out of bounds");
        base = Base(std::in_place_index<P + 1>, std::move(x));
    }
    template <size_t P>
    void put(rstd::nth_type<P, Elems...> &&x) {
        assert_none();
        _put<P>(std::move(x));
    }
    template <size_t P, std::enable_if_t<rstd::is_copyable_v<rstd::nth_type<P, Elems...>>, int> = 0>
    void put(const rstd::nth_type<P, Elems...> &x) {
        rstd::nth_type<P, Elems...> cx(x);
        put<P>(std::move(cx));
    }

    template <size_t P>
    void set(rstd::nth_type<P, Elems...> &&x) {
        try_destroy();
        _put<P>(std::move(x));
    }
    template <size_t P, std::enable_if_t<rstd::is_copyable_v<rstd::nth_type<P, Elems...>>, int> = 0>
    void set(const rstd::nth_type<P, Elems...> &x) {
        rstd::nth_type<P, Elems...> cx(x);
        set<P>(std::move(cx));
    }

    template <size_t P>
    const rstd::nth_type<P, Elems...> &_get() const {
        return std::get<P + 1>(base);
    }
    template <size_t P>
    const rstd::nth_type<P, Elems...> &get() const {
        assert_variant<P>();
        return _get<P>();
    }
    template <size_t P>
    rstd::nth_type<P, Elems...> &_get() {
        return std::get<P + 1>(base);
    }
    template <size_t P>
    rstd::nth_type<P, Elems...> &get() {
        assert_variant<P>();
        return _get<P>();
    }

    template <size_t P>
    rstd::nth_type<P, Elems...> _take() {
        rstd::nth_type<P, Elems...> x(std::get<P + 1>(std::move(base)));
        destroy();
        return x;
    }
    template <size_t P>
    rstd::nth_type<P, Elems...> take() {
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
    static Variant create(rstd::nth_type<P, Elems...> &&x) {
        static_assert(P < Variant::size(), "Index is out of bounds");
        Variant v;
        v.template _put<P>(std::move(x));
        return v;
    }
    template <size_t P, std::enable_if_t<rstd::is_copyable_v<rstd::nth_type<P, Elems...>>, int> = 0>
    static Variant create(const rstd::nth_type<P, Elems...> &x) {
        rstd::nth_type<P, Elems...> cx(x);
        return create<P>(std::move(cx));
    }
};


/*

public:
    template <
        typename ...Fs,
        typename R=std::common_type_t<std::invoke_result_t<Fs, Elems &&>...>
    >
    R match(Fs ...funcs) {
        this->assert_some();
        int id = this->id_;
        this->id_ = size();
        return Visitor<Matcher, size()>::visit(id, this->union_, funcs...);
    }
    template <
        typename ...Fs,
        typename R=std::common_type_t<std::invoke_result_t<Fs, Elems &>...>
    >
    R match_ref(Fs ...funcs) {
        this->assert_some();
        return Visitor<MatcherRef, size()>::visit(this->id_, this->union_, funcs...);
    }
    template <
        typename ...Fs,
        typename R=std::common_type_t<std::invoke_result_t<Fs, const Elems &>...>
    >
    R match_ref(Fs ...funcs) const {
        this->assert_some();
        return Visitor<MatcherRefConst, size()>::visit(this->id_, this->union_, funcs...);
    }
};

*/
/*
template <typename ...Elems>
struct fmt::Display<Variant<Elems...>> {
private:
    template <size_t P>
    struct Printer {
        static void call(std::ostream &o, const Variant<Elems...> &v) {
            write_(o, v.template get<P>());
        }
    };
public:
    static void fmt(const Variant<Elems...> &v, std::ostream &o) {
        assert_(v.is_some());
        o << "Variant<" << v.id() << ">(";
        Visitor<Printer, Variant<Elems...>::size()>::visit(v.id(), o, v);
        o << ")";
    }
};
*/

} // namespace rstd
