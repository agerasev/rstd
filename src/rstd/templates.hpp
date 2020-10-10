#pragma once

#include <cstdlib>
#include <memory>

namespace rstd {

template <size_t P, typename ...Types>
struct NthType {};
template <size_t P, typename T, typename ...Types>
struct NthType<P, T, Types...> {
    typedef typename NthType<P - 1, Types...>::type type;
};
template <typename T, typename ...Types>
struct NthType<0, T, Types...> {
    typedef T type;
};
template <size_t P, typename ...Types>
using nth_type = typename NthType<P, Types...>::type;

template <size_t P, typename ...Args>
struct NthArg {};
template <size_t P, typename T, typename ...Args>
struct NthArg<P, T, Args...> {
    static nth_type<P - 1, Args...> &get(const T &, Args &...args) {
        return NthArg<P - 1, Args...>::get(args...);
    }
    static const nth_type<P - 1, Args...> &get(const T &, const Args &...args) {
        return NthArg<P - 1, Args...>::get(args...);
    }
};
template <typename T, typename ...Args>
struct NthArg<0, T, Args...> {
    static T &get(T &t, Args &...) {
        return t;
    }
    static const T &get(const T &t, const Args &...) {
        return t;
    }
};
template <size_t P, typename ...Args>
nth_type<P, Args...> &nth_arg(Args &...args) {
    return NthArg<P, Args...>::get(args...);
}
template <size_t P, typename ...Args>
const nth_type<P, Args...> &nth_arg(const Args &...args) {
    return NthArg<P, Args...>::get(args...);
}

template <bool ...Values>
struct Any {
    static const bool value = false;
};
template <bool X, bool ...Values>
struct Any<X, Values...> {
    static const bool value = X || Any<Values...>::value;
};
template <bool ...Values>
inline constexpr bool any_v = Any<Values...>::value;

template <bool ...Values>
struct All {
    static const bool value = true;
};
template <bool X, bool ...Values>
struct All<X, Values...> {
    static const bool value = X && All<Values...>::value;
};
template <bool ...Values>
inline constexpr bool all_v = All<Values...>::value;

} // namespace rstd
