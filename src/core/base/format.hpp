#pragma once

#include <iostream>
#include <sstream>

#include "macros.hpp"
#include "panic.hpp"


namespace core {

namespace fmt {

template <typename T>
struct Display {
    static void fmt(const T &t, std::ostream &o) {
        o << t;
    }
};
template <typename T>
void display(std::ostream &o, const T &t) {
    fmt::Display<T>::fmt(t, o);
}

} // namespace fmt

class FmtRes {
public:
    int pos;
    enum Code {
        OK = 0,
        BADFMT,
        UNTERM,
        FEWARGS,
        MANYARGS
    } type = OK;

    FmtRes() = delete;
    inline explicit FmtRes(int pos, Code type = OK) : pos(pos), type(type) {}
    inline std::string message() const {
        std::string msg;
        switch (type) {
            case OK:
                msg = "Ok";
                break;
            case BADFMT:
                msg = "Bad format";
                break;
            case UNTERM:
                msg = "Unterminated";
                break;
            case FEWARGS:
                msg = "Too few arguments";
                break;
            case MANYARGS:
                msg = "Too many arguments";
                break;
        }
        return msg + " at pos " + std::to_string(pos);
    }
};

template <int N>
FmtRes _fmt_until_entry(std::ostream &o, const char (&fstr)[N], int i) {
    int state = 0;
    for (; i < N - 1; ++i) {
        char c = fstr[i];
        if (c == '{') {
            if (state == 0) {
                state = 1;
            } else if (state == 1) {
                o << '{';
                state = 0;
            } else {
                return FmtRes{i, FmtRes::BADFMT};
            }
        } else if (c == '}') {
            if (state == 0) {
                state = -1;
            } else if (state == -1) {
                o << '}';
                state = 0;
            } else if (state == 1) {
                state = 0;
                return FmtRes{i + 1};
            } else {
                return FmtRes{i, FmtRes::BADFMT};
            }
        } else {
            if (state != 0) {
                return FmtRes{i, FmtRes::BADFMT};
            } else {
                o << c;
            }
        }
    }
    if (state != 0) {
        return FmtRes{i, FmtRes::UNTERM};
    } else {
        return FmtRes{N};
    }
}
template <int N>
FmtRes _fmt_recurse(std::ostream &o, const char (&fstr)[N], int i) {
    FmtRes res = _fmt_until_entry(o, fstr, i);
    if (res.type == FmtRes::OK && res.pos != N) {
        res.type = FmtRes::FEWARGS;
    }
    return res;
}
template <int N, typename T, typename ...Args>
FmtRes _fmt_recurse(std::ostream &o, const char (&fstr)[N], int i, const T &t, const Args &...args) {
    FmtRes res = _fmt_until_entry(o, fstr, i);
    if (res.type == FmtRes::OK) {
        if (res.pos < N) {
            fmt::display(o, t);
            res = _fmt_recurse(o, fstr, res.pos, args...);
        } else {
            res.type = FmtRes::MANYARGS;
        }
    }
    return res;
}

// FIXME: Use compile-time format-string parsing

inline void write(std::ostream &) {}

template <typename T>
void write(std::ostream &o, const T &t) {
    fmt::display(o, t);
}

template <int N, typename ...Args>
void write(std::ostream &o, const char (&fstr)[N], const Args &...args) {
    FmtRes res = _fmt_recurse(o, fstr, 0, args...);
    if (res.type == FmtRes::OK){
        return;
    }
    panic("Format error: " + res.message());
}

template <typename ...Args>
void writeln(std::ostream &o, const Args &...args) {
    write(o, args...);
    o << std::endl;
}


template <typename ...Args>
std::string format(const Args &...args) {
    std::stringstream ss;
    write(ss, args...);
    return ss.str();
}

template <typename ...Args>
void print(const Args &...args) {
    write(std::cout, args...);
}

template <typename ...Args>
void println(const Args &...args) {
    print(args...);
    std::cout << std::endl;
}

template <typename ...Args>
void eprint(const Args &...args) {
    write(std::cerr, args...);
}

template <typename ...Args>
void eprintln(const Args &...args) {
    eprint(args...);
    std::cerr << std::endl;
}

template <typename ...Args>
void panic(const Args &...args) {
    panic(format(args...));
}

} // namespace core

#define write_(...)     ::core::write    (__VA_ARGS__)
#define writeln_(...)   ::core::writeln  (__VA_ARGS__)
#define format_(...)    ::core::format   (__VA_ARGS__)
#define print_(...)     ::core::print    (__VA_ARGS__)
#define println_(...)   ::core::println  (__VA_ARGS__)
#define eprint_(...)    ::core::eprint   (__VA_ARGS__)
#define eprintln_(...)  ::core::eprintln (__VA_ARGS__)

#define panic_(...)     ::core::panic    (__VA_ARGS__)
