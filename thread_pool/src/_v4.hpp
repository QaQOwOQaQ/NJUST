#ifndef THREAD_POOL_V5_H
#define THREAD_POOL_V5_H

#include <vector>
#include <queue>
#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <memory>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <map>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <stdexcept>

//
// =========================== 整体设计说明 ===========================
//
// 本实现将“任务队列”和“线程池管理”彻底解耦：
//
// 1. TaskQueue
//    - 只负责：任务的存储、同步、多线程安全的 push/pop、延时任务调度。
//    - 内部使用：
//        * std::deque<Task>              普通任务（FIFO）
//        * std::priority_queue<TimeTask> 延时任务（按时间排序）
//        * std::mutex + std::condition_variable 进行同步
//    - 特点：
//        * 支持普通任务、高优先级任务（头插）、延时任务（按时间点执行）。
//        * pop() 支持 idle_timeout，用于线程池判断“空闲太久可以缩容”。
//        * stop() 仅设置标记并唤醒所有等待线程，不直接清理任务。
//          线程池在接收到 STOPPED 后自行结束。
//
// 2. ThreadPool
//    - 只负责：线程的创建 / 缩容 / 生命周期管理，以及对外暴露 add_task 等接口。
//    - 内部使用：
//        * 一个 TaskQueue 实例 queue_ 作为任务来源。
//        * std::map<std::thread::id, std::thread> workers_ 管理活跃线程。
//        * std::list<std::thread> dead_workers_ 存放待 join 的“死线程”。
//        * idle_threads_ 统计当前空闲线程数量，用于扩容决策。
//        * stopping_ 表示线程池是否已进入停止流程。
//
// 目标：
//   - 支持：
//      * 异步任务 + future 返回值
//      * 高优先级任务（插队）
//      * 延时任务
//      * 自动扩容到 max_threads，空闲后缩回 min_threads
//   - 避免：
//      * 数据竞争（所有共享状态都通过 mutex/atomic 保护）
//      * 死锁（特别是 stop() 和 worker_loop 中的锁顺序）
//


// ================= TaskQueue =================
class TaskQueue {
public:
    using Task = std::function<void()>;

    // pop 操作的结果：
    //  - OK      : 成功取到任务
    //  - STOPPED : 队列已停止且无任何剩余任务，消费者应退出
    //  - TIMEOUT : 在指定 idle_timeout 内一直没有新任务，用于线程池缩容
    enum class PopResult { OK, STOPPED, TIMEOUT };

