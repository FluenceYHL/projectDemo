#include "test.h"

int test::cnt = 0;
std::mutex test::m;

int test::fun() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "id  :  " << std::this_thread::get_id() << std::endl;
    std::lock_guard<std::mutex> lck(m);
    return ++cnt;
}
