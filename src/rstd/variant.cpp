#include <rtest.hpp>
#include <memory>

#include "tuple.hpp"
#include "variant.hpp"

using namespace rstd;


rtest_module_(variant) {
    rtest_(base) {
        auto a = std::variant<std::monostate, int, std::string>(123);
        assert_eq_(std::get<1>(a), 123);
        auto b = std::variant<std::monostate, int, std::string>("abc");
        a = std::move(b);
        b = std::variant<std::monostate, int, std::string>();
        assert_eq_(std::get<2>(a), "abc");
    }
    rtest_(base_repeated_types) {
        auto a = std::variant<std::monostate, int, int>(std::in_place_index<1>, 123);
        assert_eq_(std::get<1>(a), 123);
    }
    rtest_(primitive) {
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
    rtest_(move) {
        std::unique_ptr<int> ptr = std::make_unique<int>(123);
        auto a = Variant<Tuple<>, std::unique_ptr<int>>::create<1>(std::move(ptr));
        //auto b = a;
        assert_eq_(*a.get<1>(), 123);
        ptr = a.take<1>();
        assert_eq_(*ptr, 123);
    }
    rtest_(ctor_dtor) {
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
    rtest_(move_assignment) {
        auto a = Variant<int, std::string>::create<0>(123);
        assert_eq_(a.get<0>(), 123);
        a = Variant<int, std::string>::create<1>("abc");
        assert_eq_(a.get<1>(), "abc");
    }
    rtest_(copy_assignment) {
        auto a = Variant<int, std::string>::create<0>(123);
        assert_eq_(a.get<0>(), 123);
        auto b = Variant<int, std::string>::create<1>("abc");
        a = b;
        assert_eq_(a.get<1>(), "abc");
    }
    struct Visitor {
        size_t *p;
        template <size_t P, typename T>
        void operator()(const T &) {
            assert_eq_(*p, 2);
            *p = P;
        }
    };
    rtest_(visit) {
        size_t p = 2;
        auto a = Variant<int, std::string>::create<0>(123);
        a.visit_ref(Visitor{&p});
        assert_eq_(p, 0);

        p = 2;
        a = Variant<int, std::string>::create<1>("abc");
        a.visit_ref(Visitor{&p});
        assert_eq_(p, 1);
    }
    rtest_(format) {
        auto a = Variant<int, std::string>::create<0>(123);
        assert_eq_(format_(a), "Variant<0>(123)");
        a = Variant<int, std::string>::create<1>("abc");
        assert_eq_(format_(a), "Variant<1>(abc)");
    }
    rtest_(bool) {
        auto a = Variant<Tuple<>, int, bool>::create<1>(123);
        assert_eq_(a.get<1>(), 123);

        auto b = Variant<Tuple<>, int, bool>::create<2>(true);
        assert_eq_(b.get<2>(), true);
    }
    rtest_(match) {
        int p = -1;
        auto a = Variant<bool, int, double>::create<1>(123);
        a.match(
            [&](bool) { p = 0; },
            [&](int x) {
                p = 1;
                assert_eq_(x, 123);
            },
            [&](double) { p = 2; }
        );
        assert_eq_(p, 1);
        assert_(a.is_none());
        
        auto b = Variant<std::string, double, int>::create<2>(321);
        assert_eq_(b.match(
            [](std::string &&) { return 0; },
            [](double) { return 1; },
            [](int x) {
                assert_eq_(x, 321);
                return 2;
            }
        ), 2);
        assert_(b.is_none());
    }
    rtest_(match_ref) {
        int p = -1;
        auto a = Variant<bool, int, double>::create<1>(123);
        a.match_ref(
            [&](bool&) { p = 0; },
            [&](int &x) {
                p = 1;
                assert_eq_(x, 123);
                x = 321;
            },
            [&](double&) { p = 2; }
        );
        assert_eq_(p, 1);
        assert_(a.is_some());
        assert_eq_(a.template get<1>(), 321);
    }
    rtest_(pointer) {
        int x = 123;
        auto a = Variant<bool, int*, const double*>::create<1>(&x);
        assert_eq_(*a.get<1>(), x);
    }
    rtest_(get_take_funcs) {
        auto a = Variant<bool, int, double>::create<1>(123);
        assert_eq_(a.size(), 3u);

        assert_eq_(get<1>(a), 123);
        get<1>(a) = -321;
        assert_eq_(take<1>(std::move(a)), -321);

        a.put<2>(3.1415);
        assert_(std::abs(get<2>(a) - 3.1415) < 1e-8);
        get<2>(a) = -2.71;
        assert_eq_(take<2>(std::move(a)), -2.71);
    }
}
