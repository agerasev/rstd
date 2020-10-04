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
    rtest_(print) {
        auto a = Option<int>::Some(123);
        assert_eq_(format_(a), "Some(123)");

        auto b = Option<int>::None();
        assert_eq_(format_(b), "None");
    }
}
