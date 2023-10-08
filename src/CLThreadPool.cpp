#include "CLThreadPool.h"
#include "CLThread.h"
#include <pthread.h>

CLThreadPool::CLThreadPool() {
    CLThread thread_1(thread_function);
    CLThread thread_2(thread_function);
}

CLThreadPool::~CLThreadPool() {}

void *thread_function(void *){
    pthread_exit(NULL);
}