#pragma once

#include <cstdlib>
#include <utility>
#include <numeric>
#include <algorithm>

namespace rstd {

template <typename ...Types>
struct CommonSize {
    static const size_t value = 0;
};
template <typename T, typename ...Types>
struct CommonSize<T, Types...> {
    static const size_t value = std::max(sizeof(T), CommonSize<Types...>::value);
};
template <typename ...Types>
struct CommonSize<void, Types...> {
    static const size_t value = std::max(0, CommonSize<Types...>::value);
};
template <typename ...Types>
inline constexpr size_t common_size = CommonSize<Types...>::value;


template <typename ...Types>
struct CommonAlign {
    static const size_t value = 1;
};
template <typename T, typename ...Types>
struct CommonAlign<T, Types...> {
    static const size_t value = std::lcm(alignof(T), CommonAlign<Types...>::value);
};
template <typename ...Types>
struct CommonAlign<void, Types...> {
    static const size_t value = std::lcm(1, CommonAlign<Types...>::value);
};
template <typename ...Types>
inline constexpr size_t common_align = CommonAlign<Types...>::value;


template <template <size_t> typename F, size_t S, size_t Q = S - 1>
struct Visitor {
    static const size_t P = S - Q - 1;
    template <typename ...Args>
    static decltype(auto) visit(size_t i, Args &&...args) {
        if (i == P) {
            return F<P>::call(std::forward<Args>(args)...);
        } else {
            return Visitor<F, S, Q - 1>::visit(i, std::forward<Args>(args)...);
        }
    }
};
template <template <size_t> typename F, size_t S>
struct Visitor<F, S, 0> {
    template <typename ...Args>
    static decltype(auto) visit(size_t, Args &&...args) {
        return F<S - 1>::call(std::forward<Args>(args)...);
    }
};

template <typename T>
inline constexpr bool is_copyable_v = 
    std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>;


} // namespace rstd
