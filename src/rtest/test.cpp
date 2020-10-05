#include "test.hpp"


using namespace rstd;

rtest_module_(rtest) {
    rtest_(dummy) {
        
    }
    rtest_should_panic_(panic) {
        panic_("Panic!");
    }
    rtest_should_panic_(segfault) {
        ((void(*)())nullptr)();
    }
}
