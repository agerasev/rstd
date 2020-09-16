#pragma once

#include <core/prelude.hpp>
#include <core/lazy_static.hpp>
#include "test.hpp"


lazy_static_(::core::TestRegistrar, __core_test_registrar) {
    return ::core::TestRegistrar();
}

int main(int, const char *[]) {
    println_("Found {} tests", __core_test_registrar->size());
    for (const auto &test : *__core_test_registrar) {
        println_(test.first);
        test.second();
    }
    return 0;
}
