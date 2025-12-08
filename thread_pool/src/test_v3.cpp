#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <future>

#include "v3.hpp"

// 一个普通的函数，用于测试非 Lambda 传参
int multiply(int a, int b) {
    // 模拟耗时操作
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return a * b;
}

int main() {
    // 1. 初始化线程池，开启 4 个工作线程
    ThreadPool pool(4);
    std::cout << "=== 线程池初始化完成 (4 线程) ===" << std::endl;

    // ==========================================
    // 场景一：批量并行计算 (Lambda + 返回值)
    // ==========================================
    std::cout << "\n[场景 1] 提交 8 个计算平方的任务..." << std::endl;
    std::vector<std::future<int>> results;

    for(int i = 0; i < 8; ++i) {
        results.emplace_back(
            pool.push_task([i] {
                return i * i;
            })
        );
    }

    // 获取结果
    std::cout << "计算结果: ";
    for(auto && result: results)
        std::cout << result.get() << " "; 
    std::cout << std::endl;


    // ==========================================
    // 场景二：普通函数 + 参数传递 (测试 Perfect Forwarding)
    // ==========================================
    std::cout << "\n[场景 2] 提交普通函数 multiply(10, 20)..." << std::endl;
    
    // 这里的参数 10, 20 会被完美转发给 multiply
    auto future_res = pool.push_task(multiply, 10, 20); 
    
    std::cout << "乘法结果: " << future_res.get() << std::endl;


    // ==========================================
    // 场景三：并发验证 (观察打印顺序)
    // ==========================================
    std::cout << "\n[场景 3] 并发验证：同时提交 4 个耗时 2秒 的任务" << std::endl;
    std::cout << "如果是串行，需要 8秒；如果是并行，只需要约 2秒。" << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<std::future<void>> void_futures;

    for(int i = 0; i < 4; ++i) {
        void_futures.emplace_back(
            pool.push_task([i] {
                // 模拟耗时
                std::this_thread::sleep_for(std::chrono::seconds(2));
                // 为了避免打印混乱，加个简单的锁（仅演示用，生产环境尽量少用 cout）
                static std::mutex io_mutex;
                {
                    std::unique_lock<std::mutex> lock(io_mutex);
                    std::cout << "任务 " << i << " 由线程 " << std::this_thread::get_id() << " 完成" << std::endl;
                }
            })
        );
    }

    // 等待所有任务完成
    for(auto& f : void_futures) f.get();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    std::cout << "总耗时: " << duration << " ms" << std::endl;


    // ==========================================
    // 场景四：异常处理 (Exception Propagation)
    // ==========================================
    std::cout << "\n[场景 4] 测试任务抛出异常..." << std::endl;

    auto error_future = pool.push_task([] {
        throw std::runtime_error("这是一个故意的错误！");
        return 0; 
    });

    try {
        error_future.get(); // 这里应该抛出异常
    } catch (const std::exception& e) {
        std::cout << "主线程捕获到了子线程的异常: " << e.what() << std::endl;
    }

    return 0;
}

// === 线程池初始化完成 (4 线程) ===

// [场景 1] 提交 8 个计算平方的任务...
// 计算结果: 0 1 4 9 16 25 36 49 

// [场景 2] 提交普通函数 multiply(10, 20)...
// 乘法结果: 200

// [场景 3] 并发验证：同时提交 4 个耗时 2秒 的任务
// 如果是串行，需要 8秒；如果是并行，只需要约 2秒。
// 任务 1 由线程 140673004914240 完成
// 任务 0 由线程 140673013306944 完成
// 任务 2 由线程 140673021699648 完成
// 任务 3 由线程 140673030092352 完成
// 总耗时: 2001 ms

// [场景 4] 测试任务抛出异常...
// 主线程捕获到了子线程的异常: 这是一个故意的错误！