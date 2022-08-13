#pragma once

#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

#include <rcore/mem/lazy_static.hpp>
#include <rstd/sync/mutex.hpp>

namespace rtest {

struct TestCase {
    std::string name;
    std::function<void()> func;
    bool should_panic;
};

class Registrar {
private:
    mutable std::string section;
    mutable std::vector<TestCase> tests;

public:
    void register_(const std::string &name, std::function<void()> &&func, bool should_panic = false) const {
        tests.push_back(TestCase{section + "::" + name, std::move(func), should_panic});
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

using StaticRegistrar = const ::rcore::mem::LazyStatic<::rstd::sync::Mutex<::rtest::Registrar>>;

} // namespace rtest

// FIXME: Don't use `rtest_case` without `rtest_module`
#define rtest_module(name) \
    extern ::rtest::StaticRegistrar RTEST_REGISTRAR; \
    rcore_static_block(__rtest__##name##__namespacer) { \
        RTEST_REGISTRAR->set_section(#name); \
    } \
    namespace __rtest_section__##name

#define rtest_case(name) \
    extern ::rtest::StaticRegistrar RTEST_REGISTRAR; \
    void _rtest_case__##name(); \
    rcore_static_block(__rtest__##name##__registrator) { \
        ::RTEST_REGISTRAR->register_(#name, _rtest_case__##name); \
    } \
    void _rtest_case__##name()

#define rtest_case_should_panic(name) \
    extern ::rtest::StaticRegistrar RTEST_REGISTRAR; \
    void _rtest_case__##name(); \
    rcore_static_block(__rtest__##name##__registrator) { \
        ::RTEST_REGISTRAR->register_(#name, _rtest_case__##name, true); \
    } \
    void _rtest_case__##name()
