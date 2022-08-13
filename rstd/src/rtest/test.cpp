#include "test.hpp"


using namespace rstd;

rtest_module_(rtest) {
    rtest_(dummy) {
        
    }
    rtest_should_panic_(panic) {
        panic_("Panic!");
    }
#if defined(__MINGW32__) || defined(__MINGW64__)
    // Cannot catch SegFault on MinGW
#else
    rtest_should_panic_(segfault) {
        ((void(*)())nullptr)();
    }
#endif
}
