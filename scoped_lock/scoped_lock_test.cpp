// scoped_lock_test.cpp
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

#include "scoped_lock.hpp"  // 这里换成你自己的 ScopedLock 头文件路径

// ========== 测试 1：单 mutex 保护计数器 ==========
TEST(ScopedLockTest, SingleMutexIncrements) {
    std::mutex m;
    int counter = 0;
    const int thread_count = 8;
    const int per_thread = 10000; // 可根据机器性能调大

    auto worker = [&]() {
        for (int i = 0; i < per_thread; ++i) {
            ScopedLock lock(m);  // 单 mutex 版本
            ++counter;
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(worker);
    }
    for (auto& t : threads) t.join();

    int expected = thread_count * per_thread;
    EXPECT_EQ(counter, expected);
}

// ========== 测试 2：两个 mutex，转账模型，检查无死锁 & 数据不丢失 ==========
TEST(ScopedLockTest, TwoMutexTransferKeepsTotal) {
    std::mutex m1, m2;
    int account1 = 100000;
    int account2 = 200000;

    const int thread_count = 8;
    const int per_thread_ops = 20000;

    auto transfer_1_to_2 = [&]() {
        for (int i = 0; i < per_thread_ops; ++i) {
            ScopedLock lock(m1, m2); // 多 mutex 版本
            if (account1 > 0) {
                --account1;
                ++account2;
            }
        }
    };

    auto transfer_2_to_1 = [&]() {
        for (int i = 0; i < per_thread_ops; ++i) {
            ScopedLock lock(m1, m2);
            if (account2 > 0) {
                --account2;
                ++account1;
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (int i = 0; i < thread_count / 2; ++i) {
        threads.emplace_back(transfer_1_to_2);
        threads.emplace_back(transfer_2_to_1);
    }

    for (auto& t : threads) t.join();

    int sum = account1 + account2;
    EXPECT_EQ(sum, 300000); // 初始总额 100000 + 200000
}

// ========== 测试 3：adopt_lock 构造 ==========
TEST(ScopedLockTest, AdoptLockTakesOwnership) {
    std::mutex m1, m2;
    int x = 0;
    int y = 0;

    // 先手动 lock，再用 ScopedLock(adopt_lock) 接管
    std::lock(m1, m2);
    {
        ScopedLock<std::mutex, std::mutex> guard(std::adopt_lock, m1, m2);
        x = 42;
        y = 24;
        // guard 析构时，会 unlock m1, m2
    }

    // 再次加锁，如果 ScopedLock 正确解锁了，这里应该不会死锁
    {
        ScopedLock lock(m1, m2);
        EXPECT_EQ(x, 42);
        EXPECT_EQ(y, 24);
    }
}

// ========== 测试 4：零 mutex 特化 ==========
TEST(ScopedLockTest, ZeroMutexSpecialization) {
    // 构造/析构不应该出问题，只要能编译通过就算 OK
    ScopedLock<> lock;
    ScopedLock<> lock2(std::adopt_lock);
    SUCCEED();
}

// ========== 测试 5：多 mutex 并发压力测试 ==========
TEST(ScopedLockTest, MultiMutexStressKeepsSum) {
    const int mutex_count = 4;
    std::mutex m[mutex_count];
    int data[mutex_count] = {0, 0, 0, 0};

    const int thread_count = 16;
    const int per_thread_ops = 20000;

    auto worker = [&](int id) {
        for (int i = 0; i < per_thread_ops; ++i) {
            int a = id % mutex_count;
            int b = (id + 1) % mutex_count;

            ScopedLock lock(m[a], m[b]);
            ++data[a];
            --data[b];
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(worker, i);
    }
    for (auto& t : threads) t.join();

    int sum = 0;
    for (int i = 0; i < mutex_count; ++i) {
        sum += data[i];
    }
    // 每次操作 +1 和 -1，不改变总和
    EXPECT_EQ(sum, 0);
}

TEST(FUCK_YOU_TEST, FUCK_1)
{
    int x = 1;
    EXPECT_EQ(x, 1);
}

// ========== GoogleTest main ==========


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
