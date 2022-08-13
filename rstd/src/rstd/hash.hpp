#pragma once

#include <functional>


namespace rstd {

template <typename T>
struct Hash {
    template <typename H>
    static void hash(const T &value, H &hasher) {
        hasher._hash(value);
    }
};

class DefaultHasher {
private:
    size_t state = 0;
public:
    void _hash_raw(size_t value) {
        state ^= value + 0x9e3779b9 + (state << 6) + (state >> 2);
    }
    template <typename T>
    void _hash(const T &value) {
        _hash_raw(std::hash<T>()(value));
    }
    template <typename T>
    void hash(const T &x) {
        Hash<T>::hash(x, *this);
    }
    size_t finish() const {
        return state;
    }
};

} // namespace rstd
