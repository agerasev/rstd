#include <rtest.hpp>

#include<vector>
#include<memory>
#include<string>

#include "iterator.hpp"

using namespace rstd;


rtest_module_(iterator) {
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
    rtest_(map_other_type) {
        auto iter = Range(0, 10).map([](int x) { return x % 2 == 0; });
        for (int i = 0; i < 10; ++i) {
            assert_eq_(iter.next().unwrap(), i % 2 == 0);
        }
        assert_(iter.next().is_none());
    }
    rtest_(map_string) {
        auto iter = Range(0, 10).map([](int x) { return std::to_string(x); });
        for (int i = 0; i < 10; ++i) {
            assert_eq_(std::stoi(iter.next().unwrap()), i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(map_while) {
        auto iter = Range(0, 20).map_while([](int x) {
            if (x < 10) {
                return Option<int>::Some(x*x);
            } else {
                return Option<int>::None();
            }
        });
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
    rtest_(scan) {
        std::vector<int> data = Range(0, 100)
        .map([](int x) { return (257*x + 123) % 10; })
        .collect<std::vector>();

        auto iter = iter_ref(data).cloned()
        .scan(0, [](int *s, int x) {
            int p = *s;
            if (p < 50) {
                *s += x;
                return Option<int>::Some(p);
            } else {
                return Option<int>::None();
            }
        });
        
        int state = 0;
        for (int i = 0; i < int(data.size()); ++i) {
            assert_eq_(iter.next().unwrap(), state);
            state += data[i];
            if (state >= 50) {
                break;
            }
        }
        assert_(iter.next().is_none());
        assert_(iter.next().is_none());
    }
    rtest_(chain) {
        auto iter = Range(0, 10).chain(Range(10, 20));
        for (int i = 0; i < 20; ++i) {
            assert_eq_(iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(cycle) {
        auto iter = Range(0, 10).cycle();
        for (int i = 0; i < 33; ++i) {
            assert_eq_(iter.next().unwrap(), i % 10);
        }
    }
    rtest_(zip) {
        auto iter = Range(0, 10).zip(
            Range(0, 10).map([](int x) {
                return std::to_string(x);
            })
        );
        for (int i = 0; i < 10; ++i) {
            auto ns = iter.next().unwrap();
            assert_eq_(ns.get<0>(), i);
            assert_eq_(ns.get<1>(), std::to_string(i));
        }
        assert_(iter.next().is_none());
    }
    rtest_(fold) {
        int fac = Range(1, 10).fold(1, [](int x, int y) { return x*y; });
        assert_eq_(fac, 362880);
    }
    rtest_(count) {
        size_t cnt = Range(0, 10).count();
        assert_eq_(cnt, 10);
    }
    rtest_(any_all) {
        assert_(!Range(0, 10).any([](int x) { return x >= 10; }));
        assert_(Range(0, 10).any([](int x) { return x != 5; }));
        assert_(Range(0, 10).all([](int x) { return x < 10; }));
        assert_(!Range(0, 10).all([](int x) { return x != 5; }));
    }
    rtest_(cloned) {
        std::vector<int> data = {0, 1, 2, 3, 4};
        auto iter = iter_ref(data).cloned();
        for (int i = 0; i < int(data.size()); ++i) {
            assert_eq_(iter.next().unwrap(), i);
        }
        assert_(iter.next().is_none());
    }
    rtest_(empty) {
        iter::empty<int>().next().unwrap_none();
    }
    rtest_(once) {
        auto iter = iter::once(123);
        assert_eq_(iter.next().unwrap(), 123);
        iter.next().unwrap_none();
    }
    rtest_(once_with) {
        auto iter = iter::once_with([]() { return 123; });
        assert_eq_(iter.next().unwrap(), 123);
        iter.next().unwrap_none();
    }
    rtest_(repeat) {
        auto iter = iter::repeat(123);
        assert_eq_(iter.next().unwrap(), 123);
        assert_eq_(iter.next().unwrap(), 123);
    }
    rtest_(repeat_with) {
        auto iter = iter::repeat_with([]() { return 123; });
        assert_eq_(iter.next().unwrap(), 123);
        assert_eq_(iter.next().unwrap(), 123);
    }
    rtest_(successors) {
        auto iter = iter::successors(Some(1), [](int x) -> Option<int> {
            if (x < 1000) {
                return Some(10*x);
            } else {
                return None();
            }
        });
        assert_eq_(iter.next().unwrap(), 10);
        assert_eq_(iter.next().unwrap(), 100);
        assert_eq_(iter.next().unwrap(), 1000);
        iter.next().unwrap_none();
    }
}
