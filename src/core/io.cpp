#include "io.hpp"

#include "thread.hpp"


using namespace core;

std::istream &core::stdin_() {
    Thread &ct = thread::current();
    if (ct.stdio.in != nullptr) {
        return *ct.stdio.in;
    } else {
        return std::cin;
    }
}
std::ostream &core::stdout_() {
    Thread &ct = thread::current();
    if (ct.stdio.out != nullptr) {
        return *ct.stdio.out;
    } else {
        return std::cout;
    }
}
std::ostream &core::stderr_() {
    Thread &ct = thread::current();
    if (ct.stdio.err != nullptr) {
        return *ct.stdio.err;
    } else {
        return std::cerr;
    }
}
