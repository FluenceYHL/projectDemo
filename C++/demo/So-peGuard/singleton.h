#ifndef SINGLETON_H
#define SINGLETON_H
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace YHL {

template<typename T>
class singleton final {
private:
    static std::shared_ptr<T> instance;

    singleton() = default;
    singleton(const singleton&) = delete;
    singleton(singleton&&) = delete;

public:
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


#endif // SINGLETON_H
