#ifndef SAFE_BLOCKING_QUEUE_H
#define SAFE_BLOCKING_QUEUE_H



#include <queue>
#include <mutex>
#include <condition_variable>

// simple thread safe blocking queue
template<typename T>
class SafeQueue {
public:
    void push(const T &item) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(item);
        }
        cond_.notify_one();
    }
    void push(T &&item) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(std::move(item));
        }
        cond_.notify_one();
    }
    bool pop(T &item) {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [&](){return !queue_.empty() || stop_;});
        if(queue_.empty())
            return false;
        item = std::move(queue_.front());
        queue_.pop();
        return true;
    }
    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.size();
    }
    std::size_t empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stop_ = true;
        }
        cond_.notify_all();
    }

private: 
    std::queue<T> queue_;
    mutable std::mutex mtx_;
    std::condition_variable cond_;
    bool stop_ = false;
};

#endif 