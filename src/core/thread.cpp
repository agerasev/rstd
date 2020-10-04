#include "thread.hpp"

#include <pthread.h>


using namespace core;

static pthread_key_t ti_key;
static pthread_once_t ti_once = PTHREAD_ONCE_INIT;

static void ti_free(void *p) {
    if (p != nullptr) {
        delete (Thread *)p;
    }
}

static void ti_init() {
    pthread_key_create(&ti_key, ti_free);
}

Thread &thread::current() {
    pthread_once(&ti_once, ti_init);
    Thread *ti = (Thread *)pthread_getspecific(ti_key);
    if (ti == nullptr) {
        ti = new Thread();
        pthread_setspecific(ti_key, ti);
    }
    return *ti;
}
