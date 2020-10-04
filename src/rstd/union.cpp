#include <rtest.hpp>
#include <memory>

#include "tuple.hpp"
#include "union.hpp"
#include "traits.hpp"

using namespace rstd;


template <size_t P>
struct TestDispatch {
    static void call(bool mask[]) {
        mask[P] = true;
    }
};

rtest_module_(union) {
    rtest_(primitive) {
        Union<bool, int, double> a;
        a.init<1>(123);
        assert_eq_(a.size(), 3u);

        assert_eq_(a.get<1>(), 123);
        a.get<1>() = -321;
        assert_eq_(a.take<1>(), -321);
    }
    rtest_(move) {
        std::unique_ptr<int> ptr = std::make_unique<int>(123);
        Union<Tuple<>, std::unique_ptr<int>> a;
        a.init<1>(std::move(ptr));
        assert_eq_(*a.get<1>(), 123);
        ptr = a.take<1>();
        assert_eq_(*ptr, 123);
    }
    rtest_(ctor_dtor) {
        std::shared_ptr<int> ptr = std::make_shared<int>(123);
        assert_eq_(ptr.use_count(), 1);
        Union<Tuple<>, std::shared_ptr<int>> a;
        a.init<1>(rstd::clone(ptr));
        assert_eq_(ptr.use_count(), 2);
        assert_eq_(*a.get<1>(), 123);
        assert_eq_(ptr.use_count(), 2);
        {
            std::shared_ptr<int> cptr = a.take<1>();
            assert_eq_(*cptr, 123);
            assert_eq_(ptr.use_count(), 2);
        }
        assert_eq_(ptr.use_count(), 1);
    }
    rtest_(dispatch) {
        bool mask[3] = {false, false, false};
        Union<bool, int, double> a;
        a.init<1>(123);
        rstd::Dispatcher<TestDispatch, a.size()>::dispatch(1, mask);
        assert_eq_(mask[0], false);
        assert_eq_(mask[1], true);
        assert_eq_(mask[2], false);
        a.destroy<1>();
    }
};
