#ifndef THREAD_POOL_V2_H
#define THREAD_POOL_V2_H 

#include <stdexcept>
#include <type_traits>
#include <future>
#include <utility>
#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <memory>

#include "SafeQueue.hpp"

class ThreadPool {
public: 
    explicit ThreadPool(int capacity);
    ~ThreadPool();
    
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<typename F, typename... Args>
    auto push_task(F &&f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;


private:
    std::vector<std::thread> workers_;
    SafeQueue<std::function<void()>> tasks_;
};

inline ThreadPool::ThreadPool(int capacity) {
    if(capacity <= 0)
        throw std::runtime_error("capacity of thread pool must be positive");

    workers_.reserve(capacity);
    for(int i = 0; i < capacity; i ++ ) {
        workers_.emplace_back([this]{
            for(;;) {
                std::function<void()> task;
                if(!tasks_.pop(task)) // queue has been stopped
                    return ;
                if(task)
                    task(); 
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    tasks_.stop();
    for(auto &worker : workers_) {
        // programm robustness: use joinable to check if can be joined 
        if(worker.joinable()) {
            worker.join();
        }
    }
}


template<typename F, typename... Args>
auto ThreadPool::push_task(F &&f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
    using result_type = std::result_of_t<F(Args...)>;

    auto task = std::make_shared<std::packaged_task<result_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<result_type> result = task->get_future();

    // encapsulation packaged_task to a lambda function and push into tasks_
    tasks_.push([task](){ (*task)(); });

    return result;
}


#endif 