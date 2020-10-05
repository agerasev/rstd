#include <rtest.hpp>

#include "thread.hpp"

using namespace rstd;


rtest_module_(thread) {
    rtest_(run) {
        int x = 321;
        auto jh = thread::spawn([&x]() {
            x = 123;
        });
        auto res = jh.join();
        assert_(res.is_ok());
        res.clear();
        assert_eq_(x, 123);
    }
    rtest_(input_and_output) {
        int x = 321;
        auto jh = thread::spawn([x]() -> int {
            assert_eq_(x, 321);
            return 123;
        });
        assert_eq_(jh.join().unwrap(), 123);
    }
    rtest_(thread_panic) {
        auto jh = thread::spawn([]() {
            panic_("Panic!");
        });
        auto res = jh.join();
        assert_(res.is_err());
        res.clear();
    }
}
