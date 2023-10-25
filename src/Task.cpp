#include "Task.h"

Task_Split::Task_Split(const function<void(const string &, int)> &func,
                       const string &filename)
    : func(func), filename(filename) {}

Task_Merge::Task_Merge(
    const function<void(const string &, const string &, int)> &func,
    const string &file_1, const string &file_2, bool flag)
    : func(func), filename_1(file_1), filename_2(file_2), flag(flag) {}