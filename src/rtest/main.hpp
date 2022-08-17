#pragma once

#include <csignal>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <rcore/mem/lazy_static.hpp>
#include <rcore/ops.hpp>
#include <rstd/fmt/format.hpp>
#include <rstd/io/ansi_color.hpp>
#include <rstd/sync/mutex.hpp>
#include <rstd/thread/builder.hpp>
#include <rstd/thread/thread.hpp>

#include "test.hpp"

constinit rtest::StaticRegistrar RTEST_REGISTRAR;

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
        name = rstd_format("Unknown signal {}", signal);
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

    size_t test_count = tests.size();
    auto b = tests.cbegin();
    auto e = tests.cend();
    rstd::sync::Mutex<decltype(b)> mb(b);

    std::vector<rstd::thread::JoinHandle<std::monostate>> workers(
        std::min(size_t(std::thread::hardware_concurrency()), test_count));

#ifndef RTEST_NO_COLOR
    std::string result_name[2] = {
        ansi_color("ok", rstd::io::AnsiColor::FG_GREEN),
        ansi_color("FAILED", rstd::io::AnsiColor::FG_RED, rstd::io::AnsiColor::BOLD)};
#else // RTEST_NO_COLOR
    std::string result_name[2] = {"ok", "FAILED"};
#endif // RTEST_NO_COLOR

    rstd_println("");
    rstd_println("running {} tests in {} threads", test_count, workers.size());

    auto log = rstd::sync::Mutex<rstd::fmt::OstreamFormatter>(rstd::fmt::OstreamFormatter(std::cout));
    rstd::sync::Mutex<std::vector<TestResult>> failures_;

    for (const auto sig : _impl::SIGNALS) {
        signal(sig.first, _impl::signal_handler);
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
                rcore::ops::drop(i);

                std::stringstream output;
                auto res =
                    [&]() {
                        auto builder = rstd::thread::Builder();
                        builder.set_stdout(output).set_stderr(output);
                        return builder.spawn([&]() { test.func(); });
                    }()
                        .join();

                std::string rn;
                if (res.is_ok() == !test.should_panic) {
                    rn = result_name[0];
                } else {
                    rn = result_name[1];
                    failures_.lock()->push_back(rtest::TestResult{test.name, output.str()});
                }
                // res.clear();
                rstd_writeln(*log.lock(), "test {} ... {}", test.name, rn);
            }
        });
    }
    for (auto &worker : workers) {
        worker.join().unwrap();
    }
    for (const auto sig : _impl::SIGNALS) {
        signal(sig.first, nullptr);
    }
    rstd_println();

    std::vector<rtest::TestResult> failures = std::move(*failures_.lock());
    if (!failures.empty()) {
        rstd_println("failures:");
        rstd_println("");
        for (const auto &fail : failures) {
            rstd_println("---- {} output ----", fail.name);
            rstd_println("{}", fail.output);
            rstd_println("");
        }

        rstd_println("failures:");
        for (const auto &fail : failures) {
            rstd_println("    {}", fail.name);
        }
        rstd_println("");
    }

    int fails = failures.size();
    rstd_println("test result: {}. {} passed; {} failed;", result_name[fails != 0], test_count - fails, fails);
    rstd_println("");

    return int(fails != 0);
}

} // namespace rtest
