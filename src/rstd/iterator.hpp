#pragma once

#include <type_traits>
#include "prelude.hpp"


namespace rstd {

template <typename I>
class Iterator {
public:
    typedef I Item;
    virtual rstd::Option<Item> next() = 0;
};
/*
template <template <typename> typename C>
struct FromIterator {
    template <typename T>
    std::vector<T> from_iter(Iterator<T> &iter) {
        
    }
};
*/
template <template <typename...> typename C, typename T>
class Iter : public Iterator<const T *> {
public:
    typedef typename Iterator<const T *>::Item Item;
private:
    typename C<T>::const_iterator cur;
    typename C<T>::const_iterator end;
public:
    explicit Iter(const C<T> &cont) {
        cur = cont.cbegin();
        end = cont.cend();
    }
    virtual rstd::Option<Item> next() override {
        if (cur != end) {
            Item i = &*cur;
            ++cur;
            return Some(i);
        } else {
            return None();
        }
    }
};
/*
template <template <typename...> typename C, typename T>
class IterMut : public Iterator<T *> {
private:
    C<T>::iterator cur;
    C<T>::iterator end;
public:
    explicit IterMut(C<T> &cont) {
        cur = cont.begin();
        end = cont.end();
    }
    virtual rstd::Option<Item> next() override {
        if (cur != end) {
            Item i = &*cur;
            ++cur;
            return Some(i);
        } else {
            return None();
        }
    }
};

template <template <typename...> typename C, typename T>
class IntoIter : public Iterator<T> {
private:
    C<T>::iterator cur;
    C<T>::iterator end;
public:
    explicit Iter(C<T> &cont) {
        cur = cont.begin();
        end = cont.end();
    }
    virtual rstd::Option<Item> next() override {
        if (cur != end) {
            Item i = std::move(*cur);
            ++cur;
            return Some(std::move(i));
        } else {
            return None();
        }
    }
};

template <typename T>
struct Range : public Iterator<T> {
    T start = 0, end = 0;

    explicit Range(T end) : Range(0, end) {}
    Range(T start, T end) start(start), end(end) {}
}
*/
} // namespace rstd
