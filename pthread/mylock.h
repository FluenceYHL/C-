#ifndef MYLOCK_H
#define MYLOCK_H
#include <memory>
#include "mutex.h"

class myLock {
private:
    std::unique_ptr<mutex> ptr;
public:
    myLock(mutex *_ptr);
    ~myLock();
};

#endif // MYLOCK_H