    TaskQueue() = default;
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    // 普通任务：尾插入队，FIFO
    inline void push(Task&& task) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            // 如果已经停止，则直接丢弃任务（不再接收新的）
            if (stop_) return;
            tasks_.emplace_back(std::move(task));
        }
        // 唤醒一个等待线程
        cond_.notify_one();
    }

    // 高优先级任务：头插，保证优先于普通任务被处理
    inline void push_front(Task&& task) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stop_) return;
            tasks_.emplace_front(std::move(task));
        }
        cond_.notify_one();
    }

    // 延时任务：将在指定的时间点 exec_time 执行
    inline void push_delay(Task&& task, std::chrono::steady_clock::time_point exec_time) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stop_) return;
            delay_tasks_.emplace(exec_time, std::move(task));
        }
        // 可能新的任务比已有延时任务更早，需要唤醒线程重新计算等待时间
        cond_.notify_one();
    }

    // 核心消费接口：从队列中获取任务
    //
    // 参数：
    //   - out_task     : 输出参数，用于接收取出的任务
    //   - idle_timeout : 当队列完全空闲并且没有到期的延时任务时的“最大等待时间”
    //
    // 返回：
    //   - OK      : 取到任务（普通任务或到期的延时任务）
    //   - STOPPED : TaskQueue 被 stop() 且已无任何任务
    //   - TIMEOUT : 队列在 idle_timeout 内一直空闲，用于通知线程池可以缩容
    //
    // 说明：
    //   - 即使 stop_ 变为 true，只要队列中还有未执行的任务（包括未来的延时任务），
    //     pop() 仍会等待并依次返回这些任务，只有在“队列为空”时才返回 STOPPED。
    inline PopResult pop(Task& out_task, std::chrono::seconds idle_timeout) {
        std::unique_lock<std::mutex> lock(mutex_);

        while (true) {
            auto now = std::chrono::steady_clock::now();

            // 1. 是否有到期的延时任务（delay_tasks_ 是按时间排序的小顶堆）
            bool has_ready_delay_task = false;
            if (!delay_tasks_.empty() && delay_tasks_.top().exec_tm_ <= now) {
                has_ready_delay_task = true;
            }

            // 2. 优先取到期的延时任务
            if (has_ready_delay_task) {
                // 先复制顶元素，再 pop。避免对 top() 返回的 const 引用做 const_cast。
                TimeTask top_copy = delay_tasks_.top();
                delay_tasks_.pop();
                out_task = std::move(top_copy.task_);
                return PopResult::OK;
            }

            // 3. 其次取普通任务（队列前端）
            if (!tasks_.empty()) {
                out_task = std::move(tasks_.front());
                tasks_.pop_front();
                return PopResult::OK;
            }

            // 4. 此时不存在 ready 任务（既没有到期延时任务，也没有普通任务）
            //    如果已经 stop 且没有任何待执行任务，则真正结束。
            if (stop_ && tasks_.empty() && delay_tasks_.empty()) {
                return PopResult::STOPPED;
            }

            // 5. 队列为空，计算等待时间
            auto wait_duration = std::chrono::duration_cast<std::chrono::milliseconds>(idle_timeout);
            
            // 如果还有未到期的延时任务，则等待时间不能超过“下一个任务的触发时间”
            if (!delay_tasks_.empty()) {
                auto time_until_next = delay_tasks_.top().exec_tm_ - now;
                auto time_until_next_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_until_next);
                if (time_until_next_ms < std::chrono::milliseconds::zero()) {
                    // 理论上不会出现（因为前面已经判断 <= now 的情况），这里做个兜底
                    time_until_next_ms = std::chrono::milliseconds::zero();
                }
                if (time_until_next_ms < wait_duration) {
                    wait_duration = time_until_next_ms;
                }
            }

            // 如果“下一次延时任务的触发时间”已经到了，那么不该等待，直接下一轮循环取任务
            if (wait_duration <= std::chrono::milliseconds::zero()) {
                continue;
            }

            // 6. 进入等待
            auto wait_result = cond_.wait_for(lock, wait_duration);
            
            // 被唤醒后重新获取当前时间
            now = std::chrono::steady_clock::now();
            
            if (wait_result == std::cv_status::timeout) {
                // 超时唤醒：检查当前队列状态
                bool no_normal_task = tasks_.empty();
                bool no_ready_delay = delay_tasks_.empty() || delay_tasks_.top().exec_tm_ > now;

                // 6.1 如果已经 stop 且队列确实为空，返回 STOPPED
                if (stop_ && no_normal_task && delay_tasks_.empty()) {
                    return PopResult::STOPPED;
                }

                // 6.2 正常 idle 超时：没有任何任务 ready，可以请求线程池缩容
                if (no_normal_task && no_ready_delay) {
                    // 注意：这里使用的是“本次 wait_duration 与 idle_timeout 的比较”
                    //       作为一个近似判断，认为等满 idle_timeout 即为“可缩容”。
                    if (wait_duration >= std::chrono::duration_cast<std::chrono::milliseconds>(idle_timeout)) {
                        return PopResult::TIMEOUT;
                    }
                }

                // 如果还有任务即将 ready，则继续 while 重新判断
            }
            // 如果是 notify 唤醒：不在这里返回，继续 while 循环检查任务/状态
        }
    }

    // 标记队列停止：
    //   - 设置 stop_ = true
    //   - 唤醒所有等待线程（以便它们尽快检测到 stop_ 并退出或完成剩余任务）
    inline void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cond_.notify_all();
    }

    // 当前队列中剩余任务数量（普通 + 延时）
    inline size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.size() + delay_tasks_.size();
    }

