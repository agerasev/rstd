#include <catch.hpp>
#include <memory>

#include "tuple.hpp"
#include "variant.hpp"

using namespace core;

template <typename T>
void print_mem(const T &t) {
    const unsigned char *mem = reinterpret_cast<const unsigned char*>(&t);
    for (size_t i = 0; i < sizeof(T); ++i) {
        unsigned char c = mem[i];
        static const char *cmap = "0123456789abcdef";
        std::cout << cmap[c / 16] << cmap[c % 16] << " ";
    }
    std::cout << std::endl;
}


TEST_CASE("Variant", "[variant]") {
    SECTION("Primitive") {
        auto a = Variant<bool, int, double>::create<1>(123);
        REQUIRE(a.size() == 3);

        REQUIRE(a.get<1>() == 123);
        a.get<1>() = -321;
        REQUIRE(a.take<1>() == -321);

        a.put<2>(3.1415);
        REQUIRE(a.get<2>() == Approx(3.1415));
        a.get<2>() = -2.71;
        REQUIRE(a.take<2>() == -2.71);
    }
    SECTION("Move") {
        std::unique_ptr<int> ptr = std::make_unique<int>(123);
        auto a = Variant<Tuple<>, std::unique_ptr<int>>::create<1>(std::move(ptr));
        //auto b = a;
        REQUIRE(*a.get<1>() == 123);
        ptr = a.take<1>();
        REQUIRE(*ptr == 123);
    }
    SECTION("Ctor Dtor") {
        std::shared_ptr<int> ptr = std::make_shared<int>(123);
        REQUIRE(ptr.use_count() == 1);
        {
            auto a = Variant<Tuple<>, std::shared_ptr<int>>::create<1>(ptr);
            REQUIRE(ptr.use_count() == 2);
            REQUIRE(*a.get<1>() == 123);
            REQUIRE(ptr.use_count() == 2);
        }
        REQUIRE(ptr.use_count() == 1);
    }
    SECTION("Move assignment") {
        auto a = Variant<int, std::string>::create<0>(123);
        REQUIRE(a.get<0>() == 123);
        a = Variant<int, std::string>::create<1>("abc");
        REQUIRE(a.get<1>() == "abc");
    }
    SECTION("Copy assignment") {
        auto a = Variant<int, std::string>::create<0>(123);
        REQUIRE(a.get<0>() == 123);
        auto b = Variant<int, std::string>::create<1>("abc");
        a = b;
        REQUIRE(a.get<1>() == "abc");
    }
    SECTION("Format") {
        auto a = Variant<int, std::string>::create<0>(123);
        REQUIRE(format_(a) == "Variant<0>(123)");
        a = Variant<int, std::string>::create<1>("abc");
        REQUIRE(format_(a) == "Variant<1>(abc)");
    }
}
