#include <rtest.hpp>

#include<vector>

#include "iterator.hpp"

using namespace rstd;


rtest_module_(iterator) {
    rtest_(iter) {
        std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = Iter<std::vector, int>(data);
        for (int i = 0; i < int(data.size()); ++i) {
            assert_eq_(*iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(iter_mut) {
        std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = IterMut<std::vector, int>(data);
        for (int i = 0; i < int(data.size()); ++i) {
            int *p = iter.next().unwrap();
            assert_eq_(*p, i);
            *p = -i;
        }
        assert_(iter.next().is_none());
        for (int i = 0; i < int(data.size()); ++i) {
            assert_eq_(data[i], -i);
        }
    }
    rtest_(into_iter) {
        std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = IntoIter<std::vector, int>(data);
        for (int i = 0; i < int(data.size()); ++i) {
            assert_eq_(iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
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
    rtest_(collect) {
        std::vector<int> vec = Range(5).collect<std::vector>();
        assert_eq_(vec.size(), 5);
        for (int i = 0; i < int(vec.size()); ++i) {
            assert_eq_(vec[i], i);
        }
    }
    rtest_(map) {
        auto iter = Range(0, 5).map([](int x) { return x*x; });
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter.next().unwrap(), i*i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(copied) {
        std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = Iter<std::vector, int>(data).copied();
        for (int i = 0; i < int(data.size()); ++i) {
            assert_eq_(iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
}
