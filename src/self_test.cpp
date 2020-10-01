#include <rstd/prelude.hpp>
#include <rtest/prelude.hpp>

test_section_(base) {
    test_case_(one) {
        println_("b1");
    }
    test_case_(two) {
        println_("b2");
    }
}

test_section_(dummy) {
    test_case_(one) {
        println_("d1");
    }
    test_case_(two) {
        println_("d2");
    }
}


#include <rtest/main.hpp>
