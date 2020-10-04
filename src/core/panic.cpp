#include "panic.hpp"

#include <pthread.h>
#include <iostream>
#include "io.hpp"
#include "thread.hpp"


using namespace core;

static void default_panic_hook(const std::string &message) {
    stderr_()
    << "Thread " << pthread_self()
    << " panicked: " << std::endl
    << message << std::endl;
}

std::function<void(const std::string &)> core::panic_hook() {
    auto hook = thread::current().panic_hook;
    if (hook) {
        return hook;
    } else {
        return default_panic_hook;
    }
}

// FIXME: Print call stack trace
[[ noreturn ]] void core::panic(const std::string &message) {
    panic_hook()(message);
    pthread_exit(nullptr);
}
