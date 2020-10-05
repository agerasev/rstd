#include <rtest.hpp>
#include <memory>

#include "tuple.hpp"
#include "option.hpp"

using namespace rstd;


rtest_module_(option) {
    rtest_(create) {
        auto a = Option<int>::Some(123);
        assert_(a.is_some());
        assert_eq_(a.unwrap(), 123);

        auto b = Option<int>::None();
        assert_(b.is_none());
    }
    rtest_(move) {
        std::unique_ptr<int> ptr = std::make_unique<int>(123);
        auto a = Option<std::unique_ptr<int>>::Some(std::move(ptr));
        assert_eq_(*a.get(), 123);
        ptr = a.unwrap();
        assert_eq_(*ptr, 123);
    }
    rtest_(destroy) {
        std::shared_ptr<int> ptr = std::make_shared<int>(123);
        assert_eq_(ptr.use_count(), 1);
        auto a = Option<std::shared_ptr<int>>::Some(ptr);
        assert_eq_(ptr.use_count(), 2);
        assert_eq_(*a.get(), 123);
        assert_eq_(ptr.use_count(), 2);
        {
            std::shared_ptr<int> cptr = a.unwrap();
            assert_eq_(*cptr, 123);
            assert_eq_(ptr.use_count(), 2);
        }
        assert_eq_(ptr.use_count(), 1);
    }
    rtest_(tuple_of_non_copy_option) {
        std::unique_ptr<int> ptr = std::make_unique<int>(123);
        Tuple<Option<std::unique_ptr<int>>, int> a(
            Option<std::unique_ptr<int>>::Some(std::move(ptr)),
            321
        );
        assert_eq_(*a.get<0>().get(), 123);
        assert_eq_(a.get<1>(), 321);
        //ptr = a.get<0>().unwrap();
        //assert_eq_(*ptr, 123);
    }
    rtest_(empty_option_is_none) {
        assert_(Option<>().is_none());

        auto a = Option<int>::Some(123);
        assert_(a.is_some());
        assert_eq_(a.get(), 123);

        drop(a);
        assert_(a.is_none());
    }
    rtest_(match_ref) {
        auto opt = Option<int>(123);
        bool some = opt.match_ref(
            [](int x) {
                assert_eq_(x, 123);
                return true;
            },
            []() { return false; }
        );
        assert_(some);

        opt.take();
        some = opt.match_ref(
            [](int) { return true; },
            []() { return false; }
        );
        assert_(!some);
    }
    rtest_(match) {
        auto opt = Option<int>(321);
        bool some = opt.match(
            [](int x) {
                assert_eq_(x, 321);
                return true;
            },
            []() { return false; }
        );
        assert_(some);

        some = opt.match(
            [](int) { return true; },
            []() { return false; }
        );
        assert_(!some);
    }
    rtest_(map) {
        auto src = Option<double>::Some(3.1415);
        Option<int> dst = src.map([](double x) { return int(100*x); });
        assert_(dst.is_some());
        assert_eq_(dst.get(), 314);

        auto none = Option<>::None();
        assert_(none.map([](auto) { return 123; }).is_none());
    }
    rtest_(unwrap_or) {
        const int x = 123;
        assert_eq_(Option<int>(321).unwrap_or(x), 321);
        assert_eq_(Option<int>().unwrap_or(x), 123);
    }
    rtest_(and_) {
        assert_eq_(Option<bool>::Some(false).and_(Option<int>::Some(123)).get(), 123);
        assert_(Option<bool>::Some(false).and_(Option<int>::None()).is_none());
        assert_(Option<bool>::None().and_(Option<int>::Some(123)).is_none());
        assert_(Option<bool>::None().and_(Option<int>::None()).is_none());
    }
    rtest_(or_) {
        assert_eq_(Option<int>::Some(321).or_(Option<int>::Some(123)).get(), 321);
        assert_eq_(Option<int>::Some(321).or_(Option<int>::None()).get(), 321);
        assert_eq_(Option<int>::None().or_(Option<int>::Some(123)).get(), 123);
        assert_(Option<int>::None().or_(Option<int>::None()).is_none());
    }
    rtest_(and_then) {
        assert_eq_(
            (Option<double>::Some(1.23)
            .and_then([](double x) {
                return Option<int>::Some(int(100*x));
            }).get()),
            123
        );
    }
    rtest_(or_else) {
        assert_eq_(
            (Option<int>::None()
            .or_else([]() {
                return Option<int>::Some(123);
            }).get()),
            123
        );
    }
    rtest_(print) {
        auto a = Option<int>::Some(123);
        assert_eq_(format_(a), "Some(123)");

        auto b = Option<int>::None();
        assert_eq_(format_(b), "None");
    }
}
