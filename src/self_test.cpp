#include <rstd/prelude.hpp>
#include <rtest/prelude.hpp>

rtest_section_(one) {
    rtest_case_(dummy) {
        println_("1");
    }
    rtest_case_should_panic_(panic) {
        println_("panic");
        panic_();
    }
    rtest_case_should_panic_(segfault) {
        println_("segfault");
        ((void(*)())nullptr)();
    }
}

rtest_section_(two) {
    rtest_case_(dummy) {
        println_("2");
    }
}

#include <rtest/main.hpp>
