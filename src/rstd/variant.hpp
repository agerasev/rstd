#pragma once

#include <type_traits>
#include "traits.hpp"
#include "container.hpp"
#include "union.hpp"
#include "format.hpp"
#include "assert.hpp"


namespace rstd {

// Safe union with id (similar to Rust enum type)

template <bool C, typename ...Elems>
class _Variant {
protected:
    _Union<Elems...> union_;
    size_t id_ = size();

    void assert_some() const {
        assert_(this->id_ < size());
    }
    template <size_t P>
    void assert_variant() const {
        assert_(this->id_ == P);
    }
    void assert_none() const {
        assert_(this->id_ == size());
    }

public:
    static const bool copyable = false;

protected:
    template <size_t P>
    struct Destroyer{
        static void call(_Union<Elems...> &u) {
            u.template destroy<P>();
        }
    };
    template <size_t P>
    struct Mover {
        static void call(_Union<Elems...> &dst, _Union<Elems...> &src) {
            dst.template move_from<P>(src);
        }
    };

public:
    _Variant() = default;

    _Variant(_Variant &&v) {
        if (v.is_some()) {
            Dispatcher<Mover, size()>::dispatch(v.id_, this->union_, v.union_);
        }
        this->id_ = v.id_;
        v.id_ = size();
    }
    _Variant &operator=(_Variant &&v) {
        this->try_destroy();
        if (v.is_some()) {
            Dispatcher<Mover, size()>::dispatch(v.id_, this->union_, v.union_);
        }
        this->id_ = v.id_;
        v.id_ = size();
        return *this;
    }

    _Variant(const _Variant &var) = delete;
    _Variant &operator=(const _Variant &var) = delete;

    ~_Variant() {
        this->try_destroy();
    }

    static constexpr size_t size() {
        return sizeof...(Elems);
    }
    size_t id() const {
        return this->id_;
    }

    _Union<Elems...> &_as_union() {
        return union_;
    }
    const _Union<Elems...> &_as_union() const {
        return union_;
    }

    template <size_t P>
    void _put(nth_type<P, Elems...> &&x) {
        static_assert(P < size(), "Index is out of bounds");
        this->union_.template put<P>(std::move(x));
        this->id_ = P;
    }
    template <size_t P>
    void put(nth_type<P, Elems...> &&x) {
        this->assert_none();
        this->union_.template put<P>(std::move(x));
        this->id_ = P;
    }
    template <size_t P, std::enable_if_t<rstd::is_copyable_v<nth_type<P, Elems...>>, int> = 0>
    void put(const nth_type<P, Elems...> &x) {
        nth_type<P, Elems...> cx(x);
        this->put<P>(std::move(cx));
    }

    template <size_t P>
    void set(nth_type<P, Elems...> &&x) {
        this->try_destroy();
        this->union_.template put<P>(std::move(x));
        this->id_ = P;
    }
    template <size_t P, std::enable_if_t<rstd::is_copyable_v<nth_type<P, Elems...>>, int> = 0>
    void set(const nth_type<P, Elems...> &x) {
        nth_type<P, Elems...> cx(x);
        this->set<P>(std::move(cx));
    }

    template <size_t P>
    const nth_type<P, Elems...> &_get() const {
        return this->union_.template get<P>();
    }
    template <size_t P>
    const nth_type<P, Elems...> &get() const {
        this->assert_variant<P>();
        return this->template _get<P>();
    }
    template <size_t P>
    nth_type<P, Elems...> &_get() {
        return this->union_.template get<P>();
    }
    template <size_t P>
    nth_type<P, Elems...> &get() {
        this->assert_variant<P>();
        return this->template _get<P>();
    }

    template <size_t P>
    nth_type<P, Elems...> _take() {
        this->id_ = size();
        return std::move(this->union_.template take<P>());
    }
    template <size_t P>
    nth_type<P, Elems...> take() {
        this->assert_variant<P>();
        return this->template _take<P>();
    }

    bool is_some() const {
        return this->id_ < size();
    }
    bool is_none() const {
        return !this->is_some();
    }
    explicit operator bool() const {
        return this->is_some();
    }

