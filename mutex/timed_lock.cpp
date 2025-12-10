// 标准库 std::timed_mutex 
// lock() 阻塞到成功；
// try_lock() 立即返回；
// try_lock_for/try_lock_until 带超时

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>

class TimedLock {
public:
    TimedLock() = default;
    TimedLock(const TimedLock&) = delete;
    TimedLock& operator=(const TimedLock&) = delete;
    
    void lock() {
        std::unique_lock<std::mutex> lock(mtx_);
        // 阻塞直到成功
        cond_.wait(lock, [&]{return !flag_;});
        flag_ = true;
    }
    
    void unlock() {
        // 这里不检查是否是由加锁的那个线程去释放
        {
            std::lock_guard<std::mutex> lock(mtx_);
            flag_ = false; // 先释放锁
        }
        cond_.notify_one(); // 再 notify
    }
    
    bool try_lock() noexcept {
        // 使用 try_to_lock，立即返回
        // 获取 mtx_ 通常很短暂，没必要 try_to_lock 获取
        std::lock_guard<std::mutex> lock(mtx_);
        if(flag_)   return false;
        flag_ = true;
        return true;
    }

    template<typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& rel_time) {
        return try_lock_until(std::chrono::steady_clock::now() + rel_time);
    }

    template<typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& abs_time) {
        std::unique_lock<std::mutex> lock(mtx_);
        if(!flag_) {
            flag_ = true;
            return true;
        }

        if(!cond_.wait_until(lock, abs_time, [&]{return !flag_;}))
            return false;
        flag_ = true;
        return true;
    }

private:
    std::condition_variable cond_;
    std::mutex mtx_; // 保护 flag_
    bool flag_{false};
};

using namespace std::chrono;

static void test_try_lock_immediate() {
    std::cout << "[1] try_lock immediate...\n";
    TimedLock m;

    bool ok1 = m.try_lock();
    assert(ok1 && "first try_lock should succeed");

    bool ok2 = m.try_lock();
    assert(!ok2 && "second try_lock should fail immediately when locked");

    m.unlock();

    bool ok3 = m.try_lock();
    assert(ok3 && "try_lock should succeed after unlock");
    m.unlock();
}

static void test_lock_blocks_until_unlock() {
    std::cout << "[2] lock blocks until unlock...\n";
    TimedLock m;
    m.lock();

    std::atomic<bool> acquired{false};
    auto t0 = steady_clock::now();

    std::thread waiter([&]{
        m.lock();
        acquired.store(true, std::memory_order_release);
        m.unlock();
    });

    // 确保 waiter 真的在等
    std::this_thread::sleep_for(50ms);
    assert(!acquired.load(std::memory_order_acquire));

    // 再等一会儿再释放
    std::this_thread::sleep_for(80ms);
    m.unlock();

    waiter.join();
    auto dt = duration_cast<milliseconds>(steady_clock::now() - t0).count();

    assert(acquired.load(std::memory_order_acquire));
    // waiter 至少应该等到我们 unlock 之后：这里给一个宽松下限
    assert(dt >= 100 && "waiter should have been blocked for a while");
}

static void test_try_lock_for_timeout_and_success() {
    std::cout << "[3] try_lock_for timeout then success...\n";
    TimedLock m;
    m.lock();

    // 在锁被占用时应超时失败
    auto start = steady_clock::now();
    bool ok = m.try_lock_for(120ms);
    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();

    assert(!ok);
    assert(elapsed >= 100 && "should wait close to timeout"); // 宽松断言

    // 释放后应能成功
    m.unlock();
    ok = m.try_lock_for(50ms);
    assert(ok);
    m.unlock();
}

static void test_try_lock_until_timeout_and_success() {
    std::cout << "[4] try_lock_until timeout then success...\n";
    TimedLock m;
    m.lock();

    auto deadline = steady_clock::now() + 120ms;
    bool ok = m.try_lock_until(deadline);
    assert(!ok && "should time out while locked");

    m.unlock();

    deadline = steady_clock::now() + 120ms;
    ok = m.try_lock_until(deadline);
    assert(ok && "should succeed when unlocked");
    m.unlock();
}

static void test_stress_exclusion() {
    std::cout << "[5] stress: mutual exclusion...\n";
    TimedLock m;
    std::atomic<int> in_cs{0};
    std::atomic<int> passes{0};

    constexpr int kThreads = 8;
    constexpr int kIters   = 2000;

    auto worker = [&](int id){
        for (int i = 0; i < kIters; ++i) {
            // 混合使用：有时阻塞 lock，有时 try_lock_for
            if ((i + id) % 3 == 0) {
                m.lock();
            } else {
                while (!m.try_lock_for(1ms)) {
                    // 小睡一下减少忙等
                    std::this_thread::yield();
                }
            }

            int prev = in_cs.fetch_add(1, std::memory_order_acq_rel);
            assert(prev == 0 && "more than one thread in critical section!");

            // 临界区做一点事
            passes.fetch_add(1, std::memory_order_relaxed);

            prev = in_cs.fetch_sub(1, std::memory_order_acq_rel);
            assert(prev == 1);

            m.unlock();
        }
    };

    std::vector<std::thread> ts;
    ts.reserve(kThreads);
    for (int i = 0; i < kThreads; ++i) ts.emplace_back(worker, i);
    for (auto& t : ts) t.join();

    std::cout << "    passes = " << passes.load() << "\n";
    assert(passes.load() == kThreads * kIters);
}

int main() {
    test_try_lock_immediate();
    test_lock_blocks_until_unlock();
    test_try_lock_for_timeout_and_success();
    test_try_lock_until_timeout_and_success();
    test_stress_exclusion();

    std::cout << "All tests passed ✅\n";
    return 0;
}