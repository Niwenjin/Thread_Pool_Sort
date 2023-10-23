#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>
#include <queue>
#include <string>

using std::queue;
using std::string;

class ThreadPool {
  public:
    ThreadPool();
    ~ThreadPool();
    void thread_run();
    long *buf;

  private:
    pthread_t *threads;
    queue<void (*)(void *)> task_queue;
    pthread_mutex_t mutex;
    void task_init();
};

void *thread_func(void *arg);
void quicksort(ThreadPool &pool, int no);
void readfile(const string filename, long *buf, size_t size);

#endif