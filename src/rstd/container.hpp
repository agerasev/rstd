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


template <size_t S, size_t Q = S - 1>
struct _Visit {
    static const size_t P = S - Q - 1;
    template <typename F>
    static decltype(auto) visit(size_t i, F &&f) {
        if (i == P) {
            return f.template operator()<P>();
        } else {
            return _Visit<S, Q - 1>::visit(i, std::forward<F>(f));
        }
    }
};
template <size_t S>
struct _Visit<S, 0> {
    template <typename F>
    static decltype(auto) visit(size_t, F &&f) {
        return f.template operator()<S - 1>();
    }
};

template <typename T>
inline constexpr bool is_copyable_v = 
    std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>;


} // namespace rstd
