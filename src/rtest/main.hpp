#pragma once

#include <pthread.h>
#include <rstd/prelude.hpp>
#include <lazy_static.hpp>
#include "test.hpp"


lazy_static_(::rstd::TestRegistrar, __rtest_registrar) {
    return ::rstd::TestRegistrar();
}

int main(int, const char *[]) {
    println_("Found {} tests", __rtest_registrar->size());
    for (const auto &test : *__rtest_registrar) {
        println_(test.first);
        test.second();
    }
    return 0;
}
