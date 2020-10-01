#include <catch.hpp>
#include "format.hpp"

using namespace rstd;


TEST_CASE("Format", "[format]") {
    SECTION("Empty") {
        REQUIRE(format_() == "");
    }
    SECTION("Single string") {
        REQUIRE(format_("abc") == "abc");
    }
    SECTION("Single int") {
        REQUIRE(format_("int") == "int");
    }
    SECTION("Format one") {
        REQUIRE(format_("a: {};", 123) == "a: 123;");
    }
    SECTION("Format two") {
        REQUIRE(format_("a: {}, b: {};", 123, "abc") == "a: 123, b: abc;");
    }
    SECTION("Format escape") {
        REQUIRE(format_("{{abc}}") == "{abc}");
    }
    SECTION("Format no escape") {
        REQUIRE(format_(std::string("{abc}")) == "{abc}");
    }
    /*
    // should panic
    SECTION("Wrong format") {
        format_("}{");
        format_("}");
        format_("{");
        format_("{ {");
        format_("} }");
        format_("{}");
        format_("", 1);
        format_("{}", 1, 2);
    }
    */
}