    void _destroy() {
        Dispatcher<Destroyer, size()>::dispatch(this->id_, this->union_);
        this->id_ = size();
    }
    void destroy() {
        this->assert_some();
        this->_destroy();
    }
    void try_destroy() {
        if (this->is_some()) {
            this->_destroy();
        }
    }

protected:
    template <size_t P>
    struct Matcher {
        template <typename ...Fs, typename R=std::common_type_t<std::invoke_result_t<Fs, Elems>...>>
        static R call(_Union<Elems...> &u, Fs ...funcs) {
            return nth_arg<P, Fs...>(funcs...)(u.template take<P>());
        }
    };
    template <size_t P>
    struct MatcherRef {
        template <typename ...Fs, typename R=std::common_type_t<std::invoke_result_t<Fs, Elems &>...>>
        static R call(_Union<Elems...> &u, Fs ...funcs) {
            return nth_arg<P, Fs...>(funcs...)(u.template get<P>());
        }
    };
    template <size_t P>
    struct MatcherRefConst {
        template <typename ...Fs, typename R=std::common_type_t<std::invoke_result_t<Fs, const Elems &>...>>
        static R call(const _Union<Elems...> &u, Fs ...funcs) {
            return nth_arg<P, Fs...>(funcs...)(u.template get<P>());
        }
    };

public:
    template <
        typename ...Fs,
        typename R=std::common_type_t<std::invoke_result_t<Fs, Elems &&>...>
    >
    R match(Fs ...funcs) {
        this->assert_some();
        int id = this->id_;
        this->id_ = size();
        return Dispatcher<Matcher, size()>::dispatch(id, this->union_, funcs...);
    }
    template <
        typename ...Fs,
        typename R=std::common_type_t<std::invoke_result_t<Fs, Elems &>...>
    >
    R match_ref(Fs ...funcs) {
        this->assert_some();
        return Dispatcher<MatcherRef, size()>::dispatch(this->id_, this->union_, funcs...);
    }
    template <
        typename ...Fs,
        typename R=std::common_type_t<std::invoke_result_t<Fs, const Elems &>...>
    >
    R match_ref(Fs ...funcs) const {
        this->assert_some();
        return Dispatcher<MatcherRefConst, size()>::dispatch(this->id_, this->union_, funcs...);
    }
};

template <typename ...Elems>
class _Variant<true, Elems...> : public _Variant<false, Elems...> {
public:
    static const bool copyable = true;

private:
    template <size_t P>
    struct Copier {
        static void call(_Union<Elems...> &dst, const _Union<Elems...> &src) {
            dst.template put<P>(src.template get<P>());
        }
    };

public:
    _Variant() = default;

    _Variant(const _Variant &var) {
        if (var) {
            Dispatcher<Copier, _Variant::size()>::dispatch(var.id_, this->union_, var.union_);
            this->id_ = var.id_;
        }
    }
    _Variant &operator=(const _Variant &var) {
        this->try_destroy();
        if (var) {
            Dispatcher<Copier, _Variant::size()>::dispatch(var.id_, this->union_, var.union_);
            this->id_ = var.id_;
        }
        return *this;
    }

    _Variant(_Variant &&v) = default;
    _Variant &operator=(_Variant &&v) = default;

    ~_Variant() = default;
};

template <typename ...Elems>
class Variant final : public _Variant<all_v<rstd::is_copyable_v<Elems>...>, Elems...> {
public:
    Variant() = default;

    Variant(const Variant &var) = default;
    Variant &operator=(const Variant &var) = default;

    Variant(Variant &&v) = default;
    Variant &operator=(Variant &&v) = default;

    ~Variant() = default;

    template <size_t P>
    static Variant create(nth_type<P, Elems...> &&x) {
        static_assert(P < Variant::size(), "Index is out of bounds");
        Variant v;
        v.template _put<P>(std::move(x));
        return v;
    }
    template <size_t P, std::enable_if_t<rstd::is_copyable_v<nth_type<P, Elems...>>, int> = 0>
    static Variant create(const nth_type<P, Elems...> &x) {
        nth_type<P, Elems...> cx(x);
        return create<P>(std::move(cx));
    }
};

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
        assert_(bool(v));
        o << "Variant<" << v.id() << ">(";
        Dispatcher<Printer, Variant<Elems...>::size()>::dispatch(v.id(), o, v);
        o << ")";
    }
};

} // namespace rstd
