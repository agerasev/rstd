#pragma once

#include <array>
#include <cstdint>
#include <utility>

namespace rcore::mem {

/// Type which size and alignment are identical to `T` but it can be uninitialized.
/// MaybeUninit<T> is POD for any `T`.
template <typename T>
struct MaybeUninit {
    alignas(T) std::array<uint8_t, sizeof(T)> payload;

    MaybeUninit() = default;

    [[nodiscard]] const T &assume_init() const {
        return *static_cast<const T *>(this->payload->data());
    }
    [[nodiscard]] T &assume_init() {
        return *static_cast<T *>(this->payload->data());
    }

    template <typename... Args>
    void init_in_place(Args &&...args) {
        new (&assume_init()) T(std::forward<Args>(args)...);
    }
};

} // namespace rcore::mem
