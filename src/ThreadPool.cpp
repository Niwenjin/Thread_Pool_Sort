#include "ThreadPool.h"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <pthread.h>
#include <string>

#define THREAD_NUM 4
#define BUFFER_SIZE 67108864
#define MIN_SIZE 1677216
const string filepath = "/home/Niwenjin/Projects/Thread_Pool_Sort/files/";

using std::bind;
using std::find;
using std::ifstream;
using std::ofstream;
using std::to_string;
using std::filesystem::directory_iterator;
using std::filesystem::remove;

ThreadPool::ThreadPool() {
    threads = (pthread_t *)malloc(sizeof(pthread_t) * THREAD_NUM);
    buf = (long *)malloc(BUFFER_SIZE);
    mutex = PTHREAD_MUTEX_INITIALIZER;
    queue_cond = PTHREAD_COND_INITIALIZER;
    fcnt = -1;
    task_init();
}

ThreadPool::~ThreadPool() {
    free(threads);
    free(buf);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&queue_cond);
}

void ThreadPool::thread_run() {
    for (int i = 0; i < THREAD_NUM; ++i)
        pthread_create(threads + i, nullptr, thread_func, this);
    for (int i = 0; i < THREAD_NUM; ++i)
        pthread_join(threads[i], nullptr);
}

void ThreadPool::task_init() {
    // 添加文件分块排序任务
    for (const auto &entry : directory_iterator(filepath))
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            function<int(const std::string &, int)> func =
                bind(&ThreadPool::task_split, this, entry.path().filename(), 0);
            Task task(func, entry.path().filename());
            add_task(task);
        }
    // 添加合并文件任务
    function<int(const string &, int)> func =
        bind(&ThreadPool::task_merge, this, string(), 0);
    Task task(func, string());
    add_task(task);
}

void ThreadPool::add_task(Task task) {
    pthread_mutex_lock(&mutex);
    task_queue.push(task);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&queue_cond);
}

void *ThreadPool::thread_func(void *arg) {
    ThreadPool *pool = static_cast<ThreadPool *>(arg);

    // 获取线程编号
    pthread_t self = pthread_self();
    auto it = find(pool->threads, pool->threads + THREAD_NUM, self);
    int thread_no = it - pool->threads;

    while (pool->task_queue.empty()) {
        pthread_mutex_lock(&pool->mutex);
        pthread_cond_wait(&pool->queue_cond, &pool->mutex);

        // 从队列中取出一个任务
        Task task = pool->task_queue.front();
        pool->task_queue.pop();

        pthread_mutex_unlock(&pool->mutex);

        // 执行任务函数
        int result = task.func(task.filename, thread_no);
        // 任务执行完毕，出队
        if (result == 0)
            pool->task_queue.pop();
    }

    return nullptr;
}

int cmp(const void *a, const void *b) { return (*(long *)a - *(long *)b); }

void ThreadPool::quicksort(long *buf, size_t size) {
    qsort(buf, size, sizeof(long), cmp);
}

void ThreadPool::split_sort(const string &filename, long *buf, size_t size) {
    ifstream input(filepath + filename);
    while (!input.eof()) {
        // 将文件分成大小相等的块
        for (int i = 0; i < size; i++)
            input >> buf[i];
        // 块内排序
        quicksort(buf, size);
        // 写回临时文件
        string tmpname = to_string(++fcnt) + ".tmp";
        ofstream output(filepath + tmpname);
        for (int i = 0; i < size; i++)
            output << buf[i] << " ";
        output.close();
    }
    input.close();
}

void ThreadPool::merge(const string file_1, const string file_2, long *buf,
                       size_t size) {
    ifstream input_1(filepath + file_1);
    ifstream input_2(filepath + file_2);
    int mid = size / 2;
    // 从两个文件中各读取一块数据
    for (int i = 0; i < mid; i++) {
        input_1 >> buf[i];
        input_2 >> buf[i + mid];
    }

    string tmpname = to_string(++fcnt) + ".tmp";
    ofstream output("");
    int i = 0, j = mid;
    while (true) {
        // 逐个比较两个有序文件中的数据
        if (buf[i] < buf[j])
            output << buf[i++] << " ";
        else
            output << buf[j++] << " ";

        if (i != mid && j != size)
            continue;
        // 第一块比较结束
        else if (i == mid) {
            if (input_1.eof()) {
                // 输入文件1读完
                for (; j < size; j++)
                    output << buf[j] << " ";
                // 将剩余文件写入输出文件
                output << input_2.rdbuf();
                break;
            } else {
                // 输入文件1未读完
                for (i = 0; i < mid; i++)
                    input_1 >> buf[i];
                i = 0;
            }
        }
        // 第二块比较结束
        else {
            if (input_2.eof()) {
                // 输入文件2读完
                for (; i < mid; i++)
                    output << buf[i] << " ";
                // 将剩余文件写入输出文件
                output << input_1.rdbuf();
                break;
            } else {
                // 输入文件2未读完
                for (j = mid; j < size; j++)
                    input_2 >> buf[j];
                j = mid;
            }
        }
    }
    output.close();
    input_1.close();
    remove(filepath + file_1);
    input_2.close();
    remove(filepath + file_2);
}

int ThreadPool::getfile(string &file_1, string &file_2) {
    int cnt = 0;
    for (const auto &entry : directory_iterator(filepath))
        if (entry.is_regular_file() && entry.path().extension() == ".tmp") {
            cnt++;
            if (cnt == 1)
                file_1 = entry.path().filename();
            else {
                file_2 = entry.path().filename();
                return 0;
            }
        }
    // 剩余文件不足两个
    return -1;
}

void ThreadPool::rename(const string &filename) {
    std::filesystem::rename(filename, "output.txt");
}

int ThreadPool::task_split(const string &filename, int thread_no) {
    split_sort(filename, buf + thread_no * MIN_SIZE, MIN_SIZE);
    return 0;
}

int ThreadPool::task_merge(const string &, int thread_no) {
    string file_1, file_2;
    if (getfile(file_1, file_2) < 0) {
        // 剩余一个文件，即为所求的输出结果
        rename(file_1);
        return 0;
    } else
        merge(file_1, file_2, buf + thread_no * MIN_SIZE, MIN_SIZE);
    return -1;
}