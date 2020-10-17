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
}
