#pragma once

#include "iter_decl.hpp"


namespace rstd {

template <template <typename...> typename Cont>
struct FromIterator {
    template <typename T, typename I> 
    static Cont<T> from_iter(I &&iter) {
        Cont<T> cont;
        for (;;) {
            Option<T> ne = iter.next();
            if (ne.is_some()) {
                cont.push_back(ne.unwrap());
            } else {
                break;
            }
        }
        return cont;
    }
};

template <typename T, typename Self>
class Iterator {
private:
    //static_assert(std::is_base_of_v<Iterator<Self, T>, Self>);
    Self &self() { return *static_cast<Self *>(this); }
    const Self &self() const { return *static_cast<const Self *>(this); }

public:
    typedef T Item;

    template <typename F>
    Option<T> find(F &&f) {
        static_assert(std::is_same_v<std::invoke_result_t<F, T>, bool>);
        for (;;) {
            Option<T> res = self().next();
            if (res.is_some()) {
                T x = res.unwrap();
                if (f(x)) {
                    return Some(std::move(x));
                }
            } else {
                return None();
            }
        }
    }
    template <typename F, typename U=option_some_type<std::invoke_result_t<F, T>>>
    Option<T> find_map(F &&f) {
        static_assert(std::is_same_v<std::invoke_result_t<F, T>, Option<U>>);
        for (;;) {
            Option<T> res = self().next();
            if (res.is_some()) {
                Option<U> ox = f(res.unwrap());
                if (ox.is_some()) {
                    return ox;
                }
            } else {
                return None();
            }
        }
    }
    template <typename B, typename F>
    B fold(B init, F &&f) {
        for (;;) {
            Option<T> ox = self().next();
            if (ox.is_some()) {
                init = f(init, ox.unwrap());
            } else {
                return init;
            }
        }
    }
    template <template <typename...> typename C>
    C<T> collect() {
        return FromIterator<C>::template from_iter<T>(std::move(self()));
    }
    size_t count() {
        return self().fold((size_t)0, [](size_t a, auto _) { return a + 1; });
    }
    
    template <typename F>
    iter::Map<T, Self, F> map(F &&f) {
        return iter::Map<T, Self, F>(std::move(self()), std::move(f));
    }
    template <typename F>
    iter::MapWhile<T, Self, F> map_while(F &&f) {
        return iter::MapWhile<T, Self, F>(std::move(self()), std::move(f));
    }
    template <typename F>
    iter::Filter<T, Self, F> filter(F &&f) {
        return iter::Filter<T, Self, F>(std::move(self()), std::move(f));
    }
    template <typename F>
    iter::FilterMap<T, Self, F> filter_map(F &&f) {
        return iter::FilterMap<T, Self, F>(std::move(self()), std::move(f));
    }
    iter::Cycle<T, Self> cycle() {
        return iter::Cycle<T, Self>(std::move(self()));
    }
    template <typename J>
    iter::Chain<T, Self, J> chain(J &&other) {
        return iter::Chain<T, Self, J>(std::move(self()), std::move(other));
    }
    template <typename S, typename F>
    iter::Scan<T, Self, S, F> scan(S &&s, F &&f) {
        return iter::Scan<T, Self, S, F>(std::move(self()), std::move(s), std::move(f));
    }
    iter::Fuse<T, Self> fuse() {
        return iter::Fuse<T, Self>(std::move(self()));
    }
    template <typename J, typename U=iterator_item<J>>
    iter::Zip<T, U, Self, J> zip(J &&other) {
        return iter::Zip<T, U, Self, J>(std::move(self()), std::move(other));
    }

    template <typename F>
    bool any(F f=[](bool x) { return x; }) {
        return self().find(f).is_some();
    }
    template <typename F>
    bool all(F f=[](bool x) { return x; }) {
        return self().find([&f](const T &x) { return !f(x); }).is_none();
    }

    template <typename T_=T, typename X=std::enable_if_t<std::is_pointer_v<T_>, void>>
    decltype(auto) cloned() {
        return self().map([](T_ x) { return *x; });
    }
    decltype(auto) enumerate() {
        auto endless = iter::successors(Some<size_t>(0), [](size_t x) {
            return Option<size_t>::Some(x + 1);
        });
        return endless.zip(std::move(self()));
    }

private:
    class End {};
    class Cur {
    private:
        Self *owner;
        Option<T> value;
    public:
        Cur(Self *o) : owner(o), value(o->next()) {}
        bool operator!=(End) const {
            return value.is_some();
        }
        Cur &operator++() {
            value = owner->next();
            return *this;
        }
        T &operator*() {
            return value.get();
        }
    };
    
public:
    Cur begin() {
        return Cur(&self());
    }
    End end() {
        return End();
    }
};

} // namespace rstd
