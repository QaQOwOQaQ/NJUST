// 实现一个阻塞互斥锁（自旋锁）
// 基于 TAS 原语
#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include <mutex>

class SpinLock {
public:
    constexpr SpinLock() = default;
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;

    void lock() {
#if __cplusplus >= 202002L
        // TTAS
        while(true) {
            while(flag_.test(std::memory_order_relaxed)) {
                // std::this_thread::yield(); 
            }
            if(!flag_.test_and_set(std::memory_order_acquire)) return ;
        }
#else 
        // TAS
        while(flag_.test_and_set(std::memory_order_acquire)) {
            // std::this_thread::yield(); 
        }
#endif 
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};


int sum = 0;
SpinLock mtx;

void worker(int N = 10000000) {
    int local = 0;
    for(int i = 0; i < N; i ++ ) {
        local ++ ;
    }
    std::lock_guard<SpinLock> lock(mtx);
    sum += local;
}

int main()
{
    const int N = 3;
    std::vector<std::thread> threads;
    threads.reserve(N);
    for(int i = 0; i < N; i ++ ) threads.emplace_back([&]{worker();});
    for(auto &t : threads) t.join();
    std::cout << "sum: " << sum << std::endl;
    return 0;
}