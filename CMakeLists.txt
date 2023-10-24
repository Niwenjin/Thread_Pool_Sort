cmake_minimum_required(VERSION 3.5)

project("Thread_Pool_Sort")

include_directories(include)

SET(CMAKE_BUILD_TYPE "Debug")

# 设置输出compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_executable(test main.cpp src/ThreadPool.cpp src/Task.cpp)

target_link_libraries(test pthread)