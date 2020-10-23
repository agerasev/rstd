#include <rtest.hpp>
#include <memory>

#include "tuple.hpp"

using namespace rstd;


rtest_module_(tuple) {
    rtest_(getters) {
        Tuple<bool, int, double> a(true, 1, 3.1415);
        assert_eq_(a.size(), 3u);

        assert_eq_(a.get<0>(), true);
        assert_eq_(a.get<1>(), 1);
        assert_(std::abs(a.get<2>() - 3.1415) < 1e-8);
    }
    rtest_(move) {
        Tuple<std::unique_ptr<int>, int> a(std::make_unique<int>(123), 456);
        assert_eq_(a.size(), 2u);
        assert_eq_(*a.get<0>(), 123);
        assert_eq_(a.get<1>(), 456);

        auto b(std::move(a));
        assert_eq_(b.size(), 2u);
        assert_eq_(*b.get<0>(), 123);
        assert_eq_(b.get<1>(), 456);
    }
    rtest_(print) {
        Tuple<bool, int, std::string> a(true, 123, "abc");
        assert_eq_(format_(a), "(1, 123, abc)");
        assert_eq_(format_(Tuple<>()), "()");
    }
    rtest_(get_func) {
        Tuple<bool, int, double> a(true, 1, 3.1415);
        assert_eq_(a.size(), 3u);

        assert_eq_(get<0>(a), true);
        assert_eq_(get<1>(a), 1);
        assert_(std::abs(get<2>(a) - 3.1415) < 1e-8);
    }
}
