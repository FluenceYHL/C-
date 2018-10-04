#ifndef MUTEX_H
#define MUTEX_H

class mutex {
public:
    virtual ~mutex() = default;
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

#endif // LOCK_H
