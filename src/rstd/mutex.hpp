#pragma once

#include <pthread.h>
#include "prelude.hpp"


namespace rstd {

class _Mutex final {
private:
    Option<pthread_mutex_t> raw;

    _Mutex(pthread_mutex_t m) :
        raw(Option<pthread_mutex_t>::Some(m))
    {}

public:
    _Mutex() = default;
    static _Mutex create() {
        pthread_mutex_t m;
        assert_(pthread_mutex_init(&m, nullptr) == 0);
        return _Mutex(m);
    }
    ~_Mutex() {
        if (raw.is_some()) {
            pthread_mutex_t m = raw.take().unwrap();
            assert_(pthread_mutex_destroy(&m) == 0);
        }
    }

    _Mutex(const _Mutex &) = delete;
    _Mutex &operator=(const _Mutex &) = delete;

    _Mutex(_Mutex &&) = default;
    _Mutex &operator=(_Mutex &&) = default;

    void lock() {
        assert_(pthread_mutex_lock(&raw.get()) == 0);
    }
    bool try_lock() {
        int r = pthread_mutex_trylock(&raw.get());
        if (r == 0) {
            return true;
        } else if (r == EBUSY) {
            return false;
        } else {
            panic_("Mutex try_lock error");
        }
    }
    void unlock() {
        assert_(pthread_mutex_unlock(&raw.get()) == 0);
    }
};

template <typename T>
class Mutex {
private:
    mutable T value;
    mutable _Mutex mutex;

    void check_free() const {
#ifdef DEBUG
        assert_(this->mutex.try_lock() == true);
        this->mutex.unlock();
#endif // DEBUG
    }

public:
    Mutex() = default;
    Mutex(T &&v) :
        value(std::move(v)),
        mutex(_Mutex::create())
    {}
    ~Mutex() = default;

    T into_inner() {
        this->check_free();
        drop(this->mutex);
        return T(std::move(this->value));
    }
    T &get() {
        this->check_free();
        return this->value;
    }
};

} // namespace rstd
