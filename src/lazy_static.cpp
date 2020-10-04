#include <rtest.hpp>
#include <memory>

#include "lazy_static.hpp"

lazy_static_(int, _test_primitive) {
    return 1;
}

lazy_static_(std::vector<int>, _test_compound) {
    return std::vector<int>{0, 1, 2, 3, 4, 5};
}

rtest_section_(lazy_static) {
    rtest_case_(primitive) {
        assert_eq_(*_test_primitive, 1);
    }
    rtest_case_(compound) {
        const std::vector<int> &vec = *_test_compound;
        for (int i = 0; i < int(vec.size()); ++i) {
            assert_eq_(vec[i], i);
        }
    }
}
