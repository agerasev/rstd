#include "io.hpp"

#include <pthread.h>


static pthread_key_t stdout_key, stderr_key;
static pthread_once_t io_once = PTHREAD_ONCE_INIT;

static void init_io_once() {
    pthread_key_create(&stdout_key, nullptr);
    pthread_key_create(&stderr_key, nullptr);
}

static std::ostream *get_ostream(pthread_key_t *key) {
    pthread_once(&io_once, init_io_once);
    return (std::ostream*)pthread_getspecific(*key);
}

static void set_ostream(pthread_key_t *key, std::ostream *ostream) {
    pthread_once(&io_once, init_io_once);
    pthread_setspecific(*key, (void*)ostream);
}

std::ostream &stdout_() {
    std::ostream *ostream = get_ostream(&stdout_key);
    if (ostream == nullptr) {
        ostream = &std::cout;
        set_ostream(&stdout_key, ostream);
    }
    return *ostream;
}
std::ostream &stderr_() {
    std::ostream *ostream = get_ostream(&stderr_key);
    if (ostream == nullptr) {
        ostream = &std::cerr;
        set_ostream(&stderr_key, ostream);
    }
    return *ostream;
}

void set_stdout(std::ostream &ostream) {
    set_ostream(&stdout_key, &ostream);
}
void set_stderr(std::ostream &ostream) {
    set_ostream(&stderr_key, &ostream);
}
