// 实现一个可重入锁
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <vector>

class RecursiveLock {
public:
    RecursiveLock() = default;
    RecursiveLock(const RecursiveLock&) = delete;
    RecursiveLock& operator=(const RecursiveLock&) = delete;

    void lock() {
        std::thread::id this_id = std::this_thread::get_id();
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [&]{ return cnt_ == 0 || owner_ == this_id; });
        if(cnt_ == 0) owner_ = this_id;
        ++ cnt_;
    }

    bool try_lock() {
        std::thread::id this_id = std::this_thread::get_id();
        std::unique_lock<std::mutex> lock(mtx_, std::try_to_lock);

        // 没拿到锁就不能访问 owner_ 和 cnt_
        // 也就无法判断当前锁是被它人占用还是被自己占用还是没被占用
        if(!lock.owns_lock()) return false;
        
        // 没被占用
        if(cnt_ == 0) {
            owner_ = this_id;
            cnt_ = 1;
            return true;
        }
        // 被自己占用
        if(owner_ == this_id) {
            ++ cnt_;
            return true;
        }
        // 被别人占用
        return false;
    }

    void unlock() {
        std::thread::id this_id = std::this_thread::get_id();
        std::unique_lock<std::mutex> lock(mtx_);
        if(this_id != owner_) {
            throw std::runtime_error("unlock by another thread");
        }
        if( -- cnt_ == 0) {
            owner_ = std::thread::id{};
            // 这里我们推荐先 unlock 再 notify
            // 考虑下面这种情况：T1 正在 unlock，T2 卡在 lock 的 cond_.wait（此时 T2 已经释放了锁）
            // T1 进入 unlock，拿到 mtx_，将 cnt_ 减到 0，设置 owner_ 为空
            // 此时如果我们不 unlock(mtx_)，直接 notify，T2 可能立刻被唤醒
            // 但 T2 被唤醒后会立刻去抢 mtx_，而此时 mtx_ 还在 T1 手里（因为 T1 还没退出作用域释放 unique_lock）
            // 结果就是：T2 醒了，但智能卡在抢 mtx_ 上，白白发生一次线程唤醒/调度开销
            // 而如果我们先 lock.unlock()，再 notify_one()
            // T2 被唤醒时，mtx_ 已经空了
            // T2 更可能“醒来就拿到 mtx_”，马上检查谓词并成功获取逻辑锁
            lock.unlock();
            cond_.notify_one();
        }
    }

private:
    std::condition_variable cond_;
    std::mutex mtx_; // mtx_ 用来保护 owner_ 和 cnt_
    std::thread::id owner_{};
    uint64_t cnt_{0}; // 暂时不考虑 cnt_ 溢出的问题
};

RecursiveLock mtx_;
int value_ = 0;

void recursive_increment(int depth, int max_depth, int thread_id) {
    std::lock_guard<RecursiveLock> lock(mtx_);
    std::cout << "[thread " << thread_id 
            << "] depth=" << depth 
            << " value=" << value_ << std::endl;
    
    ++ value_;
    // 模拟一点工作量
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    if(depth < max_depth) {
        recursive_increment(depth + 1, max_depth, thread_id);
    }
}

void worker(int id) {
    for(int i = 0; i < 3; i ++ ) {
        recursive_increment(1, 3, id);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// 测试 RecursiveLock::try_lock：尝试在后台抢锁
void try_lock_test() {
    for(int i = 0; i < 10; i ++ ) {
        if(mtx_.try_lock()) {
            std::cout << "[try_lock_test " << i + 1 << "] got lock, value=" << value_ << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            mtx_.unlock();
        }
        else {
            std::cout << "[try_lock_test " << i + 1 << "] lock busy, retry..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
}

int main()
{
    std::cout << __cplusplus << std::endl;

    const int N = 3;
    std::vector<std::thread> threads;
    for(int i = 0; i < N; i ++ ) {
        threads.emplace_back(worker, i);
    }
    std::thread t_try(try_lock_test);

    for(auto &t : threads) {
        if(t.joinable())    t.join();
    }
    t_try.join();

    // 理论值：N 个线程  * 每个线程 3 词调用 * 每次递归深度 3
    // => N * 3 * 3
    std::cout << "final value = " << value_ << std::endl;
    std::cout << "expected value = " << N * 3 * 3 << std::endl;

    return 0;
}