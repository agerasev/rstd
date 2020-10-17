#pragma once

#include <type_traits>
#include "prelude.hpp"


namespace rstd {

template <typename I>
class Iterator {
public:
    typedef I Item;

    virtual rstd::Option<Item> next() = 0;

private:
    class End {};
    class Cur {
    private:
        Iterator *owner;
        Option<I> value;
    public:
        Cur(Iterator *o) : owner(o), value(o->next()) {}
        bool operator!=(End) const {
            return value.is_some();
        }
        Cur &operator++() {
            value = owner->next();
            return *this;
        }
        I &operator*() {
            return value.get();
        }
    };
    
public:
    Cur begin() {
        return Cur(this);
    }
    End end() {
        return End();
    }
};
/*
template <template <typename> typename C>
struct FromIterator {
    template <typename T>
    std::vector<T> from_iter(Iterator<T> &iter) {
        
    }
};
*/
template <template <typename...> typename C, typename T, typename I=const T *>
class Iter : public Iterator<I> {
public:
    typedef I Item;
private:
    typename C<T>::const_iterator cur;
    typename C<T>::const_iterator end;
public:
    explicit Iter(const C<T> &cont) {
        cur = cont.cbegin();
        end = cont.cend();
    }
    virtual rstd::Option<I> next() override {
        if (cur != end) {
            I i = &*cur;
            ++cur;
            return Option<I>::Some(i);
        } else {
            return Option<I>::None();
        }
    }
};

template <template <typename...> typename C, typename T, typename I=T *>
class IterMut : public Iterator<I> {
public:
    typedef I Item;
private:
    typename C<T>::iterator cur;
    typename C<T>::iterator end;
public:
    explicit IterMut(C<T> &cont) {
        cur = cont.begin();
        end = cont.end();
    }
    virtual rstd::Option<I> next() override {
        if (cur != end) {
            I i = &*cur;
            ++cur;
            return Option<I>::Some(i);
        } else {
            return Option<I>::None();
        }
    }
};

template <template <typename...> typename C, typename T, typename I=T>
class IntoIter : public Iterator<I> {
public:
    typedef I Item;
private:
    typename C<T>::iterator cur;
    typename C<T>::iterator end;
public:
    explicit IntoIter(C<T> &cont) {
        cur = cont.begin();
        end = cont.end();
    }
    virtual rstd::Option<I> next() override {
        if (cur != end) {
            I i = std::move(*cur);
            ++cur;
            return Option<I>::Some(std::move(i));
        } else {
            return Option<I>::None();
        }
    }
};

template <typename T>
struct Range : public Iterator<T> {
    T start_ = 0, end_ = 0;

    explicit Range(T e) : Range(0, e) {}
    Range(T s, T e) : start_(s), end_(e) {}

    virtual rstd::Option<T> next() override {
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
