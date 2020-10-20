#pragma once

#include "iter_def.hpp"


namespace rstd {
namespace iter {

template <typename T>
Empty<T> empty() {
    return Empty<T>();
}
template <typename T>
Once<T> once(T &&t) {
    return Once<T>(std::move(t));
}
template <typename F>
decltype(auto) once_with(F &&f) {
    return OnceWith<F>(std::move(f));
}
template <typename T>
Repeat<T> repeat(T &&t) {
    return Repeat<T>(std::move(t));
}
template <typename T>
Repeat<T> repeat(const T &t) {
    return Repeat<T>(t);
}
template <typename F>
decltype(auto) repeat_with(F &&f) {
    return RepeatWith<F>(std::move(f));
}
template <typename T, typename F>
Successors<T, F> successors(Option<T> &&init, F &&f) {
    return Successors<T, F>(std::move(init), std::move(f));
}

} // namespace iter
} // namespace rstd