private:
    // 延时任务结构：包含执行时间点和具体任务
    struct TimeTask {
        std::chrono::steady_clock::time_point exec_tm_;
        Task task_;
        TimeTask(std::chrono::steady_clock::time_point tm, Task&& t)
            : exec_tm_(tm), task_(std::move(t)) {}
        // priority_queue 默认是大顶堆，为了让“时间最早”的任务在顶部，这里使用 > 号
        bool operator<(const TimeTask& other) const { return exec_tm_ > other.exec_tm_; }
    };

    // 普通任务队列：从 front 取任务，back 插入
    std::deque<Task> tasks_;
    // 延时任务队列：按执行时间排序的小顶堆（通过 operator< 反转实现）
    std::priority_queue<TimeTask> delay_tasks_;

    mutable std::mutex mutex_;
    std::condition_variable cond_;
    bool stop_ = false;  // 为 true 时表示不再接收新任务，但未完成的任务仍会被处理
};


// ================= ThreadPool =================
//
// 线程池：使用 TaskQueue 作为任务源，负责：
//   - 初始化 min_threads 个核心线程；
//   - 根据负载自动扩容到 max_threads；
//   - 空闲超过 idle_timeout 的线程会被回收（但不低于 min_threads）；
//   - 提供 add_task / add_future_task / add_priority_task / delay_task 等接口；
//   - stop() 后停止接收新任务，并等待所有任务执行完毕再退出。
//
class ThreadPool {
public:
    // 构造参数：
    //   - min_threads      : 核心线程数（不会被缩容删除）；
    //   - max_threads      : 最大线程数；
    //   - idle_timeout_sec : 非核心线程连续空闲的时间，超过则允许回收。
    inline ThreadPool(int min_threads = 2,
                      int max_threads = std::thread::hardware_concurrency(),
                      int idle_timeout_sec = 2);
    inline ~ThreadPool();

    // 停止线程池：
    //   - 幂等：多次调用只有第一次有效；
    //   - 停止接受新任务；
    //   - 等待队列中已存在的任务全部执行完；
    //   - 等待所有线程退出（可缩容线程退出 + 核心线程退出）。
    inline void stop();

    // 当前队列中待执行任务数（近似值，仅供观察使用）
    inline int pending() const { return static_cast<int>(queue_.size()); }

    // 当前活跃线程数量（map 中线程数，不含 dead_workers_）
    inline int active_threads_count();

    // 普通任务（无返回值）：自动推导参数，丢进队列执行
    template<class F, class... Args>
    void add_task(F&& f, Args&&... args) {
        // 若线程池处于停止中，禁止再提交任务，避免任务永远不执行
        if (stopping_.load(std::memory_order_acquire))
            throw std::runtime_error("ThreadPool has been stopped");

        // 使用 std::bind 将函数和参数打包成一个无参可调用对象
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        queue_.push(std::move(task));
        try_expand_workers();
    }

    // 批量任务：一次性提交多个 Task（容器元素为可调用对象）
    template<class TaskContainer>
    bool add_batch_task(TaskContainer&& task_list) {
        if (stopping_.load(std::memory_order_acquire))
            return false;
        if (task_list.empty()) return false;

        for (auto& t : task_list)
            queue_.push(std::move(t));

        // 批量提交之后只需触发一次扩容逻辑，内部根据 pending/idle 计算需要多少线程
        try_expand_workers();
        return true;
    }

