#pragma once

#include <type_traits>
#include <rstd/prelude.hpp>


namespace rstd {

template <template <typename...> typename Cont>
struct FromIterator {
    template <typename T, typename I> 
    static Cont<T> from_iter(I &&iter) {
        Cont<T> cont;
        for (;;) {
            auto ne = iter.next();
            if (ne.is_some()) {
                cont.push_back(std::move(ne.unwrap()));
            } else {
                break;
            }
        }
        return cont;
    }
};

namespace iter {

template <
    typename T, typename I, typename F,
    typename R=std::invoke_result_t<F, T>
>
class Map;
template <
    typename T, typename I, typename F,
    typename R=option_some_type<std::invoke_result_t<F, T>>
>
class MapWhile;
template <typename T, typename I, typename F>
class Filter;
template <
    typename T, typename I, typename F,
    typename R=option_some_type<std::invoke_result_t<F, T>>
>
class FilterMap;
template <typename T, typename I>
class Cycle;
template <typename T, typename I, typename J>
class Chain;
template <
    typename T, typename I, typename S, typename F,
    typename R=option_some_type<std::invoke_result_t<F, S*, T>>
>
class Scan;

} // namespace iter

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
            auto res = self().next();
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
            auto res = self().next();
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
            auto ox = self().next();
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

    template <typename T_=T, typename X=std::enable_if_t<std::is_pointer_v<T_>, void>>
    decltype(auto) copied() {
        return self().map([](T_ x) { return *x; });
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

namespace iter {

template <typename T, typename I, typename F, typename R>
class Map final : public Iterator<R, Map<T, I, F, R>> {
private:
    I iter;
    F func;
public:
    Map(I &&i, F &&f) :
        iter(std::move(i)), func(std::move(f))
    {}
    Option<R> next() {
        return iter.next().map(func);
    }
    typedef Map<T, typename I::Rev, F> Rev;
    Rev rev() {
        return Rev(iter.rev(), std::move(func));
    }
};
template <typename T, typename I, typename F, typename R>
class MapWhile final : public Iterator<R, MapWhile<T, I, F, R>> {
private:
    I iter;
    F func;
public:
    MapWhile(I &&i, F &&f) :
        iter(std::move(i)), func(std::move(f))
    {}
    Option<R> next() {
        Option<T> ox = iter.next();
        if (ox.is_some()) {
            auto res = func(ox.unwrap());
            if (res.is_some()) {
                return res;
            } else {
                drop(iter);
                return None();
            }
        } else {
            return None();
        }
    }
    typedef Map<T, typename I::Rev, F> Rev;
    Rev rev() {
        return Rev(iter.rev(), std::move(func));
    }
};
template <typename T, typename I, typename F>
class Filter final : public Iterator<T, Filter<T, I, F>> {
private:
    I iter;
    F func;
public:
    Filter(I &&i, F &&f) :
        iter(std::move(i)), func(std::move(f))
    {}
    Option<T> next() {
        return iter.find(func);
    }
    typedef Filter<T, typename I::Rev, F> Rev;
    Rev rev() {
        return Rev(iter.rev(), std::move(func));
    }
};
template <typename T, typename I, typename F, typename R>
class FilterMap final : public Iterator<R, FilterMap<T, I, F, R>> {
private:
    I iter;
    F func;
public:
    FilterMap(I &&i, F &&f) :
        iter(std::move(i)), func(std::move(f))
    {}
    Option<R> next() {
        return iter.find_map(func);
    }
    typedef FilterMap<T, typename I::Rev, F> Rev;
    Rev rev() {
        return Rev(iter.rev(), std::move(func));
    }
};
template <typename T, typename I>
class Cycle final : public Iterator<T, Cycle<T, I>> {
private:
    I origin, iter;
public:
    Cycle(I &&i) :
        origin(clone(i)),
        iter(std::move(i))
    {}
    Option<T> next() {
        auto ox = iter.next();
        if (ox.is_some()) {
            return ox;
        } else {
            iter = clone(origin);
            return iter.next();
        }
    }
};
template <typename T, typename I, typename J>
class Chain final : public Iterator<T, Chain<T, I, J>> {
private:
    I first;
    J second;
public:
    Chain(I &&i, J &&j) :
        first(std::move(i)),
        second(std::move(j))
    {}
    Option<T> next() {
        return first.next().or_else([&]() { return second.next(); });
    }
    typedef Chain<T, typename J::Rev, typename I::Rev> Rev;
    Rev rev() {
        return Rev(second.rev(), first.rev());
    }
};
template <typename T, typename I, typename S, typename F, typename R>
class Scan final : public Iterator<R, Scan<T, I, S, F, R>> {
private:
    I iter;
    S state;
    F func;
public:
    Scan(I &&i, S &&s, F &&f) :
        iter(std::move(i)),
        state(std::move(s)),
        func(std::move(f))
    {}
    Option<R> next() {
        auto ox = iter.next();
        if (ox.is_some()) {
            auto res = func(&state, ox.unwrap());
            if (res.is_some()) {
                return res;
            } else {
                drop(iter);
                return None();
            }
        } else {
            return None();
        }
    }
};

} // namespace iter

} // namespace rstd
