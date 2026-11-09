# [Google Benchmark](https://github.com/google/benchmark/blob/main/docs/user_guide.md)

> **benchmark**
>
> * *n.*基准；（监测计算机性能的）基准（问题）标准检查程序；（测量高度的）水准点，基准点
> * *v.*（根据某种标准）评估，衡量；基准问题检测结果为

## 微基准测试 vs 宏基准测试

在软件性能优化领域，**<font color=blue>微基准测试（Microbenchmarking）</font>**和**<font color=blue>宏基准测试（Macrobenchmarking）</font>**是两种互补的测试策略。它们的区别就像是“用显微镜观察细胞”与“在赛道上测试整车性能”的区别。

以下是两者的核心对比与详细解析：

| **维度**         | **微基准测试 (Microbenchmark)**                              | **宏基准测试 (Macrobenchmark)**                              |
| ---------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| **测试范围**     | 代码片段、单个函数、特定算法或数据结构                       | 完整的应用程序、端到端（End-to-End）系统流程                 |
| **主要目标**     | 测量具体的开销（如 CPU 时钟周期、内存分配、缓存命中率）      | 测量整体系统表现（如吞吐量、用户感知的延迟、并发承载力）     |
| **运行环境**     | 高度隔离，人为构造的“真空”环境                               | 尽可能模拟真实的生产环境（包括网络、数据库、IO 等）          |
| **典型执行时间** | 纳秒 (ns) 到 毫秒 (ms) 级别                                  | 秒 (s) 到 分钟/小时 级别                                     |
| **常见痛点**     | 容易被编译器优化（如死代码消除）、CPU 缓存预热导致结果不真实 | 环境噪音多（网络波动、后台进程干扰），难以定位具体慢在哪一行代码 |
| **常用工具**     | **Google Benchmark**, JMH (Java), Catch2                     | Apache JMeter, wrk, LoadRunner, Locust                       |

### 1. 微基准测试 (Microbenchmarking)

微基准测试关注的是**<font color=blue>局部细节</font>**。当你想要榨干某一段核心代码的最后一滴性能时，就会用到它。

- **典型场景**：
  - 对比 `std::vector` 和 `std::list` 在特定数据量下的插入速度。
  - 测试一个自定义的字符串分割函数（String Split）是否比标准库的实现更快。
  - 测量某种哈希算法处理 1KB 数据的 CPU 周期消耗。
- **为什么难做？** 现代编译器非常聪明。如果你写了一个循环计算结果，但最后没有打印或使用这个结果，编译器在 `-O3` 优化下会直接把这段代码删掉，导致你测出的时间是 0 纳秒。这就是为什么我们需要像 **Google Benchmark** 这样的专业框架，<font color=blur>利用 `benchmark::DoNotOptimize()` 来欺骗编译器，强制它执行这些指令。</font>

### 2. 宏基准测试 (Macrobenchmarking)

宏基准测试关注的是**<font color=blue>全局体验</font>**。它不在乎你底层用的是什么排序算法，只在乎整个系统跑起来能不能满足业务需求。

- **典型场景**：
  - 压测一个 Web API，看看在 1000 个并发用户下，系统的 QPS（每秒查询率）能达到多少，99% 的请求是否能在 200 毫秒内返回。
  - 测试一个机器学习模型从接收到输入图片，经过网络传输、预处理、推理计算，最后返回结果的端到端总延迟。
  - 模拟双十一抢购，测试数据库连接池和消息队列的抗压能力。
- **核心价值**： 它能暴露系统级的架构瓶颈。比如，微基准测试告诉你你的算法极快，但宏基准测试可能会发现，系统大部分时间其实都阻塞在等待数据库的磁盘 I/O 上，此时优化算法毫无意义。

------

### <font color=blue>它们如何协同工作？（性能优化的标准流程）</font>

在实际的工程实践中，我们通常**自顶向下**地结合使用这两种测试：

1. **宏基准测试发现问题**：通过压测工具发现 API 响应慢，或者系统整体吞吐量不达标。
2. **Profiling（性能剖析）定位瓶颈**：使用 `perf`、`gprof` 或火焰图（FlameGraph）等工具运行系统，找出消耗 CPU 时间最多的是哪个函数（比如发现是一个 JSON 解析函数占用了 60% 的 CPU）。
3. **微基准测试验证优化**：把那个 JSON 解析函数单独剥离出来，用 Google Benchmark 写一个测试。然后尝试用不同的算法或库重写它，在微基准测试中验证新代码确实快了 3 倍。
4. **宏基准测试最终确认**：将优化后的代码集成回主系统，再次运行宏基准测试，确认整体 API 的响应时间确实得到了改善。

