#pragma once

#include <iostream>
#include <functional>
#include "once.hpp"
#include "io.hpp"


namespace rcore {

class Thread {
public:
    rcore::StdIo stdio;
    std::function<void(const std::string &)> panic_hook;
};

template <typename T, void (*FO)(), T (*FT)()>
class ThreadLocal {
private:
    rcore::Once once;
    pthread_key_t key;

public:
    void __init() {
        pthread_key_create(&key, [](void *p) {
            if (p != nullptr) {
                delete (T*)p;
            }
        });
    }

public:
    ThreadLocal() = default;
    ~ThreadLocal() = default;

    ThreadLocal(const ThreadLocal &) = delete;
    ThreadLocal &operator=(const ThreadLocal &) = delete;

    T &operator*() {
        once.call_once(FO);
        T *x = (T *)pthread_getspecific(key);
        if (x == nullptr) {
            x = new T(FT());
            pthread_setspecific(key, x);
        }
        return *x;
    }
    T *operator->() {
        return &this->operator*();
    }
};

namespace thread {

Thread &current();

} // namespace thread

} // namespace rcore


#define __thread_local_(prefix, Type, name) \
    prefix void __##name##__init(); \
    prefix Type __##name##__create(); \
    prefix ::rcore::ThreadLocal<Type, __##name##__init, __##name##__create> name; \
    prefix void __##name##__init() { \
        name.__init(); \
    } \
    prefix Type __##name##__create()

#define thread_local_(Type, name) \
    __thread_local_(, Type, name)

#define static_thread_local_(Type, name) \
    __thread_local_(static, Type, name)

#define extern_thread_local_(Type, name) \
    extern void __##name##__init(); \
    extern Type __##name##__create(); \
    extern ::rcore::ThreadLocal<Type, __##name##__init, __##name##__create> name

