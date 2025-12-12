#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <numeric>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <barrier>

class shared_mutex {
public:
    shared_mutex() = default;
    shared_mutex(const shared_mutex&) = delete;
    shared_mutex& operator=(const shared_mutex&) = delete;

    // ==========================================
    // 独占锁 (Writer)
    // ==========================================
    void lock() {
        Waiter w;
        std::unique_lock<std::mutex> lk(mtx_);

        const std::uint64_t my_ticket = next_ticket_ ++ ;
        q_.push_back(Node{Mode::Write, my_ticket, &w});

        // 入队后触发调度
        wake_next_();

        // 仅在被精准唤醒后继续
        w.cond_.wait(lk, [&]{ return w.go; });

        // 防御性：处理伪唤醒/异常情况
        while(!(can_run_writer_unsafe_() && 
                !q_.empty() && 
                q_.front().mode == Mode::Write && 
                q_.front().ticket == my_ticket)) {
            w.go = false;
            w.cond_.wait(lk, [&]{ return w.go; });
        }

        q_.pop_front();
        has_writer_ = true;
    }

    bool try_lock() {
        std::unique_lock<std::mutex> lk(mtx_);
        // 没有正在进行的写
        // 没有正在进行的读
        // 没有正在等待的线程（先进先出，不允许插队）
        if(has_writer_) return false;
        if(reader_cnt_ != 0 || pending_readers_ != 0)    return false;
        if(!q_.empty())  return false;

        has_writer_ = true;
        return true;
    }

    void unlock() {
        std::unique_lock<std::mutex> lk(mtx_);
        has_writer_ = true;
        wake_next_();
    }

    // ==========================================
    // 共享锁 (Reader)
    // ==========================================
    void lock_shared() {
        Waiter w;
        std::unique_lock<std::mutex> lk(mtx_);

        const std::uint64_t my_ticket = next_ticket_ ++ ;
        q_.push_back(Node{Mode::Read, my_ticket, &w});

        // 入队后尝试触发调度
        wake_next_();
        w.cond_.wait(lk, [&]{ return w.go; });
        
        // 读者正式入场
        ++ reader_cnt_;
        // 阻止在读者真正入场前放行写者
        if(pending_readers_ > 0) {
            -- pending_readers_;
        }
        // 不需要 wake_next_：只要 reader_cnt_ > 0 写者就不能被调度
    }

    bool try_lock_shared() {
        std::unique_lock<std::mutex> lk(mtx_);
        // 没有正在进行的写
        // 没有正在等待的读
        // 没有正在等待的线程（先进先出，不允许插队）
        if(has_writer_) return false;
        if(pending_readers_ != 0)   return false;
        if(!q_.empty()) return false;

        ++ reader_cnt_;
        return true;
    }

    void unlock_shared() {
        std::unique_lock<std::mutex> lk(mtx_);
        if( -- reader_cnt_ == 0) {
            wake_next_(); // 若仍有 pending_readers_，wake_next_ 会自动不放行写者
        }
    }


private:
    enum class Mode { Read, Write };

    struct Waiter {
        std::condition_variable cond_;
        bool go{false};
    };

    struct Node {
        Mode mode;
        std::uint64_t ticket; // 可用于调试/断言；公平性主要靠队列顺序
        Waiter* waiter;        //  Read/Write 都用私有 waiter；try_lock* 不入队则无 waiter
    };

    [[nodiscard]] bool can_run_writer_unsafe_() const {
        // 没有正在进行的写
        // 没有正在进行的读和等待进行的读
        // 队列头有可进行的写线程
        return !has_writer_ && 
                reader_cnt_ == 0 && 
                pending_readers_ == 0 && 
                !q_.empty() && q_.front().mode == Mode::Write;
    }


    // 开启读批次：淡出队头的连续 Read，并仅唤醒这一批读者（无 notify_all 惊群）
    void open_read_batch_and_wake_unsafe_() {
        std::vector<Waiter*> to_wake;
        to_wake.reserve(32);

        std::size_t count = 0;
        while(!q_.empty() && q_.front().mode == Mode::Read) {
            Waiter* w = q_.front().waiter;
            q_.pop_front();
            if(w) {
                to_wake.push_back(w);
            }
            ++ count;
        }

        pending_readers_ = count;

        // 精准唤醒该批次的读者（每个读者一个 notify_one）
        for(Waiter* w : to_wake) {
            w->go = true;
            w->cond_.notify_one();
        }
    }

    // 核心调度器：所有调用都在持有 mtx_ 的情况下进行
    void wake_next_() {
        // 1) 有人在持有锁：不调度
        if(has_writer_ || reader_cnt_ != 0) return ;

        // 2) 读批次“入场中”：不调度写者/下一批
        if(pending_readers_ != 0)   return ;

        // 3) 队列空：结束
        if(q_.empty())  return ;

        // 4) 按队列头调度
        if(q_.front().mode == Mode::Write) {
            // 精准唤醒对头写者（无惊群）
            Waiter* w = q_.front().waiter;
            if(w) {
                w->go = true;
                w->cond_.notify_one();
            }
        }
        else {
            // 开启读批次并精准唤醒该批次读者（无 notify_all 惊群）
        }
    }

private:
    std::mutex mtx_;
    std::deque<Node> q_;
    
    bool has_writer_{false};
    std::size_t reader_cnt_{0};
    std::size_t pending_readers_{0}; // 已经批准（被唤醒）但尚未完成“入场”的读者数量
    std::uint64_t next_ticket_{0}; // 暂时不考虑溢出问题
};