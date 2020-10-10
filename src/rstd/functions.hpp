#pragma once

#include <cstdlib>
#include <memory>

namespace rstd {

template <typename T>
T clone(const T &x) {
    return T(x);
}

template <typename T>
T move(T &x) {
    return T(std::move(x));
}
template <typename T>
T move(T &&x) {
    return T(std::move(x));
}

template <typename T>
void drop(T &x) {
    move(x);
}

} // namespace rstd
