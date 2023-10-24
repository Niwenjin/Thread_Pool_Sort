#include "Task.h"

Task::Task(const std::function<int(const std::string &)> &func,
           const std::string &filename)
    : func(func), filename(filename) {}