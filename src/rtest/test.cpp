#include "test.hpp"

using namespace rstd;

rtest_module(rtest) {
    rtest_case(dummy) {}
    rtest_case_should_panic(panic) {
        panic_("Panic!");
    }
#if defined(__MINGW32__) || defined(__MINGW64__)
    // Cannot catch SegFault on MinGW
#else
    rtest_case_should_panic(segfault) {
        ((void (*)()) nullptr)();
    }
#endif
}
