#include <rtest.hpp>
#include <memory>

#include "tuple.hpp"
#include "variant.hpp"

using namespace rstd;

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

rtest_section_(variant) {
    rtest_case_(primitive) {
        auto a = Variant<bool, int, double>::create<1>(123);
        assert_eq_(a.size(), 3u);

        assert_eq_(a.get<1>(), 123);
        a.get<1>() = -321;
        assert_eq_(a.take<1>(), -321);

        a.put<2>(3.1415);
        assert_(std::abs(a.get<2>() - 3.1415) < 1e-8);
        a.get<2>() = -2.71;
        assert_eq_(a.take<2>(), -2.71);
    }
    rtest_case_(move) {
        std::unique_ptr<int> ptr = std::make_unique<int>(123);
        auto a = Variant<Tuple<>, std::unique_ptr<int>>::create<1>(std::move(ptr));
        //auto b = a;
        assert_eq_(*a.get<1>(), 123);
        ptr = a.take<1>();
        assert_eq_(*ptr, 123);
    }
    rtest_case_(ctor_dtor) {
        std::shared_ptr<int> ptr = std::make_shared<int>(123);
        assert_eq_(ptr.use_count(), 1);
        {
            auto a = Variant<Tuple<>, std::shared_ptr<int>>::create<1>(ptr);
            assert_eq_(ptr.use_count(), 2);
            assert_eq_(*a.get<1>(), 123);
            assert_eq_(ptr.use_count(), 2);
        }
        assert_eq_(ptr.use_count(), 1);
    }
    rtest_case_(move_assignment) {
        auto a = Variant<int, std::string>::create<0>(123);
        assert_eq_(a.get<0>(), 123);
        a = Variant<int, std::string>::create<1>("abc");
        assert_eq_(a.get<1>(), "abc");
    }
    rtest_case_(copy_assignment) {
        auto a = Variant<int, std::string>::create<0>(123);
        assert_eq_(a.get<0>(), 123);
        auto b = Variant<int, std::string>::create<1>("abc");
        a = b;
        assert_eq_(a.get<1>(), "abc");
    }
    rtest_case_(format) {
        auto a = Variant<int, std::string>::create<0>(123);
        assert_eq_(format_(a), "Variant<0>(123)");
        a = Variant<int, std::string>::create<1>("abc");
        assert_eq_(format_(a), "Variant<1>(abc)");
    }
    rtest_case_(bool) {
        auto a = Variant<Tuple<>, int, bool>::create<1>(123);
        assert_eq_(a.get<1>(), 123);

        auto b = Variant<Tuple<>, int, bool>::create<2>(true);
        assert_eq_(b.get<2>(), true);
    }
}
