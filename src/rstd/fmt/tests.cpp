#include "format.hpp"

#include <rstd/assert.hpp>
#include <rtest/test.hpp>

rtest_module(rtest) {
    rtest_case(empty) {
        rstd_assert_eq(rstd_format(""), "");
    }
    rtest_case(single_string) {
        rstd_assert_eq(rstd_format("abc"), "abc");
    }
    rtest_case(format_one) {
        rstd_assert_eq(rstd_format("a: {};", 123), "a: 123;");
    }
    rtest_case(format_two) {
        rstd_assert_eq(rstd_format("a: {}, b: {};", 123, "abc"), "a: 123, b: abc;");
    }
    rtest_case(format_escape) {
        rstd_assert_eq(rstd_format("{{abc}}"), "{abc}");
    }
    /*
    rtest_case_should_panic(wrong_format) {
        rstd_format("}{");
        rstd_format("}");
        rstd_format("{");
        rstd_format("{ {");
        rstd_format("} }");
        rstd_format("{}");
        rstd_format("", 1);
        rstd_format("{}", 1, 2);
    }
    */
}
