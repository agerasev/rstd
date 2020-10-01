#include <catch.hpp>

#include "thread.hpp"

using namespace rstd;


TEST_CASE("Thread", "[thread]") {
    SECTION("Run") {
        int x = 321;
        auto jh = thread::spawn(std::function<void()>([&x]() {
            x = 123;
        }));
        auto res = jh.join();
        REQUIRE(res.is_ok());
        res.clear();
        REQUIRE(x == 123);
    }
    SECTION("Input and output") {
        int x = 321;
        auto jh = thread::spawn(std::function<int()>([x]() -> int {
            assert_eq_(x, 321);
            return 123;
        }));
        REQUIRE(jh.join().unwrap() == 123);
    }
    SECTION("Thread panic") {
        auto jh = thread::spawn(std::function<void()>([]() {
            panic_("Panic!");
        }));
        auto res = jh.join();
        REQUIRE(res.is_err());
        res.clear();
    }
}
