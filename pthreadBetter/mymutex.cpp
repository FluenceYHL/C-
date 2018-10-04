#include "mymutex.h"

YHL::myMutex::myMutex() {
    pthread_mutex_init (&mtx, nullptr);
}

YHL::myMutex::~myMutex() {
    pthread_mutex_destroy (&mtx);
}

void YHL::myMutex::lock() {
    pthread_mutex_lock (&mtx);
}

void YHL::myMutex::unlock() {
    pthread_mutex_unlock (&mtx);
}
