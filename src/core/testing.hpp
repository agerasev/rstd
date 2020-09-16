#pragma once

#include <core/prelude.hpp>
#include <lazy_static.hpp>

#include <string>
#include <thread>
#include <functional>
#include <unordered_map>

namespace core {

class TestRegistrar {
public:
    std::unordered_map<std::string, std::function<void()>> tests;
};

} // namespace core

#define _core_test_extern_registrar \
    extern ::core::TestRegistrar __lazy_static____core_testing_registrar__create(); \
    extern ::LazyStatic<::core::TestRegistrar, __lazy_static____core_testing_registrar__create> __core_testing_registrar;

#define CORE_TEST_(name) \
    _core_test_extern_registrar \
    void __core_testing_case_##name(); \
    __core_testing_registrar->insert(#name, __core_testing_case_##name); \
    void __core_testing_case_##name()
