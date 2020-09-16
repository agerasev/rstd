#pragma once

#include <core/prelude.hpp>
#include <core/testing.hpp>

#include <lazy_static.hpp>


lazy_static_(::core::TestRegistrar, __core_testing_registrar) {
    return ::core::TestRegistrar();
}

int main(int, const char *[]) {
    const auto &tests = __core_testing_registrar->tests;
    println_("Found {} tests", tests.size());
    for (const auto &test : tests) {
        println_(test.first);
        test.second();
    }
    return 0;
}
