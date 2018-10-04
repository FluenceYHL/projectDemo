#ifndef ASPECT_AOP_H
#define ASPECT_AOP_H
#include <string>
#include <functional>
#include <boost/noncopyable.hpp>

namespace AOP {   // 面向切面编程

    class Aspect : boost::noncopyable {
    private:
        std::function<void()> fun;
    public:
        template<typename F>
        Aspect(const F& _fun) : fun(_fun)
            {}

        template<typename T>
        void invoke(T&& object) {

            object.before();

            fun();

            object.after();
        }

        template<typename Head, typename... Tail>
        void invoke(Head&& head, Tail&&... tail) {

            head.before();

            invoke(std::forward<Tail>(tail)...);

            head.after();
        }
    };

    template<typename... AP>
    inline void invoke(const std::function<void()> fun) {
        Aspect(fun).invoke(AP()...);
    }

    template<typename... AP>                  // typename T,
    inline void invoke(const std::function<int()> fun) {  // T()
        Aspect(fun).invoke(AP()...);
    }

    template<typename... AP>                  // typename T,
    inline void invoke(const std::function<std::string()> fun) {  // T()
        Aspect(fun).invoke(AP()...);
    }

    template<typename... AP>
    inline void invoke(const std::function<void(int)> fun, const int arg) {
        auto _fun = std::bind(&fun, arg);
        Aspect(_fun).invoke(AP()...);
    }

    template<typename... AP>
    inline void invoke(const std::function<void(std::string)> fun, const std::string arg) {
        auto _fun = std::bind(&fun, arg);
        Aspect(_fun).invoke(AP()...);
    }

}


#endif // ASPECT_AOP_H
