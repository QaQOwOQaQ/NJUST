#include <iostream>
#include <vector>
#include <chrono>
#include <atomic>
#include <cassert>
#include <string>
#include <sstream>
#include <stdexcept>

#include "tmp.hpp" 

// 简单的测试辅助宏
#define TEST_CASE(name) \
    std::cout << "-------------------------------------------------------" << std::endl; \
    std::cout << "[TEST] " << name << " running..." << std::endl;

#define ASSERT_TRUE(condition, msg) \
    if (!(condition)) { \
        std::cerr << "[FAIL] " << msg << " at line " << __LINE__ << std::endl; \
        std::exit(1); \
    } else { \
        std::cout << "[PASS] " << msg << std::endl; \
    }

// 模拟耗时任务
void heavy_task(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// 一个简单的“等待直到条件成立或超时”的工具函数
template<typename Pred>
bool wait_until(std::chrono::milliseconds timeout, Pred pred, std::chrono::milliseconds step = std::chrono::milliseconds(20)) {
    auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (pred()) return true;
        std::this_thread::sleep_for(step);
    }
    return pred();
}

// =========================================================================
// 1. 基础功能测试：返回值与 Future
// =========================================================================
void test_basic_future() {
    TEST_CASE("Basic Future & Return Value");
    
    ThreadPool pool(2, 4, 1);
    
    auto f1 = pool.add_future_task([](int a, int b) {
        return a + b;
    }, 10, 20);

    // 这里显式构造 std::string
    auto f2 = pool.add_future_task([] {
        return std::string("Hello ThreadPool");
        // 或者加 trailing return:
        // []() -> std::string { return "Hello ThreadPool"; }
    });

    int v1 = f1.get();
    std::string v2 = f2.get();

    ASSERT_TRUE(v1 == 30, "Future<int> should return 30");
    ASSERT_TRUE(v2 == std::string("Hello ThreadPool"),
                "Future<string> should return correct string");

    pool.stop();
}


// =========================================================================
// 2. 并发压测：数据竞争检查
//    这里利用原子变量和 ThreadPool::stop 语义：
//    - TaskQueue::pop 在 stop 后仍会把队列里剩余任务全部返回，直到队列清空
// =========================================================================
void test_concurrency() {
    TEST_CASE("Concurrency & Atomic Integrity");
    
    ThreadPool pool(4, 8, 1);
    std::atomic<int> counter{0};
    const int task_count = 1000;

    for (int i = 0; i < task_count; ++i) {
        pool.add_task([&counter] {
            counter++; // 原子操作
            int x = 0; 
            for(int j = 0; j < 1000; ++j) x += j;
        });
    }

    // 直接 stop() 即可：
    // - 根据 TaskQueue::stop + pop 的语义，队列中所有任务会被执行完，
    //   worker_loop 在处理完所有任务后返回，并被 stop() join 掉。
    pool.stop();

    ASSERT_TRUE(counter.load() == task_count, "All 1000 tasks executed correctly");
}

// =========================================================================
// 3. 优先级测试：验证 deque 头插 / 尾插的行为
// =========================================================================
void test_priority() {
    TEST_CASE("Priority Task Queue");

    // 只用 1 个线程，方便观察顺序
    ThreadPool pool(1, 1, 1);
    std::vector<int> results;
    std::mutex res_mutex;

    // 1. 先扔一个耗时任务占住线程，让后面的任务在队列积压
    pool.add_task([]{
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    });

    // 2. 扔几个普通任务（deque 尾插）
    pool.add_task([&]{ 
        std::lock_guard<std::mutex> lk(res_mutex); 
        results.push_back(1); 
    });
    pool.add_task([&]{ 
        std::lock_guard<std::mutex> lk(res_mutex); 
        results.push_back(2); 
    });

    // 3. 扔一个高优先级任务（deque 头插）
    pool.add_priority_task([&]{ 
        std::lock_guard<std::mutex> lk(res_mutex); 
        results.push_back(999); 
    });

    pool.stop(); // 等待执行完

    // 预期顺序：999 (插队), 1, 2
    ASSERT_TRUE(results.size() == 3, "All tasks executed");
    ASSERT_TRUE(results[0] == 999, "Priority task should execute first (deque front)");
    ASSERT_TRUE(results[1] == 1, "Normal task 1 executes after priority");
    ASSERT_TRUE(results[2] == 2, "Normal task 2 executes last");
}

