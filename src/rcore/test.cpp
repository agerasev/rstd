#include <iostream>

#include <rcore/fmt/display.hpp>
#include <rcore/fmt/format.hpp>
#include <rcore/fmt/formatter.hpp>
#include <rcore/mem/maybe_uninit.hpp>
#include <rcore/option.hpp>
#include <rcore/result.hpp>
#include <rcore/sync/lazy_static.hpp>
#include <rcore/sync/once.hpp>

#include <array>

constexpr size_t static_strlen(const char *str) {
    size_t i = 0;
    for (; str[i] != '\0'; ++i) {}
    return i;
}

template <size_t N, const char STR[N]>
void print_static_str() {
    static_assert(STR[0] == 'a');
    std::cout << STR << std::endl;
}

int main(int argc, const char *argv[]) {
    std::cout << "It works!" << std::endl;

    std::cout << "Array: ";
    static constexpr char STR[] = "abc";
    static constexpr size_t N = static_strlen(STR);
    print_static_str<N, STR>();
    std::cout << std::endl;

    return 0;
}
