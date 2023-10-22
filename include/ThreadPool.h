#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>
#include <queue>

using std::queue;

void *thread_func(void *arg);

class ThreadPool {
  public:
    ThreadPool();
    ~ThreadPool();
    void thread_run();

  private:
    pthread_t *threads;
    queue<void (*)(void *)> task_queue;
    long *buf;
    pthread_mutex_t mutex;
    void task_init();
};

#endif