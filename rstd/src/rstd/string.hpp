#pragma once

#include <string>


namespace rstd {

inline std::string to_upper(std::string &&s) {
    std::string str(std::move(s));
    for (char &c : str) { c = std::toupper(c); }
    return str;
}
inline std::string to_upper(const std::string &str) {
    return to_upper(std::string(str));
}

inline std::string to_lower(std::string &&s) {
    std::string str(std::move(s));
    for (char &c : str) { c = std::tolower(c); }
    return str;
}
inline std::string to_lower(const std::string &str) {
    return to_lower(std::string(str));
}

} // namespace rstd
