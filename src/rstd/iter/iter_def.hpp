#pragma once

#include "iter_trait.hpp"

namespace rstd {
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
            Option<R> res = func(ox.unwrap());
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
        Option<T> ox = iter.next();
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
        Option<T> ox = iter.next();
        if (ox.is_some()) {
            Option<R> res = func(&state, ox.unwrap());
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

template <typename T, typename I>
class Fuse final : public Iterator<T, Fuse<T, I>> {
private:
    I iter;
public:
    Fuse(I &&i) :
        iter(std::move(i))
    {}
    Option<T> next() {
        Option<T> ox = iter.next();
        if (ox.is_some()) {
            return ox;
        } else {
            drop(iter);
            return None();
        }
    }
};

template <typename T, typename U, typename I, typename J>
class Zip final : public Iterator<Tuple<T, U>, Zip<T, U, I, J>> {
private:
    I first;
    J second;
public:
    Zip(I &&i, J &&j) :
        first(std::move(i)),
        second(std::move(j))
    {}
    Option<Tuple<T, U>> next() {
        auto ret = first.next().and_then([&](T &&x) {
            return second.next().map([&](U &&y) {
                return Tuple<T, U>(std::move(x), std::move(y));
            });
        });
        if (ret.is_none()) {
            drop(first);
            drop(second);
        }
        return ret;
    }
};

template <typename T>
class Empty final : public Iterator<T, Empty<T>> {
public:
    Option<T> next() { return None(); }
    typedef Empty<T> Rev;
    Rev rev() { return std::move(*this); }
};

template <typename T>
class Once final : public Iterator<T, Once<T>> {
private:
    Option<T> elem;
    Once(Option<T> &&oe) : elem(std::move(oe)) {}
public:
    Once(T &&e) : Once(Some(std::move(e))) {}
    Option<T> next() { return std::move(elem); }
    typedef Once<T> Rev;
    Rev rev() { return Rev(std::move(elem)); }
};

template <typename F, typename R=std::invoke_result_t<F>>
class OnceWith final : public Iterator<R, OnceWith<F>> {
private:
    Option<F> func;
    OnceWith(Option<F> &&of) : func(std::move(of)) {}
public:
    OnceWith(F &&f) : OnceWith(Some(std::move(f))) {}
    Option<R> next() { return func.map([](F &&f) { return f(); }); }
    typedef OnceWith<F> Rev;
    Rev rev() { return Rev(std::move(func)); }
};

template <typename T>
class Repeat final : public Iterator<T, Repeat<T>> {
private:
    Option<T> elem;
    Repeat(Option<T> &&oe) : elem(std::move(oe)) {}
public:
    Repeat(T &&e) : Repeat(Some(std::move(e))) {}
    Repeat(const T &e) : Repeat(clone(e)) {}
    Option<T> next() { return clone(elem); }
    typedef Repeat<T> Rev;
    Rev rev() { return Rev(std::move(elem)); }
};

template <typename F, typename R=std::invoke_result_t<F>>
class RepeatWith final : public Iterator<R, RepeatWith<F>> {
private:
    Option<F> func;
    RepeatWith(Option<F> &&of) : func(std::move(of)) {}
public:
    RepeatWith(F &&f) : RepeatWith(Some(std::move(f))) {}
    Option<R> next() { return clone(func).map([](F &&f) { return f(); }); }
    typedef RepeatWith<F> Rev;
    Rev rev() { return Rev(std::move(func)); }
};

template <typename T, typename F>
class Successors final : public Iterator<T, Successors<T, F>> {
private:
    Option<T> prev;
    F func;
public:
    Successors(Option<T> &&init, F &&f) :
        prev(std::move(init)),
        func(std::move(f))
    {}
    Option<T> next() {
        prev = prev.and_then([&](T &&x) { return func(x); });
        return clone(prev);
    }
};

} // namespace rstd
} // namespace iter
