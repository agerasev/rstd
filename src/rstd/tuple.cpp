#include <catch.hpp>
#include <memory>

#include "tuple.hpp"

using namespace rstd;


TEST_CASE("Tuple", "[tuple]") {
    SECTION("Getters") {
        Tuple<bool, int, double> a(true, 1, 3.1415);
        REQUIRE(a.size() == 3);

        REQUIRE(a.get<0>() == true);
        REQUIRE(a.get<1>() == 1);
        REQUIRE(a.get<2>() == Approx(3.1415));
    }
    SECTION("Move") {
        Tuple<std::unique_ptr<int>, int> a(std::make_unique<int>(123), 456);
        REQUIRE(a.size() == 2);
        REQUIRE(*a.get<0>() == 123);
        REQUIRE(a.get<1>() == 456);

        auto b(std::move(a));
        REQUIRE(b.size() == 2);
        REQUIRE(*b.get<0>() == 123);
        REQUIRE(b.get<1>() == 456);
    }
    SECTION("Print") {
        Tuple<bool, int, std::string> a(true, 123, "abc");
        REQUIRE(format_(a) == "(1, 123, abc)");
        REQUIRE(format_(Tuple<>()) == "()");
    }
}
