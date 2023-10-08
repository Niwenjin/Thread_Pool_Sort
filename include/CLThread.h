#ifndef CLTHREAD_H
#define CLTHREAD_H

#include <fstream>
#include <pthread.h>
#include <stdio.h>

#define MAX_SIZE 8388608

class CLThread {
  public:
    CLThread(void *(*)(void *));
    ~CLThread();
    void readfile(std::ifstream);
    
  private:
    pthread_t *thread;
    long *buf;
    void inside_sort();
    void write_to_small_file(int);
};

int compare_long(const void *, const void *);

#endif