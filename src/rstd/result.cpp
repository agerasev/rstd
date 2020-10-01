#include <catch.hpp>
#include <memory>

#include "result.hpp"

using namespace rstd;


TEST_CASE("Result", "[result]") {
    SECTION("Create") {
        auto a = Result<int, float>::Ok(123);
        REQUIRE(a.is_ok());
        REQUIRE(a.unwrap() == 123);

        auto b = Result<int, float>::Err(3.1415f);
        REQUIRE(b.is_err());
        REQUIRE(b.unwrap_err() == Approx(3.1415f));
    }
    SECTION("Move") {
        std::unique_ptr<int> ptr = std::make_unique<int>(123);
        auto a = Result<std::unique_ptr<int>, std::string>::Ok(std::move(ptr));
        REQUIRE(*a.get() == 123);
        ptr = a.unwrap();
        REQUIRE(*ptr == 123);
    }
    SECTION("Destroy") {
        std::shared_ptr<int> ptr = std::make_shared<int>(123);
        REQUIRE(ptr.use_count() == 1);
        auto a = Result<std::shared_ptr<int>, std::string>::Ok(ptr);
        REQUIRE(ptr.use_count() == 2);
        REQUIRE(*a.get() == 123);
        REQUIRE(ptr.use_count() == 2);
        {
            std::shared_ptr<int> cptr = a.unwrap();
            REQUIRE(*cptr == 123);
            REQUIRE(ptr.use_count() == 2);
        }
        REQUIRE(ptr.use_count() == 1);
    }
    SECTION("Print") {
        auto a = Result<int, std::string>::Ok(123);
        REQUIRE(format_(a) == "Ok(123)");
        REQUIRE(a.unwrap() == 123);

        auto b = Result<int, std::string>::Err("abc");
        REQUIRE(format_(b) == "Err(abc)");
        REQUIRE(b.unwrap_err() == "abc");
    }
    SECTION("Move empty") {
        auto a = Result<int, std::string>::Ok(123);
        REQUIRE(bool(a) == true);
        REQUIRE(a.unwrap() == 123);
        REQUIRE(bool(a) == false);

        auto b = std::move(a);
        REQUIRE(bool(b) == false);
    }
    SECTION("Int-bool") {
        auto a = Result<int, bool>::Ok(123);
        REQUIRE(a.unwrap() == 123);
    }
}