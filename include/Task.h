#ifndef TASK_H
#define TASK_H
#include <functional>
#include <string>

using std::function;
using std::string;

class Task {
  public:
    Task(const std::function<int(const std::string &)> &, const string &);
    function<int(const string &)> func;
    const string filename;
};
#endif