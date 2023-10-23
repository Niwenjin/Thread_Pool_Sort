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
    int fcnt;

  private:
    pthread_t *threads;
    queue<void (*)(void *)> task_queue;
    pthread_mutex_t mutex;
    void task_init();
};

void *thread_func(void *arg);
void quicksort(long *buf, size_t size);
void split_sort(const string filename, int cnt, long *buf, size_t size);
void merge(const string file_1, const string file_2, long *buf, size_t size);
int getfile(string &file_1, string &file_2);

#endif