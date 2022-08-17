#include "thread.hpp"

#include <pthread.h>

#include "local.hpp"

namespace rstd::thread {

static constinit LocalKey<Thread> CURRENT;

Thread &current() {
    return *CURRENT;
}

} // namespace rstd::thread
