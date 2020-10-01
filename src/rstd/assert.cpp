#include <catch.hpp>
#include <string>
#include "assert.hpp"

using namespace rstd;


TEST_CASE("Assert", "[assert]") {
    SECTION("Bool") {
        assert_(1 + 1 == 2);
    }
    SECTION("Equal") {
        assert_eq_("132", std::to_string(11*12));
    }
}