// =========================================================================
// 4. 延时任务测试：测试“不提前执行”，并结合 TaskQueue 的时间逻辑
// =========================================================================
void test_delay_task() {
    TEST_CASE("Delay Task Accuracy");

    ThreadPool pool(2, 2, 1);
    
    auto start = std::chrono::steady_clock::now();
    int delay_ms = 500;
    std::atomic<bool> executed{false};

    pool.add_delay_task(delay_ms, [start, delay_ms, &executed] {
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "  -> Delay task executed after " << duration << "ms" << std::endl;
        
        // 允许少许误差，比如系统调度可能稍微慢一点，但绝不能明显早于 delay_ms
        ASSERT_TRUE(duration >= delay_ms - 5, "Delay task executed too early!"); 
        executed = true;
    });

    // 等到任务执行完或 2 秒超时（足够覆盖 500ms 延迟）
    bool ok = wait_until(std::chrono::milliseconds(2000), [&] { return executed.load(); });
    ASSERT_TRUE(ok, "Delay task should eventually execute");

    pool.stop();
}

// =========================================================================
// 4.1 延时任务 + idle_timeout 交互：
//     延时任务时间 > idle_timeout，验证：
//     - pop() 可能会返回 TIMEOUT（用于缩容），但对于 min_threads 来说不会被裁掉
//     - 延时任务最终仍会被执行
// =========================================================================
void test_delay_vs_idle_timeout() {
    TEST_CASE("Delay Task with Idle Timeout Interaction");

    int min_threads = 1;
    int max_threads = 3;
    int idle_timeout_sec = 1;
    ThreadPool pool(min_threads, max_threads, idle_timeout_sec);

    std::atomic<bool> executed{false};

    // 延时 1500ms，大于 idle_timeout 1s
    auto start = std::chrono::steady_clock::now();
    int delay_ms = 1500;

    pool.add_delay_task(delay_ms, [&] {
        executed = true;
        auto end = std::chrono::steady_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "  -> Long delay task executed after " << dur << "ms" << std::endl;
        ASSERT_TRUE(dur >= delay_ms - 5, "Long delay task executed too early!");
    });

    // 尽管 idle_timeout 会触发一些 worker TIMEOUT，
    // 但 min_threads 个 worker 不会被缩容，最终仍会执行延时任务。
    bool ok = wait_until(std::chrono::milliseconds(4000), [&] { return executed.load(); });
    ASSERT_TRUE(ok, "Long delay task should be executed despite idle_timeout");

    pool.stop();
}

