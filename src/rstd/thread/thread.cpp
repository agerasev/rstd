#include "thread.hpp"

#include <pthread.h>

#include "local.hpp"

namespace rstd::thread {

static const LocalKey<Thread> CURRENT;

Thread &current() {
    return *CURRENT;
}

} // namespace rstd::thread
