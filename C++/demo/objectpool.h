#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H
#include <vector>
#include <memory>
#include <functional>
#include <mutex>

/*
 * 使用说明
   YHL::objecePool<int> pool;
    {
        for(int i = 0;i < 10; ++i) {
            pool.emplace (new int(7));
            auto example = pool.get();
            std::cout << example.get () << std::endl;
        }
    }
    pool.emplace (new int(8));
    auto example = pool.get ();
    std::cout << example.get() << std::endl;

    std::cout << "size = " << pool.size () << std::endl;

    auto example2 = pool.get ();
 */

/*
 * 注意事项
 * 1. 思想：（1）初始创建具有一定大小的对象池
 *         （2）从对象池获取一个对象
 *         （3）将对象返回给对象池
 * 2. 使用 std::unique<> 更方便使用自定义 deleter
 * 3. 何时自定义删除器 ?
 *         （1）不能在初始化的时候，因为回收之后无法再定义删除器
 *         （2）对象池释放内存时，不能释放带有自定义删除器的 unique_ptr
 *    所以要点如下：
 *         （1）对象池中的 unique_ptr 不能带有自定义 deleter
 *         （2）从对象池获取时定义 deleter，回收无 deleter 的 raw_ptr 部分
 * 4. 改进：可以采用 “原型模式” 简化构造
 */

namespace YHL {

    template<typename T>
    class objecePool final {
    private:
        std::vector< std::unique_ptr<T> > pool;
        std::mutex mtx;
    public:
        using deleterType = std::function<void(T*)>;

        objecePool() = default;

        objecePool(const size_t initSize) {
            std::lock_guard<std::mutex> lck(mtx);
            for(int i = 0;i < initSize; ++i)
                this->emplace (new T);
        }

        ~objecePool() {
            pool.clear ();
            pool.shrink_to_fit ();
        }

        void emplace(T* one) {
            pool.emplace_back(std::unique_ptr<T>(one));
        }

        void emplace(std::unique_ptr<T> one) {
            std::lock_guard<std::mutex> lck(mtx);
            pool.emplace_back(one);
        }

        size_t size() const {
            return pool.size ();
        }

        bool empty() const {
            return pool.empty ();
        }

        std::unique_ptr<T, deleterType> get() {

            std::lock_guard<std::mutex> lck(mtx);

            if(pool.empty ())
                throw std::logic_error("对象池已空\n");

            std::unique_ptr<T, deleterType> ptr(
                pool.back().release(),
                [&](T *raw){
                    this->pool.emplace_back(std::unique_ptr<T>(raw));
                }
            );
            pool.pop_back ();
            return std::move(ptr);
        }
    } ;

}

#endif // OBJECTPOOL_H
