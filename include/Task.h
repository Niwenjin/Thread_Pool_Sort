#ifndef TASK_H
#define TASK_H
#include <functional>
#include <string>

using std::function;
using std::string;

class Task {
  public:
    Task(const function<int(const string &, int)> &, const string &);
    function<int(const string &, int)> func;
    const string filename;
    int thread_no;
};
#endif