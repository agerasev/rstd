#include <rtest.hpp>

#include "range.hpp"

using namespace rstd;


rtest_module_(iter_range) {
    rtest_(range) {
        auto iter = Range(1, 6);
        for (int i = 1; i < 6; ++i) {
            assert_eq_(iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(range_based_for) {
        auto iter = Range(10);
        int i = 0;
        for (int x : iter) {
            assert_(i < 10);
            assert_eq_(x, i);
            ++i;
        }
        assert_eq_(i, 10);
        assert_(iter.next().is_none());
    }
    rtest_(range_rev) {
        auto iter = Range(10);
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter.next().unwrap(), i);
        }
        auto iter_rev = iter.rev();
        assert_(iter.next().is_none());
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter_rev.next().unwrap(), 9 - i);
        }
        assert_(iter_rev.next().is_none());
        assert_(iter_rev.rev().next().is_none());
    }
}
