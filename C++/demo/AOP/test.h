#ifndef TEST_H
#define TEST_H
#include <iostream>
#include <thread>
#include <mutex>

#include "scopeguard.h"
#include "threadpool.h"
#include "objectpool.h"
#include "logcall.h"
#include "singleton.h"
#include "aspect_aop.h"

namespace test {

    extern int cnt;
    extern std::mutex m;

    int fun();

    void testSingleton();

    void testMultiSingleton();

    void testAOP();
}

#endif // TEST_H
