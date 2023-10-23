#include "ThreadPool.h"
#include <cstdlib>
#include <fstream>
#include <pthread.h>

#define THREAD_NUM 4
#define BUFFER_SIZE 67108864
#define MIN_SIZE 1677216

using std::ifstream;

ThreadPool::ThreadPool() {
    threads = (pthread_t *)malloc(sizeof(pthread_t) * THREAD_NUM);
    buf = (long *)malloc(BUFFER_SIZE);
    mutex = PTHREAD_MUTEX_INITIALIZER;
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

void quicksort(ThreadPool &pool, int no) {
    qsort(pool.buf + MIN_SIZE * no, MIN_SIZE, sizeof(long), cmp);
}

void readfile(const string filename, long *buf, size_t size) {
    ifstream file(filename);
    for (int i = 0; i < size; i++)
        file >> buf[i];
    file.close();
}

void ThreadPool::task_init() {}