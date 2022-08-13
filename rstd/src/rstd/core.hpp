#pragma once

#include <rcore/prelude.hpp>


namespace rstd {

inline std::istream &stdin_() { return rcore::stdin_(); }
inline std::ostream &stdout_() { return rcore::stdout_(); }
inline std::ostream &stderr_() { return rcore::stderr_(); }

typedef rcore::Once Once;

typedef rcore::Thread Thread;

namespace thread {

inline Thread &current() { return rcore::thread::current(); }

} // namespace thread

} // namespace rstd
