#ifndef THREAD_POOL_V2_H
#define THREAD_POOL_V2_H 

#include <functional>
#include <future>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>
#include <memory>
#include <type_traits>
#include <atomic>

#include "SafeQueue.hpp"

class ThreadPool {
    using Job = std::function<void()>;
public:
    explicit ThreadPool(int capacity);
    ~ThreadPool();
    template<typename F, typename... Args> 
    auto push_task(F &&f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;
 
private:  
    std::vector<std::thread> workers_;
    // use pointer to avoid the problem that mutex can't copy and move 
    // and the fact that std::vector require the element can copy or move 
    std::vector<std::unique_ptr<SafeQueue<Job>>> queues_;
    // use round-robin scheduling to dispense tasks
    std::atomic<size_t> next_queue_idx_{0};
};

inline ThreadPool::ThreadPool(int capacity) {
    if(capacity <= 0) {
        throw std::runtime_error("capacity must be a positive");
    }

    for(int i = 0; i < capacity; i ++ ) {
        queues_.emplace_back(std::make_unique<SafeQueue<Job>>());
    }

    workers_.reserve(capacity);
    for(int i = 0; i < capacity; i ++ ) {
        workers_.emplace_back([this](int id){
            for(;;) {
                Job task;
                if(!queues_[id]->pop(task)) {
                    break;
                }
                task();
            }
        }, i);
    }
}

inline ThreadPool::~ThreadPool() {
    for(auto &que : queues_) {
        que->stop();
    }
    for(auto &worker : workers_) {
        if(worker.joinable()) {
            worker.join();
        }
    }
}

template<typename F, typename... Args> 
auto ThreadPool::push_task(F &&f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
    using return_type = std::invoke_result_t<F, Args...>;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future(); 

    // optimization: use relaxed memory order 
    // because it is only necessary to ensure the atomicity of addition
    // and there is no need to synchronize memory visibility across threads
    size_t idx = next_queue_idx_.fetch_add(1, std::memory_order_relaxed) % queues_.size();
    queues_[idx]->push([task]{ (*task)(); });
    return res;
}


#endif 