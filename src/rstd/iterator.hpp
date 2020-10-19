#pragma once

#include <type_traits>
#include "prelude.hpp"


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

template <typename Self, typename T>
class Iterator {
private:
    //static_assert(std::is_base_of_v<Iterator<Self, T>, Self>);
    Self &self() { return *static_cast<Self *>(this); }
    const Self &self() const { return *static_cast<const Self *>(this); }

public:
    typedef T Item;

    template <template <typename...> typename C>
    C<T> collect() {
        return FromIterator<C>::template from_iter<T>(self());
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

template <template <typename...> typename C, typename T, typename U=const T *>
class Iter : public Iterator<Iter<C, T>, T> {
private:
    typename C<T>::const_iterator cur;
    typename C<T>::const_iterator end;
public:
    explicit Iter(const C<T> &cont) {
        cur = cont.cbegin();
        end = cont.cend();
    }
    rstd::Option<U> next() {
        if (cur != end) {
            U i = &*cur;
            ++cur;
            return Option<U>::Some(i);
        } else {
            return Option<U>::None();
        }
    }
};

template <template <typename...> typename C, typename T, typename U=T *>
class IterMut : public Iterator<IterMut<C, T>, T> {
private:
    typename C<T>::iterator cur;
    typename C<T>::iterator end;
public:
    explicit IterMut(C<T> &cont) {
        cur = cont.begin();
        end = cont.end();
    }
    rstd::Option<U> next() {
        if (cur != end) {
            U i = &*cur;
            ++cur;
            return Option<U>::Some(i);
        } else {
            return Option<U>::None();
        }
    }
};

template <template <typename...> typename C, typename T, typename U=T>
class IntoIter : public Iterator<IterMut<C, T>, T> {
private:
    typename C<T>::iterator cur;
    typename C<T>::iterator end;
public:
    explicit IntoIter(C<T> &cont) {
        cur = cont.begin();
        end = cont.end();
    }
    rstd::Option<U> next() {
        if (cur != end) {
            U i = std::move(*cur);
            ++cur;
            return Option<U>::Some(std::move(i));
        } else {
            return Option<U>::None();
        }
    }
};

template <typename T>
struct Range : public Iterator<Range<T>, T> {
    T start_ = 0, end_ = 0;

    explicit Range(T e) : Range(0, e) {}
    Range(T s, T e) : start_(s), end_(e) {}

    rstd::Option<T> next() {
        if (start_ < end_) {
            T i = start_;
            ++start_;
            return Option<T>::Some(i);
        } else {
            return Option<T>::None();
        }
    }
};

} // namespace rstd
