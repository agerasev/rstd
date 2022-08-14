#pragma once

#include <atomic>
#include <concepts>

namespace rcore::sync {

class Once {
private:
    mutable std::atomic_flag done;

public:
    Once() = default;
    ~Once() = default;

    Once(const Once &) = delete;
    Once &operator=(const Once &) = delete;
    Once(Once &&) = delete;
    Once &operator=(Once &&) = delete;

    template <typename F>
        requires std::invocable<F>
    inline void call_once(F &&f) const {
        if (!done.test_and_set()) {
            f();
        }
    }
};

} // namespace rcore::sync
