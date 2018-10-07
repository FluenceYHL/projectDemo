#ifndef ASPECT_AOP_H
#define ASPECT_AOP_H
#include <string>
#include <functional>
#include <memory>
#include <iostream>
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


namespace AOP_static {

    template<typename T, typename Derived>
    class Aspect {
    private:
        T* ptr;
        Derived& cast() {
            return static_cast<Derived&>(*this);
        }
    public:
        explicit Aspect(T *_ptr) : ptr(_ptr) {}

        void before_wrapper(T *p) {

            // pre

            this->cast().before(p);

            // then
        }

        void after_wrapper(T *p) {

            // pre

            this->cast().after(p);

            // then
        }

        std::shared_ptr<T> operator->() {  // AOP 关键体现在重载 -> 然后

            before_wrapper(ptr);

            return std::shared_ptr<T>(ptr, [this](T *p){ this->after_wrapper(p); });
        }
    };

    template<template <typename> class aspect, typename T>
    inline aspect<T> make_aspect(T *ptr) {
        return aspect<T>(ptr);
    }

    template<typename T>
    class log_aspect : public Aspect<T, log_aspect<T> > {

        using base_type = Aspect<T, log_aspect<T> >;
    public:

        log_aspect(T *ptr) : base_type(ptr){}

        void before(T *ptr) {
            std::cout << "before.....\n\n";
        }

        void after(T *ptr) {
            std::cout << "after......\n\n";
        }
    };

    template<typename T>
    class time_aspect : public Aspect<T, time_aspect<T> > {

        using base_type = Aspect<T, time_aspect<T> >;

    private:
        time_t beg = 0;
    public:

        time_aspect(T *ptr) : base_type(ptr){}

        void before(T *ptr) {
            std::cout << "start.....time  :  " << beg << "\n\n";
        }

        void after(T *ptr) {
            time_t cur = 4;
            std::cout << "end......time  :  " << cur - beg << "\n\n";
        }
    };

} // AOP2



#endif // ASPECT_AOP_H
