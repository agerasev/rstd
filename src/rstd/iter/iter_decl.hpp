#pragma once

#include <type_traits>
#include <rstd/prelude.hpp>


namespace rstd {
namespace iter {

template <
    typename T, typename I, typename F,
    typename R=std::invoke_result_t<F, T &&>
>
class Map;

template <
    typename T, typename I, typename F,
    typename R=option_some_type<std::invoke_result_t<F, T &&>>
>
class MapWhile;

template <typename T, typename I, typename F>
class Filter;

template <
    typename T, typename I, typename F,
    typename R=option_some_type<std::invoke_result_t<F, T &&>>
>
class FilterMap;

template <typename T, typename I>
class Cycle;

template <typename T, typename I, typename J>
class Chain;

template <
    typename T, typename I, typename S, typename F,
    typename R=option_some_type<std::invoke_result_t<F, S *, T &&>>
>
class Scan;

template <typename T, typename I>
class Fuse;

template <typename T, typename U, typename I, typename J>
class Zip;

} // namespace iter

template <typename I>
struct IteratorItem {
    typedef decltype(((I*)nullptr)->next().unwrap()) type;
};
template <typename I>
using iterator_item = typename IteratorItem<I>::type;

} // namespace rstd
