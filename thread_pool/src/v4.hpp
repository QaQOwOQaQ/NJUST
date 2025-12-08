#ifndef THREAD_POOL_V4_H
#define THREAD_POOL_V4_H

#include <atomic>
#include <cassert>
#include <chrono>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "TaskQueue.hpp"

class ThreadPool {
public:
    // 初始化线程池
    //  - 初始化 min_threads 个核心线程
    //  - 根据负载自动扩容到 max_threads
    //  - 空闲超过 idle_timeout_sec 的线程会被回收（但线程数不少于 min_threads）
    ThreadPool(int min_threads = 2,
               int max_threads = std::thread::hardware_concurrency(),
               int idle_timeout_sec = 2);
    ~ThreadPool() { stop(); }
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

public:
    // 停止线程池
    //   - 停止接受新任务
    //   - 等待队列中已存在的任务全部执行完
    //   - 等待所有线程退出
    //   - 幂等：多次调用只有第一次有效
    void stop();

    // 当前队列中待执行任务数（近似值，仅供观察使用）
    int pending() const { return static_cast<int>(queue_.size()); }

    // 当前活跃活线程数量
    int active_threads_count() {
        std::lock_guard<std::mutex> lock(thread_mutex_);
        return static_cast<int>(workers_.size());
    }

public:
    // 普通任务
    template <typename F, typename... Args>
    void add_task(F&& f, Args&&... args) {
        if (stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("ThreadPool has been stopped");
        }
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        queue_.push(std::move(task));
        try_expand_workers();
    }

    // 批量普通任务
    template <typename TaskContainer>
    void add_batch_task(TaskContainer&& task_list) {
        static_assert(std::is_rvalue_reference_v<decltype(task_list)&&>,
                      "task_list must be an rvalue");

        if (stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("ThreadPool has been stopped");
        }
        // task_list 为空时直接返回，避免执行 try_expand_workers
        if (task_list.empty()) return;
        for (auto& t : task_list) {
            queue_.push(std::move(t));
        }
        try_expand_workers();
    }

    // 有返回值的任务
    template <typename F, typename... Args>
    auto add_future_task(F&& f, Args&&... args)
            -> std::future<std::invoke_result_t<F, Args...>> {
        if (stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("ThreadPool has been stopped");
        }
        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        queue_.push([task] { (*task)(); });
        try_expand_workers();
        return res;
    }

    // 高优先级任务
    template <typename F, typename... Args>
    void add_priority_task(F&& f, Args&&... args) {
        if (stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("ThreadPool has been stopped");
        }
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        queue_.push_priority(std::move(task));
        try_expand_workers();
    }

    // 延时任务
    template <typename F, typename... Args>
    void add_delay_task(int64_t delay_ms, F&& f, Args&&... args) {
        if (stop_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("ThreadPool has been stopped");
        }
        auto exec_tm = std::chrono::steady_clock::now() +
                       std::chrono::milliseconds(delay_ms);
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        queue_.push_delay(std::move(task), exec_tm);
        try_expand_workers();
    }

private:
    // 工作线程主循环：不断从 TaskQueue 中 pop 任务并执行
    void worker_loop();

    // 扩容逻辑：根据 pending/idle/active 情况决定是否创建新线程
    void try_expand_workers();

    // 清理已退出线程：将 dead_workers_ 中的线程 join 掉
    void clean_inactive_threads();

private:
    int min_threads_;                        // 核心线程数
    int max_threads_;                        // 最大线程数
    std::chrono::seconds idle_timeout_sec_;  // 非核心线程的空闲回收时间

    TaskQueue queue_;  // 任务队列

    mutable std::mutex thread_mutex_;  // 保护 workers_ / dead_workers_
    std::unordered_map<std::thread::id, std::thread> workers_;  // 当前活跃线程
    std::list<std::thread> dead_workers_;  // 已经从 workers_ 中移除的线程，用于稍后 join
                                          // 使用 list 管理是因为 list 移动效率很高（splice）

    std::atomic<int> idle_threads_{0};  // 当前处于“阻塞等待任务”的线程数（近似值）
    std::atomic<bool> stop_{false};  // 线程池是否停止
};

// 类外定义不需要再次执行默认值
inline ThreadPool::ThreadPool(int min_threads, 
                              int max_threads,
                              int idle_timeout_sec)
    : min_threads_(min_threads),
      max_threads_(std::max(min_threads, max_threads)),
      idle_timeout_sec_(idle_timeout_sec) 
{
    std::lock_guard<std::mutex> lock(thread_mutex_);
    for (int i = 0; i < min_threads; i++) {
        std::thread t(&ThreadPool::worker_loop, this);
        workers_.emplace(t.get_id(), std::move(t));
    }
}

