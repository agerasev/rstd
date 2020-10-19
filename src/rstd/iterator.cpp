#include <rtest.hpp>

#include<vector>
#include<memory>

#include "iterator.hpp"

using namespace rstd;


rtest_module_(iterator) {
    rtest_(iter_const) {
        const std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = iter_ref(data);
        for (int i = 0; i < int(data.size()); ++i) {
            assert_eq_(*iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(iter_mut) {
        std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = iter_ref(data);
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
    rtest_(iter_rev) {
        std::vector<int> data;
        for (int i = 0; i < 10; ++i) {
            data.push_back(i);
        }
        auto iter = iter_ref(data);
        for (int i = 0; i < 5; ++i) {
            assert_eq_(*iter.next().unwrap(), i);
        }
        auto iter_rev = iter.rev();
        assert_(iter.next().is_none());
        for (int i = 0; i < 5; ++i) {
            assert_eq_(*iter_rev.next().unwrap(), 9 - i);
        }
        assert_(iter_rev.next().is_none());
        assert_(iter_rev.rev().next().is_none());
    }
    rtest_(into_iter) {
        std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = into_iter(std::move(data));
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(into_iter_move) {
        std::vector<std::unique_ptr<int>> data;
        for (int i = 0; i < 10; ++i) {
            data.push_back(std::make_unique<int>(i));
        }

        auto iter = into_iter(std::move(data));
        assert_(data.empty());

        for (int i = 0; i < 10; ++i) {
            assert_eq_(*iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(into_iter_ownership) {
        std::vector<std::shared_ptr<int>> origin;
        for (int i = 0; i < 10; ++i) {
            origin.push_back(std::make_shared<int>(i));
        }
        std::vector<std::shared_ptr<int>> copy = origin;
        for (const auto &sh : origin) { assert_eq_(sh.use_count(), 2); }

        auto iter = into_iter(std::move(copy));
        assert_(copy.empty());
        for (const auto &sh : origin) { assert_eq_(sh.use_count(), 2); }

        for (int i = 0; i < int(origin.size()); ++i) {
            assert_eq_(*iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());

        for (const auto &sh : origin) { assert_eq_(sh.use_count(), 1); }
    }
    rtest_(into_iter_rev) {
        std::vector<int> data;
        for (int i = 0; i < 10; ++i) {
            data.push_back(i);
        }
        auto iter = into_iter(std::move(data));
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
    rtest_(find) {
        auto iter = Range(10);
        auto f = [](int x) { return (x % 2) == 0; };
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter.find(f).unwrap(), 2*i);
        }
        iter.find(f).unwrap_none();
    }
    rtest_(find_map) {
        auto iter = Range(10);
        auto f = [](int x) {
            if (x % 2 == 0) {
                return Option<int>::Some(x*x);
            } else {
                return Option<int>::None();
            }
        };
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter.find_map(f).unwrap(), 4*i*i);
        }
        iter.find_map(f).unwrap_none();
    }
    rtest_(collect) {
        std::vector<int> vec = Range(10).collect<std::vector>();
        assert_eq_(vec.size(), 10);
        for (int i = 0; i < int(vec.size()); ++i) {
            assert_eq_(vec[i], i);
        }
    }
    rtest_(map) {
        auto iter = Range(0, 10).map([](int x) { return x*x; });
        for (int i = 0; i < 10; ++i) {
            assert_eq_(iter.next().unwrap(), i*i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(filter) {
        auto iter = Range(0, 10).filter([](int x) { return (x % 2) == 0; });
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter.next().unwrap(), 2*i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(filter_map) {
        auto iter = Range(10).filter_map([](int x) {
            if (x % 2 == 0) {
                return Option<int>::Some(x*x);
            } else {
                return Option<int>::None();
            }
        });
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter.next().unwrap(), 4*i*i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(map_rev) {
        auto iter = Range(0, 10).map([](int x) { return 2*x; });
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter.next().unwrap(), 2*i);
        }
        auto iter_rev = iter.rev();
        assert_(iter.next().is_none());
        for (int i = 0; i < 5; ++i) {
            assert_eq_(iter_rev.next().unwrap(), 18 - 2*i);
        }
        assert_(iter_rev.next().is_none());
        assert_(iter_rev.rev().next().is_none());
    }
    rtest_(cycle) {
        auto iter = Range(0, 10).cycle();
        for (int i = 0; i < 33; ++i) {
            assert_eq_(iter.next().unwrap(), i % 10);
        }
    }
    rtest_(fold) {
        int fac = Range(1, 10).fold(1, [](int x, int y) { return x*y; });
        assert_eq_(fac, 362880);
    }
    rtest_(count) {
        size_t cnt = Range(0, 10).count();
        assert_eq_(cnt, 10);
    }
    rtest_(copied) {
        std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = iter_ref(data).copied();
        for (int i = 0; i < int(data.size()); ++i) {
            assert_eq_(iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
}
