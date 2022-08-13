#pragma once

#include <csignal>
#include <functional>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

#include <rcore/mem/lazy_static.hpp>
#include <rstd/io/ansi_color.hpp>

#include "test.hpp"

rtest::StaticRegistrar RTEST_REGISTRAR;

namespace rtest {

namespace _impl {

const std::unordered_map<int, const std::string_view> SIGNALS = {
    std::pair(SIGTERM, "SIGTERM"),
    std::pair(SIGSEGV, "SIGSEGV"),
    std::pair(SIGINT, "SIGINT"),
    std::pair(SIGILL, "SIGILL"),
    std::pair(SIGABRT, "SIGABRT"),
    std::pair(SIGFPE, "SIGFPE"),
};

void signal_handler(int signal) {
    std::string name;
    auto entry = SIGNALS.find(signal);
    if (entry != SIGNALS.end()) {
        name = entry->second;
    } else {
        name = "Unknown signal"; // format_("Unknown signal {}", sig);
    }
    rcore_panic("{} caught", name);
}

} // namespace _impl

struct TestResult {
    std::string name;
    std::string output;
};

int main(int argc, const char *const *argv) {
    std::vector<rtest::TestCase> tests;
    auto guard = RTEST_REGISTRAR->lock();
    for (const TestCase &c : *guard) {
        bool add = argc < 2;
        for (int j = 1; j < argc; ++j) {
            if (c.name.find(argv[j]) != std::string::npos) {
                add = true;
            }
        }
        if (add) {
            tests.push_back(c);
        }
    }

    int test_count = tests.size();
    auto b = tests.cbegin();
    auto e = tests.cend();
    rstd::sync::Mutex<decltype(b)> mb(std::move(b));

    std::vector<rstd::JoinHandle<>> workers(std::min(std::thread::hardware_concurrency(), unsigned(test_count)));

#ifndef RTEST_BW
    std::string result_name[2] = {
        ansi_color("ok", AnsiColor::FG_GREEN),
        ansi_color("FAILED", AnsiColor::FG_RED, AnsiColor::BOLD)};
#else // RTEST_BW
    std::string result_name[2] = {"ok", "FAILED"};
#endif // RTEST_BW

    println_();
    println_("running {} tests in {} threads", test_count, workers.size());

    rstd::Mutex<std::ostream *> log(&std::cout);
    rstd::Mutex<std::vector<TestResult>> failures_;

    for (auto sig : *__main::SIGNALS) {
        signal(sig.first, __main::signal_handler);
    }
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
                               .stdout_(output)
                               .stderr_(output)
                               .spawn([&]() {
                                   test.func();
                               })
                               .join();

                std::string rn;
                if (res.is_ok() == !test.should_panic) {
                    rn = result_name[0];
                } else {
                    rn = result_name[1];
                    failures_.lock()->push_back(rtest::TestResult{test.name, output.str()});
                }
                res.clear();
                writeln_(**log.lock(), "test {} ... {}", test.name, rn);
            }
        });
    }
    for (auto &worker : workers) {
        worker.join().unwrap();
    }
    for (auto sig : *__main::SIGNALS) {
        signal(sig.first, nullptr);
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

} // namespace rtest
