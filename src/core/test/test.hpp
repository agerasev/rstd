#pragma once

#include <core/prelude.hpp>
#include <core/lazy_static.hpp>

#include <string>
#include <thread>
#include <functional>
#include <unordered_map>

namespace core {

class TestRegistrar {
private:
    mutable std::unordered_map<std::string, std::function<void()>> tests;
public:
    void _register(const std::string &name, std::function<void()> func) const {
        tests.insert(std::make_pair(name, func));
    }
    auto begin() const {
        return tests.cbegin();
    }
    auto end() const {
        return tests.cend();
    }
    size_t size() const {
        return tests.size();
    }
};

} // namespace core

#define test_(name) \
    extern_lazy_static_(::core::TestRegistrar, __core_test_registrar); \
    void __core_test_case_##name(); \
    struct __core_test_##name##_Registrator { \
        __core_test_##name##_Registrator() { \
            __core_test_registrar->_register(#name, __core_test_case_##name); \
        } \
    } __core_test_##name##_registrator; \
    void __core_test_case_##name()
