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

    // 任务队列
    queue<Task_Split> task_split_queue;
    queue<Task_Merge> task_merge_queue;
    // 临时文件队列
    queue<string> tmp_queue;

    // 互斥量
    pthread_mutex_t mutex_split_queue;
    pthread_mutex_t mutex_merge_queue;
    pthread_mutex_t mutex_tmp_queue;
    pthread_mutex_t mutex_fcnt;
    // 信号量
    pthread_cond_t cond_merge_queue;

    long *buf;     // 排序缓冲区
    int fcnt = -1; // 临时文件序号

    bool active_threads[4] = {true, true, true, true};

    static void *thread_func(void *arg); // 线程运行函数
    void task_init();                    // 初始化任务队列

    // 封装任务函数
    void task_split(const string &, int);
    void task_merge(const string &, const string &, int);

    void add_task_split(const string &);
    void add_task_merge();
    void add_tmp_file(const string &);

    void quicksort(long *buf, size_t size);
    void split_sort(const string &filename, long *buf,
                    size_t size);                // 在特定缓冲区内排序
    void merge(const string file_1, const string file_2, long *buf,
               size_t size);                     // 在特定缓冲区内合并文件并写回
    int getfile(string &file_1, string &file_2); // 从临时文件队列中取得文件
    void rename(const string &);
    bool all_done();
};

#endif