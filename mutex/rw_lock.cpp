

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
#include <cassert>

using namespace std::chrono;

// ============================================================
// 1) Reader-preference shared_mutex (may starve writers)
// ============================================================
namespace reader_pref {

class shared_mutex {
public:
    shared_mutex() = default;
    shared_mutex(const shared_mutex&) = delete;
    shared_mutex& operator=(const shared_mutex&) = delete;

    void lock() {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [&]{ return !has_writer_ && reader_cnt_ == 0; });
        has_writer_ = true;
    }
    bool try_lock() {
        std::unique_lock<std::mutex> lock(mtx_);
        if(!has_writer_ && reader_cnt_ == 0) {
            has_writer_ = true;
            return true;
        }
        return false;
    }
    void unlock() {
        std::unique_lock<std::mutex> lock(mtx_);
        has_writer_ = false;
        cond_.notify_all();
    }
    void lock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [&]{ return !has_writer_; });
        ++ reader_cnt_;
    }
    bool try_lock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        if(has_writer_) return false;
        ++ reader_cnt_;
        return true;
    }
    void unlock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        if( -- reader_cnt_ == 0) cond_.notify_all();
    }
private:
    std::mutex mtx_;
    std::condition_variable cond_;
    bool has_writer_{false};
    int reader_cnt_{0};
};

} // namespace reader_pref

// ============================================================
// 2) Writer-preference shared_mutex (avoid writer starvation)
// ============================================================
namespace writer_pref {

class shared_mutex {
public:
    shared_mutex() = default;
    shared_mutex(const shared_mutex&) = delete;
    shared_mutex& operator=(const shared_mutex&) = delete;

    void lock() {
        std::unique_lock<std::mutex> lock(mtx_);
        ++ writer_waiter_;
        cond_.wait(lock, [&]{ return !has_writer_ && reader_cnt_ == 0; });
        -- writer_waiter_;
        has_writer_ = true;
    }
    bool try_lock() {
        std::unique_lock<std::mutex> lock(mtx_);
        if(has_writer_ || reader_cnt_ != 0) return false;
        has_writer_ = true;
        return true;
    }
    void unlock() {
        std::unique_lock<std::mutex> lock(mtx_);
        has_writer_ = false;
        cond_.notify_all();
    }
    void lock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [&]{ return !has_writer_ && writer_waiter_ == 0; });
        ++ reader_cnt_;
    }
    bool try_lock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        if(has_writer_ || writer_waiter_ != 0) return false;
        ++ reader_cnt_;
        return true;
    }
    void unlock_shared() {
        std::unique_lock<std::mutex> lock(mtx_);
        if( -- reader_cnt_ == 0) cond_.notify_all();
    }
private:
    std::mutex mtx_;
    std::condition_variable cond_;
    bool has_writer_{false};
    int writer_waiter_{0};
    int reader_cnt_{0};
};

} // namespace writer_pref

// ============================================================
// 3) Fair FIFO shared_mutex (no starvation, queue-based)
// ============================================================
namespace fair_fifo {

#define COMPLEX_FIFO_SHARED_MUTEX

#if defined(SIMPLE_FIFO_SHARED_MUTEX)

// 一个简单的，基于 FIFO 思想的公平读写锁
class shared_mutex {
public:
    shared_mutex() = default;
    shared_mutex(const shared_mutex&) = delete;
    shared_mutex& operator=(const shared_mutex&) = delete;

    // ---- writer ----
    void lock() {
        std::unique_lock<std::mutex> lk(m_);
        q_.push_back(Mode::Write);
        cv_.wait(lk, [&] { return can_run_writer_(); });

        // consume head write request
        q_.pop_front();
        writer_active_ = true;
    }

    bool try_lock() {
        std::lock_guard<std::mutex> lk(m_);
        if (writer_active_ || active_readers_ != 0) return false;
        if (!q_.empty() || reader_batch_remaining_ != 0) return false; // no cutting
        writer_active_ = true;
        return true;
    }

