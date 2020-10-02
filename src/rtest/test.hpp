#pragma once

#include <rstd/prelude.hpp>
#include <lazy_static.hpp>

#include <string>
#include <thread>
#include <functional>
#include <unordered_map>

namespace rstd {

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

} // namespace rstd

// FIXME: Don't use `rtest_case_` without `rtest_section_`
#define rtest_section_(name) \
    extern_lazy_static_(::rstd::TestRegistrar, __rtest_registrar); \
    static_block_(__rtest__##name##__namespacer) { \
        __rtest_registrar->set_section(#name); \
    } \
    namespace __rtest_section__##name

#define rtest_case_(name) \
    extern_lazy_static_(::rstd::TestRegistrar, __rtest_registrar); \
    void __rtest_case__##name(); \
    static_block_(__rtest__##name##__registrator) { \
        ::__rtest_registrar->_register(#name, __rtest_case__##name); \
    } \
    void __rtest_case__##name()
