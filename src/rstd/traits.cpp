#include <rtest.hpp>

#include "traits.hpp"

using namespace rstd;


rtest_module_(traits) {
    rtest_(any_all) {
        assert_eq_((any_v<false, false, true>), true);
        assert_eq_((any_v<false, false, false>), false);

        assert_eq_((all_v<true, true, true>), true);
        assert_eq_((all_v<true, false, true>), false);
    }
}
