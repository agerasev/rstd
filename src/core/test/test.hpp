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
    mutable std::string section;
    mutable std::unordered_map<std::string, std::function<void()>> tests;
public:
    void _register(const std::string &name, std::function<void()> func) const {
        tests.insert(std::make_pair(section + "::" + name, func));
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
    void set_section(const std::string &s) const {
        section = s;
    }
};

} // namespace core

// FIXME: Don't use `test_case_` without `test_section_`
#define test_section_(name) \
    extern_lazy_static_(::core::TestRegistrar, __core_test_registrar); \
    static_block_(__core_test__##name##__namespacer) { \
        __core_test_registrar->set_section(#name); \
    } \
    namespace __core_test_section__##name

#define test_case_(name) \
    extern_lazy_static_(::core::TestRegistrar, __core_test_registrar); \
    void __core_test_case__##name(); \
    static_block_(__core_test__##name##__registrator) { \
        ::__core_test_registrar->_register(#name, __core_test_case__##name); \
    } \
    void __core_test_case__##name()
