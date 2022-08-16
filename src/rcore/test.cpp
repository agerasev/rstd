#include <iostream>
#include <type_traits>

#include <rcore/assert.hpp>
#include <rcore/fmt/display.hpp>
#include <rcore/fmt/format.hpp>
#include <rcore/mem/maybe_uninit.hpp>
#include <rcore/option.hpp>
#include <rcore/panic.hpp>
#include <rcore/result.hpp>
#include <rcore/sync/lazy_static.hpp>
#include <rcore/sync/once.hpp>

[[noreturn]] void rcore_panic_handler(const rcore::panic::PanicInfo &) {
    std::cout << "Panic!" << std::endl;
    std::abort();
}

constexpr int static_test() {
    rcore::Option<int> opt = rcore::Some(0);
    return opt.unwrap();
}

int main(int argc, const char *argv[]) {
    constexpr int r = static_test();

    std::cout << "It works!" << std::endl;

    return static_test();
}
