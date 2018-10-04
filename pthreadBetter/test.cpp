#include "test.h"
#include "mymutex.h"

YHL::mutex* test::mtx = new YHL::myMutex();

void* test::fun(void *pParam) {

    std::cout << "fun\n\n";

    char *pmsg = static_cast<char*>(pParam);
    if(!pmsg)
        return reinterpret_cast<void*>(1);

//    myLock lck(mtx);

    for(int i = 0;i < 10; ++i) {
        std::cout << pmsg << std::endl;
        sleep(1);
    }
    return reinterpret_cast<void*>(0);
}


void test::testPthread() {
    std::cout << "testPthread\n\n";
    constexpr int initSize = 5;
    char msg[10][10] = {"first", "second", "third", "fourth", "fifth"};

    pthread_t threads[initSize];
    pthread_attr_t attr[initSize];

    for(int i = 0;i < initSize; ++i) {

        pthread_attr_init (&attr[i]);
        pthread_attr_setdetachstate (&attr[i], PTHREAD_CREATE_JOINABLE);

        if(pthread_create (&threads[i], &attr[i], fun, msg[i]) == -1)
            throw std::runtime_error("thread create failure");
    }

    void *result;
    for(int i = 0;i < initSize; ++i) {

        pthread_join (threads[i], &result);

        pthread_attr_destroy (&attr[i]);
    }
}
