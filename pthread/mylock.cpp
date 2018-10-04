#include "mylock.h"

myLock::myLock(mutex *_ptr)
        : ptr(_ptr) {
    ptr->lock();
}
myLock::~myLock() {
    ptr->unlock();
}
