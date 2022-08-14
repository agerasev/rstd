#pragma once

#include <atomic>

#include <rcore/mem/maybe_uninit.hpp>

namespace rcore::mem {

// NOTE: Must subject to [constant initialization](https://en.cppreference.com/w/cpp/language/constant_initialization).
template <typename T>
class LazyStatic {
private:
    mutable std::atomic_flag init_started;
    mutable std::atomic_bool init_complete;
    mutable MaybeUninit<T> value_ = {};

    void init_once() const {
        if (!init_started.test_and_set()) {
            value_.init_in_place();
            init_complete.store(true);
        } else {
            // Spinlock.
            // TODO: Use std::atomic::{wait, notify_all} when awailable.
            while (!init_complete.load()) {}
        }
    }

    const T &value() const {
        return value_.assume_init();
    }

public:
    constexpr LazyStatic() = default;
    ~LazyStatic() {
        if (init_complete.load()) {
            value_.assume_init().~T();
        }
    }

    LazyStatic(const LazyStatic &) = delete;
    LazyStatic &operator=(const LazyStatic &) = delete;
    LazyStatic(LazyStatic &&) = delete;
    LazyStatic &operator=(LazyStatic &&) = delete;

    const T &operator*() const {
        init_once();
        return value();
    }
    const T *operator->() const {
        return &*(*this);
    }
};

} // namespace rcore::mem

#define rcore_static_block(name) \
    struct _rcore_static_block__##name##__struct { \
        _rcore_static_block__##name##__struct(); \
    } _rcore_static_block__##name##__instance; \
    _rcore_static_block__##name##__struct::_rcore_static_block__##name##__struct()

#define rcore_static_atexit(name) \
    struct _rcore_static_atexit__##name##__struct { \
        ~_rcore_static_atexit__##name##__struct(); \
    } _rcore_static_atexit__##name##__instance; \
    _rcore_static_atexit__##name##__struct::~_rcore_static_atexit__##name##__struct()
