#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H 

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
#include <list>
#include <map>
#include <list>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

// 并发安全的，带优先级控制和延时功能的阻塞式任务队列
class TaskQueue {
public:
    using Task = std::function<void()>;

    // pop 操作的结果：
    // - OK:     : 成功取到任务
    // - STOPPED : 队列已停止且无任何剩余任务，消费者应退出
    // - TIMEOUT : 在指定 idle_timeout 内一直没有新任务，用于线程池缩容
    enum class PopResult { OK, STOPPED, TIMEOUT };

    TaskQueue() = default;
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    // 关闭队列
    void stop() {
        {
            // 注意对 stop_ 的访问需要加锁
            std::lock_guard<std::mutex> lock(mtx_);
            stop_ = true;
        }
        // 唤醒所有等待线程，以便它们尽快检测到 stop_=true，并退出或完成剩余任务
        cond_.notify_all();
    }

    // 当前队列中总剩余任务数量
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return tasks_.size() + delay_tasks_.size();
    }

    // 普通任务: 尾部插入
    void push(Task&& task) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if(stop_)   return ;
            // 使用 emplace_back 和移动语义优化
            tasks_.emplace_back(std::move(task));
        }
        cond_.notify_one();
    }

    // 优先级任务: 头部插入
    void push_priority(Task&& task) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if(stop_)   return ;
            tasks_.emplace_front(std::move(task));
        }
        cond_.notify_one();
    }

    // 延时任务: 将在指定的时间点 exec_tm 执行
    void push_delay(Task&& task, std::chrono::steady_clock::time_point exec_tm) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if(stop_)   return ;
            delay_tasks_.emplace(exec_tm, std::move(task));
        }
        cond_.notify_one();
    }

    // 从队列中取出任务
    //
    // 参数:
    //   - out_task     : 输出参数，用于接受取出的任务
    //   - idle_timeout : 没有可执行任务（没有普通任务且没有到期的延时任务）时的“最大等待时间”
    //
    // 返回值:
    //   - OK:     : 成功取到任务
    //   - STOPPED : 队列已停止且无任何剩余任务，消费者应退出
    //   - TIMEOUT : 在指定 idle_timeout 内一直没有新任务，用于线程池缩容
    // 说明:
    //   - 即使 stop_=true，只要队列中还有未执行的任务（包括未来的延时任务），
    //     pop() 仍会等待并以此返回这些任务，只有在“队列为空”时才返回 STOPPED
    //   - 即使队列中存在未完成的延时任务，只要 idle_timeout 截至，仍然会返回 TIMEOUT
    PopResult pop(Task& out_task, std::chrono::seconds idle_timeout) {
        std::unique_lock<std::mutex> lock(mtx_);

        using namespace std::chrono;
        auto deadline = steady_clock::now() + idle_timeout;

        // (begining)是否停机？
        //   YES : return STOPPED
        //   NO  : 是否存在可执行的任务？
        //          YES ： return OK
        //          NO  :  等待一段时间
        //                    超时唤醒 && 真的超时: return TIMEOUT
        //                    ELSE: go begining
        while(true) {
            auto now = std::chrono::steady_clock::now();

            // 1. 队列已停止且无任何剩余任务
            //    注意这里不能通过成员函数 size() 判断队列是否为空，这将会导致死锁
            if(stop_ && delay_tasks_.empty() && tasks_.empty()) {
                return PopResult::STOPPED;
            }

            // 2. 优先取到期的延时任务
            if(!delay_tasks_.empty() && delay_tasks_.top().exec_tm_ <= now) {
                TimeTask top_copy = delay_tasks_.top();
                delay_tasks_.pop();
                out_task = std::move(top_copy.task_);
                return PopResult::OK;
            }

            // 3. 其次取普通任务
            if(!tasks_.empty()) {
                out_task = std::move(tasks_.front());
                tasks_.pop_front();
                return PopResult::OK;
            } 

            // 4. 等待下一个延时任务
            //    注意此时延时队列可能为空，只不过 stop_=false，因此还没有停机
            auto wait_until_tm = deadline; // 默认等到 idle_timeout 截止
        
            if(!delay_tasks_.empty()) {
                // 如果存在未到期的延时任务，则等待时间不能超过最早的延时任务执行时间
                wait_until_tm = std::min(deadline, delay_tasks_.top().exec_tm_);
            }
            
            // 等待直到 wait_until_tm
            // cond_.wait_until 会返回 std::cv_status::timeout 如果它等待到了 wait_until_tm
            // 否则返回 std::cv_status::no_timeout (被 notify 唤醒或虚假唤醒)
            if(cond_.wait_until(lock, wait_until_tm) == std::cv_status::timeout) {
                
                // 3. 被唤醒后，重新检查当前时间是否达到 idle_timeout 的 deadline
                //    注意：
                //    * 如果 wait_until_tm == earliest_delay_task_tm 且超时，需回到循环开头检查延时任务。
                //    * 如果 wait_until_tm == deadline 且超时，则应返回 TIMEOUT。

                if (steady_clock::now() >= deadline) {
                    // 已经到达或超过线程池缩容的截止时间
                    return PopResult::TIMEOUT;
                }
                // 如果是延时任务时间到期唤醒的，则继续循环，让 (2. 优先取到期的延时任务) 步骤处理
            }      
        }
    }

private:
    // 延时任务结构: 包含执行时间点和具体任务
    struct TimeTask {
        std::chrono::steady_clock::time_point exec_tm_;
        Task task_;
        TimeTask(std::chrono::steady_clock::time_point tm, Task&& t)
            : exec_tm_(tm), task_(std::move(t)) {}
        // 让时间最早的任务在堆的顶部
        bool operator<(const TimeTask& other) const { return exec_tm_ > other.exec_tm_; }
    };

    // 普通任务队列: 从 front 取任务
    // 普通任务在 back 添加任务；高优先级任务在 front 添加任务
    std::deque<Task> tasks_;
    // 延时任务队列: 按执行时间排序的小顶堆
    std::priority_queue<TimeTask> delay_tasks_;
    // 为 true 时表示不再接受新任务，但未完成的任务仍会被处理
    bool stop_ = false; 

    mutable std::mutex mtx_;
    std::condition_variable cond_;
};

#endif 