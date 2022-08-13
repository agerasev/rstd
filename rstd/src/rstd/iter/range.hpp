#pragma once

#include "iterator.hpp"


namespace rstd {

template <typename T>
struct Range : public Iterator<T, Range<T>> {
private:
    T start_ = 0, end_ = 0;
    bool rev_ = false;

public:
    explicit Range(T e) : Range(0, e) {}
    Range(T s, T e) : start_(s), end_(e) {}
    Range(const Range &) = default;
    Range &operator=(const Range &) = default;
    Range(Range &&other) :
        start_(other.start_),
        end_(other.end_),
        rev_(other.rev_)
    {
        other.start_ = other.end_;
    }
    Range &operator=(Range &&other) {
        start_ = other.start_;
        end_ = other.end_;
        rev_ = other.rev_;
        other.start_ = other.end_;
        return *this;
    }
    rstd::Option<T> next() {
        if (start_ < end_) {
            T i;
            if (!rev_) {
                i = start_;
                ++start_;
            } else {
                --end_;
                i = end_;
            }
            return Option<T>::Some(i);
        } else {
            return Option<T>::None();
        }
    }
    typedef Range Rev;
    Range rev() {
        Range r = clone(*this);
        r.rev_ = !rev_;
        start_ = end_;
        return r;
    }
};

} // namespace rstd
