#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "Task.h"
#include <pthread.h>
#include <queue>
#include <string>

using std::function;
using std::queue;
using std::string;

class ThreadPool {
  public:
    ThreadPool();
    ~ThreadPool();
    void thread_run();

  private:
    pthread_t *threads;
    queue<Task_Split> task_split_queue;
    queue<Task_Merge> task_merge_queue;
    // pthread_mutex_t mutex;
    // pthread_cond_t queue_cond;
    long *buf;
    int fcnt = -1;
    static void *thread_func(void *arg);
    void task_init();
    void add_task_split(const string &);
    void add_task_merge();
    void quicksort(long *buf, size_t size);
    void split_sort(const string &filename, long *buf, size_t size);
    void merge(const string file_1, const string file_2, long *buf,
               size_t size);
    int getfile(string &file_1, string &file_2);
    void rename(const string &);
    void task_split(const string &, int);
    void task_merge(const string &, const string &, int);
};

#endif