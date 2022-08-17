#include "test.hpp"

#include <rstd/panic.hpp>
#include <rtest/main.hpp>

rtest_module(rtest) {
    rtest_case(dummy) {}
    rtest_case_should_panic(panic) {
        rstd_panic("Panic!");
    }
#if defined(__MINGW32__) || defined(__MINGW64__)
    // Cannot catch SegFault on MinGW
#else
    rtest_case_should_panic(segfault) {
        ((void (*)()) nullptr)();
    }
#endif
}

int main(int argc, char *argv[]) {
    return rtest::main(argc, argv);
}
