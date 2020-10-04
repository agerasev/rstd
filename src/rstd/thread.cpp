#include <rtest.hpp>

#include "thread.hpp"

using namespace rstd;


rtest_section_(thread) {
    rtest_case_(run) {
        int x = 321;
        auto jh = thread::spawn(std::function<void()>([&x]() {
            x = 123;
        }));
        auto res = jh.join();
        assert_(res.is_ok());
        res.clear();
        assert_eq_(x, 123);
    }
    rtest_case_(input_and_output) {
        int x = 321;
        auto jh = thread::spawn(std::function<int()>([x]() -> int {
            assert_eq_(x, 321);
            return 123;
        }));
        assert_eq_(jh.join().unwrap(), 123);
    }
    rtest_case_(thread_panic) {
        auto jh = thread::spawn(std::function<void()>([]() {
            panic_("Panic!");
        }));
        auto res = jh.join();
        assert_(res.is_err());
        res.clear();
    }
}
