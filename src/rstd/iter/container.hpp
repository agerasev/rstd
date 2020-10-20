#pragma once

#include "iterator.hpp"


namespace rstd {

template <template <typename...> typename C, typename T, typename U, typename J>
class Iter : public Iterator<U, Iter<C, T, U, J>> {
private:
    J cur, end;
public:
    Iter(J begin, J end) :
        cur(begin), end(end)
    {}
    Iter(const Iter &) = default;
    Iter &operator=(const Iter &) = default;
    Iter(Iter &&other) :
        cur(std::move(other.cur)),
        end(std::move(other.end))
    {
        other.cur = other.end;
    }
    Iter &operator=(Iter &&other) {
        cur = std::move(other.cur);
        end = std::move(other.end);
        other.cur = other.end;
        return *this;
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
    typedef Iter<C, T, U, std::reverse_iterator<J>> Rev;
    Rev rev() {
        auto r = Rev(
            std::reverse_iterator<J>(end),
            std::reverse_iterator<J>(cur)
        );
        cur = end;
        return r;
    }
};

template <
    template <typename...> typename C,
    typename T,
    typename I=Iter<C, T, const T *, typename C<T>::const_iterator>
>
I iter_ref(const C<T> &cont) {
    return I(cont.begin(), cont.end());
}
template <
    template <typename...> typename C,
    typename T,
    typename I=Iter<C, T, T *, typename C<T>::iterator>
>
I iter_ref(C<T> &cont) {
    return I(cont.begin(), cont.end());
}

template <template <typename...> typename C, typename T, typename J>
class IntoIter : public Iterator<T, IntoIter<C, T, J>> {
private:
    C<T> cont;
    J cur, end;
public:
    IntoIter(C<T> &&c, J begin, J end) :
        cont(std::move(c)),
        cur(begin), end(end)
    {}
    IntoIter(const IntoIter &) = default;
    IntoIter &operator=(const IntoIter &) = default;
    IntoIter(IntoIter &&other) :
        cont(std::move(other.cont)),
        cur(std::move(other.cur)),
        end(std::move(other.end))
    {
        other.cur = other.end;
    }
    IntoIter &operator=(IntoIter &&other) {
        cont = std::move(other.cont);
        cur = std::move(other.cur);
        end = std::move(other.end);
        other.cur = other.end;
        return *this;
    }
    rstd::Option<T> next() {
        if (cur != end) {
            T t = std::move(*cur);
            ++cur;
            return Option<T>::Some(std::move(t));
        } else {
            return Option<T>::None();
        }
    }
    typedef IntoIter<C, T, std::reverse_iterator<J>> Rev;
    Rev rev() {
        auto r = Rev(
            std::move(cont),
            std::reverse_iterator<J>(end),
            std::reverse_iterator<J>(cur)
        );
        cur = end;
        return r;
    }
};
template <
    template <typename...> typename C,
    typename T,
    typename I=IntoIter<C, T, typename C<T>::iterator>
>
I into_iter(C<T> &&cont) {
    return I(std::move(cont), cont.begin(), cont.end());
}

} // namespace rstd
