#pragma once

#include <vector>
#include <thread>
#include <functional>
#include <rstd/prelude.hpp>
#include <lazy_static.hpp>
#include "test.hpp"


lazy_static_(::rstd::TestRegistrar, __rtest_registrar) {
    return ::rstd::TestRegistrar();
}

int main(int, const char *[]) {
    int test_count = __rtest_registrar->size();
    auto b = __rtest_registrar->begin();
    auto e = __rtest_registrar->end();
    rstd::Mutex<decltype(b)> mb(std::move(b));

    std::vector<rstd::JoinHandle<int>> workers(std::min(
        std::thread::hardware_concurrency(),
        unsigned(test_count)
    ));

    std::string result_name[2] = {"ok", "FAILED"};
    println_("running {} tests in {} threads", test_count, workers.size());

    for (auto &worker : workers) {
        worker = rstd::thread::spawn(std::function<int()>([&]() -> int {
            int fails = 0;
            for (;;) {
                auto i = mb.lock();
                if (*i == e) {
                    break;
                }
                const auto &test = **i;
                ++*i;
                rstd::drop(i);

                auto res = rstd::thread::spawn(std::function<void()>([&test]() {
                    test.second();
                })).join();
                
                std::string rn;
                if (res.is_ok()) {
                    rn = result_name[0];
                } else {
                    rn = result_name[1];
                    fails += 1;
                }
                res.clear();
                println_("test {} ... {}", test.first, rn);
            }
            return fails;
        }));
    }
    
    int fails = 0;
    for (auto &worker : workers) {
        fails += worker.join().unwrap();
    }

    println_();
    println_("test result: {}. {} passed; {} failed;", result_name[fails != 0], test_count - fails, fails);
    println_();

    return int(fails != 0);
}
