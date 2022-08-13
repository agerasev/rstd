#ifdef CORE_TESTS

#include <string>

#include <gtest/gtest.h>

#include "format.hpp"

using namespace core::fmt;

static_assert(IsDisplay<std::string>);
static_assert(IsDisplay<const char *>);

TEST(Format, empty) {
    ASSERT_EQ(core_format(""), "");
}

TEST(Format, text) {
    ASSERT_EQ(core_format("abc"), "abc");
}

TEST(Format, one_arg) {
    ASSERT_EQ(core_format("a{}", 1), "a1");
}

TEST(Format, two_args) {
    ASSERT_EQ(core_format("a{} {}2", 1, "b"), "a1 b2");
}

TEST(Format, escape) {
    ASSERT_EQ(core_format("}}{{"), "}{");
}
/*
TEST(Log, info) {
    core_log_info("test {}", 123);
}
*/

#endif CORE_TESTS
