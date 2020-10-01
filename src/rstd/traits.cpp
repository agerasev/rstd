#include <catch.hpp>

#include "traits.hpp"

using namespace rstd;


TEST_CASE("Traits", "[traits]") {
    SECTION("Any/all") {
        REQUIRE((any_v<false, false, true>) == true);
        REQUIRE((any_v<false, false, false>) == false);

        REQUIRE((all_v<true, true, true>) == true);
        REQUIRE((all_v<true, false, true>) == false);
    }
}
