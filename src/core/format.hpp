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

inline void _write(
    const char *, int,
    std::ostream &
) {}

template <typename T>
void _write(
    const char *, int ,
    std::ostream &o, const T &t
) {
    fmt::display(o, t);
}

template <int N, typename ...Args>
void _write(
    const char *_file_, int _line_,
    std::ostream &o, const char (&fstr)[N], const Args &...args
) {
    FmtRes res = _fmt_recurse(o, fstr, 0, args...);
    if (res.type == FmtRes::OK){
        return;
    }
    _panic(_file_, _line_, "Format error: " + res.message());
}

template <typename ...Args>
void _writeln(
    const char *_file_, int _line_,
    std::ostream &o, const Args &...args
) {
    _write(_file_, _line_, o, args...);
    o << std::endl;
}


template <typename ...Args>
std::string _format(
    const char *_file_, int _line_,
    const Args &...args
) {
    std::stringstream ss;
    _write(_file_, _line_, ss, args...);
    return ss.str();
}

template <typename ...Args>
void _print(
    const char *_file_, int _line_,
    const Args &...args
) {
    _write(_file_, _line_, std::cout, args...);
}

template <typename ...Args>
void _println(
    const char *_file_, int _line_,
    const Args &...args
) {
    _print(_file_, _line_, args...);
    std::cout << std::endl;
}

template <typename ...Args>
void _eprint(
    const char *_file_, int _line_,
    const Args &...args
) {
    _write(_file_, _line_, std::cerr, args...);
}

template <typename ...Args>
void _eprintln(
    const char *_file_, int _line_,
    const Args &...args
) {
    _eprint(_file_, _line_, args...);
    std::cerr << std::endl;
}

template <typename ...Args>
void _panic(
    const char *_file_, int _line_,
    const Args &...args
) {
    _panic(_file_, _line_, _format(_file_, _line_, args...));
}

} // namespace core

#define write_(...)     ::core::_write    (__FILE__, __LINE__  CORE_VA_ARGS_ZERO(__VA_ARGS__))
#define writeln_(...)   ::core::_writeln  (__FILE__, __LINE__  CORE_VA_ARGS_ZERO(__VA_ARGS__))
#define format_(...)    ::core::_format   (__FILE__, __LINE__  CORE_VA_ARGS_ZERO(__VA_ARGS__))
#define print_(...)     ::core::_print    (__FILE__, __LINE__  CORE_VA_ARGS_ZERO(__VA_ARGS__))
#define println_(...)   ::core::_println  (__FILE__, __LINE__  CORE_VA_ARGS_ZERO(__VA_ARGS__))
#define eprint_(...)    ::core::_eprint   (__FILE__, __LINE__  CORE_VA_ARGS_ZERO(__VA_ARGS__))
#define eprintln_(...)  ::core::_eprintln (__FILE__, __LINE__  CORE_VA_ARGS_ZERO(__VA_ARGS__))

#define panic_(...)     ::core::_panic    (__FILE__, __LINE__  CORE_VA_ARGS_ZERO(__VA_ARGS__))