// =========================================================================
// 5. 核心测试：动态扩容与缩容 (Elasticity)
//    使用“轮询+超时”替代固定 sleep，避免受调度抖动影响
// =========================================================================
void test_elasticity() {
    TEST_CASE("Dynamic Expansion & Shrinking");

    // 配置：最小2，最大10，空闲超时 1秒
    int min_t = 2;
    int max_t = 10;
    int idle_s = 1;
    ThreadPool pool(min_t, max_t, idle_s);

    std::cout << "  [Initial] Active threads: " << pool.active_threads_count() << std::endl;
    ASSERT_TRUE(pool.active_threads_count() == min_t, "Should start with min_threads");

    // --- Phase 1: 扩容测试 ---
    std::cout << "  [Expansion] Submitting 20 blocking tasks..." << std::endl;
    std::atomic<int> running_tasks{0};
    
    for(int i = 0; i < 20; ++i) {
        pool.add_task([&running_tasks] {
            running_tasks++;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            running_tasks--;
        });
    }

    // 给线程池一些时间反应，通过轮询等待扩容到 max_threads
    bool expanded = wait_until(std::chrono::milliseconds(2000), [&] {
        return pool.active_threads_count() == max_t;
    });

    int active = pool.active_threads_count();
    std::cout << "  [Expansion] Active threads now: " << active << std::endl;
    ASSERT_TRUE(expanded, "Should expand to max_threads under load");

    // --- Phase 2: 缩容测试 ---
    std::cout << "  [Shrinking] Waiting for tasks to finish and idle timeout..." << std::endl;

    // 等待所有任务跑完（大约 500ms），然后再等待线程因 idle_timeout 被回收
    bool shrunk = wait_until(std::chrono::milliseconds(4000), [&] {
        return pool.active_threads_count() == min_t;
    });

    active = pool.active_threads_count();
    std::cout << "  [Shrinking] Active threads now: " << active << std::endl;
    ASSERT_TRUE(shrunk, "Should shrink back to min_threads after idle timeout");

    pool.stop();
}

// =========================================================================
// 6. 批量提交测试
// =========================================================================
void test_batch() {
    TEST_CASE("Batch Submission");
    ThreadPool pool(4);
    std::vector<std::function<void()>> tasks;
    std::atomic<int> sum{0};

    for(int i = 0; i < 100; ++i) {
        tasks.emplace_back([&sum]{ sum++; });
    }

    pool.add_batch_task(std::move(tasks));
    pool.stop();

    ASSERT_TRUE(sum == 100, "Batch tasks all executed");
}

// =========================================================================
// 7. stop 语义测试：
//    - stop() 之后队列现有的普通任务和延时任务必须执行完
//    - stop() 之后再 add_task/add_future_task 应抛异常（ThreadPool 层控制）
// =========================================================================
void test_stop_semantics() {
    TEST_CASE("Stop Semantics with Normal & Delay Tasks");

    ThreadPool pool(2, 4, 1);
    std::atomic<int> normal_count{0};
    std::atomic<int> delay_count{0};

    // 提交普通任务
    for (int i = 0; i < 10; ++i) {
        pool.add_task([&normal_count] {
            normal_count++;
        });
    }

    // 提交延时任务
    for (int i = 0; i < 5; ++i) {
        pool.add_delay_task(200, [&delay_count] {
            delay_count++;
        });
    }

    // 调用 stop(): 根据 TaskQueue 的设计，
    // - stop_ 置位
    // - pop() 仍会把已有任务（包括延时任务）全部返回执行
    // - 当两类队列都空时，pop() 才返回 STOPPED，worker_loop 退出
    pool.stop();

    ASSERT_TRUE(normal_count == 10, "All normal tasks finished before stop returned");
    ASSERT_TRUE(delay_count == 5, "All delay tasks finished before stop returned");

    // 再验证 stop 之后无法再提交任务（ThreadPool 层抛异常）
    bool caught1 = false, caught2 = false;
    try {
        pool.add_task([]{});
    } catch (const std::runtime_error&) {
        caught1 = true;
    }

    try {
        auto f = pool.add_future_task([]{ return 42; });
    } catch (const std::runtime_error&) {
        caught2 = true;
    }

    ASSERT_TRUE(caught1, "add_task after stop should throw");
    ASSERT_TRUE(caught2, "add_future_task after stop should throw");
}

int main() {
    std::cout << "=======================================" << std::endl;
    std::cout << "   ThreadPool v4 Complete Test Suite   " << std::endl;
    std::cout << "=======================================" << std::endl;

    test_basic_future();
    test_concurrency();
    test_priority();
    test_delay_task();
    test_delay_vs_idle_timeout();
    test_batch();
    test_elasticity();
    test_stop_semantics();

    std::cout << "=======================================" << std::endl;
    std::cout << "   ALL TESTS PASSED SUCCESSFULLY       " << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "DEBUG BY JYYYYX" << std::endl;
    return 0;
}
