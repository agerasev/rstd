#pragma once

#include <rstd/prelude.hpp>
#include <lazy_static.hpp>
#include "test.hpp"


lazy_static_(::rstd::TestRegistrar, __core_test_registrar) {
    return ::rstd::TestRegistrar();
}

int main(int, const char *[]) {
    println_("Found {} tests", __core_test_registrar->size());
    for (const auto &test : *__core_test_registrar) {
        println_(test.first);
        test.second();
    }
    return 0;
}
