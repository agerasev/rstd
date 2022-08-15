#include <iostream>

#include <rcore/fmt/display.hpp>
#include <rcore/fmt/format.hpp>
#include <rcore/fmt/formatter.hpp>
#include <rcore/mem/maybe_uninit.hpp>
#include <rcore/option.hpp>
#include <rcore/panic.hpp>
#include <rcore/result.hpp>
#include <rcore/sync/lazy_static.hpp>
#include <rcore/sync/once.hpp>

void rcore_panic_handler(const rcore::panic::PanicInfo &) {
    std::cout << "Panic!" << std::endl;
    std::abort();
}

int main(int argc, const char *argv[]) {
    std::cout << "It works!" << std::endl;

    return 0;
}
