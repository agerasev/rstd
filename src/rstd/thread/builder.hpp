#pragma once

#include <concepts>
#include <functional>
#include <pthread.h>
#include <type_traits>

#include "prelude.hpp"
#include <rcore/thread.hpp>

namespace rstd::thread {

template <typename T>
class JoinHandle;

class Builder;

class Builder final {
private:
    rcore::Thread info;

public:
    Builder() : info(rcore::thread::current()) {
        info.is_main = false;
    }

    Builder &set_stdin(std::istream &stream) {
        this->info.stdio.in = &stream;
    }
    Builder &set_stdout(std::ostream &stream) {
        this->info.stdio.out = &stream;
    }
    Builder &set_stderr(std::ostream &stream) {
        this->info.stdio.err = &stream;
    }

    Builder &set_panic_hook(std::function<void(const std::string &)> hook) {
        this->info.panic_hook = hook;
    }

private:
    template <typename F, typename T>
    struct Arg {
        rcore::Thread info;
        F main;
    };

    template <typename T>
    static void __delete(void *obj) {
        delete static_cast<T *>(obj);
    }
    template <typename F, typename T>
    static void *__call(void *a) {
        Arg<F, T> *arg = (Arg<F, T> *)a;
        T *ret = nullptr;
        rcore::thread::current() = arg->info;

        pthread_cleanup_push((__delete<Arg<F, T>>), static_cast<void *>(arg));
        ret = new T((arg->main)());
        pthread_cleanup_pop(1);

        return (void *)ret;
    }

public:
    template <std::invocable F>
    JoinHandle<std::invoke_result_t<F>> spawn(F main) const {
        using T = std::invoke_result_t<F>;
        pthread_t thread_;
        Arg<F, T> *arg = new Arg<F, T>{info, std::move(main)};

        assert_(pthread_create(&thread_, nullptr, (__call<F, T>), (void *)arg) == 0);

        return JoinHandle<T>(thread_);
    }
};

template <typename F>
decltype(auto) spawn(F main) {
    return Builder().spawn(main);
}

} // namespace rstd::thread
