#pragma once

#include <csignal>
#include <vector>
#include <array>
#include <thread>
#include <functional>
#include <sstream>
#include <rstd/prelude.hpp>
#include <lazy_static.hpp>
#include "test.hpp"


lazy_static_(::rstd::TestRegistrar, __rtest_registrar) {
    return ::rstd::TestRegistrar();
}

static void signal_handler(int sig) {
    std::string signame;
    switch (sig) {
        case SIGTERM: signame = "SIGTERM"; break;
        case SIGSEGV: signame = "SIGSEGV"; break;
        case SIGINT: signame = "SIGINT"; break;
        case SIGILL: signame = "SIGILL"; break;
        case SIGABRT: signame = "SIGABRT"; break;
        case SIGFPE: signame = "SIGFPE"; break;
        default: signame = "Unknown signal"; break;
    }
    panic_("{} caught", signame);
}

int main(int, const char *[]) {
    int test_count = __rtest_registrar->size();
    auto b = __rtest_registrar->begin();
    auto e = __rtest_registrar->end();
    rstd::Mutex<decltype(b)> mb(std::move(b));

    std::vector<rstd::JoinHandle<>> workers(std::min(
        std::thread::hardware_concurrency(),
        unsigned(test_count)
    ));

    std::array<int, 6> sigcodes{SIGTERM, SIGSEGV, SIGINT, SIGILL, SIGABRT, SIGFPE};
    for (int sig : sigcodes) {
        signal(sig, signal_handler);
    }

    std::string result_name[2] = {"ok", "FAILED"};
    println_("running {} tests in {} threads", test_count, workers.size());

    rstd::Mutex<std::ostream*> log(&std::cout);
    rstd::Mutex<std::unordered_map<std::string, std::string>> failures_;

    for (auto &worker : workers) {
        worker = rstd::thread::spawn(std::function<void()>([&]() {
            for (;;) {
                auto i = mb.lock();
                if (*i == e) {
                    break;
                }
                const auto &test = **i;
                ++*i;
                rstd::drop(i);
                const auto &name = test.first;
                auto &func = test.second.first;
                bool should_panic = test.second.second;

                std::stringstream output;
                auto res = rstd::thread::Builder()
                .stdout_(output).stderr_(output)
                .spawn(std::function<void()>([&]() {
                    func();
                })).join();
                
                std::string rn;
                if (res.is_ok() == !should_panic) {
                    rn = result_name[0];
                } else {
                    rn = result_name[1];
                    failures_.lock()->insert(std::make_pair(name, output.str()));
                }
                res.clear();
                writeln_(**log.lock(), "test {} ... {}", name, rn);
            }
        }));
    }
    for (auto &worker : workers) {
        worker.join().unwrap();
    }
    println_();

    std::unordered_map<std::string, std::string> failures = failures_.into_inner();
    if (!failures.empty()) {
        println_("failures:");
        println_();
        for (const auto &fail : failures) {
            println_("---- {} output ----", fail.first);
            println_(fail.second);
            println_();
        }

        println_("failures:");
        for (const auto &fail : failures) {
            println_("    {}", fail.first);
        }
        println_();
    }

    int fails = failures.size();
    println_("test result: {}. {} passed; {} failed;", result_name[fails != 0], test_count - fails, fails);
    println_();

    return int(fails != 0);
}
