#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>

using namespace std;

// #include "recursive_mutex_like.hpp"

class recursive_mutex_like {
public: 
    recursive_mutex_like() = default;
    ~recursive_mutex_like() = default;
    recursive_mutex_like(const recursive_mutex_like&) = delete;
    recursive_mutex_like(recursive_mutex_like&&) noexcept = delete;
    recursive_mutex_like& operator=(const recursive_mutex_like&) = delete;
    recursive_mutex_like& operator=(recursive_mutex_like&&) = delete;

public:
    void lock() {
        std::thread::id this_id = std::this_thread::get_id();
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [&]{
            return (cnt_ == 0 || owner_ == this_id);
        });
        if(cnt_ == 0) owner_ = this_id;
        ++ cnt_;
    }

    void unlock() {
        std::lock_guard<std::mutex> lock(mtx_);
        if(owner_ != std::this_thread::get_id())
            throw std::runtime_error("unlock by non-owner thread");
        if( -- cnt_ == 0) {
            owner_ = std::thread::id{};
            cond_.notify_one();
        }
    }

    bool try_lock() {
        std::thread::id this_id = std::this_thread::get_id();
        std::unique_lock<std::mutex> lock(mtx_, std::try_to_lock);
        if(!lock.owns_lock())   return false;
        // 无人持锁
        if(cnt_ == 0) {
            owner_ = this_id;
            cnt_ = 1;
            return true;
        }
        // 自己持锁
        if(owner_ == this_id) {
            ++ cnt_;
            return true;
        }
        // 它人持锁
        return false;
    }

    // Rep: 底层数值类型，如 int64_t
    // Period: 单位，如 std::milli
    template<typename Rep, typename Period> 
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout) {
        return try_lock_until(std::chrono::steady_clock::now() + timeout);
    }

    // Clock: 时钟类型，如 std::chrono::steady_clock
    // Duration: 该时钟类型使用的 Duration 类型
    template<typename Clock, typename Duration> 
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& timeout_time) {
        std::thread::id this_id = std::this_thread::get_id();
        std::unique_lock<std::mutex> lock(mtx_);

        bool ready = cond_.wait_until(lock, timeout_time, [&]{
            return (cnt_ == 0) || (owner_ == this_id);
        });

        if(!ready)  return false;

        if(cnt_ == 0) owner_ = this_id;
        ++ cnt_;
        return true;
    }

private:
    // 可重入锁本质上是通过记录当前锁持有者的 ID 和计数器实现的
    // 这里的锁 mtx_ 是用来保护 cnt_ 和 owner_ 而不是用来实现可重入锁的
    std::mutex mtx_;
    uint32_t cnt_{0};
    std::thread::id owner_{};

    std::condition_variable cond_;
};

recursive_mutex_like rmtx;

void bar() {
    std::lock_guard<recursive_mutex_like> lock(rmtx);
    cout << "bar() begin" << endl;
    cout << "bar() end" << endl;
}

void foo() {
    std::lock_guard<recursive_mutex_like> lock(rmtx);
    cout << "foo() begin" << endl;
    bar();
    cout << "foo() end" << endl;
}

int main() 
{
    foo();
    return 0; 
}
