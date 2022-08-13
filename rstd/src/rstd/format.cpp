#include <rtest.hpp>
#include "format.hpp"

using namespace rstd;


rtest_module_(format) {
    rtest_(empty) {
        assert_eq_(format_(), "");
    }
    rtest_(single_string) {
        assert_eq_(format_("abc"), "abc");
    }
    rtest_(single_int) {
        assert_eq_(format_("int"), "int");
    }
    rtest_(format_one) {
        assert_eq_(format_("a: {};", 123), "a: 123;");
    }
    rtest_(format_two) {
        assert_eq_(format_("a: {}, b: {};", 123, "abc"), "a: 123, b: abc;");
    }
    rtest_(format_escape) {
        assert_eq_(format_("{{abc}}"), "{abc}");
    }
    rtest_(format_no_escape) {
        assert_eq_(format_(std::string("{abc}")), "{abc}");
    }
    rtest_should_panic_(wrong_format) {
        format_("}{");
        format_("}");
        format_("{");
        format_("{ {");
        format_("} }");
        format_("{}");
        format_("", 1);
        format_("{}", 1, 2);
    }
}
