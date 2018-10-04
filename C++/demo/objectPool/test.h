#ifndef TEST_H
#define TEST_H
#include <iostream>
#include <thread>
#include <mutex>

namespace test {
    extern int cnt;
    extern std::mutex m;
    int fun();
}

#endif // TEST_H
