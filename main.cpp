#include "ThreadPool.h"
#include <chrono>
#include <iostream>

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    // 创建线程池对象并运行
    ThreadPool *pool = new ThreadPool();
    pool->thread_run();
    delete (pool);

    // 输出运行时间
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "Execution time: " << duration.count() << " seconds."
              << std::endl;

    return 0;
}