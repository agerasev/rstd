#pragma once

#include <pthread.h>
#include <functional>
#include <type_traits>
#include <rcore/thread.hpp>
#include "prelude.hpp"


namespace rstd {

template <typename T>
class JoinHandle;

namespace thread {

class Builder;

} // namespace thread

template <typename T=Tuple<>>
class JoinHandle final {
private:
    Option<pthread_t> thread_;

public:
    JoinHandle() : thread_(Option<pthread_t>::none()) {}

private:
    explicit JoinHandle(pthread_t thread_) : thread_(Option<pthread_t>::some(thread_)) {}

public:
    JoinHandle(const JoinHandle &) = delete;
    JoinHandle &operator=(const JoinHandle &) = delete;

    JoinHandle(JoinHandle &&other) = default;
    JoinHandle &operator=(JoinHandle &&other) {
        assert_(thread_.is_none());
        thread_ = std::move(other.thread_);
        return *this;
    }

    Result<T> join() {
        T *rv = nullptr;
        assert_(pthread_join(thread_.take().unwrap(), (void**)&rv) == 0);
        if (rv != nullptr) {
            auto ret = Result<T>::ok(std::move(*rv));
            delete rv;
            return ret;
        } else {
            return Result<T>::err(Tuple<>());
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

    friend class thread::Builder;
};

namespace thread {

class Builder final {
private:
    rcore::Thread info;

public:
    Builder() : info(rcore::thread::current()) {
        info.is_main = false;
    }
    
    void set_stdin(std::istream &stream) {
        this->info.stdio.in = &stream;
    }
    void set_stdout(std::ostream &stream) {
        this->info.stdio.out = &stream;
    }
    void set_stderr(std::ostream &stream) {
        this->info.stdio.err = &stream;
    }
    Builder stdin_(std::istream &stream) {
        Builder self = std::move(*this);
        self.set_stdin(stream);
        return self;
    }
    Builder stdout_(std::ostream &stream) {
        Builder self = std::move(*this);
        self.set_stdout(stream);
        return self;
    }
    Builder stderr_(std::ostream &stream) {
        Builder self = std::move(*this);
        self.set_stderr(stream);
        return self;
    }

    void set_panic_hook(std::function<void(const std::string &)> hook) {
        this->info.panic_hook = hook;
    }
    Builder panic_hook(std::function<void(const std::string &)> hook) {
        Builder self = std::move(*this);
        self.set_panic_hook(hook);
        return self;
    }

private:
    template <typename F, typename T>
    struct Arg {
        rcore::Thread info;
        F main;
    };

    template <typename T>
    static void __delete(void *obj) {
        delete (T*)obj;
    }
    template <typename F, typename T>
    static void *__call(void *a) {
        Arg<F, T> *arg = (Arg<F, T> *)a;
        T *ret = nullptr;
        rcore::thread::current() = arg->info;

        pthread_cleanup_push((__delete<Arg<F, T>>), (void*)arg);
        ret = new T((arg->main)());
        pthread_cleanup_pop(1);

        return (void*)ret;
    }

public:
    template <
        typename F,
        typename T=std::invoke_result_t<F>,
        typename X=std::enable_if_t<!std::is_void_v<T>, void>
    >
    JoinHandle<T> spawn(F main) const {
        pthread_t thread_;
        Arg<F, T> *arg = new Arg<F, T>{
            info,
            std::move(main)
        };

        assert_(pthread_create(
            &thread_,
            nullptr,
            (__call<F, T>),
            (void*)arg
        ) == 0);

        return JoinHandle<T>(thread_);
    }

    template <
        typename F,
        typename T=std::invoke_result_t<F>,
        typename X=std::enable_if_t<std::is_void_v<T>, void>
    >
    inline JoinHandle<> spawn(F main) const {
        return this->spawn([main]() {
            main();
            return Tuple<>();
        });
    }
};

template <typename F>
decltype(auto) spawn(F main) {
    return Builder().spawn(main);
}

} // namespace thread

} // namespace rstd