    void unlock() {
        std::lock_guard<std::mutex> lk(m_);
        writer_active_ = false;
        cv_.notify_all();
    }

    // ---- reader ----
    void lock_shared() {
        std::unique_lock<std::mutex> lk(m_);
        q_.push_back(Mode::Read);

        // wait until we're in an opened reader batch
        cv_.wait(lk, [&] { return can_run_reader_(); });

        // enter as reader
        ++active_readers_;
        --reader_batch_remaining_;

        // if batch just got consumed, wake others (writers may become eligible later)
        if (reader_batch_remaining_ == 0) {
            cv_.notify_all();
        }
    }

    bool try_lock_shared() {
        std::lock_guard<std::mutex> lk(m_);
        if (writer_active_) return false;
        if (!q_.empty() || reader_batch_remaining_ != 0) return false; // no cutting
        ++active_readers_;
        return true;
    }

    void unlock_shared() {
        std::lock_guard<std::mutex> lk(m_);
        --active_readers_;
        if (active_readers_ == 0) {
            cv_.notify_all();
        }
    }

private:
    enum class Mode { Read, Write };

    // Open a reader batch if possible: head is Read, no active writer, no batch currently open.
    void maybe_open_reader_batch_() {
        if (reader_batch_remaining_ != 0) return;
        if (writer_active_) return;
        if (q_.empty() || q_.front() != Mode::Read) return;

        // Count head consecutive reads
        std::size_t k = 0;
        while (k < q_.size() && q_[k] == Mode::Read) ++k;

        // Remove those k read requests from the queue
        for (std::size_t i = 0; i < k; ++i) q_.pop_front();

        reader_batch_remaining_ = static_cast<int>(k);
    }

    bool can_run_writer_() {
        // Writers can run only when:
        // - no active writer
        // - no active readers
        // - no open reader batch
        // - head of queue is Write
        maybe_open_reader_batch_();
        return !writer_active_
            && active_readers_ == 0
            && reader_batch_remaining_ == 0
            && !q_.empty()
            && q_.front() == Mode::Write;
    }

    bool can_run_reader_() {
        // Readers can run only inside an opened reader batch
        maybe_open_reader_batch_();
        return !writer_active_ && reader_batch_remaining_ > 0;
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    std::deque<Mode> q_;

    int  active_readers_ = 0;
    bool writer_active_ = false;

    // how many readers are permitted in the current FIFO head batch
    int reader_batch_remaining_ = 0;
};
#else 

// 基于 FIFO 思想的读写锁
// 相较于上一份代码，在唤醒时不在 notify_all 广播，
// 而是给每个等待线程一个私有闸门（私有 cond），调度器只打开该打开的闸门
// 主要理解 lock() 和 lock_shared()

class shared_mutex {
public:
    shared_mutex() = default;
    shared_mutex(const shared_mutex&) = delete;
    shared_mutex& operator=(const shared_mutex&) = delete;

    // ==========================================
    // 独占锁 (Writer)
    // ==========================================
    void lock() {
        Waiter w; // 创建一个当前等待调度的对象
        std::unique_lock<std::mutex> lk(mtx_);

        const std::uint64_t my_ticket = next_ticket_++;
        q_.push_back(Node{Mode::Write, my_ticket, &w});

        // 入队后尝试触发调度（尤其是队列原本为空时）
        wake_next_();

        // 仅在被“精准唤醒”后继续
        w.cv.wait(lk, [&]{ return w.go; });

        // 断言：当前唤醒节点为对头写者节点
        assert(!q_.empty() && q_.front().mode == Mode::Write);
        assert(q_.front().ticket == my_ticket);

        // 写者获取锁：自己出队
        q_.pop_front();
        has_writer_ = true;
    }

