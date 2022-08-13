#include "io.hpp"

#include "thread.hpp"


using namespace rcore;

std::istream &rcore::stdin_() {
    Thread &ct = thread::current();
    if (ct.stdio.in != nullptr) {
        return *ct.stdio.in;
    } else {
        return std::cin;
    }
}
std::ostream &rcore::stdout_() {
    Thread &ct = thread::current();
    if (ct.stdio.out != nullptr) {
        return *ct.stdio.out;
    } else {
        return std::cout;
    }
}
std::ostream &rcore::stderr_() {
    Thread &ct = thread::current();
    if (ct.stdio.err != nullptr) {
        return *ct.stdio.err;
    } else {
        return std::cerr;
    }
}
