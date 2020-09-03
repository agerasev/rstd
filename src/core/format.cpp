#include <catch.hpp>


template <int N>
constexpr int fmt_count_args(const char (&str)[N]) {
    int count = 0;
    bool open = false;
    for (int i = 0; i < N; ++i) {
        if (open) {
            if (str[i] != '}') {
                return -1;
            }
            count += 1;
            open = false;
        } else {
            if (str[i] == '{') {
                open = true;
            }
        }
    }
    if (open) {
        return -2;
    }
    return count;
}

TEST_CASE("Format", "[format]") {
    SECTION("Dot count") {
        static const int N = fmt_count_args("{}{}");
        REQUIRE(N == 2);
    }
}