    bool try_lock() {
        std::unique_lock<std::mutex> lk(mtx_);

        // 没有正在进行的写者
        // 没有正在进行的或等待进行的读者
        // 不能插队
        if (has_writer_) return false;
        if (reader_cnt_ != 0 || pending_readers_ != 0) return false;
        if (!q_.empty()) return false;             // 严格公平：有人排队就不能抢

        has_writer_ = true;
        return true;
    }

    void unlock() {
        std::unique_lock<std::mutex> lk(mtx_);
        has_writer_ = false;
        wake_next_();
    }

    // ==========================================
    // 共享锁 (Reader)
    // ==========================================
    void lock_shared() {
        Waiter w;
        std::unique_lock<std::mutex> lk(mtx_);

        const std::uint64_t my_ticket = next_ticket_++;
        q_.push_back(Node{Mode::Read, my_ticket, &w});

        // 入队后尝试触发调度（尤其是队列原本为空时）
        wake_next_();

        // 等待被批次调度“精准唤醒”
        w.cv.wait(lk, [&]{ return w.go; });

        // 注意读者无法在这里断言，因为节点 node 已经在批量出中被处理了
        // assert(!q_.empty() && q_.front().mode == Mode::Read);
        // assert(q_.front().ticket == my_ticket);

        // 读者正式入场
        ++ reader_cnt_;
        -- pending_readers_;

        // 不需要 wake_next_：因为只要 reader_cnt_ > 0 写者就不能跑
    }

    bool try_lock_shared() {
        std::unique_lock<std::mutex> lk(mtx_);

        // 不能有正在写的线程
        // 不能有未完成的读
        // 不能插队
        if (has_writer_) return false;
        if (pending_readers_ != 0) return false;   // 有批次在“入场中”，视为忙
        if (!q_.empty()) return false;             // 严格公平：有人排队就不能抢

        ++ reader_cnt_;
        return true;
    }

    void unlock_shared() {
        std::unique_lock<std::mutex> lk(mtx_);
        if (--reader_cnt_ == 0) {
            wake_next_(); // 若仍有 pending_readers_，wake_next_ 会自动不放行写者
        }
    }

private:
    enum class Mode { Read, Write };

    struct Waiter {
        std::condition_variable cv; // 通知信号：唤醒线程
        bool go{false}; // 状态位：线程醒来后检查自己是不是真的被允许通过
                        // 可以防止伪唤醒和“先 notify，后 wait 导致通知信号丢失的情况”
    };

    struct Node {
        Mode mode;
        std::uint64_t ticket; 
        Waiter* waiter;       // Read/Write 都用私有 waiter；try_lock* 不入队则无 waiter
    };

    // 注意：以下 *_unsafe_ 仅在已持有 mtx_ 时调用
    bool can_run_writer_unsafe_() const {
        // 没有正在进行的写
        // 没有正在进行或等待进行的读
        // 可以写
        return !has_writer_
            && reader_cnt_ == 0 && pending_readers_ == 0
            && !q_.empty() && q_.front().mode == Mode::Write;
    }

    // 开启读批次：弹出队头连续 Read，并仅唤醒这一批读者（无 notify_all 惊群）
    void open_read_batch_and_wake_unsafe_() {
        std::vector<Waiter*> to_wake;
        to_wake.reserve(32);

        std::size_t count = 0;
        while (!q_.empty() && q_.front().mode == Mode::Read) {
            Waiter* w = q_.front().waiter;
            q_.pop_front();
            if (w) {
                to_wake.push_back(w);
            }
            ++ count;
        }

        // 标记：已批准但尚未入场的读者数
        pending_readers_ = count;

        // 精准唤醒该批次的读者（每个读者一个 notify_one）
        for (Waiter* w : to_wake) {
            w->go = true;
            w->cv.notify_one();
        }
    }

