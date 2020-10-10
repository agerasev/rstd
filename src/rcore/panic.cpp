#include "panic.hpp"

#include <cstdlib>
#include <pthread.h>
#include <iostream>
#include "io.hpp"
#include "thread.hpp"


using namespace rcore;

static void default_panic_hook(const std::string &message) {
    std::string thread_name;
    if (thread::current().is_main) {
        thread_name = "Main";
    } else {
        thread_name = std::to_string(pthread_self());
    }
    stderr_()
    << "Thread " << thread_name
    << " panicked: " << std::endl
    << message << std::endl;
}

std::function<void(const std::string &)> rcore::panic_hook() {
    auto hook = thread::current().panic_hook;
    if (hook) {
        return hook;
    } else {
        return default_panic_hook;
    }
}

// FIXME: Print call stack trace
[[ noreturn ]] void rcore::panic(const std::string &message) {
    panic_hook()(message);
    if (!thread::current().is_main) {
        pthread_exit(nullptr);
    } else {
        exit(1);
    }
}