## C++ 中的基准测试工具

C++中两个流行的基准测试选项是： [Google Benchmark](https://github.com/google/benchmark) 和 [Catch2](https://github.com/catchorg/Catch2)。

Google Benchmark 是一个强大且多功能的 C++ 基准测试库，允许开发者以高精度测量代码的性能。其主要优点之一是易于集成到现有项目中，特别是那些已经使用[GoogleTest](https://github.com/google/googletest)的项目。Google Benchmark 提供详细的性能指标，包括测量CPU时间、墙钟时间和内存使用情况的能力。它支持广泛的基准测试场景，从简单的函数基准测试到复杂的参数化测试。

Catch2 是一个现代的、只需头文件的C++测试框架，简化了编写和运行测试的过程。它的主要优点之一是易用性，其语法直观且表达力强，使开发者能够快速清楚地编写测试。Catch2 支持广泛的测试类型，包括单元测试、集成测试、行为驱动开发（BDD）风格的测试和基本的微基准测试功能。

两者都得到 [Bencher 的支持](https://bencher.dev/zh/docs/explanation/adapters/)。那么为什么选择 Google Benchmark？ Google Benchmark 无缝集成到 GoogleTest 中，而 GoogleTest 是 C++ 生态系统的事实标准单元测试框架。 我建议使用 Google Benchmark 来评估代码的延迟，特别是当你已经使用 GoogleTest 时。 也就是说，Google Benchmark 非常适合测量挂钟时间。

## Google Benchmark

**Google Benchmark** 是由 Google 开源的一款针对 C++ 的微基准测试（Microbenchmark）框架。它的设计理念与 Google Test (gtest) 非常相似，但其核心目的是 **测量代码片段的执行性能**，例如运行时间、CPU 占用时间、内存分配情况以及算法的时间复杂度。

核心特性

- **<font color=ee11ff>自动迭代推导</font>**：你不需要手动指定代码应该运行多少次。框架会在死循环中不断执行你的测试代码，直到收集到具有足够统计意义的稳定数据为止。
- **<font color=ee11ff>防止编译器过度优化</font>**：在进行微基准测试时，现代 C++ 编译器常常会把没有实际输出的测试代码直接优化（删除）掉。Benchmark 提供了 `benchmark::DoNotOptimize()` 和 `benchmark::ClobberMemory()` 等工具，强制编译器执行这些操作。
- **<font color=ee11ff>参数化与范围测试</font>**：可以非常方便地传入不同的参数（如不同的数组大小），测试代码在不同数据量下的性能表现。
- **<font color=ee11ff>大 O 复杂度分析</font>**：框架可以通过多组不同规模的输入数据，自动拟合出算法的时间复杂度（如 O(N), O(N log N)）。
- **<font color=ee11ff>多线程支持</font>**：内置对多线程并发性能测试的支持。
- **<font color=ee11ff>多格式输出</font>**：默认在终端输出人类可读的表格，但也支持导出为 JSON 或 CSV 格式，方便后续与 CI/CD 管道集成或生成图表。

##  基础使用

```cpp
#include <iostream>

int main()
{
    for (int i = 1; i <= 100; i++)
    {
        if ((i % 15) == 0)
            std::cout << "FizzBuzz\n";
        else if ((i % 3) == 0)
            std::cout << "Fizz\n";
        else if ((i % 5) == 0)
            std::cout << "Buzz\n";
        else
            std::cout << i << "\n";
    }
    return 0;
}
```

为了测试我们的 FizzBuzz 应用程序， 我们需要将逻辑与程序的 `main` 函数解耦。 <font color=blur>基准测试工具不能对 `main` 函数进行基准测试。</font> 为此，我们需要进行一些更改。

```cpp
// play_game.hpp
#ifndef GAME_HPP
#define GAME_HPP

void play_game(int n, bool should_print);

#endif  // GAME_HPP


// play_game.cpp
#include "play_game.hpp"
#include <iostream>
#include <string>

std::string fizz_buzz(int n) {
    if (n % 15 == 0) {
        return "FizzBuzz";
    } else if (n % 3 == 0) {
        return "Fizz";
    } else if (n % 5 == 0) {
        return "Buzz";
    } else {
        return std::to_string(n);
    }
}

void play_game(int n, bool should_print) {
    std::string result = fizz_buzz(n);
    if (should_print) {
        std::cout << result << std::endl;
    }
}
```

然后更新 `game.cpp` 中的 `main` 函数以使用头文件中的 `play_game` 函数定义：

```cpp
// game.cpp
#include "play_game.hpp"

int main() {
    for (int i = 1; i <= 100; i++) {
        play_game(i, true);
    }
}
```

我们的程序的 `main` 函数遍历从 `1` 到 `100`（包括 100）并为每个数字调用 `play_game`，将 `should_print` 设置为 `true`。

现在让我们创建一个名为 `benchmark_game.cpp` 的新文件：

```cpp
// benchmark_game.cpp

// 导入函数定义和 benchmark 头文件
#include "play_game.hpp"
#include <benchmark/benchmark.h>
#include <iostream>

// 创建一个名为 BENCHMARK_game 的函数，该函数接收 benchmark::State 的引用
static void BENCHMARK_game(benchmark::State &state) {
    // 迭代 benchmark::State 对象
    for (auto _ : state) {
        for (int i = 1; i <= 100; i++) {
            play_game(i, false);
        }
    }
}

// 将 BENCHMARK_game 函数传递给 BENCHMARK 运行器。
BENCHMARK(BENCHMARK_game);

// 使用 BENCHMARK_MAIN 运行基准测试
BENCHMARK_MAIN();
```

现在我们准备好对我们的代码进行基准测试了：

```bash
g++ -std=c++11 -O3 play_game.cpp benchmark_game.cpp -lbenchmark -lpthread -o app && ./app
```

* **`-lpthread` (或 `-pthread`)**： Google Benchmark 底层使用了 POSIX 线程库来实现多线程基准测试等功能。如果你不加上这个链接标志，链接器（Linker）很可能会报错，提示找不到类似 `pthread_create` 的未定义引用（Undefined reference）。
* **`-O3` (极其重要)**： 这是开启 GCC 最高级别优化的标志。**<font color=blue>做基准测试时，绝对不能在默认的 `-O0`（无优化）模式下跑。</font>** 未优化的代码包含了大量用于调试的冗余指令和内存读写。如果你不加 `-O3`，你测出的只是极其低效的代码的运行时间，完全不能反映代码在真实生产环境中的性能。

```
2026-03-16T18:34:33+08:00
Running ./app
Run on (16 X 3686.4 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 20480 KiB (x1)
Load Average: 0.10, 0.14, 0.09
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
BENCHMARK_game        466 ns          471 ns      1492736
```

这段输出是 Google Benchmark 自动生成的一份“性能体检报告”。在微基准测试中，**运行环境**和**测试结果**同样重要，因为硬件状态会直接决定代码的跑分。

这套输出信息主要分为两大块，下面我为你逐行拆解：

***<font color=ee11ff>1. 测试环境信息 (环境越干净，数据越准)</font>***

框架在跑代码前，会先给你的机器“拍个快照”，确保以后排查性能问题时有据可查：

- **`2026-03-16T18:34:33+08:00`：**测试运行的标准时间。
- **`Running ./app`**：测试执行的二进制文件。
- **`Run on (16 X 3686.4 MHz CPU s)`**：你当前机器的 CPU 概况。这说明你有 16 个逻辑核心（通常是 8 核 16 线程），当前运行频率约为 3.68 GHz。
- **`CPU Caches:`**：你的 CPU 缓存层级结构。
  - **L1 / L2 / L3**：分别代表一级、二级、三级缓存。`(x8)` 说明 L1 和 L2 是 8 个物理核心各自独享的，而 `L3 (x1)` 是所有核心共享的一块 20MB 大缓存。微基准测试极其依赖缓存命中率，这段信息对底层优化（如规避 Cache Miss）至关重要。
  - 这里的 `Data` 表示只缓存数据，`Instruction` 表示只缓存指令，`Unified` 表示指令和数据共用。
- **`Load Average: 0.10, 0.14, 0.09`**：Linux 系统的平均负载（过去 1 分钟、5 分钟、15 分钟）。你的负载非常低（远小于 1），这意味着系统很闲，几乎没有其他后台程序在抢占 CPU。**结论：你的测试环境非常纯净，测出来的数据噪音极小。**

***<font color=ee11ff>2. 核心测试数据 (你的代码表现)</font>***

这三列数据是基准测试的灵魂所在：

- **`Time` (466 ns)**：**挂钟时间 (Wall-clock Time)**。
  - 含义：现实世界中真实流逝的时间。就好比你拿着一块极高精度的秒表，在代码开始和结束时按了一下。你的 FizzBuzz 跑完一遍 1 到 100 的平均时间是 466 纳秒。
- **`CPU` (471 ns)**：**CPU 计算时间 (CPU Time)**。
  - 含义：你的程序**真正在 CPU 上占用计算资源**的时间。它排除了线程被操作系统挂起、等待网络或磁盘 I/O 的时间。
  - **深度解读**：在这里，`Time` (466) 和 `CPU` (471) 几乎一模一样。这完美地符合预期，说明你的 FizzBuzz 是一段纯粹的“计算密集型”代码，中间没有任何 I/O 阻塞或线程休眠。
- **`Iterations` (1492736)**：**迭代次数**。
  - 含义：Google Benchmark 的核心机制。你不需要手动规定循环多少次，框架发现你的代码跑得太快了（只需几百纳秒），为了消除系统抖动带来的误差，它**自动**让你的那段代码循环执行了将近 150 万次，直到最终计算出一个具有统计学意义的稳定平均值。

目前你的代码是硬编码跑 1 到 100。在实际工作中，我们通常需要对比不同数据量下的性能曲线。

## `for (auto _ : state)`

<font color=blur>Google Benchmark 报告中打印出来的 `Time` 和 `CPU` 耗时，**严格且仅仅只包含 `for (auto _ : state)` 循环执行期间所花费的时间**。</font>

`for (auto _ : state)` 就像是一个结界，将你的代码分成了两部分：

- **结界外（循环前/后）：属于 Setup 和 Teardown。** 比如你在循环前分配内存、读取文件、初始化随机数生成器，或者在循环后打印一些调试信息。这些代码在每个数据规模（Range）的测试下**只会执行一次**。框架在这期间是处于“闭眼”状态的，完全不会记录时间。
- **结界内（循环中）：属于真正的 Payload（负载）。** 框架会在进入循环的瞬间按下“秒表”，在循环结束的瞬间按停“秒表”。然后，它将记录下的**总时间**除以**总迭代次数**（也就是 `state.iterations()` 的值），得出单次操作的平均耗时。

<font color=blur>那 `for` 循环本身的开销算进去了吗？</font>

- **真实情况：** 算进去了。`for (auto _ : state)` 本质上是基于迭代器的循环，每次迭代都包含一次状态检查（判断是否还要继续跑）和一次自增操作。
- **为什么不用担心：** 这个开销极其微小（通常在汇编层面只有两三条指令，耗时远小于 1 纳秒）。而且，Google Benchmark 在预热（Warm-up）阶段会动态评估你的代码执行速度。如果你的代码执行得很快，它可能会把循环次数设定为上千万次（就像你之前跑出来的 `6946781` 次）。在如此庞大的基数下，循环控制本身的开销被彻底摊薄，对最终平均值的影响微乎其微。

<font color=blur>唯一的例外：结界内的“时间黑洞”</font>

如果你在 `for` 循环内部调用了 `state.PauseTiming()`，计时的秒表就会立刻暂停；直到你调用 `state.ResumeTiming()`，秒表才会继续走。 这部分被暂停的时间，就会像“时间黑洞”一样被框架完全剔除，**不会**计入最终的测试结果。

## `benchmark::State& state`

`state` 可以理解成 **Google Benchmark 交给你的“测试控制器”**。<font color=blur>你的 benchmark 函数本身只负责描述“要测什么”，而 `state` 负责告诉你“要测多少次、参数是什么、统计什么”。</font>

最常见的用途就是控制重复执行：

```cpp
static void BM_Test(benchmark::State& state) {
    for (auto _ : state) {
        DoSomething();
    }
}
```

这里你不用自己写：

```cpp
for (int i = 0; i < 1000000; ++i)
```

因为 Google Benchmark 会根据运行时间自动决定迭代次数。`state` 就参与管理这个过程。

它还可以拿 benchmark 参数。例如：

```cpp
static void BM_Vector(benchmark::State& state) {
    int n = state.range(0);
    for (auto _ : state) {
        std::vector<int> v(n);
    }
}


BENCHMARK(BM_Vector)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000);
```

那么每次运行时：

```cpp
state.range(0)
```

会分别得到 `100`、`1000`、`10000`。

另外，`state` 还能告诉 Benchmark 一次测试处理了多少数据：

```cpp
state.SetItemsProcessed(
    state.iterations() * n
);
```

这样结果里就可以计算吞吐量，比如 `items/s`。

如果初始化代码不应该计入 benchmark 时间，也可以通过 `state` 暂停计时：

```cpp
for (auto _ : state) {
    state.PauseTiming();
    prepare(); // 准备数据，不计时
    state.ResumeTiming();

    // 真正要测的代码
    DoSomething();
}
```

所以可以把它记成：**<font color=blue>`benchmark::State` = Google Benchmark 和你的测试函数之间的接口。</font>**

最核心的三个用途是：**<font color=blue>控制测试循环、获取参数、记录统计信息。</font>**

尤其是这一句：

```cpp
for (auto _ : state)
```

实际上就是 Google Benchmark 的核心机制之一。

## Time、CPU

这是一个非常经典且极其重要的性能分析（Profiling）问题！在 Google Benchmark 的输出结果中，`Time` 和 `CPU` 代表了两种完全不同的测量维度。

你可以用一个通俗的比喻来理解它们：

- **Time（墙上时钟时间 / Wall Time）：** 就像你站在旁边，手里拿着秒表。代码开始运行你按下开始，代码结束你按下停止。它记录的是**真实世界流逝的时间**。
- **CPU（CPU 时间 / CPU Time）：** 就像是给 CPU 发的“计件工资单”。它记录的是 **CPU 核心真正在执行你这段代码指令的时间**。当你的代码在休眠、等待网络、等待硬盘读写或者被操作系统强行切走时，CPU 时间是**不走字**的。

在面试中，如果你能根据这两个值的差异来推断代码的瓶颈，会极其加分。通常有以下三种典型场景：

***<font color=ee11ff>1. 理想状态：`Time` $\approx$ `CPU`</font>***

- **场景：** 纯粹的计算密集型（CPU-Bound）任务，也就是你的程序一直在满负荷疯狂运算。
- **举例：** 就像你上一条跑的 `BM_SetInsert` 测试，你会发现它的 `Time` 和 `CPU` 几乎一模一样（比如都是 102 ns）。这说明在测试期间，线程完全占用了 CPU，没有被系统挂起，也没有等待任何外部资源。

**<font color=ee11ff>2. 存在瓶颈：`Time` $\gg$ `CPU` (Time 远大于 CPU)</font>**

- **场景：** I/O 密集型任务，或者严重的锁竞争（阻塞挂起）。
- **原因：** 你的代码在执行过程中，遇到了 `std::this_thread::sleep_for`、读写大文件（等待机械硬盘转圈）、等待网络请求返回，或者在争抢一个 `std::mutex` 锁失败后被操作系统挂起（休眠）了。
- **现象：** 这时，你手里的秒表（`Time`）还在滴答作响，但 CPU 发现你这段代码在等东西，就去跑别的进程了，所以属于你这段代码的 `CPU` 时间就停滞了。
- **排查价值：** 如果你看到 `Time` 是 10 毫秒，但 `CPU` 只有 1 毫秒，说明你的代码**有 90% 的时间都在“干等”**。这时候你去优化算法复杂度是没用的，你应该去优化 I/O 路径或减少锁冲突。

***<font color=ee11ff>3. 多线程并行：`CPU` $\gg$ `Time` (CPU 远大于 Time)</font>***

- **场景：** 成功的高并发多线程任务。
- **原因：** 在 Google Benchmark 的多线程测试（比如 `->Threads(4)`）中，`Time` 依然是真实世界流逝的时间，但 **`CPU` 时间是所有参与线程耗时的总和**。
- **现象：** 假设你开了 4 个线程，完美并行运行了 1 秒钟。那么 `Time` 会显示 1 秒，而 `CPU` 时间会显示 4 秒。
- **排查价值：** 这正是评估多线程扩展性（Scalability）的绝佳指标！如果你开了 8 个线程，发现 `Time` 没有明显缩短，而 `CPU` 时间却暴增（说明 CPU 在忙着做上下文切换或自旋锁空转），这就说明你的多线程架构存在严重的内耗（比如伪共享、锁粒度太粗）。

## 编译与运行

<font color=blur>**编译时**需要链接 `benchmark` 和 `pthread` 库，并且通常建议在 `Release` 模式下编译（开启 `-O3` 优化），因为 Debug 模式下的性能测试没有参考价值。</font>

**运行时**，Google Benchmark 提供了丰富的命令行参数，帮助你过滤测试或更改输出格式：

- `--benchmark_filter=<regex>`：只运行匹配正则表达式的测试用例。
- `--benchmark_format=<console|json|csv>`：指定输出格式，JSON 或 CSV 格式非常适合输出到文件后用脚本生成性能对比图表。
- `--benchmark_repetitions=<num>`：将同一个测试重复多次，以计算平均值和标准差，减少环境波动带来的误差。

## benchmark 代码组织结构

**<font color=blur>在实际项目中，强烈建议（且通常是必须的）将基准测试代码与你的核心业务代码分离开来，单独编写。</font>**

在实际的工程开发中，我们通常会把核心逻辑、实际运行的应用程序、以及测试代码拆分成不同的文件和编译目标。之所以必须单独编写，主要有以下几个核心原因：

***<font color=ee11ff>1. 避免 `main` 函数冲突 (入口点不同)</font>***

C++ 程序只能有一个 `main()` 函数。

- 你的实际项目需要一个 `main()` 来启动业务逻辑。
- Google Benchmark 也需要自己的入口点（通常通过 `BENCHMARK_MAIN()` 宏自动生成）来接管程序的执行流程、解析命令行参数（如 `--benchmark_filter`）并输出测试报告。 如果不分开，这两个 `main` 函数会产生冲突，导致程序无法编译。

***<font color=ee11ff>2. 隔离依赖，不污染生产环境代码</font>***

Google Benchmark 是一个**开发期/测试期依赖**。你绝对不希望在最终发布给用户的应用程序（Release 产物）中，把 `libbenchmark.so` 或它的冗余测试代码也一并打包进去。这不仅会增加包体积，还可能引入不必要的安全漏洞或运行时负担。

***<font color=ee11ff>3. 不同的编译优化需求</font>***

基准测试和日常开发的编译要求截然不同：

- **业务开发/调试**：通常使用 `-O0`（关闭优化）并附带 `-g`（调试符号），方便单步断点调试。
- **基准测试**：**必须**在 `-O3`（最高级别优化）和 `NDEBUG`（关闭断言）的 Release 模式下编译，否则测出来的性能数据毫无参考价值。 将它们分离成不同的模块，可以方便构建系统（如 CMake）为它们应用不同的编译参数。

**<font color=blue>标准的项目目录结构</font>**

为了实现这种分离，现代 C++ 项目通常会采用类似如下的目录结构：

```
my_project/
├── CMakeLists.txt          # 全局构建配置
├── src/                    # 核心业务代码 (被测试的代码)
│   ├── play_game.cpp
│   └── play_game.h
├── app/                    # 实际应用程序的入口
│   └── main.cpp            # 包含你真实的 main() 函数
└── benchmarks/             # 基准测试代码
    └── benchmark_game.cpp  # 包含 BENCHMARK_MAIN()，并 #include "play_game.h"
```

在这样的结构下：

1. `src/` 里的代码会被编译成一个静态库或动态库（比如 `libcore`）。
2. `app/main.cpp` 会链接 `libcore`，生成最终的业务可执行文件。
3. `benchmarks/benchmark_game.cpp` 也会链接 `libcore`，同时链接 Google Benchmark 库，生成一个专门用来跑分的独立可执行文件（比如 `run_benchmarks`）。

## static

<font color=blur>在使用 **Google Benchmark**（`google/benchmark`）库编写性能测试时，被测函数（即 benchmark 函数）通常被声明为 `static`。</font>这主要有以下几个原因：

*<font color=ee11ff>**1.** **避免链接冲突（ODR 问题）**</font>*

Google Benchmark 使用宏（如 `BENCHMARK(func)`）将你的函数注册到全局的 benchmark 列表中。如果你在多个 `.cc` 文件中定义了同名的非 `static` 函数，会导致 **符号重复定义（multiple definition）** 的链接错误。

通过将函数声明为 `static`，该函数就具有 **内部链接（internal linkage）**，只在当前编译单元可见，从而避免了命名冲突。

*<font color=ee11ff>**2.** **符合 Google Benchmark 的设计预期**</font>*

Google Benchmark 的宏（如 `BENCHMARK`）期望传入一个可调用对象（通常是函数指针）。如果函数不是 `static` 且定义在匿名命名空间之外，可能会因链接问题导致不可预测的行为。

虽然你也可以把函数放在 **匿名命名空间（anonymous namespace）** 中达到类似效果：

```cpp
namespace {
void BM_MyFunction(benchmark::State& state) {
  for (auto _ : state) {
    // 被测代码
  }
}
} // namespace

BENCHMARK(BM_MyFunction);
```

但使用 `static` 更简洁、明确，尤其在单文件测试中很常见。

*<font color=ee11ff>**3.** **历史习惯与示例代码风格**</font>*

Google 官方文档和大多数示例都使用 `static`，这也成为社区惯例。

**<font color=ee11ff>补充说明：C++17 及以后的替代方案</font>**

从 C++17 开始，你可以使用 `[[maybe_unused]]` + 匿名命名空间，或者直接用 lambda（配合 `BENCHMARK_CAPTURE`），但 `static` 仍然是最简单、兼容性最好的方式。

## 关键特性与进阶用法

***<font color=ee11ff>1. 对抗编译器优化 (DoNotOptimize & ClobberMemory)</font>***

在微基准测试中，现代 C++ 编译器（如 GCC, Clang）非常聪明，经常会把没有副作用的代码直接“优化掉”（Dead Code Elimination），导致测试结果为 0 纳秒。Google Benchmark 提供了两个核心工具来对抗这种情况：

- **`benchmark::DoNotOptimize(var)`**：强制编译器计算 `var` 的值，并认为该值已被外部使用，从而防止相关计算被优化掉。
- **`benchmark::ClobberMemory()`**：强制编译器将所有挂起的内存写入操作刷新到内存中，并假设内存已被外部修改，防止编译器跨越此屏障重排或优化内存访问。（确保所有内存操作（尤其是涉及指针或全局状态时）不会被重排或消除。）

> `benchmark::ClobberMemory()` 的作用是：
>
> - **告诉编译器：内存可能被“破坏”了（clobbered），不要假设内存内容不变。**
> - 它通常通过内联汇编（inline assembly）实现，带有 `"memory"` clobber 约束，强制编译器将寄存器中的值写回内存，并阻止跨调用的激进优化。
> - 这样可以确保 benchmark 中的计算不会被优化掉。
>
> 在 Google Benchmark 源码中，`ClobberMemory()` 通常是这样实现的（以 GCC/Clang 为例）：
>
> ```cpp
> inline void ClobberMemory() {
>   asm volatile("" : : : "memory");
> }
> ```
>
> 这个内联汇编语句：
>
> - 不生成任何实际指令（空字符串），
> - 但带有 `"memory"` clobber，告诉编译器：**内存内容可能已改变，不能缓存或重排内存访问**。
>
> **<font color=blue>何时需要使用？</font>**
>
> - 当你的 benchmark 涉及**写内存**（如填充数组、修改对象成员）但没有后续读取；
> - 当你担心编译器把整个循环体优化为空；
> - 在测量**内存带宽**、**缓存行为**或**副作用操作**时特别重要。
>
> `benchmark::ClobberMemory()` 是一个**编译屏障（compiler barrier）**，用于：
>
> - 防止编译器过度优化 benchmark 代码；
> - 确保测量的是真实执行开销；
> - 与 `DoNotOptimize()` 配合使用，构建可靠的微基准测试。

```cpp
static void BM_VectorPushBack(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<int> v;
    v.push_back(42);
    // 告诉编译器：不要优化掉 v 的生成和修改
    benchmark::DoNotOptimize(v.data());
    benchmark::ClobberMemory(); 
  }
}
BENCHMARK(BM_VectorPushBack);
```

***<font color=ee11ff>2. 参数化测试 (Parameterized Tests)</font>***

如果你想测试函数在不同输入规模下的性能，不需要写多个测试函数，可以通过传递参数来实现。

```cpp
static void BM_StringCopy(benchmark::State& state) {
  std::string x(state.range(0), 'a'); // 获取传入的参数
  for (auto _ : state) {
    std::string copy(x);
    benchmark::DoNotOptimize(copy);
  }
}
// 测试大小为 8, 64, 512 的情况
BENCHMARK(BM_StringCopy)->Arg(8)->Arg(64)->Arg(512);

// 或者使用 Range 自动生成 8 到 8192 之间 8 的倍数的参数
BENCHMARK(BM_StringCopy)->RangeMultiplier(8)->Range(8, 8192);
```

***<font color=ee11ff>3. 算法时间复杂度分析 (Complexity)</font>***

你可以让框架自动拟合数据，计算出代码的时间复杂度（如 *O*(*N*)、*O*(log*N*)、*O*(*N*2) 等）。

```cpp
#include <set>
#include <vector>
#include <random>
#include <benchmark/benchmark.h>

static void BM_SetInsert(benchmark::State& state) {
    const int N = state.range(0);

    // 使用更好的随机数生成器（可选）
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, N - 1);

    // 预构建 set（不在计时内）
    std::set<int> data;
    for (int i = 0; i < N; ++i) {
        data.insert(i * 2);  // 确保不重复
    }

    for (auto _ : state) {
        int val = dis(gen) * 2 + 1;  // 插入奇数，避免重复
        data.insert(val);
        // 可选：立即删除以保持 size 不变
        data.erase(val);
    }

    state.SetComplexityN(N);
}

BENCHMARK(BM_SetInsert)->Range(1 << 10, 1 << 20)->Complexity(benchmark::oLogN);

BENCHMARK_MAIN();
```

***<font color=ee11ff>4. 多线程基准测试 (Multithreading)</font>***

如果需要测试并发性能，可以通过 `Threads()` 方法启动多个线程同时执行同一个 benchmark 循环。

```cpp
static void BM_MultiThreadedTask(benchmark::State& state) {
  for (auto _ : state) {
    // 线程安全的任务
  }
}
// 用 1, 2, 4, 8 个线程分别测试
BENCHMARK(BM_MultiThreadedTask)->ThreadRange(1, 8);
```

***<font color=ee11ff>5. Fixtures (测试夹具)</font>***

对于需要复杂初始化和清理工作的测试（类似于 gtest 中的 `SetUp` 和 `TearDown`），可以使用基于类的 Fixture。

```cpp
class MyFixture : public benchmark::Fixture {
public:
  void SetUp(const benchmark::State& state) override { /* 数据初始化 */ }
  void TearDown(const benchmark::State& state) override { /* 资源释放 */ }
};

BENCHMARK_F(MyFixture, TestMethod)(benchmark::State& state) {
  for (auto _ : state) {
    // 测试逻辑
  }
}
```

## 例子：Google Benchmark 重现 False Sharing

**伪共享（False Sharing）：**多个线程各自修改完全独立的不同变量，但因为这些变量在内存中挨得太近，碰巧被 CPU 加载到了同一个“缓存行（Cache Line）”里，导致 CPU 核心之间疯狂地互相“抢夺”和“作废”对方的缓存，从而引发性能雪崩。

让我们用一段极简的 Google Benchmark 代码来重现这个“灾难现场”，以及如何用一个关键字完美修复它。

在这个测试中，我们让多个线程同时去递增一个数组里的值。**每个线程只递增属于自己的那个元素，互不干扰，逻辑上没有任何锁冲突。**

```cpp
#include <benchmark/benchmark.h>
#include <atomic>

// 灾难设计：变量挨得太紧
// 一个 atomic<int> 只占 4 个B
// 而缓存行大小一般为 64B
struct BadData {
    std::atomic<int> counter{0};
};

// 完美修复：强制内存对齐
struct GoodData {
    // 强制把这个结构体撑大到 64B（现代 CPU 常见的 Cache Line 大小）
    alignas(64) std::atomic<int> counter{0};
};

// 定义全局数组，供最多 8 个线程使用
BadData bad_array[8];
GoodData good_array[8];

// 测试 1：伪共享
static void BM_FalseSharing(benchmark::State& state) {
    // 获取当前线程的唯一编号 (0 到 state.threads()-1)
    int tid = state.thread_index();
    for (auto _ : state) {
        // 线程只修改属于自己的那个计数器，绝对没有逻辑冲突！
        benchmark::DoNotOptimize(bad_array[tid].counter.fetch_add(1, std::memory_order_relaxed));
    }
}
// 测试 1, 2, 4, 8 个线程
BENCHMARK(BM_FalseSharing)->Threads(1)->Threads(2)->Threads(4)->Threads(8);

// 测试 2：消除伪共享（修复版）
static void BM_NoFalseSharing(benchmark::State& state) {
    int tid = state.thread_index();
    for (auto _ : state) {
        benchmark::DoNotOptimize(good_array[tid].counter.fetch_add(1, std::memory_order_relaxed));
    }
}
BENCHMARK(BM_NoFalseSharing)->Threads(1)->Threads(2)->Threads(4)->Threads(8);

BENCHMARK_MAIN();
```

当你在真实的 Linux 服务器（比如 8 核机器）上跑出这份报告时，你会看到极其戏剧性的对比：

```bash
g++ -std=c++11 -O3 benchmark.cpp -lbenchmark -lpthread -o app && ./app
```

在我的机器上的运行结果：

```
2026-03-16T19:20:51+08:00
Running ./app
Run on (16 X 3686.4 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 20480 KiB (x1)
Load Average: 0.23, 0.11, 0.08
----------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations
----------------------------------------------------------------------
BM_FalseSharing/threads:1         4.22 ns         4.14 ns    171921910
BM_FalseSharing/threads:2         8.60 ns         16.9 ns     35122912
BM_FalseSharing/threads:4         11.3 ns         44.6 ns     16051832
BM_FalseSharing/threads:8         12.0 ns         95.2 ns      7804464
BM_NoFalseSharing/threads:1       4.18 ns         4.13 ns    171392446
BM_NoFalseSharing/threads:2       2.06 ns         4.09 ns    165832568
BM_NoFalseSharing/threads:4       1.19 ns         4.71 ns    157960504
BM_NoFalseSharing/threads:8      0.852 ns         6.77 ns     80000000
```

* 首先我们可以看出，CPU 时间大约为 `Time * 线程数`，无论是否发生伪共享
* 其次我们可以发现，没有伪共享时，Time 时间会逐渐变小，而且大概是单线程时的 `Time 时间 / 线程数`，这是因为多 CPU 是并行计算
* 不过我们查看有伪共享时的情况，随着线程数的增加，Time 时间也会增加，这是因为伪共享导致的 cache 失效