#include "ThreadPool.h"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string>

#define THREAD_NUM 4
#define BUFFER_SIZE 67108864
#define MIN_SIZE 2097152
const string filepath = "/home/Niwenjin/Projects/Thread_Pool_Sort/files/";

using std::bind;
using std::cerr;
using std::cout;
using std::endl;
using std::find;
using std::ifstream;
using std::ofstream;
using std::to_string;
using std::filesystem::directory_iterator;
using std::filesystem::remove;

ThreadPool::ThreadPool() {
    threads = (pthread_t *)malloc(sizeof(pthread_t) * THREAD_NUM);
    buf = (long *)malloc(BUFFER_SIZE);

    // 初始化互斥量
    mutex_split_queue = PTHREAD_MUTEX_INITIALIZER;
    mutex_merge_queue = PTHREAD_MUTEX_INITIALIZER;
    mutex_tmp_queue = PTHREAD_MUTEX_INITIALIZER;
    mutex_fcnt = PTHREAD_MUTEX_INITIALIZER;

    cond_merge_queue = PTHREAD_COND_INITIALIZER;

    fcnt = -1;
    task_init();
}

ThreadPool::~ThreadPool() {
    // 重命名输出文件
    rename(tmp_queue.front());
    free(threads);
    free(buf);

    pthread_mutex_destroy(&mutex_split_queue);
    pthread_mutex_destroy(&mutex_merge_queue);
    pthread_mutex_destroy(&mutex_tmp_queue);
    pthread_mutex_destroy(&mutex_fcnt);
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
        if (entry.is_regular_file() && entry.path().extension() == ".txt")
            add_task_split(entry.path().filename());

    // 添加合并文件任务
    // while (tmp_queue.size() > 1)
    //     add_task_merge();
}

void *ThreadPool::thread_func(void *arg) {
    ThreadPool *pool = static_cast<ThreadPool *>(arg);

    // 获取线程编号
    pthread_t self = pthread_self();
    auto it = find(pool->threads, pool->threads + THREAD_NUM, self);
    int thread_no = it - pool->threads;

    // 执行分裂任务
    while (true) {
        // 从队列中取出一个任务
        pthread_mutex_lock(&pool->mutex_split_queue);
        if (pool->task_split_queue.empty()) {
            pthread_mutex_unlock(&pool->mutex_split_queue);
            break;
        }
        Task_Split task = pool->task_split_queue.front();
        pool->task_split_queue.pop();
        pthread_mutex_unlock(&pool->mutex_split_queue);

        // 执行任务函数
        task.func(task.filename, thread_no);
    }

    // 执行合并任务
    while (true) {
        // 从队列中取出一个任务
        pthread_mutex_lock(&pool->mutex_merge_queue);

        // 任务队列为空，且活跃进程不为0时，等待任务
        while (pool->task_merge_queue.empty()) {
            // 活跃线程数减1
            pool->active_threads[thread_no] = false;
            if (pool->all_done()) {
                // 发送结束信号
                pthread_cond_signal(&pool->cond_merge_queue);

                cout << "thread:" << thread_no << " exit." << endl;
                pthread_mutex_unlock(&pool->mutex_merge_queue);
                pthread_exit(nullptr);
            }

            // 等待任务信号
            pthread_cond_wait(&pool->cond_merge_queue,
                              &pool->mutex_merge_queue);
        }
        pool->active_threads[thread_no] = true;

        Task_Merge task = pool->task_merge_queue.front();
        pool->task_merge_queue.pop();

        pthread_mutex_unlock(&pool->mutex_merge_queue);

        // 执行任务函数
        task.func(task.filename_1, task.filename_2, thread_no);
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
        pthread_mutex_lock(&mutex_fcnt);
        string tmpname = to_string(++fcnt) + ".tmp";
        pthread_mutex_unlock(&mutex_fcnt);

        ofstream output(filepath + tmpname);
        for (int i = 0; i < size; i++)
            output << buf[i] << " ";
        output.close();

        // 临时文件入队
        add_tmp_file(tmpname);
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

    pthread_mutex_lock(&mutex_fcnt);
    string tmpname = to_string(++fcnt) + ".tmp";
    pthread_mutex_unlock(&mutex_fcnt);

    ofstream output(filepath + tmpname);
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

    add_tmp_file(tmpname);

    input_1.close();
    remove(filepath + file_1);
    input_2.close();
    remove(filepath + file_2);
}

int ThreadPool::getfile(string &file_1, string &file_2) {
    int cnt = 0;
    // for (const auto &entry : directory_iterator(filepath))
    //     if (entry.is_regular_file() && entry.path().extension() ==
    //     ".tmp") {
    //         cnt++;
    //         if (cnt == 1)
    //             file_1 = entry.path().filename();
    //         else
    //             file_2 = entry.path().filename();
    //     }

    // 从临时文件队列中获取文件
    pthread_mutex_lock(&mutex_tmp_queue);
    while (!tmp_queue.empty()) {
        cnt++;
        if (cnt == 1) {
            file_1 = tmp_queue.front();
            tmp_queue.pop();
        }
        if (cnt == 2) {
            file_2 = tmp_queue.front();
            tmp_queue.pop();
            break;
        }
    }
    pthread_mutex_unlock(&mutex_tmp_queue);

    // 返回获取的文件数
    return cnt;
}

void ThreadPool::rename(const string &filename) {
    std::filesystem::rename(filepath + filename, filepath + "output.txt");
}

void ThreadPool::task_split(const string &filename, int thread_no) {
    cout << "split file:" << filename << "----thread " << thread_no << endl;
    split_sort(filename, buf + thread_no * MIN_SIZE, MIN_SIZE);
}

void ThreadPool::task_merge(const string &file_1, const string &file_2,
                            int thread_no) {
    cout << "merge file: " << file_1 << " & " << file_2 << "----thread "
         << thread_no << endl;
    merge(file_1, file_2, buf + thread_no * MIN_SIZE, MIN_SIZE);
}

void ThreadPool::add_task_split(const string &filename) {
    function<void(const string &, int)> func =
        bind(&ThreadPool::task_split, this, std::placeholders::_1,
             std::placeholders::_2);
    Task_Split task(func, filename);
    task_split_queue.push(task);
}

void ThreadPool::add_task_merge() {
    function<void(const string &, const string &, int)> func =
        bind(&ThreadPool::task_merge, this, std::placeholders::_1,
             std::placeholders::_2, std::placeholders::_3);
    string file_1, file_2;
    int n = getfile(file_1, file_2);
    if (n < 2)
        cerr << "no enough files to merge." << endl;

    Task_Merge task(func, file_1, file_2);

    pthread_mutex_lock(&mutex_merge_queue);
    task_merge_queue.push(task);
    pthread_mutex_unlock(&mutex_merge_queue);
    pthread_cond_signal(&cond_merge_queue);
}

void ThreadPool::add_tmp_file(const string &filename) {
    pthread_mutex_lock(&mutex_tmp_queue);
    tmp_queue.push(filename);
    pthread_mutex_unlock(&mutex_tmp_queue);

    // 若临时文件队列长度大于1,添加合并任务
    if (tmp_queue.size() > 1)
        add_task_merge();
}

bool ThreadPool::all_done() {
    for (bool flag : active_threads)
        if (flag)
            return false;
    return true;
}