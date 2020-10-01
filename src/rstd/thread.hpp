#pragma once

#include <pthread.h>
#include <functional>
#include "prelude.hpp"


namespace rstd {

template <typename T>
class JoinHandle;

namespace thread {

template <typename T, typename F>
JoinHandle<T> spawn(F f);

} // namespace thread

template <typename T>
class JoinHandle final {
private:
    Option<pthread_t> thread_;

public:
    JoinHandle() : thread_(Option<pthread_t>::None()) {}

private:
    JoinHandle(pthread_t thread_) : thread_(Option<pthread_t>::Some(thread_)) {}

public:
    JoinHandle(const JoinHandle &) = delete;
    JoinHandle &operator=(const JoinHandle &) = delete;

    JoinHandle(JoinHandle &&other) = default;
    JoinHandle &operator=(JoinHandle &&other) {
        assert_(thread_.is_none());
        thread_ = std::move(other.thread_);
    }

    Result<T> join() {
        void *rv = nullptr;
        assert_(pthread_join(thread_.take(), &rv) == 0);
        if (rv != nullptr) {
            auto ret = Result<T>::Ok(std::move((T*)rv));
            delete (T*)rv;
            return ret;
        } else {
            return Result<T>::Err(Tuple<>());
        }
    }

    ~JoinHandle() {
        if (thread_.is_some()) {
            join().unwrap();
        }
    }

    explicit operator bool() const {
        return thread_.is_some();
    }

    template <typename F>
    friend JoinHandle<T> thread::spawn(F f);
};

namespace thread {

template <typename T>
void __delete(void *obj) {
    delete (T*)obj;
}

template <typename T, typename F>
void *__call(void *arg) {
    F *f = (F*)arg;
    T *ret = nullptr;
    pthread_cleanup_push((__delete<F>), (void*)f);
    ret = new T(std::move(f()));
    pthread_cleanup_pop(1);
    return (void*)ret;
}

template <typename T, typename F>
JoinHandle<T> spawn(F f) {
    pthread_t thread_;
    assert_(pthread_create(
        &thread_,
        nullptr,
        (__call<T, F>),
        (void*)(new F(std::move(f)))
    ) == 0);

    return JoinHandle<T>(thread_);
}

} // namespace thread

} // namespace rstd
