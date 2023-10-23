#include "ThreadPool.h"
#include <cstdlib>
#include <fstream>
#include <pthread.h>
#include <string>

#define THREAD_NUM 4
#define BUFFER_SIZE 67108864
#define MIN_SIZE 1677216

using std::ifstream;
using std::ofstream;
using std::to_string;

ThreadPool::ThreadPool() {
    threads = (pthread_t *)malloc(sizeof(pthread_t) * THREAD_NUM);
    buf = (long *)malloc(BUFFER_SIZE);
    mutex = PTHREAD_MUTEX_INITIALIZER;
    fcnt = -1;
    task_init();
}

ThreadPool::~ThreadPool() {
    free(threads);
    free(buf);
    pthread_mutex_destroy(&mutex);
}

void ThreadPool::thread_run() {
    for (int i = 0; i < THREAD_NUM; ++i)
        pthread_create(threads + i, nullptr, thread_func, &i);
    for (int i = 0; i < THREAD_NUM; ++i)
        pthread_join(threads[i], nullptr);
}

void *thread_func(void *arg) {
    int *no = static_cast<int *>(arg);

    while (true) {
        // pthread_mutex_lock(&pool->queue_mutex);

        // 检查任务队列是否为空
        // while (pool->task_queue.empty()) {
        //     pthread_cond_wait(&pool->queue_cond, &pool->queue_mutex);
        // }

        // // 从队列中取出一个任务
        // Task task = pool->task_queue.back();
        // pool->task_queue.pop_back();

        // pthread_mutex_unlock(&pool->queue_mutex);

        // // 执行任务函数
        // task.func(task.arg);
    }
}

int cmp(const void *a, const void *b) { return (*(long *)a - *(long *)b); }

void quicksort(long *buf, size_t size) { qsort(buf, size, sizeof(long), cmp); }

void split_sort(const string filename, int cnt, long *buf, size_t size) {
    ifstream input(filename);
    while (!input.eof()) {
        // 将文件分成大小相等的块
        for (int i = 0; i < size; i++)
            input >> buf[i];
        // 块内排序
        quicksort(buf, size);
        // 写回临时文件
        string tmpname = to_string(++cnt) + ".tmp";
        ofstream output(tmpname);
        for (int i = 0; i < size; i++)
            output << buf[i] << " ";
        output.close();
    }
    input.close();
}

void ThreadPool::task_init() {}