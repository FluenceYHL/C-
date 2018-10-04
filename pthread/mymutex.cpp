#include "mymutex.h"

myMutex::myMutex() {
    pthread_mutex_init (&mtx, nullptr);
}

myMutex::~myMutex() {
    pthread_mutex_destroy (&mtx);
}

void myMutex::lock() {
    pthread_mutex_lock (&mtx);
}

void myMutex::unlock() {
    pthread_mutex_unlock (&mtx);
}
