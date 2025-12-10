// 实现一个有定时功能的可重入锁
// 在 recursive_lock 的基础上，添加定时功能 try_lock_for 和 try_lock_until 即可


// 重点要搞清楚下面三个 lock 的语义：
// lock：阻塞到成功
// try_lock：立即返回
// try_lock_for/try_lock_until：带超时
#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <sstream>
#include <functional>

class RecursiveTimedLock {
public:
    void lock() {
        const auto this_id = std::this_thread::get_id();
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [&]{ return cnt_ == 0 || owner_ == this_id; });
        if(cnt_ == 0) owner_ = this_id;
        ++ cnt_;
    }

    bool try_lock() noexcept {
        const auto this_id = std::this_thread::get_id();
        // 等待 mtx_ 的时间很短，获取它时没必要使用 try_to_lock
        std::lock_guard<std::mutex> lock(mtx_);
        if(this_id == owner_) {
            ++ cnt_;
            return true;
        }     
        if(cnt_ == 0) {
            owner_ = this_id;
            cnt_ = 1;
            return true;
        }
        return false;
    }

    void unlock() {
        const auto this_id = std::this_thread::get_id();
        std::unique_lock<std::mutex> lock(mtx_);

        if(cnt_ == 0 || owner_ != this_id) 
            std::terminate();
        
        if( -- cnt_ == 0) {
            owner_ = std::thread::id{};
            lock.unlock(); // 先释放内部 mutex 再 notify，减少惊群竞争
            cond_.notify_one();  // 标准不保证公平，notify_one 足够
        }
    }

    template<typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& rel_time) {
        return try_lock_until(std::chrono::steady_clock::now() + rel_time);
    }

    // try_lock_until
    //     1. 如果现在就能拿到锁（owner_ == this || cnt_ == 0）：返回 true
    //     2. 否则：阻塞等待，但最多只能等待到时间点 t
    //          2.1 若在 t 之前锁变得可用（cnt_ == 0）并成功获取：返回 true
    //          2.2 否则，返回 false
    template<typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& abs_time) {
        const auto this_id = std::this_thread::get_id();

        // 注意获取 mtx_ 的等待时间不需要处理超时，
        // try_lock_until 的“超时等待”主要体现在 cv_.wait_until 上；
        // 内部锁的等待通常非常短，属于实现必须付出的同步成本。
        // 若你硬要把内部锁的等待也纳入严格的 deadline 控制，可以做 defer_lock + 自旋/退避，但通常得不偿失。
        std::unique_lock<std::mutex> lock(mtx_);

        // 不需要等待
        if(cnt_ == 0) {
            owner_ = this_id;
            cnt_ = 1;
            return true;
        }
        if(owner_ == this_id) { 
            ++ cnt_;
            return true;
        }

        // 定时等待
        if(!cond_.wait_until(lock, abs_time, [&]{return cnt_ == 0;}))
            return false;

        // 此时不可能出现 owner_ == this_id
        // 因为 this_id 所在线程卡在上面的 wait_until
        // 该线程不可能对 cnt_ 和 owner_ 产生影响

        // if (cnt_ == 0)
        owner_ = this_id;
        cnt_ = 1;
        return true;
    }

private:
    std::condition_variable cond_;
    std::mutex mtx_;
    std::thread::id owner_{};
    uint64_t cnt_{0}; // 暂时不考虑溢出问题
};

// ------------------------ Tests ------------------------

static RecursiveTimedLock g_lock;
static int g_value = 0;

static std::string tid() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

// 测试1：同线程递归可重入
void test_reentrant_single_thread() {
    std::cout << "\n[Test1] reentrant in same thread\n";
    g_value = 0;

    std::function<void(int)> dfs = [&](int d) {
        std::lock_guard<RecursiveTimedLock> lg(g_lock);
        ++g_value;
        if (d > 0) dfs(d - 1);
    };

    dfs(5);
    std::cout << "  g_value=" << g_value << " (expect 6)\n";
}

// 测试2：try_lock_for 超时失败 + 后续成功
void test_timeout_then_success() {
    std::cout << "\n[Test2] timeout then success\n";
    g_value = 0;

    std::atomic<bool> holder_entered{false};

    std::thread holder([&]{
        g_lock.lock();
        holder_entered.store(true, std::memory_order_release);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        g_lock.unlock();
    });

    // 等待 holder 确实持锁
    while (!holder_entered.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }

    auto t0 = std::chrono::steady_clock::now();
    bool ok1 = g_lock.try_lock_for(std::chrono::milliseconds(50));
    auto t1 = std::chrono::steady_clock::now();

    std::cout << "  try_lock_for(50ms) => " << (ok1 ? "true" : "false")
              << ", waited " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
              << "ms (expect false, ~>=50ms)\n";

    bool ok2 = g_lock.try_lock_for(std::chrono::milliseconds(400));
    std::cout << "  try_lock_for(400ms) => " << (ok2 ? "true" : "false")
              << " (expect true)\n";
    if (ok2) {
        ++g_value;
        g_lock.unlock();
    }

    holder.join();
    std::cout << "  g_value=" << g_value << " (expect 1)\n";
}

// 测试3：多线程互斥 + 递归调用，最终值校验
void test_multi_thread_final_value() {
    std::cout << "\n[Test3] multi-thread mutual exclusion + recursion\n";
    g_value = 0;

    auto recursive_work = [&](auto&& self, int depth, int max_depth, int id) -> void {
        std::lock_guard<RecursiveTimedLock> lg(g_lock);
        ++g_value;
        // 模拟一些工作
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        if (depth < max_depth) self(self, depth + 1, max_depth, id);
    };

    const int N = 4;
    const int loops = 5;
    const int max_depth = 3; // depth:1..3 => +3 次

    std::vector<std::thread> ts;
    for (int i = 0; i < N; ++i) {
        ts.emplace_back([&, i]{
            for (int k = 0; k < loops; ++k) {
                recursive_work(recursive_work, 1, max_depth, i);
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        });
    }
    for (auto& t : ts) t.join();

    int expected = N * loops * max_depth;
    std::cout << "  g_value=" << g_value << " expected=" << expected << "\n";
}

// 测试4：try_lock_until（用 system_clock 的绝对时间点）
void test_try_lock_until_system_clock() {
    std::cout << "\n[Test4] try_lock_until(system_clock)\n";
    std::atomic<bool> holder_entered{false};

    std::thread holder([&]{
        g_lock.lock();
        holder_entered.store(true, std::memory_order_release);
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        g_lock.unlock();
    });

    while (!holder_entered.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }

    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(50);
    bool ok1 = g_lock.try_lock_until(deadline);
    std::cout << "  try_lock_until(now+50ms) => " << (ok1 ? "true" : "false")
              << " (expect false)\n";

    auto deadline2 = std::chrono::system_clock::now() + std::chrono::milliseconds(300);
    bool ok2 = g_lock.try_lock_until(deadline2);
    std::cout << "  try_lock_until(now+300ms) => " << (ok2 ? "true" : "false")
              << " (expect true)\n";
    if (ok2) g_lock.unlock();

    holder.join();
}

int main() {
    std::cout << "__cplusplus=" << __cplusplus << "\n";
    test_reentrant_single_thread();
    test_timeout_then_success();
    test_multi_thread_final_value();
    test_try_lock_until_system_clock();
    std::cout << "\nAll tests finished.\n";
    return 0;
}