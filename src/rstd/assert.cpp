#include <rtest/test.hpp>
#include <string>
#include "assert.hpp"

using namespace rstd;


rtest_section_(assert) {
    rtest_case_(bool) {
        assert_(1 + 1 == 2);
    }
    rtest_case_(equal) {
        assert_eq_("132", std::to_string(11*12));
    }
}
