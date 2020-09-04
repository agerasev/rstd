#pragma once

#include <type_traits>
#include <core/traits.hpp>
#include "container.hpp"
#include "union.hpp"
#include "format.hpp"
#include "assert.hpp"


namespace core {

// Safe union with id (similar to Rust enum type)

template <bool C, typename ...Elems>
class _Variant {
protected:
    Union<Elems...> union_;
    size_t id_ = size();

    void assert_valid() const {
#ifdef DEBUG
        assert(this->id_ < size());
#endif // DEBUG
    }
    template <size_t P>
    void assert_variant() const {
        this->assert_valid();
#ifdef DEBUG
        assert(this->id_ == P);
#endif // DEBUG
    }
    void assert_empty() const {
#ifdef DEBUG
        assert(this->id_ == size());
#endif // DEBUG
    }

public:
    static const bool copyable = false;

protected:
    template <size_t P>
    struct Destroyer{
        static void call(Union<Elems...> &u) {
            u.template destroy<P>();
        }
    };
    template <size_t P>
    struct Mover {
        static void call(Union<Elems...> &dst, Union<Elems...> &src) {
            dst.template move_from<P>(src);
        }
    };

public:
    _Variant() = default;

    _Variant(_Variant &&v) {
        Dispatcher<Mover, size()>::dispatch(this->id_, this->union_, v.union_);
        this->id_ = v.id_;
        v.id_ = size();
    }
    _Variant &operator=(_Variant &&v) {
        this->try_destroy();
        Dispatcher<Mover, size()>::dispatch(this->id_, this->union_, v.union_);
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

    Union<Elems...> &_as_union() {
        return union_;
    }
    const Union<Elems...> &_as_union() const {
        return union_;
    }

    template <size_t P>
    void put(nth_type<P, Elems...> &&x) {
        static_assert(P < size(), "Index is out of bounds");
        this->try_destroy();
        this->union_.template put<P>(std::move(x));
        this->id_ = P;
    }
    template <size_t P, std::enable_if_t<core::is_copyable_v<nth_type<P, Elems...>>, int> = 0>
    void put(const nth_type<P, Elems...> &x) {
        nth_type<P, Elems...> cx(x);
        this->put<P>(std::move(cx));
    }

    template <size_t P>
    const nth_type<P, Elems...> &get() const {
        this->assert_variant<P>();
        return this->union_.template get<P>();
    }
    template <size_t P>
    nth_type<P, Elems...> &get() {
        this->assert_variant<P>();
        return this->union_.template get<P>();
    }

    template <size_t P>
    nth_type<P, Elems...> take() {
        this->assert_variant<P>();
        this->id_ = size();
        return std::move(this->union_.template take<P>());
    }

    operator bool() const {
        return this->id_ < size();
    }

    void try_destroy() {
        if (*this) {
            Dispatcher<Destroyer, size()>::dispatch(this->id_, this->union_);
            this->id_ = size();
        }
    }
};

template <typename ...Elems>
class _Variant<true, Elems...> : public _Variant<false, Elems...> {
public:
    static const bool copyable = true;

private:
    template <size_t P>
    struct Copier {
        static void call(Union<Elems...> &dst, const Union<Elems...> &src) {
            dst.template put<P>(src.template get<P>());
        }
    };

public:
    _Variant() = default;

    _Variant(const _Variant &var) {
        if (var) {
            Dispatcher<Copier, this->size()>::dispatch(var.id_, this->union_, var.union_);
            this->id_ = var.id_;
        }
    }
    _Variant &operator=(const _Variant &var) {
        this->try_destroy();
        if (var) {
            Dispatcher<Copier, this->size()>::dispatch(var.id_, this->union_, var.union_);
            this->id_ = var.id_;
        }
        return *this;
    }

    _Variant(_Variant &&v) = default;
    _Variant &operator=(_Variant &&v) = default;

    ~_Variant() = default;
};

template <typename ...Elems>
class Variant final : public _Variant<all_v<core::is_copyable_v<Elems>...>, Elems...> {
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
        v.template put<P>(std::move(x));
        return v;
    }
    template <size_t P, std::enable_if_t<core::is_copyable_v<nth_type<P, Elems...>>, int> = 0>
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
            o << v.template get<P>();
        }
    };
public:
    static void fmt(const Variant<Elems...> &v, std::ostream &o) {
        assert_(v);
        o << "Variant<" << v.id() << ">(";
        Dispatcher<Printer, v.size()>::dispatch(v.id(), o, v);
        o << ")";
    }
};

} // namespace core
