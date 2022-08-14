#pragma once

#include <functional>
#include <iostream>
#include <pthread.h>

#include <rcore/sync/lazy_static.hpp>

namespace rstd::thread {

namespace _impl {

template <typename T>
class PthreadKey final {
private:
    pthread_key_t raw = 0;

public:
    PthreadKey() {
        pthread_key_create(&raw, [](void *p) { delete (T *)p; });
    }
    ~PthreadKey() {
        pthread_key_delete(raw);
    }

    PthreadKey(const PthreadKey &) = delete;
    PthreadKey &operator=(const PthreadKey &) = delete;
    PthreadKey(PthreadKey &&) = delete;
    PthreadKey &operator=(PthreadKey &&) = delete;

    T &value() const {
        T *value = static_cast<T *>(pthread_getspecific(raw));
        if (value == nullptr) {
            value = new T();
            pthread_setspecific(raw, static_cast<void *>(value));
        }
        return value;
    }
};

} // namespace _impl

template <typename T>
class LocalKey final {
private:
    rcore::mem::LazyStatic<_impl::PthreadKey<T>> key;

public:
    T &operator*() const {
        return key->value();
    }
    T *operator->() const {
        return &*(*this);
    }
};

} // namespace rstd::thread
