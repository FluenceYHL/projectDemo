#include "test.h"
#include <fstream>
#include <string>
#include <list>

int test::cnt = 0;
std::mutex test::m;

int test::fun() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "id  :  " << std::this_thread::get_id() << std::endl;
    std::lock_guard<std::mutex> lck(m);
    return ++cnt;
}

void logCall(const std::string& message) {
    std::cout << message << "\n";
}

void test::testAny () {
    YHL::any one(std::string("YHL"));
    std::cout << YHL::any_cast<std::string>(one) << std::endl;

    one = 3;
    std::cout << YHL::any_cast<int>(one) << std::endl;

    struct example{
        void print() {
            logCall("Success !\n");
        }
    }a ;

    YHL::any_cast<example>(a).print ();

    std::list<YHL::any> bukket;
    bukket.emplace_back("YHL");
    bukket.emplace_back(15);
    bukket.emplace_back(a);
    for(auto &it : bukket) {
        if(it.type () == typeid(example))
            YHL::any_cast<example>(it).print ();
    }
    // 使用还是不方便, 可以考虑隐式转换 ？ operator()
}

void test::testScopeGuard () {
    /*
    template <typename funType, typename... Args>
    inline scopeGuard0<std::function<void()> > makeGuard(funType&& onExitScope, Args&&... args) {
        return scopeGuard0<std::function<void()> >::makeGuard([&]{
            onExitScope(std::forward<Args>(args)...);
        });
    }
    */

    std::string message = "YHL, may you a good life\n";
    YHL::scopeGuard<std::function<void()> > guard =
        YHL::makeGuard<std::function<void(std::string)>, const std::string&>
            (logCall, message);

/*
    std::ofstream out("RAII.txt");
    YHL::scopeGuard0< std::function<void()> > guard
            = YHL::makeGuard<std::function<void()> >([&]{
        std::cout << "文件已关闭\n\n";
        out.close ();
    });
    // guard.Dismiss (true);
    out << "YHL, good afternoon\n";
*/
}

void test::testAOP_static () {
    struct example {
        void test() {
            std::cout << "---------------\n\n";
        }
    } ;

    std::shared_ptr<example> ptr = std::make_shared<example>();

    AOP_static::make_aspect<AOP_static::log_aspect>(ptr.get())->test();

    AOP_static::make_aspect<AOP_static::time_aspect>(ptr.get())->test();
}

void test::testAOP () {

    struct A {
        void before() {
            std::cout << "before  in  A\n\n";
        }
        void after() {
            std::cout << "after  in  A\n\n";
        }
    };

    struct B {
        void before() {
            std::cout << "before  in  B\n\n";
        }
        void after() {
            std::cout << "after  in  B\n\n";
        }
    };

    struct C {
        void before() {
            std::cout << "before  in  C\n\n";
        }
        void after() {
            std::cout << "after  in  C\n\n";
        }
    };

    struct D {
        bool flag = false;
        void turnFlag() {
            flag = !flag;
            std::cout << std::boolalpha;
            std::cout << "flag  :  " << flag << "\n\n";
        }

        void print(const std::string& message) {
            std::cout << message << std::endl;
        }
    };


    std::cout << "-------------- 1 -------------\n\n";
    AOP::invoke<A, B>([]{ std::cout << "-------\n\n"; });  // 织入一般函数

    std::cout << "-------------- 2 -------------\n\n";
    D d;
    AOP::invoke<A>([&](){ d.turnFlag(); });                // 织入对象

    std::cout << "-------------- 3 -------------\n\n";

    std::function<void()> fun_obj = std::bind(&D::turnFlag, d);

    AOP::invoke<A>([&fun_obj] { fun_obj(); });             // 织入绑定对象的函数

    std::cout << "-------------- 4 -------------\n\n";

    std::function<void(std::string)> fun_obj2 = std::bind(&D::print, d, std::placeholders::_1);

    std::string message = "AOP 面向切面编程\n";             // 织入绑定带参数的对象函数

    AOP::invoke<A, B, C>([&message, &fun_obj2](){ fun_obj2(message); });

    std::cout << "-------------- 5 -------------\n\n";

    AOP::invoke<B, C>([&d, &message]{d.print(message); });// 织入带参数的对象和函数

    std::cout << "-------------- 6 -------------\n\n";    // 把参数放到参数列表

    AOP::invoke<B, C>([](int arg = 10){ std::cout << "arg = " << arg << "\n\n"; });
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
        example2(){
            std::cout << "默认 ctor in example2\n";
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

    for(int i = 0;i < 10; ++i) {
        auto d = YHL::singleton<example2>::getInstance();
        std::cout << "多构造函数............   " << d.get () << std::endl;
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

void test::testSingletonCtor () {
    class example2{
    public:
        example2() {
            std::cout << "ctor in default ctor\n";
        }
        example2(const size_t initSize) {
            std::cout << "ctor in example2\t" << initSize << std::endl;
        }
        example2(const std::string& message, const int NO) {
            std::cout << "message  :  " << message << std::endl;
            std::cout << "   NO    :  " << NO << std::endl;
        }
        ~example2() { std::cout << "dtor in example2\n"; }
    };

    for(int i = 0;i < 10; ++i) {
        auto it = YHL::singletonCtor<example2>::getInstance ();
        std::cout << it.get () << std::endl;
    }

    for(int i = 0;i < 10; ++i) {
        auto it = YHL::singletonCtor<example2>::getInstance (5);
        std::cout << it.get () << std::endl;
    }

    for(int i = 0;i < 10; ++i) {
        auto it = YHL::singletonCtor<example2>::getInstance ("YHL", 1022);
        std::cout << it.get () << std::endl;
    }
}