    // 有返回值的任务：返回 std::future<ReturnType>
    // 使用 std::packaged_task 封装可调用对象并获取 future。
    template<class F, class... Args>
#if __cplusplus >= 201703L
    auto add_future_task(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result_t<F, Args...>>;
#else
    auto add_future_task(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
#endif

    // 高优先级任务：插队到普通任务之前执行（仍然先于后续普通任务）
    template<class F, class... Args>
    void add_priority_task(F&& f, Args&&... args) {
        if (stopping_.load(std::memory_order_acquire))
            throw std::runtime_error("ThreadPool has been stopped");
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        queue_.push_front(std::move(task));
        try_expand_workers();
    }

    // 延时任务：延迟 delay_ms 毫秒后执行
    template<class F, class... Args>
    void delay_task(int64_t delay_ms, F&& f, Args&&... args) {
        if (stopping_.load(std::memory_order_acquire))
            throw std::runtime_error("ThreadPool has been stopped");
        auto exec_tm = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_ms);
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        queue_.push_delay(std::move(task), exec_tm);
        try_expand_workers();
    }

private:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // 工作线程主循环：不断从 TaskQueue 中 pop 任务并执行
    inline void worker_loop();
    // 扩容逻辑：根据 pending/idle/active 情况决定是否创建新线程
    inline void try_expand_workers();
    // 清理已退出线程：将 dead_workers_ 中的线程 join 掉
    inline void clean_inactive_threads();

private:
    TaskQueue queue_;                    // 核心任务队列

    mutable std::mutex thread_mutex_;    // 保护 workers_ / dead_workers_

    // 当前活跃线程：key 为 std::thread::id，value 为对应线程对象
    std::unordered_map<std::thread::id, std::thread> workers_;

    // 已经从 workers_ 中移除的线程，用于稍后 join
    std::list<std::thread> dead_workers_;

    int min_threads_;                    // 核心线程数
    int max_threads_;                    // 最大线程数
    std::chrono::seconds idle_timeout_;  // 非核心线程的空闲回收时间

    // 当前处于“阻塞等待任务”的线程数（近似值）
    std::atomic<int> idle_threads_{0};

    // 线程池是否正在停止 / 已停止
    std::atomic<bool> stopping_{false};
};

//--------------------------------------------------
// Implementation
//--------------------------------------------------

inline ThreadPool::ThreadPool(int min_threads, int max_threads, int idle_timeout_sec)
    : min_threads_(min_threads),
      max_threads_(std::max(min_threads, max_threads)), // 确保 max >= min
      idle_timeout_(idle_timeout_sec)
{
    // 初始化时直接创建 min_threads_ 个核心线程
    std::lock_guard<std::mutex> lock(thread_mutex_);
    for (int i = 0; i < min_threads_; ++i) {
        std::thread t(&ThreadPool::worker_loop, this);
        workers_.emplace(t.get_id(), std::move(t));
    }
}

inline ThreadPool::~ThreadPool() {
    // 析构时自动调用 stop()，确保不发生“线程仍在跑而对象已析构”的问题
    stop();
}

inline void ThreadPool::stop() {
    // 使用 CAS 保证 stop() 幂等：只有第一个线程真正执行停止流程
    bool expected = false;
    if (!stopping_.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        return;

    // 通知任务队列停止（不再接受新任务），并唤醒所有等待线程
    queue_.stop();

    // 将所有线程对象从 workers_ / dead_workers_ 中移动出来，避免在持锁时 join 导致死锁
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

    // 在不持锁的情况下依次 join
    for (auto& t : threads_to_join)
        if (t.joinable()) t.join();
}

inline int ThreadPool::active_threads_count() {
    std::lock_guard<std::mutex> lock(thread_mutex_);
    return static_cast<int>(workers_.size());
}

inline void ThreadPool::worker_loop() {
    // 线程启动时：认为自己一开始是“空闲”的
    idle_threads_.fetch_add(1, std::memory_order_relaxed);

    for (;;) {
        TaskQueue::Task task;
        TaskQueue::PopResult result = queue_.pop(task, idle_timeout_);

        if (result == TaskQueue::PopResult::STOPPED) {
            // 队列通知：已经 stop 且没有任务可执行，线程可以安全退出
            idle_threads_.fetch_sub(1, std::memory_order_relaxed);
            return;
        } 
        else if (result == TaskQueue::PopResult::TIMEOUT) {
            // TIMEOUT 表示该线程在 idle_timeout 时间内一直没有任务
            // 若线程池正在停止，则直接退出
            if (stopping_.load(std::memory_order_acquire)) {
                idle_threads_.fetch_sub(1, std::memory_order_relaxed);
                return;
            }

            // 正常情况下：尝试缩容（裁掉非核心线程）
            std::lock_guard<std::mutex> lock(thread_mutex_);
            if (!stopping_.load(std::memory_order_relaxed) &&
                workers_.size() > static_cast<size_t>(min_threads_)) {

                auto my_id = std::this_thread::get_id();
                auto it = workers_.find(my_id);
                if (it != workers_.end()) {
                    // 退出前先将自己从“空闲线程计数”中移除
                    idle_threads_.fetch_sub(1, std::memory_order_relaxed);

                    // 将线程对象移至 dead_workers_，稍后由其他线程统一 join
                    dead_workers_.push_back(std::move(it->second));
                    workers_.erase(it);
                    return;
                }
            }
            // 如果没有被缩容（例如当前线程数刚好等于 min_threads_），继续下一轮等待
            continue;
        }

        // result == OK：成功从队列中取到任务
        // 开始执行任务：先标记自己为“非空闲”
        idle_threads_.fetch_sub(1, std::memory_order_relaxed);
        
        if (task) {
            task();
        }
        
        // 任务执行完毕：重新标记为“空闲”
        idle_threads_.fetch_add(1, std::memory_order_relaxed);
    }
}

inline void ThreadPool::clean_inactive_threads() {
    // 将 dead_workers_ 中的线程移到局部变量中，在不持锁的环境下 join
    std::list<std::thread> local_dead;

    {
        // 使用 try_to_lock，避免与其他地方（例如 stop()）产生锁竞争的长时间阻塞
        std::unique_lock<std::mutex> lock(thread_mutex_, std::try_to_lock);
        if (!lock.owns_lock() || dead_workers_.empty()) return;
        local_dead.splice(local_dead.begin(), dead_workers_);
    }

    for (auto& t : local_dead)
        if (t.joinable()) t.join();
}

// 扩容策略：根据 pending / idle / active 状态一次性创建所需线程，避免频繁创建销毁
inline void ThreadPool::try_expand_workers() {
    if (stopping_.load(std::memory_order_acquire)) return;

    // 顺便清理掉已结束的线程（dead_workers_）
    clean_inactive_threads();

    // 当前队列中待执行任务数量（普通 + 延时）
    size_t pending = static_cast<size_t>(queue_.size());
    // 当前空闲线程数（近似值）
    int idle = idle_threads_.load(std::memory_order_relaxed);

    std::lock_guard<std::mutex> lock(thread_mutex_);
    if (stopping_.load(std::memory_order_relaxed)) return;

    size_t active = workers_.size();

    // 扩容触发条件：
    //   - 当前活跃线程数 < max_threads_
    //   - pending 任务数 > idle 空闲线程数 + 一定阈值（这里是 1）
    //
    // 这样避免“刚有一点任务就频繁创建线程”的抖动。
    const int threshold = 1;
    
    if (active < static_cast<size_t>(max_threads_) && 
        pending > static_cast<size_t>(idle + threshold)) {

        // 需要的线程数量 = pending - idle （每个空闲线程可以处理一个任务）
        // 最终创建的线程数不能超过 max_threads_ - active
        size_t threads_needed = std::min(
            pending - static_cast<size_t>(idle),
            static_cast<size_t>(max_threads_) - active
        );
        
        for (size_t i = 0; i < threads_needed; ++i) {
            std::thread t(&ThreadPool::worker_loop, this);
            workers_.emplace(t.get_id(), std::move(t));
            // 注意：新线程启动后会在 worker_loop 的开头对 idle_threads_++，
            //       这里无需手动调整 idle_threads_。
        }
    }
}

// 有返回值任务：使用 std::packaged_task 封装可调用对象，返回对应的 future。
template<class F, class... Args>
#if __cplusplus >= 201703L
auto ThreadPool::add_future_task(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type>
{
    using return_type = typename std::invoke_result<F, Args...>::type;
#else
auto ThreadPool::add_future_task(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;
#endif
    if (stopping_.load(std::memory_order_acquire))
        throw std::runtime_error("ThreadPool has been stopped");

    // 将任务及其参数打包为无参函数 return_type()
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    // 先获取 future，之后任务执行完会自动填充结果
    std::future<return_type> res = task->get_future();

    // 实际 push 进队列的是一个简单的 lambda，在执行时调用 *task
    queue_.push([task]() { (*task)(); });

    // 每次新提交带返回值的任务，也尝试做一次扩容
    try_expand_workers();
    return res;
}

#endif // THREAD_POOL_V5_H
