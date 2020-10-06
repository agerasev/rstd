#include <rtest.hpp>
#include <memory>

#include "result.hpp"

using namespace rstd;


rtest_module_(result) {
    rtest_(create) {
        auto a = Result<int, float>::Ok(123);
        assert_(a.is_ok());
        assert_eq_(a.unwrap(), 123);

        auto b = Result<int, float>::Err(3.1415f);
        assert_(b.is_err());
        assert_((b.unwrap_err() - 3.1415f) < 1e-6f);
    }
    rtest_(move) {
        std::unique_ptr<int> ptr = std::make_unique<int>(123);
        auto a = Result<std::unique_ptr<int>, std::string>::Ok(std::move(ptr));
        assert_eq_(*a.get(), 123);
        ptr = a.unwrap();
        assert_eq_(*ptr, 123);
    }
    rtest_(destroy) {
        std::shared_ptr<int> ptr = std::make_shared<int>(123);
        assert_eq_(ptr.use_count(), 1);
        auto a = Result<std::shared_ptr<int>, std::string>::Ok(ptr);
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
    rtest_(print) {
        auto a = Result<int, std::string>::Ok(123);
        assert_eq_(format_(a), "Ok(123)");
        assert_eq_(a.unwrap(), 123);

        auto b = Result<int, std::string>::Err("abc");
        assert_eq_(format_(b), "Err(abc)");
        assert_eq_(b.unwrap_err(), "abc");
    }
    rtest_(move_empty) {
        auto a = Result<int, std::string>::Ok(123);
        assert_eq_(bool(a), true);
        assert_eq_(a.unwrap(), 123);
        assert_eq_(bool(a), false);

        auto b = std::move(a);
        assert_eq_(bool(b), false);
    }
    rtest_(int_bool) {
        auto a = Result<int, bool>::Ok(123);
        assert_eq_(a.unwrap(), 123);
    }
    rtest_(match) {
        auto a = Result<int, float>::Ok(123);
        a.match(
            [](int x) { assert_eq_(x, 123); },
            [](float) { panic_(); }
        );
        assert_(a.is_none());

        auto b = Result<int, float>::Err(3.1415f);
        b.match(
            [](int) { panic_(); },
            [](float x) { assert_((x - 3.1415f) < 1e-6f); }
        );
        assert_(b.is_none());
    }
    rtest_(match_ref) {
        auto a = Result<int, float>::Ok(123);
        a.match_ref(
            [](int &x) {
                assert_eq_(x, 123);
                x = 321;
            },
            [](float) { panic_(); }
        );
        assert_(a.is_ok());
        assert_eq_(a.get(), 321);
        a.clear();

        auto b = Result<int, float>::Err(3.1415f);
        static_cast<const decltype(b) &>(b).match_ref(
            [](int) { panic_(); },
            [](float x) { assert_((x - 3.1415f) < 1e-6f); }
        );
        assert_(b.is_err());
        b.clear();
    }
    rtest_should_panic_(match_empty) {
        auto a = Result<int, float>();
        a.match(
            [](int) {},
            [](float) {}
        );
    }
    rtest_(ok_err) {
        auto a = Result<int, float>::Ok(123);
        assert_eq_(a.ok().unwrap(), 123);
        a = Result<int, float>::Ok(123);
        assert_(a.err().is_none());
    }
    rtest_(map) {
        auto src = Result<double>::Ok(3.1415);
        Result<int> dst = src.map([](double x) { return int(100*x); });
        assert_(dst.is_some());
        assert_eq_(dst.unwrap(), 314);

        auto none = Result<>::Err(Tuple<>());
        none.map([](auto) { return 123; }).unwrap_err();
    }
    rtest_(unwrap_or) {
        const int x = 123;
        assert_eq_(Result<int>::Ok(321).unwrap_or(x), 321);
        assert_eq_(Result<int>::Err(Tuple<>()).unwrap_or(x), 123);
    }
    rtest_(and_) {
        assert_eq_(Result<bool>::Ok(false).and_(Result<int>::Ok(123)).unwrap(), 123);
        Result<bool>::Ok(false).and_(Result<int>::Err(Tuple<>())).unwrap_err();
        Result<bool>::Err(Tuple<>()).and_(Result<int>::Ok(123)).unwrap_err();
        Result<bool>::Err(Tuple<>()).and_(Result<int>::Err(Tuple<>())).unwrap_err();
    }
    rtest_(or_) {
        assert_eq_(Result<int>::Ok(321).or_(Result<int>::Ok(123)).unwrap(), 321);
        assert_eq_(Result<int>::Ok(321).or_(Result<int>::Err(Tuple<>())).unwrap(), 321);
        assert_eq_(Result<int>::Err(Tuple<>()).or_(Result<int>::Ok(123)).unwrap(), 123);
        Result<int>::Err(Tuple<>()).or_(Result<int>::Err(Tuple<>())).unwrap_err();
    }
    rtest_(and_then) {
        assert_eq_(
            Result<double>::Ok(1.23)
            .and_then([](double x) {
                return Result<int>::Ok(int(100*x));
            }).unwrap(),
            123
        );
    }
    rtest_(or_else) {
        assert_eq_(
            Result<int>::Err(Tuple<>())
            .or_else([]() {
                return Result<int>::Ok(123);
            }).unwrap(),
            123
        );
    }
}
