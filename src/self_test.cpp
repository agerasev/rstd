#include <rstd/prelude.hpp>
#include <rtest/prelude.hpp>

rtest_section_(base) {
    rtest_case_(one) {
        println_("b1");
    }
    rtest_case_(two) {
        println_("b2");
    }
}

rtest_section_(dummy) {
    rtest_case_(one) {
        println_("d1");
    }
    rtest_case_(two) {
        println_("d2");
        //panic_();
    }
}


#include <rtest/main.hpp>
