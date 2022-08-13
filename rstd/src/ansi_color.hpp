#pragma once

#include <string>

/*
black        30         40
red          31         41
green        32         42
yellow       33         43
blue         34         44
magenta      35         45
cyan         36         46
white        37         47

reset             0  (everything back to normal)
bold/bright       1  (often a brighter shade of the same colour)
underline         4
inverse           7  (swap foreground and background colours)
bold/bright off  21
underline off    24
inverse off      27
*/

enum struct AnsiColor {
    RESET           = 0,
    BOLD            = 1,
    UNDERLINE       = 4,
    INVERSE         = 7,
    BOLD_OFF        = 21,
    UNDERLINE_OFF   = 24,
    INVERSE_OFF     = 27,

    FG_BLACK        = 30,
    FG_RED          = 31,
    FG_GREEN        = 32,
    FG_YELLOW       = 33,
    FG_BLUE         = 34,
    FG_MAGENTA      = 35,
    FG_CYAN         = 36,
    FG_WHITE        = 37,

    BG_BLACK        = 40,
    BG_RED          = 41,
    BG_GREEN        = 42,
    BG_YELLOW       = 43,
    BG_BLUE         = 44,
    BG_MAGENTA      = 45,
    BG_CYAN         = 46,
    BG_WHITE        = 47
};

template <typename ...Args>
std::string _ansi_color_escape(Args ...colors) {
    static_assert(sizeof...(colors) > 0, "There should be at least one color");
    std::array<AnsiColor, sizeof...(colors)> color_array{colors...};
    std::string out = "\033[";
    for (AnsiColor color : color_array) {
        out += std::to_string(int(color));
        out += ';';
    }
    out.back() = 'm';
    return out;
}


template <typename ...Args>
std::string ansi_color(const std::string &str, Args ...colors) {
    std::string out;
    out += _ansi_color_escape(colors...);
    out += str;
    out += _ansi_color_escape(AnsiColor::RESET);
    return out;
}