    // 核心调度器：所有调用都在持有 mtx_ 的情况下进行
    void wake_next_() {
        // 1) 有人在持锁：不调度
        if (has_writer_ || reader_cnt_ != 0) return;

        // 2) 读批次“入场中”：不调度写者/下一批
        if (pending_readers_ != 0) return;

        // 3) 队列空：结束
        if (q_.empty()) return;

        // 4) 按队头调度
        if (q_.front().mode == Mode::Write) {
            // 精准唤醒队头写者（无惊群）
            Waiter* w = q_.front().waiter;
            if (w) {
                w->go = true;
                w->cv.notify_one();
            }
        } else {
            // 开启读批次并精准唤醒该批次读者（无 notify_all 惊群）
            open_read_batch_and_wake_unsafe_();
        }
    }

private:
    std::mutex mtx_;
    std::deque<Node> q_;

    bool has_writer_{false};
    std::size_t reader_cnt_{0};      // 已经入场的读者
    std::size_t pending_readers_{0}; // 已经批准（被唤醒）但尚未完成“入场”的读者数量
    
    std::uint64_t next_ticket_{0};   // 等待线程的票号
                                     // ticket 在这里无实际价值，它仅用于辅助用途
                                     //     1. 调试/日志/可观察性：打印 ticket 能查看入队/出队/排队情况
                                     //     2. 防御性校验：等待线程醒来后断言“我真的是对头那个节点对应的线程”，可以在开发期更早的发现 bug
};

#endif
} // namespace fair_fifo


// ============================================================
// Choose ONE namespace here by changing #define
// ============================================================

// #define READER_PREF  
// #define WRITER_PREF
#define FAIR_FIFO 


#if defined(FAIR_FIFO)
#  define RWLOCK_IMPL_NAME "fair_fifo"
using rw_mutex = fair_fifo::shared_mutex;

#elif defined(WRITER_PREF)
#  define RWLOCK_IMPL_NAME "writer_pref"
using rw_mutex = writer_pref::shared_mutex;

#elif defined(READER_PREF)
#  define RWLOCK_IMPL_NAME "reader_pref"
using rw_mutex = reader_pref::shared_mutex;

#endif

// ============================================================
// Test harness
// ============================================================
struct LatStats {
    double avg_us = 0;
    double p95_us = 0;
    double p99_us = 0;
    double max_us = 0;
    size_t n = 0;
};

static LatStats compute_lat(std::vector<double>& us) {
    LatStats s;
    s.n = us.size();
    if (us.empty()) return s;
    std::sort(us.begin(), us.end());
    s.avg_us = std::accumulate(us.begin(), us.end(), 0.0) / us.size();
    auto idx = [&](double p){
        size_t i = (size_t)std::floor(p * (us.size() - 1));
        return us[i];
    };
    s.p95_us = idx(0.95);
    s.p99_us = idx(0.99);
    s.max_us = us.back();
    return s;
}

static void busy_work(int iters) {
    // tiny CPU work to simulate read/write section cost
    // volatile prevents optimizing away too aggressively
    volatile uint64_t x = 0x12345678ULL;
    for (int i = 0; i < iters; ++i) x = x * 1103515245ULL + 12345ULL;
    (void)x;
}

int main(int argc, char** argv) {
    // Parameters tuned to make differences visible.
    // You can override by command line:
    // ./a.out [seconds=5] [readers=12] [writers=2]
    int seconds = (argc > 1) ? std::stoi(argv[1]) : 5;
    int readers = (argc > 2) ? std::stoi(argv[2]) : 12;
    int writers = (argc > 3) ? std::stoi(argv[3]) : 2;

    std::cout << "Testing namespace: " << RWLOCK_IMPL_NAME << std::endl;
    std::cout << "Duration: " << seconds << "s, readers=" << readers << ", writers=" << writers << "\n\n";

    rw_mutex m;
    std::atomic<bool> stop{false};

    // Shared state under the lock
    alignas(64) std::uint64_t shared_value = 0;

    // Metrics
    std::atomic<uint64_t> read_ops{0}, write_ops{0};
    std::atomic<uint64_t> read_fails{0}, write_fails{0};

    // Per-writer latency samples (time spent waiting to acquire exclusive lock)
    std::vector<std::vector<double>> writer_wait_us(writers);

    // start barrier: all threads begin together to amplify contention
    std::barrier start_bar(readers + writers + 1);

    // Readers: tight loop to create continuous read pressure (exposes writer starvation in reader_pref)
    std::vector<std::thread> ts;
    ts.reserve(readers + writers);

    for (int i = 0; i < readers; ++i) {
        ts.emplace_back([&, i] {
            std::mt19937_64 rng(0xBADC0FFEEULL + i);
            start_bar.arrive_and_wait();
            while (!stop.load(std::memory_order_relaxed)) {
                m.lock_shared();
                // simulate read critical section
                auto v = shared_value;
                (void)v;
                busy_work(80); // adjust to tune read-hold time
                m.unlock_shared();

                read_ops.fetch_add(1, std::memory_order_relaxed);

                // small random pause -> creates churn and new arrivals
                if ((rng() & 0xFF) == 0) std::this_thread::yield();
            }
        });
    }

    // Writers: attempt frequent writes; measure time-to-acquire
    for (int wi = 0; wi < writers; ++wi) {
        ts.emplace_back([&, wi] {
            start_bar.arrive_and_wait();
            writer_wait_us[wi].reserve(200000);
            std::mt19937_64 rng(0xC0FFEEULL + wi);

            while (!stop.load(std::memory_order_relaxed)) {
                // Pace writers a bit so they repeatedly collide with reads
                // (too fast -> they dominate; too slow -> not enough samples)
                if ((rng() % 8) == 0) std::this_thread::sleep_for(100us);

                auto t0 = steady_clock::now();
                m.lock();
                auto t1 = steady_clock::now();

                double us = duration<double, std::micro>(t1 - t0).count();
                writer_wait_us[wi].push_back(us);

                // simulate write critical section
                ++shared_value;
                busy_work(200); // adjust to tune write-hold time

                m.unlock();
                write_ops.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    start_bar.arrive_and_wait();

    // run
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    stop.store(true, std::memory_order_relaxed);

    for (auto& t : ts) t.join();

    // Aggregate writer latencies
    std::vector<double> all;
    size_t total_samples = 0;
    for (auto& v : writer_wait_us) total_samples += v.size();
    all.reserve(total_samples);
    for (auto& v : writer_wait_us) all.insert(all.end(), v.begin(), v.end());

    auto stats = compute_lat(all);

    double rps = (double)read_ops.load() / seconds;
    double wps = (double)write_ops.load() / seconds;

    std::cout << "Ops:\n";
    std::cout << "  reads : " << read_ops.load()  << " (" << std::fixed << std::setprecision(1) << rps << "/s)\n";
    std::cout << "  writes: " << write_ops.load() << " (" << std::fixed << std::setprecision(1) << wps << "/s)\n\n";

    std::cout << "Writer wait (exclusive lock acquire latency):\n";
    std::cout << "  samples: " << stats.n << "\n";
    std::cout << "  avg  : " << std::fixed << std::setprecision(2) << stats.avg_us << " us\n";
    std::cout << "  p95  : " << stats.p95_us << " us\n";
    std::cout << "  p99  : " << stats.p99_us << " us\n";
    std::cout << "  max  : " << stats.max_us << " us\n\n";

    std::cout << "Interpretation tips:\n";
    std::cout << "  - reader_pref: reads/s usually highest, but writer wait max/p99 can explode (writer starvation).\n";
    std::cout << "  - writer_pref: writer waits stay bounded, but reads/s may drop under sustained writers.\n";
    std::cout << "  - fair_fifo  : waits are stable (no starvation), throughput often between the two.\n";
}
