#include <rtest.hpp>
#include <string>
#include "assert.hpp"

using namespace rstd;


rtest_module_(assert) {
    rtest_(bool) {
        assert_(1 + 1 == 2);
    }
    rtest_(equal) {
        assert_eq_("132", std::to_string(11*12));
    }
}
