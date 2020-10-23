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
    template <typename T>
    struct Adder;
    template <typename ...Args>
    struct Adder<Tuple<Args...>> {
        int *cnt;
        explicit Adder(int *c) : cnt(c) {}
        template <size_t P>
        void operator()(const nth_type<P, Args...> &x) {
            *cnt += int(x);
        }
    };
    rtest_(visit) {
        Tuple<bool, int, double> a(true, 1, 3.1415);
        int cnt = 0;
        a.visit_ref(Adder<decltype(a)>(&cnt));
        assert_eq_(cnt, 5);
    }
    struct Incrementer { 
        template <typename ...Args>
        Tuple<std::remove_reference_t<Args>...> operator()(Args &&...args) {
            return Tuple(((std::remove_reference_t<Args>)(args + 1))...);
        }
    };
    rtest_(unpack) {
        Tuple<bool, int, double> a(false, 1, 2.0);
        auto b = a.unpack(Incrementer());
        assert_eq_(b.get<0>(), true);
        assert_eq_(b.get<1>(), 2);
        assert_eq_(b.get<2>(), 3.0);
    }
    rtest_(unpack_ref) {
        Tuple<bool, int, double> a(false, 1, 2.0);
        auto b = a.unpack_ref(Incrementer());
        assert_eq_(b.get<0>(), true);
        assert_eq_(b.get<1>(), 2);
        assert_eq_(b.get<2>(), 3.0);
    }
}
