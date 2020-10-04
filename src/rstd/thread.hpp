#pragma once

#include <pthread.h>
#include <functional>
#include <core/thread.hpp>
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
        return *this;
    }

    Result<T> join() {
        T *rv = nullptr;
        assert_(pthread_join(thread_.take().unwrap(), (void**)&rv) == 0);
        if (rv != nullptr) {
            auto ret = Result<T>::Ok(std::move(*rv));
            delete rv;
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

    friend class thread::Builder;
};

namespace thread {

class Builder final {
private:
    core::Thread info;

public:
    Builder() : info(core::thread::current()) {}
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
    template <typename T>
    struct Arg {
        core::Thread info;
        std::function<T()> main;
    };

    template <typename T>
    static void __delete(void *obj) {
        delete (T*)obj;
    }
    template <typename T>
    static void *__call(void *a) {
        Arg<T> *arg = (Arg<T> *)a;
        T *ret = nullptr;
        core::thread::current() = arg->info;

        pthread_cleanup_push((__delete<Arg<T>>), (void*)arg);
        ret = new T((arg->main)());
        pthread_cleanup_pop(1);

        return (void*)ret;
    }

public:
    template <typename T>
    JoinHandle<T> spawn(std::function<T()> main) const {
        pthread_t thread_;
        Arg<T> *arg = new Arg<T>();
        arg->info = info;
        arg->main = std::move(main);

        assert_(pthread_create(
            &thread_,
            nullptr,
            (__call<T>),
            (void*)arg
        ) == 0);

        return JoinHandle<T>(thread_);
    }

    inline JoinHandle<> spawn(std::function<void()> main) const {
        return this->spawn(std::function<Tuple<>()>([main]() {
            main();
            return Tuple<>();
        }));
    }
};

template <typename T>
JoinHandle<T> spawn(std::function<T()> main) {
    return Builder().spawn(main);
}
inline JoinHandle<> spawn(std::function<void()> main) {
    return Builder().spawn(main);
}

} // namespace thread

} // namespace rstd
