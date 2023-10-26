#ifndef TASK_H
#define TASK_H
#include <functional>
#include <string>

using std::function;
using std::string;

class Task_Split {
  public:
    Task_Split(const function<void(const string &, int)> &, const string &);
    function<void(const string &, int)> func;
    const string filename;
    int thread_no;
};

class Task_Merge {
  public:
    Task_Merge(const function<void(const string &, const string &, int)> &,
               const string &, const string &);
    function<void(const string &, const string &, int)> func;
    const string filename_1;
    const string filename_2;
    int thread_no;
};
#endif