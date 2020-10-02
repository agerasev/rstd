#include <catch.hpp>

#include <chrono>
#include <thread>
#include "thread.hpp"
#include "mutex.hpp"

using namespace rstd;


TEST_CASE("Mutex", "[mutex]") {
    SECTION("Multiple access") {
        Mutex<int> x(0);
        auto t0 = thread::spawn(std::function<void()>([&x]() {
            for (int i = 0; i < 1024; ++i) {
                *x.lock() += 2;
            }
        }));
        auto t1 = thread::spawn(std::function<void()>([&x]() {
            for (int i = 0; i < 1024; ++i) {
                *x.lock() -= 1;
            }
        }));
        t0.join().unwrap();
        t1.join().unwrap();
        REQUIRE(x.into_inner() == 1024);
    }
    SECTION("Ping pong") {
        _Mutex m[3];
        for (int i = 0; i < 3; ++i) {
            m[i] = _Mutex::create();
            m[i].lock();
        }

        volatile int x = 0;
        volatile bool y = false;

        auto t0 = thread::spawn(std::function<void()>([&]() {
            m[0].lock();
            m[2].lock();
            for (int i = 0; i < 300; ++i) {
                assert_eq_(x, 2*i);
                assert_(!y);
                x += 1;
                y = true;
                m[(2*i + 0) % 3].unlock();
                m[(2*i + 1) % 3].lock();
            }
            m[0].unlock();
            m[2].unlock();
        }));

        auto t1 = thread::spawn(std::function<void()>([&]() {
            m[1].lock();
            m[0].lock();
            for (int i = 0; i < 300; ++i) {
                assert_eq_(x, 2*i + 1);
                assert_(y);
                x += 1;
                y = false;
                m[(2*i + 1) % 3].unlock();
                m[(2*i + 2) % 3].lock();
            }
            m[1].unlock();
            m[0].unlock();
        }));

        for (int i = 0; i < 3; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            m[i].unlock();
        }

        t0.join().unwrap();
        t1.join().unwrap();
        REQUIRE(x == 600);
    }
    SECTION("Drop guard") {
        Mutex<int> x(321);
        auto guard = x.lock();
        *guard += 123;
        drop(guard);
        REQUIRE(x.into_inner() == 444);
    }
}
