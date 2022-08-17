#pragma once

#include <concepts>
#include <functional>
#include <type_traits>

#include "thread.hpp"

namespace rstd::thread {

inline namespace {

class Builder final {
private:
    Thread handle_;

public:
    Builder() : handle_(current()) {
        handle_.is_main = false;
        handle_.panic_message.reset();
    }

    Builder &set_stdin(std::istream &stream) {
        this->handle_.stdio.in = &stream;
        return *this;
    }
    Builder &set_stdout(std::ostream &stream) {
        this->handle_.stdio.out = &stream;
        return *this;
    }
    Builder &set_stderr(std::ostream &stream) {
        this->handle_.stdio.err = &stream;
        return *this;
    }

    Builder &set_panic_hook(PanicHook hook) {
        this->handle_.panic_hook = std::move(hook);
        return *this;
    }

    template <
        std::invocable F,
        typename T = std::invoke_result_t<F>,
        typename R = std::conditional_t<std::is_same_v<T, void>, std::monostate, T>>
    auto spawn(F main) const {
        auto rmain = [main]() {
            if constexpr (!std::is_same_v<T, void>) {
                return main;
            } else {
                return [main]() {
                    main();
                    return std::monostate();
                };
            }
        }();
        return _impl::PosixThread<R>(handle_, rmain);
    }
};

} // namespace

template <std::invocable F>
decltype(auto) spawn(F main) {
    return Builder().spawn(main);
}

} // namespace rstd::thread
