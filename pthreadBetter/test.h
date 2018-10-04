#ifndef TEST_H
#define TEST_H
#include "mylock.h"
#include "mymutex.h"

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

namespace test {

extern YHL::mutex* mtx;

void *fun(void *pParam);

void testPthread();

}


#endif // TEST_H
