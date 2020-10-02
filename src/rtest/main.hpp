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
    println_("Found {} tests", __rtest_registrar->size());
    auto b = __rtest_registrar->begin();
    auto e = __rtest_registrar->end();
    rstd::Mutex<decltype(b)> mb(std::move(b));

    std::vector<rstd::JoinHandle<bool>> workers(std::thread::hardware_concurrency());
    println_("Running in {} threads", workers.size());
    for (auto &worker : workers) {
        worker = rstd::thread::spawn(std::function<bool()>([&mb, &e]() -> bool {
            bool passed = true;
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
                    rn = "ok";
                } else {
                    rn = "fail";
                    passed = false;
                }
                res.clear();
                println_("{} ... {}", test.first, rn);
            }
            return passed;
        }));
    }
    bool passed = true;
    for (auto &worker : workers) {
        if (!worker.join().unwrap()) {
            passed = false;
        }
    }
    return int(!passed);
}
