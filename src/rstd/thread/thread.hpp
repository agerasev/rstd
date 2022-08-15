#pragma once

#include <functional>
#include <iostream>
#include <string>

#include <rcore/sync/lazy_static.hpp>
#include <rstd/assert.hpp>
#include <rstd/option.hpp>
#include <rstd/panic.hpp>

namespace rstd::thread {

struct Stdio {
    std::istream *in = nullptr;
    std::ostream *out = nullptr;
    std::ostream *err = nullptr;
};

class Thread final {
private:
    Stdio stdio_;
    panic::PanicHook panic_hook_ = nullptr;

    friend class Builder;
};

template <typename T>
class JoinHandle final {
private:
    Option<pthread_t> thread_;

public:
    JoinHandle() : thread_(None()) {}

private:
    explicit JoinHandle(pthread_t thread_) : thread_(Some(thread_)) {}

public:
    JoinHandle(const JoinHandle &) = delete;
    JoinHandle &operator=(const JoinHandle &) = delete;

    JoinHandle(JoinHandle &&other) = default;
    JoinHandle &operator=(JoinHandle &&other) {
        rstd_assert(thread_.is_none());
        thread_ = std::move(other.thread_);
        return *this;
    }

    Result<T, std::monostate> join() {
        T *rv = nullptr;
        rstd_assert(pthread_join(thread_.take().unwrap(), (void **)&rv) == 0);
        if (rv != nullptr) {
            auto ret = Ok<T>(std::move(*rv));
            delete rv;
            return ret;
        } else {
            return Err(std::monostate());
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

[[nodiscard]] Thread &current();

} // namespace rstd::thread
