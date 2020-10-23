#pragma once

#include <vector>
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

template <typename T>
class IntoIter : public Iterator<T, IntoIter<T>> {
private:
    std::vector<T> data;
    size_t cur, end;
    bool rev_;
public:
    template <typename J>
    IntoIter(J b, J e, bool r=false) {
        for (J j = b; j != e; ++j) {
            data.push_back(std::move(*j));
        }
        cur = 0;
        end = data.size();
        rev_ = r;
    }
    IntoIter(const IntoIter &) = default;
    IntoIter &operator=(const IntoIter &) = default;
    IntoIter(IntoIter &&other) :
        data(std::move(other.data)),
        cur(other.cur),
        end(other.end),
        rev_(other.rev_)
    {
        other.cur = other.end;
    }
    IntoIter &operator=(IntoIter &&other) {
        data = std::move(other.data);
        cur = other.cur;
        end = other.end;
        rev_ = other.rev_;
        other.cur = other.end;
        return *this;
    }

    rstd::Option<T> next() {
        if (cur != end) {
            T t;
            if (!rev_) {
                t = std::move(data[cur]);
                ++cur;
            } else {
                t = std::move(data[end - 1]);
                --end;
            }
            return Option<T>::Some(std::move(t));
        } else {
            return Option<T>::None();
        }
    }
    typedef IntoIter Rev;
    Rev rev() {
        rev_ = !rev_;
        return std::move(*this);
    }
};
template <
    template <typename...> typename C,
    typename T,
    typename I=IntoIter<T>
>
I into_iter(C<T> &&cont) {
    auto r = I(cont.begin(), cont.end());
    drop(cont);
    return r;
}

} // namespace rstd
