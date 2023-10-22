#include "ThreadPool.h"
#include <iostream>

int main() {
    ThreadPool *pool = new ThreadPool();
    pool->thread_run();
    delete (pool);
    return 0;
}