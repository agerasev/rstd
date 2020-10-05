#pragma once

#include <csignal>
#include <vector>
#include <array>
#include <thread>
#include <functional>
#include <sstream>
#include <rstd/prelude.hpp>
#include <lazy_static.hpp>
#include <ansi_color.hpp>
#include "test.hpp"


lazy_static_(::rtest::TestRegistrar, __rtest_registrar) {
    return ::rtest::TestRegistrar();
}

namespace rtest {

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

struct TestResult {
    std::string name;
    std::string output;
};

} // namespace rtest

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
        signal(sig, rtest::signal_handler);
    }

#ifndef RTEST_BW
    std::string result_name[2] = {
        ansi_color("ok", AnsiColor::FG_GREEN),
        ansi_color("FAILED", AnsiColor::FG_RED, AnsiColor::BOLD)
    };
#else // RTEST_BW
    std::string result_name[2] = {"ok", "FAILED"};
#endif // RTEST_BW

    println_();
    println_("running {} tests in {} threads", test_count, workers.size());

    rstd::Mutex<std::ostream*> log(&std::cout);
    rstd::Mutex<std::vector<rtest::TestResult>> failures_;

    for (auto &worker : workers) {
        worker = rstd::thread::spawn([&]() {
            for (;;) {
                auto i = mb.lock();
                if (*i == e) {
                    break;
                }
                const auto &test = **i;
                ++*i;
                rstd::drop(i);

                std::stringstream output;
                auto res = rstd::thread::Builder()
                .stdout_(output).stderr_(output)
                .spawn([&]() {
                    test.func();
                }).join();
                
                std::string rn;
                if (res.is_ok() == !test.should_panic) {
                    rn = result_name[0];
                } else {
                    rn = result_name[1];
                    failures_.lock()->push_back(rtest::TestResult {
                        test.name,
                        output.str()
                    });
                }
                res.clear();
                writeln_(**log.lock(), "test {} ... {}", test.name, rn);
            }
        });
    }
    for (auto &worker : workers) {
        worker.join().unwrap();
    }
    println_();

    std::vector<rtest::TestResult> failures = failures_.into_inner();
    if (!failures.empty()) {
        println_("failures:");
        println_();
        for (const auto &fail : failures) {
            println_("---- {} output ----", fail.name);
            println_(fail.output);
            println_();
        }

        println_("failures:");
        for (const auto &fail : failures) {
            println_("    {}", fail.name);
        }
        println_();
    }

    int fails = failures.size();
    println_("test result: {}. {} passed; {} failed;", result_name[fails != 0], test_count - fails, fails);
    println_();

    return int(fails != 0);
}
