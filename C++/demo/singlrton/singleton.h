#ifndef SINGLETON_H
#define SINGLETON_H
#include <iostream>
#include <memory>
#include <mutex>

/* 使用说明
 * void test::testSingleton () {

    class example{
    public:
        example() { std::cout << "ctor in example\n"; }
        ~example() { std::cout << "dtor in example\n"; }
    };

    for(int i = 0;i < 10; ++i) {
        auto a = singleton<example>::getInstance ();
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
        auto b = singleton<example2>::getInstance (i + 1);
        std::cout << "example2-------address  :  " << b.get () << std::endl;
    }

    for(int i = 0;i < 10; ++i) {
        auto c = singleton<example2>::getInstance (i + 1);
        std::cout << "example2  raw ptr -------address  :  " << c << std::endl;
    }
}

 */

template<typename T>
class singleton final {
private:
    static std::shared_ptr<T> instance;   // 唯一实例

    /* 模板中, 内部类不能访问外部类
    class cleaner {
    private:
        std::mutex mtx;
    public:
        ~cleaner() {

            std::lock_guard<std::mutex> lck(mtx);

            if(instance != nullptr) {
                delete instance;
                instance = nullptr;
                std::cout << "dtor in cleaner\n";
            }
        }
    };

    cleaner cleanHolder; // 利用内部 GC 类的析构函数，销毁外部类的静态成员

    */

    singleton() = default;
    singleton(const singleton&) = delete;
    singleton(singleton&&) = delete;

public:
    // 接收任意参数构造
    template<typename... Args>
    static std::shared_ptr<T> getInstance(Args&&... args) {

        static std::once_flag flag;

        std::call_once(flag, [&]{
            if(instance == nullptr)
                instance = std::make_shared<T>(std::forward<Args>(args)...);
        });
        return instance;
    }

    template<typename... Args>
    static T* getRaw(Args&&... args) {
        return getInstance(args...).get();
    }
} ;

template<typename T>
std::shared_ptr<T> singleton<T>::instance = nullptr;

#endif // SINGLETON_H
