#include <rtest.hpp>

#include "traits.hpp"

using namespace rstd;


rtest_section_(traits) {
    rtest_case_(any_all) {
        assert_eq_((any_v<false, false, true>), true);
        assert_eq_((any_v<false, false, false>), false);

        assert_eq_((all_v<true, true, true>), true);
        assert_eq_((all_v<true, false, true>), false);
    }
}
