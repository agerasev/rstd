#include <rtest.hpp>

#include <vector>
#include <memory>

#include "container.hpp"

using namespace rstd;


rtest_module_(iter_container) {
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
}
