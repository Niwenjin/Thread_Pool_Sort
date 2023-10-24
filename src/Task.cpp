#include "Task.h"

Task::Task(const function<int(const string &, int)> &func,
           const string &filename)
    : func(func), filename(filename) {}