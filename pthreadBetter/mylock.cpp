#include "mylock.h"

YHL::myLock::myLock(mutex *_ptr)
        : ptr(_ptr) {
    ptr->lock();
}

YHL::myLock::~myLock() {
    ptr->unlock();
}
