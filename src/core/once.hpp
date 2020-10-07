#pragma once

#include <pthread.h>


namespace core {

class Once {
private:
    pthread_once_t control = PTHREAD_ONCE_INIT;

public:
    Once() = default;
    ~Once() = default;

    Once(const Once &) = delete;
    Once &operator=(const Once &) = delete;

    inline void call_once(void (*routine)()) {
        pthread_once(&control, routine);
    }
};

}
