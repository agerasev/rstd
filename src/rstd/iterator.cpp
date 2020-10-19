#include <rtest.hpp>

#include<vector>
#include<memory>

#include "iterator.hpp"

using namespace rstd;


rtest_module_(iterator) {
    rtest_(iter) {
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

        auto iter = IntoIter<std::vector, std::shared_ptr<int>>(std::move(copy));
        assert_(copy.empty());
        for (const auto &sh : origin) { assert_eq_(sh.use_count(), 2); }

        for (int i = 0; i < int(origin.size()); ++i) {
            assert_eq_(*iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());

        for (const auto &sh : origin) { assert_eq_(sh.use_count(), 1); }
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
    rtest_(copied) {
        std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = Iter<std::vector, int>(data).copied();
        for (int i = 0; i < int(data.size()); ++i) {
            assert_eq_(iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
}
