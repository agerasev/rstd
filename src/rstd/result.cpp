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
        assert_((b.unwrap_err() - 3.1415f) < 1e-6);
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
}
