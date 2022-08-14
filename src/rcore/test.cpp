#include <iostream>

#include <rcore/fmt/display.hpp>
#include <rcore/fmt/formatter.hpp>
#include <rcore/mem/lazy_static.hpp>
#include <rcore/mem/maybe_uninit.hpp>
#include <rcore/option.hpp>
#include <rcore/result.hpp>
#include <rcore/sync/once.hpp>

int main(int argc, const char *argv[]) {
    std::cout << "It works!" << std::endl;
    return 0;
}
