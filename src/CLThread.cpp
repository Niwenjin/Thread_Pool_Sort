#include "CLThread.h"

#include <cstdlib>
#include <fstream>
#include <malloc.h>
#include <pthread.h>
#include <string>
#include <unistd.h>

CLThread::CLThread(void *(*func)(void *)) {
    pthread_create(thread, NULL, func, NULL);
    buf = (long *)malloc(MAX_SIZE * sizeof(long));
}

CLThread::~CLThread() { free(buf); }

void CLThread::readfile(std::ifstream in) {
    for (int i = 0; i < MAX_SIZE; ++i)
        in >> buf[i];
}

void CLThread::inside_sort() {
    qsort(buf, MAX_SIZE, sizeof(long), compare_long);
}

int compare_long(const void *p1, const void *p2) {
    return *(long *)p1 - *(long *)p2;
}

void CLThread::write_to_small_file(int cnt) {
    std::string filename = std::to_string(cnt) + ".tmp";
    std::ofstream of(filename);
    for (int i = 0; i < MAX_SIZE; ++i)
        of << buf[i] << " ";
    of.close();
}