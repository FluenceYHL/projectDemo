#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <vector>
#include <queue>
#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include <condition_variable>

/* 使用说明
    YHL::thread_pool pool(4);
    for(int i = 0;i < 10; ++i) {
        auto result = pool.enqueue(test::fun);
        std::cout << "answer  :  " << result.get() << std::endl;
    }

    pool.add_thread(2);
    for(int i = 0;i < 20; ++i) {
        auto result = pool.enqueue(test::fun);
        std::cout << "answer  :  " << result.get() << std::endl;
    }
 */

namespace YHL {

    class thread_pool {
    private:
        // 一个线程池 + 一个任务队列, 线程不断检查是否可以执行任务
        std::vector< std::thread > pool;
        std::queue< std::function<void()> > tasks;
        // sunchronization
        std::mutex mtx;
        std::condition_variable cv;
        bool stop;
    public:
        thread_pool(const size_t);
        ~thread_pool();

        // 从任务队列中获取一个任务
        std::function<void()> get_task();

        // 拓展线程池的容量
        void add_thread(const size_t);

        template<typename F, class... Args>
        auto enqueue(F&& fun, Args&& ...args)
            -> std::future<typename std::result_of<F(Args...)>::type>;
    };

    // 放入新的任务到队列中去
    template<typename F, class... Args>
    auto YHL::thread_pool::enqueue(F&& fun, Args&& ...args)
            -> std::future< typename std::result_of<F(Args...)>::type > {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto packed_task = std::make_shared< std::packaged_task<return_type()> >(
                std::bind(std::forward<F>(fun), std::forward<Args>(args)...)
            );

        {
            std::unique_lock<std::mutex> lck(this->mtx);

            if(stop == true)
                throw std::runtime_error("enqueue task on stopped pool\n");

            this->tasks.emplace([packed_task](){ (*packed_task)(); });
        }

        std::future<return_type> res = packed_task->get_future();
        this->cv.notify_one();
        return res;
    }

}


#endif // THREADPOOL_H
