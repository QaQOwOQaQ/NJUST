#ifndef THREAD_POOL_V4_H
#define THREAD_POOL_V4_H 

#include <atomic>
#include <cassert>
#include <chrono>
#include <future>
#include <list>
#include <vector>
#include <thread>
#include <unordered_map>
#include <type_traits>
#include <stdexcept>
#include <mutex>
#include <memory>
#include <chrono>
#include <cstdint>

#include "TaskQueue.hpp"

class ThreadPool {
public:  
    ThreadPool(int min_thread= 2, 
               int max_thread = std::thread::hardware_concurrency(), 
               int time_sec = 2);
    ~ThreadPool() { stop(); }
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    
private: 
    void worker_loop();
    void clean_inactive_threads();
    void try_expand_workers();


public: 
    void stop();
    int pending() const { return static_cast<int>(queue_.size()); }
    int active_threads_count() {
        std::lock_guard<std::mutex> lock(thread_mutex_);
        return static_cast<int>(workers_.size());
    }

public: 
    template<typename F, typename... Args>
    void add_task(F&& f, Args&&... args) {
        if(stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("Thread Pool has been stopped.");
        }
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);        
        queue_.push(task);
        try_expand_workers();
    }

    template<typename TaskList>
    void add_batch_task(TaskList&& tasks) {
        if(stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("Thread Pool has been stopped.");
        }
        if(tasks.empty())   return ;
        for(auto &task : tasks) {
            queue_.push(std::move(task));
        }
        try_expand_workers();
    }

    template<typename F, typename... Args>
    void add_priority_task(F&& f, Args&&... args) {
        if(stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("Thread Pool has been stopped.");
        }
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);        
        queue_.push_priority(task);
        try_expand_workers();
    }

    template<typename F, typename... Args>
    auto add_future_task(F&& f, Args&&... args) 
            -> std::future<std::invoke_result_t<F, Args...>> {
        if(stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("Thread Pool has been stopped.");
        }
        using result_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<result_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        auto res = task->get_future();
        queue_.push([task]{(*task)();});
        try_expand_workers();
        return res;
    }

    template<typename F, typename... Args>
    auto add_delay_task(uint64_t delay_ms, F&& f, Args&&... args) {
        if(stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("Thread Pool has been stopped.");
        }
        auto exec_tm = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_ms);
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        queue_.push_delay(std::move(task), exec_tm);
        try_expand_workers();
    }

private: 
    int min_threads_;
    int max_threads_;
    std::chrono::seconds idle_timeout_sec_;

    TaskQueue queue_;

    mutable std::mutex thread_mutex_;
    std::unordered_map<std::thread::id, std::thread> workers_;
    std::list<std::thread> dead_workers_;

    std::atomic<int> idle_threads_{0};
    std::atomic<bool> stop_{false};
};


ThreadPool::ThreadPool(int min_thread, int max_thread, int time_sec)
    : min_threads_(min_thread), 
    max_threads_(max_thread), 
    idle_timeout_sec_(time_sec)
{
    std::lock_guard<std::mutex> lock(thread_mutex_);
    for(int i = 0; i < min_thread; i ++ ) {
        std::thread t(&ThreadPool::worker_loop, this);
        workers_.emplace(t.get_id(), std::move(t));
    }
}

void ThreadPool::stop() {
    bool expected = false;
    if(!stop_.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
        return ;
    }

    queue_.stop();
    std::vector<std::thread> threads_to_join;
    {
        std::lock_guard<std::mutex> lock(thread_mutex_);
        for(auto &t : workers_) {
            threads_to_join.emplace_back(std::move(t.second));
        }
        workers_.clear();
        for(auto &t : dead_workers_) {
            threads_to_join.emplace_back(std::move(t));
        }
        dead_workers_.clear();
    }

    for(auto &t : threads_to_join) {
        if(t.joinable()) t.join();
    }
}

void ThreadPool::worker_loop() {
    idle_threads_.fetch_add(1, std::memory_order_relaxed);
    for(;;) {
        TaskQueue::Task task;
        auto state = queue_.pop(task, idle_timeout_sec_);
        if(state == TaskQueue::PopResult::STOPPED) {
            idle_threads_.fetch_sub(1, std::memory_order_relaxed);
            return ;
        }
        if(state == TaskQueue::PopResult::OK) {
            idle_threads_.fetch_sub(1, std::memory_order_relaxed);
            if(task)    task();
            idle_threads_.fetch_add(1, std::memory_order_relaxed);
            continue;
        }
        // state == TIMEOUT
        if(stop_.load(std::memory_order_relaxed)) {
            idle_threads_.fetch_sub(1, std::memory_order_relaxed);
            return ;
        }
        std::lock_guard<std::mutex> lock(thread_mutex_);
        if(static_cast<int>(workers_.size()) > min_threads_) {
            auto my_id = std::this_thread::get_id();
            auto it = workers_.find(my_id);
            if(it != workers_.end()) {
                idle_threads_.fetch_sub(1, std::memory_order_relaxed);
                dead_workers_.push_front(std::move(it->second));
                workers_.erase(it);
            }
            return ;
        }
    }
}

void ThreadPool::try_expand_workers() {
    clean_inactive_threads();
    if(stop_.load(std::memory_order_relaxed)) return ;

    std::lock_guard<std::mutex> lock(thread_mutex_);
    size_t pending = static_cast<size_t>(queue_.size());
    int idle = idle_threads_.load(std::memory_order_relaxed);
    size_t active = workers_.size();

    const int threshold = 1;
    if(active < static_cast<size_t>(max_threads_) && 
        pending > static_cast<size_t>(idle + threshold)) {
        size_t threads_need = std::min(
            pending - static_cast<size_t>(idle),
            static_cast<size_t>(max_threads_) - active 
        );
        for(size_t i = 0; i < threads_need; i ++ ) {
            std::thread t(&ThreadPool::worker_loop, this);
            workers_.emplace(t.get_id(), std::move(t));
        }
    }
}

void ThreadPool::clean_inactive_threads() {
    std::list<std::thread> local_dead;
    {
        std::unique_lock<std::mutex> lock(thread_mutex_, std::try_to_lock);
        if(!lock.owns_lock() || dead_workers_.empty()) return ;
        local_dead.splice(local_dead.begin(), dead_workers_);
    }
    for(auto &t : local_dead) {
        if(t.joinable()) t.join();
    }
}

#endif 