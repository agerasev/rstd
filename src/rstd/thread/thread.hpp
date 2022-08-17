#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <string>

#include <pthread.h>

#include <rstd/assert.hpp>
#include <rstd/fmt/format.hpp>
#include <rstd/option.hpp>
#include <rstd/result.hpp>

#include "handle.hpp"

namespace rstd::thread {

namespace _impl {

template <typename R>
class PosixThread final {
private:
    Option<Thread> handle_;
    Option<pthread_t> thread_;
    std::function<R()> main_;
    Option<R> result_;
    std::shared_ptr<std::string> panic_message_;

    static void *launch(void *data) {
        auto *this_ = static_cast<PosixThread *>(data);

        current() = this_->handle_.take_some();
        rcore_assert(bool(this_->main_));
        this_->result_ = Some(this_->main_());
        return nullptr;
    }

public:
    PosixThread() = default;
    PosixThread(const Thread &handle, std::function<R()> main_func) :
        handle_(Some(handle)),
        main_(main_func) //
    {
        panic_message_ = std::make_shared<std::string>();
        handle_.some().panic_message = panic_message_;
        handle_.some().is_main = false;

        pthread_t thread;
        rstd_assert_eq(pthread_create(&thread, nullptr, PosixThread::launch, static_cast<void *>(this)), 0);
        thread_ = Some(thread);
    }
    ~PosixThread() {
        if (!is_empty()) {
            join().unwrap();
        }
    }

    PosixThread(const PosixThread &) = delete;
    PosixThread &operator=(const PosixThread &) = delete;
    PosixThread(PosixThread &&) = delete;
    PosixThread &operator=(PosixThread &&) = delete;

    [[nodiscard]] bool is_empty() const {
        return thread_.is_none();
    }

    Result<R, std::string> join() {
        rstd_assert_eq(pthread_join(thread_.take_some(), nullptr), 0);
        if (result_.is_some()) {
            return Ok(result_.unwrap());
        } else {
            return Err(std::move(*panic_message_));
        }
    }
};

} // namespace _impl

template <typename T>
class JoinHandle final {
private:
    std::unique_ptr<_impl::PosixThread<T>> thread_;

public:
    JoinHandle() = default;
    explicit JoinHandle(std::unique_ptr<_impl::PosixThread<T>> &&thread) : thread_(std::move(thread)) {}

    [[nodiscard]] bool is_empty() const {
        return !thread_ || thread_->is_empty();
    }

    Result<T, std::string> join() {
        rstd_assert(thread_);
        return thread_->join();
    }
};

} // namespace rstd::thread
