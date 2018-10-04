#include "threadpool.h"

YHL::thread_pool::thread_pool(const size_t init_size)
        :stop(false) {
    for(size_t i = 0; i <init_size; ++i)
        this->pool.emplace_back(get_task());
}

// 从任务队列中获取一个任务
std::function<void()> YHL::thread_pool::get_task() {
    auto task = [this] {
        for(;;) {    // 实现线程池的关键 : 每个线程轮询队列是否有未处理的任务
            std::function<void()> cur;
            do{
                std::unique_lock<std::mutex> lck(this->mtx);
                this->cv.wait(lck, [this]{ return this->stop || !this->tasks.empty();});

                if(this->stop or this->tasks.empty())
                    return;

                cur = std::move(this->tasks.front());
                this->tasks.pop();
            } while(0);

            cur();  // 本次任务结束, 继续轮询任务队列，把可以执行的任务放到线程中
        }
    };
    return task;
}

// 拓展线程池的容量
void YHL::thread_pool::add_thread(const size_t extend) {
    for(size_t i = 0;i < extend; ++i)
        this->pool.emplace_back(get_task());
}

YHL::thread_pool::~thread_pool() {
    {
        std::unique_lock<std::mutex> lck(this->mtx);
        stop = true;
    }
    this->cv.notify_all();
    for(auto &it : pool)
        it.join();
    pool.clear();
    pool.shrink_to_fit();
}
