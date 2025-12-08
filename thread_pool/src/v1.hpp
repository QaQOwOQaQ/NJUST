#ifndef THREAD_POOL_V1_H
#define THREAD_POOL_V1_H 

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

class ThreadPool {
public: 
    explicit ThreadPool(int capacity);
    ~ThreadPool();
    
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<typename F, typename... Args>
    auto push_task(F &&f, Args&&... args) -> std::future<std::result_of_t<F(Args...)>>;


private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mtx_;
    std::condition_variable cond_;
    bool stop_ = false;
};

inline ThreadPool::ThreadPool(int capacity) {
    if(capacity <= 0)
        throw std::runtime_error("capacity of thread pool must be positive");

    workers_.reserve(capacity);
    for(int i = 0; i < capacity; i ++ ) {
        workers_.emplace_back([this]{
            for(;;) {
                std::unique_lock<std::mutex> lock(queue_mtx_);
                cond_.wait(lock, [this]{return stop_ || !tasks_.empty();});
                if(stop_ && tasks_.empty()) {
                    break;
                }
                auto task = std::move(tasks_.front()); // performance optimization: move 
                tasks_.pop();
                lock.unlock();
                task(); 
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queue_mtx_); 
        stop_ = true;
    }
    cond_.notify_all();
    for(auto &worker : workers_) {
        // programm robustness: use joinable to check if can be joined 
        if(worker.joinable()) {
            worker.join();
        }
    }
}


template<typename F, typename... Args>
auto ThreadPool::push_task(F &&f, Args&&... args) -> std::future<std::result_of_t<F(Args...)>> {
    using result_type = std::result_of_t<F(Args...)>;

    auto task = std::make_shared<std::packaged_task<result_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<result_type> result = task->get_future();

    {
        std::lock_guard<std::mutex> lock(queue_mtx_);
        if(stop_) 
            throw std::runtime_error("push task on stopped thread pool");
        // encapsulation packaged_task to a lambda function and push into tasks_
        tasks_.emplace([task](){ (*task)(); });
    }
    cond_.notify_one();
    return result;
}


#endif 