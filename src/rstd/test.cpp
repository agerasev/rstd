#include <iostream>

#include <rstd/fmt/format.hpp>
#include <rstd/io/ansi_color.hpp>
#include <rstd/io/error.hpp>
#include <rstd/io/stream.hpp>
#include <rstd/panic.hpp>
#include <rstd/sync/mutex.hpp>
#include <rstd/thread/builder.hpp>
#include <rstd/thread/handle.hpp>
#include <rstd/thread/local.hpp>
#include <rstd/thread/thread.hpp>

int main(int argc, const char *argv[]) {
    std::cout << "It works!" << std::endl;

    rstd_println("println!");
    rstd_println("a: {{{}}}, b: {}, c: {}", "aa", 2, 3.1415);
    rstd_println("{}, {}", rstd_format_args("{}, {}", 1, 2), 3);

    int a = 1;
    const int b = 2;
    rstd_println("v: {}, cv: {}, t: {}", a, b, 3);

    // rstd::thread::spawn([]() { rstd_panic("Panic in thread"); }).join().unwrap();

    return 0;
}
