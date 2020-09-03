#pragma once

#include <iostream>

namespace core {

namespace fmt {

template <typename T>
struct Display {
    static void fmt(const T &t, std::ostream &o) {
        o << t;
    }
};

} // namespace fmt

template <typename T>
void write(std::ostream &o, const T &t) {
    Display<T>::fmt(t, o);
}
template <typename ...Args>
void write(std::ostream &o, const Args &...args) {

}

} // namespace core
