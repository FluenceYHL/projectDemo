#ifndef SINGLETON_H
#define SINGLETON_H
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <boost/any.hpp>

namespace YHL {

    template<typename T>
    class singleton final {
    private:
        static std::shared_ptr<T> instance;

        singleton() = default;
        singleton(const singleton&) = delete;
        singleton(singleton&&) = delete;

    public:
        // 经过检验, 只会调用一次构造函数（即使有多个构造函数）
        // 所以这也是缺陷
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



    template<typename T, typename Key = std::string>
    class multiSingleton final {
    private:

        static std::unordered_map< Key, std::shared_ptr<T> > keyMap;

        // static std::mutex mtx;  why not allowed ?

        multiSingleton() = default;
        multiSingleton(const multiSingleton&) = delete;
        multiSingleton(multiSingleton&&) = delete;

    public:
        template<typename K, typename... Args>
        // 同样的，一个键值，任何对象都只调用一次构造函数
        static std::shared_ptr<T> getInstance(K&& key, Args&&... args) {

            std::shared_ptr<T> instance = nullptr;

            static std::mutex mtx;

            std::lock_guard<std::mutex> lck(mtx);

            auto tar = keyMap.find (std::forward<K>(key));
            if(tar == keyMap.end ()) {

                instance = std::make_shared<T>(std::forward<Args>(args)...);
                keyMap.emplace(key, instance);
            }
            else {
                instance = tar->second;
            }
            return instance;
        }
    };

    template<typename T, typename Key>
    std::unordered_map< Key, std::shared_ptr<T> > multiSingleton<T, Key>::keyMap;

}

/*
 * 想通过标记构造函数的方法, 限制一个类每种构造函数只能有一个实例
 * 也尝试了 boost::any, 但是比较复杂，暂未实现
 */
namespace YHL {

    template <typename T>
    class singletonCtor final {
        // variant
    private:
        static std::unordered_map<const char*, std::shared_ptr<T> > ctor_map;
        // function
    public:
        template<typename... Args>
        static std::shared_ptr<T> getInstance(Args&&... args) {

            static std::mutex mtx;
            std::lock_guard<std::mutex> lck(mtx);

            using ctor = std::function<std::shared_ptr<T>(Args...)>;
            auto ctorName = typeid(ctor).name ();

            std::shared_ptr<T> res = nullptr;
            if(ctor_map.find (ctorName) == ctor_map.end()) {
                // 此处可以考虑自定义删除器
                res = std::make_shared<T>(std::forward<Args>(args)...);
                ctor_map.emplace(ctorName, res);
            }
            else res = ctor_map[ctorName];

            return res;  // ctor_map 中始终存在 std::shared_ptr 的引用, 就不用删除 ctorName
        }
    private:
        singletonCtor() = default;
        singletonCtor(const singletonCtor&) = delete;
        singletonCtor(singletonCtor&&) = delete;
    };

    template <typename T>
    std::unordered_map<const char*, std::shared_ptr<T> > singletonCtor<T>::ctor_map;

}


#endif // SINGLETON_H
