#pragma once

#include <pthread.h>
#include "prelude.hpp"


namespace rstd {

class _Mutex final {
private:
    Option<pthread_mutex_t> raw;
#ifdef DEBUG
    bool locked = false;
#endif // DEBUG

public:
    _Mutex() {
        pthread_mutex_t m;
        assert_(pthread_mutex_init(&m, nullptr) == 0);
        raw = Option<pthread_mutex_t>::Some(m);
    }
    ~_Mutex() {
        if (raw.is_some()) {
#ifdef DEBUG
            assert_(!locked);
#endif // DEBUG
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
#ifdef DEBUG
        assert_(!locked);
        locked = true;
#endif // DEBUG
    }
    bool try_lock() {
        int r = pthread_mutex_trylock(&raw.get());
        if (r == 0) {
#ifdef DEBUG
            assert_(!locked);
            locked = true;
#endif // DEBUG
            return true;
        } else if (r == EBUSY) {
#ifdef DEBUG
            assert_(locked);
#endif // DEBUG
            return false;
        } else {
            panic_("Mutex try_lock error");
            // Unreachable
            return false;
        }
    }
    void unlock() {
#ifdef DEBUG
        assert_(locked);
        locked = false;
#endif // DEBUG
        assert_(pthread_mutex_unlock(&raw.get()) == 0);
    }
};



template <typename T>
class Mutex final {
public:
    class Guard final {
    private:
        Option<const Mutex *> origin;

        void release() {
            if (origin.is_some()) {
                origin.take().unwrap()->mutex.unlock();
            }
        }

    public:
        Guard() = default;
        explicit Guard(const Mutex &m) : origin(Option<const Mutex *>::Some(&m)) {}

        Guard(Guard &&) = default;
        Guard &operator=(Guard &&other) {
            this->release();
            origin = std::move(other.origin);
            return *this;
        }

        Guard(const Guard &) = delete;
        Guard &operator=(const Guard &) = delete;

        ~Guard() {
            this->release();
        }
        
        T &get() {
            assert_(origin.is_some());
            return origin.get()->value;
        }
        const T &get() const {
            assert_(origin.is_some());
            return origin.get()->value;
        }

        T &operator*() {
            return this->get();
        }
        const T &operator*() const {
            return this->get();
        }
        T *operator->() {
            return &this->get();
        }
        const T *operator->() const {
            return &this->get();
        }
    };

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
    explicit Mutex(T &&v) : value(std::move(v)) {}
    ~Mutex() = default;

    Guard lock() {
        mutex.lock();
        return Guard(*this);
    }
    Result<Guard> try_lock() {
        if (mutex.try_lock()) {
            return Result<Guard>::Ok(Guard(*this));
        } else {
            return Result<Guard>::Err(Tuple<>());
        }
    }

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
