#include "ThreadPool.h"
#include <chrono>
#include <iostream>

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    ThreadPool *pool = new ThreadPool();
    pool->thread_run();
    delete (pool);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "Execution time: " << duration.count() << " seconds."
              << std::endl;

    return 0;
}