inline void ThreadPool::stop() {
    // 使用 CAS 保证 stop 幂等: 只有第一个线程真正执行停止流程
    bool expected = false;
    if (!stop_.compare_exchange_strong(expected, true,
                                       std::memory_order_acq_rel)) {
        return;
    }

    queue_.stop();

    // 将所有线程对象从 workers_ / dead_workers_ 中移动出来，避免在持锁时 join
    // 导致死锁
    std::vector<std::thread> threads_to_join;
    {
        std::lock_guard<std::mutex> lock(thread_mutex_);
        for (auto& pair : workers_)
            threads_to_join.emplace_back(std::move(pair.second));
        workers_.clear();

        for (auto& t : dead_workers_)
            threads_to_join.emplace_back(std::move(t));
        dead_workers_.clear();
    }

    for (auto& t : threads_to_join) {
        if (t.joinable()) t.join();
    }
}

inline void ThreadPool::worker_loop() {
    // 线程启动时，认为自己一定是“空闲”的
    idle_threads_.fetch_add(1, std::memory_order_relaxed);

    // 对 idle_threads_ 的处理要小心，要确保该值不会泄漏
    for (;;) {
        TaskQueue::Task task;
        TaskQueue::PopResult result = queue_.pop(task, idle_timeout_sec_);
        if (result == TaskQueue::PopResult::STOPPED) {
            // ？
            idle_threads_.fetch_sub(1, std::memory_order_relaxed);
            return;
        }
        if (result == TaskQueue::PopResult::TIMEOUT) {
            // 如果是因为停机导致没有任务执行，关闭线程
            if (stop_.load(std::memory_order_relaxed)) {
                idle_threads_.fetch_sub(1, std::memory_order_relaxed);
                return;
            }
            // 否则: 我们认为线程数多于任务数，尝试缩容（裁掉非核心线程）
            // 这里我们的缩容策略比较简单：裁掉当前线程
            std::lock_guard<std::mutex> lock(thread_mutex_);
            if (static_cast<int>(workers_.size()) > min_threads_) {
                auto my_id = std::this_thread::get_id();
                auto it = workers_.find(my_id);
                if (it != workers_.end()) {
                    idle_threads_.fetch_sub(1, std::memory_order_relaxed);
                    // 将需要裁掉的线程移至 dead_workers_，稍后由其它线程同一
                    // join （这里就能理解 dead_workers_
                    // 的意义了，它相当于一个缓冲区，缓冲需要 join 的线程，
                    // 并一次性加锁解锁统一处理，避免频繁的加锁和解锁）
                    dead_workers_.push_back(std::move(it->second));
                    workers_.erase(it);
                }
                // 成功缩容后，当前线程退出
                return;
            }
            // 不可缩容
            continue;
        }
        // result == ok
        idle_threads_.fetch_sub(1, std::memory_order_relaxed);
        if (task) task();
        idle_threads_.fetch_add(1, std::memory_order_relaxed);
    }
}

// 扩容策略：根据 pending / idle / active
// 状态一次性创建所需线程，避免频繁创建销毁
inline void ThreadPool::try_expand_workers() {
    // 顺便清理掉已经结束的线程
    clean_inactive_threads();

    // 停机不扩容
    if (stop_.load(std::memory_order_relaxed)) return;

    std::lock_guard<std::mutex> lock(thread_mutex_);
    size_t pending = static_cast<size_t>(queue_.size());       // 总任务数
    int idle = idle_threads_.load(std::memory_order_relaxed);  // 空闲线程数
    size_t active = workers_.size();  // 活跃线程数

    // 扩容触发条件：
    //    - 当前活跃线程数 < max_threads_
    //    - pending 任务数 > idle 空闲线程数 + 一定阈值（这里是 1）
    //
    // 这样避免“刚有一点任务就频繁创建线程”的抖动
    const int threshold = 1;
    if (active < static_cast<size_t>(max_threads_) &&
        pending > static_cast<size_t>(idle + threshold)) {
        // 需要创建的线程数量 = pending - idle（每个空闲线程可以处理一个任务）
        // 最终创建的线程数不能超过 max_threads - active
        size_t threads_needed =
            std::min(pending - static_cast<size_t>(idle),
                     static_cast<size_t>(max_threads_) - active);

        for (size_t i = 0; i < threads_needed; i++) {
            std::thread t(&ThreadPool::worker_loop, this);
            workers_.emplace(t.get_id(), std::move(t));
        }
        // 注意：新线程启动后会在 worker_loop 的开头对 idle_threads ++
        //      这路无需手动调整 idle_threads_
    }
}

inline void ThreadPool::clean_inactive_threads() {
    // 将 dead_workers_ 中的线程移动到局部变量中，在不持有锁的环境下 join
    std::list<std::thread> local_dead;
    {
        // 使用 try_to_lock，避免与其它地方（例如 stop()）产生锁竞争的长时间阻塞
        std::unique_lock<std::mutex> lock(thread_mutex_, std::try_to_lock);
        if (!lock.owns_lock() || dead_workers_.empty()) return;
        local_dead.splice(local_dead.begin(), dead_workers_);
    }
    for (auto& t : local_dead) {
        if (t.joinable()) t.join();
    }
}

#endif