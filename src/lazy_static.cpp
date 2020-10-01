#include <catch.hpp>
#include <memory>

#include "lazy_static.hpp"

lazy_static_(int, _test_primitive) {
    return 1;
}

lazy_static_(std::vector<int>, _test_compound) {
    return std::vector<int>{0, 1, 2, 3, 4, 5};
}

TEST_CASE("Lazy static", "[lazy_static]") {
    SECTION("Primitive") {
        REQUIRE(*_test_primitive == 1);
    }
    SECTION("Compound") {
        const std::vector<int> &vec = *_test_compound;
        for (int i = 0; i < int(vec.size()); ++i) {
            REQUIRE(vec[i] == i);
        }
    }
}
