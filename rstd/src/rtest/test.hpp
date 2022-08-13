#pragma once

#include <rstd/prelude.hpp>
#include <lazy_static.hpp>

#include <string>
#include <thread>
#include <functional>
#include <unordered_map>

namespace rtest {

struct TestCase {
    std::string name;
    std::function<void()> func;
    bool should_panic;
};

class TestRegistrar {
private:
    mutable std::string section;
    mutable std::vector<TestCase> tests;
public:
    void _register(const std::string &name, std::function<void()> func, bool should_panic=false) const {
        tests.push_back(TestCase {
            section + "::" + name,
            func,
            should_panic
        });
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

} // namespace rtest

// FIXME: Don't use `rtest_` without `rtest_module_`
#define rtest_module_(name) \
    extern_lazy_static_(::rtest::TestRegistrar, __rtest_registrar); \
    static_block_(__rtest__##name##__namespacer) { \
        __rtest_registrar->set_section(#name); \
    } \
    namespace __rtest_section__##name

#define rtest_(name) \
    extern_lazy_static_(::rtest::TestRegistrar, __rtest_registrar); \
    void __rtest_case__##name(); \
    static_block_(__rtest__##name##__registrator) { \
        ::__rtest_registrar->_register(#name, __rtest_case__##name); \
    } \
    void __rtest_case__##name()

#define rtest_should_panic_(name) \
    extern_lazy_static_(::rtest::TestRegistrar, __rtest_registrar); \
    void __rtest_case__##name(); \
    static_block_(__rtest__##name##__registrator) { \
        ::__rtest_registrar->_register(#name, __rtest_case__##name, true); \
    } \
    void __rtest_case__##name()
