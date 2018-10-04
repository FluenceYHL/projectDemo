#include "test.h"

int test::cnt = 0;
std::mutex test::m;

int test::fun() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "id  :  " << std::this_thread::get_id() << std::endl;
    std::lock_guard<std::mutex> lck(m);
    return ++cnt;
}

void test::testSingleton () {

    class example{
    public:
        example() { std::cout << "ctor in example\n"; }
        ~example() { std::cout << "dtor in example\n"; }
    };

    for(int i = 0;i < 10; ++i) {
        auto a = YHL::singleton<example>::getInstance ();
        std::cout << "address  :  " << a.get () << std::endl;
    }

    class example2{
    public:
        example2(const size_t initSize) {
            std::cout << "ctor in example2\t" << initSize << std::endl;
        }
        ~example2() { std::cout << "dtor in example2\n"; }
    };

    for(int i = 0;i < 10; ++i) {
        auto b = YHL::singleton<example2>::getInstance (i + 1);
        std::cout << "example2-------address  :  " << b.get () << std::endl;
    }

    for(int i = 0;i < 10; ++i) {
        auto c = YHL::singleton<example2>::getRaw (i + 1);
        std::cout << "example2  raw ptr -------address  :  " << c << std::endl;
    }
}

void test::testMultiSingleton () {
    class example{
    public:
        example() { std::cout << "ctor in example\n"; }
        ~example() { std::cout << "dtor in example\n"; }
    };

    for(int i = 0;i < 10; ++i) {
        auto it = YHL::multiSingleton<example>::getInstance("boy");
        std::cout << "boy  :  " << it.get() << std::endl;
    }

    for(int i = 0;i < 10; ++i) {
        auto it = YHL::multiSingleton<example>::getInstance("girl");
        std::cout << "girl  :  " << it.get() << std::endl;
    }

    class example2{
    public:
        example2(const size_t initSize) {
            std::cout << "ctor in example2\t" << initSize << std::endl;
        }
        ~example2() { std::cout << "dtor in example2\n"; }
    };

    for(int i = 0;i < 10; ++i) {
        auto it = YHL::multiSingleton<example2>::getInstance("boy", 1229);
        std::cout << "boy with args... " << it.get () << std::endl;
    }
}
