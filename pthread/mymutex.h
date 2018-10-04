#ifndef MYMUTEX_H
#define MYMUTEX_H
#include "pthread.h"
#include "mutex.h"

class myMutex : public mutex {
private:
    pthread_mutex_t mtx;
public:
    myMutex();

    virtual ~myMutex();

    virtual void lock() override;

    virtual void unlock() override;
};

#endif // MYMUTEX_H
