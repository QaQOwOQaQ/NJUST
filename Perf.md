# Perf

<font color=blue>`perf`（Performance Counters for Linux）</font>是 <font color=blue>Linux 系统级性能分析</font> 的瑞士军刀。它深入内核，能够追踪从 <font color=blur>**硬件级别**</font> 的 CPU 周期、缓存未命中（Cache Misses），到 <font color=blur>**软件级别**</font> 的上下文切换、缺页异常（Page Faults）等各类事件。

<font color=blur>要精通 `perf`，通常只需掌握它的四个核心子命令：`stat`、`top`、`record` 和 `report`。</font>

它们刚好覆盖了最典型的性能分析流程：

| 子命令        | 主要用途                 | 你可以把它理解成        |
| ------------- | ------------------------ | ----------------------- |
| `perf stat`   | 统计整体性能指标         | “程序总体表现怎么样？”  |
| `perf top`    | 实时查看热点函数         | “现在 CPU 时间花在哪？” |
| `perf record` | 采样并保存性能数据       | “把运行过程录下来”      |
| `perf report` | 分析 `record` 产生的数据 | “看看刚才录到了什么”    |

其中 `record + report` 通常是一对：

```bash
perf record -g ./my_program
perf report
```

而 `stat` 更偏向宏观指标：

```bash
perf stat ./my_program
```

看 `cycles`、`instructions`、`cache-misses`、`branch-misses` 等。

`top` 则是实时的：

```bash
perf top
```

类似：

```
top / htop
   ↓
看哪个进程 CPU 高


perf top
   ↓
看这个 CPU 时间具体花在哪些函数
```

不过 `perf` 还有不少重要子命令，比如：

* `perf list` 查看当前机器支持的 performance events。
* `perf annotate` 进一步把热点定位到**汇编指令 / 源码行**。
* `perf trace` 跟踪系统调用和一些事件，有点类似 `strace`，但基于 perf 基础设施。
* `perf sched` 分析 Linux 调度行为。
* `perf mem` 分析内存访问。

---

## 01 `perf stat`：宏观性能统计（Counting）

如果你想知道一个程序 <font color=blur>总体运行情况如何</font>，或者 <font color=blur>想知道它为什么跑得慢（是 CPU 计算密集，还是内存/I/O 瓶颈）</font>，`stat` 是第一步。<font color=24b3aa>它通过累加硬件计数器来给出全局统计信息。</font>

基本用法：

```bash
perf stat ./your_program
```

可能的结果：

```
Performance counter stats for './your_program':

      2,345.67 msec task-clock
           123      context-switches
             8      cpu-migrations
         1,234      page-faults
 7,800,000,000      cycles
 9,500,000,000      instructions
 1,200,000,000      branches
    35,000,000      branch-misses

       2.41 seconds time elapsed
       2.30 seconds user
       0.08 seconds sys
```

`perf stat` 主要用来回答一个问题：**<font color=blur>程序整体运行得怎么样，瓶颈更像是 CPU、分支、缓存，还是操作系统调度？</font>**

我建议把这些指标分成 4 类理解。

### 1.1 CPU 执行效率

最重要的是：

1. <font color=blur>`cycles`：CPU 消耗的时钟周期数。</font>

   例如：

   ```text
   7,800,000,000 cycles
   ```

   不要简单理解成“cycles 越少越好”，因为程序做的工作量不同。它更多用于和其他指标组合分析。

2. <font color=blur>`instructions`：CPU 实际执行的机器指令数量。</font>

   例如：

   ```text
   9,500,000,000 instructions
   ```

   最重要的是它和 `cycles` 的比例：

   ```text
   IPC = instructions / cycles
   ```

   例如：

   ```text
   instructions = 9.5 billion
   cycles       = 7.8 billion
   ```

   那么：

   ```text
   IPC ≈ 1.22
   ```

   `perf stat` 通常直接帮你显示：

   ```text
   9,500,000,000 instructions    # 1.22 insn per cycle
   ```

<font color=blue>**IPC（Instructions Per Cycle）**</font>，CPU 每个时钟周期平均完成多少条指令。

粗略来看：

* **<FONT COLOR=24B3AA>IPC 高：CPU pipeline 利用率通常比较高</FONT>**
* IPC 低：CPU 经常在等待

<font color=24b3aa>低 IPC 可能来自：</font>

* cache miss
* 内存延迟
* branch misprediction
* 数据依赖
* pipeline stall

**<font color=blur>但不能说 IPC 越高，程序一定越快。</font>**

* 因为 **IPC 只描述“每个 CPU 周期执行了多少条指令”**，而程序最终快不快取决于总共用了多少个周期以及 CPU 实际频率。也即，IPC 衡量的是 CPU 执行指令的“吞吐效率”，不是程序完成任务的“总工作量”。对于执行一亿条执行的程序和只执行 10 条指令的程序，即使一亿条执行那个程序的 IPC 再高，执行时间也不可能比 10 条指令的执行快。

* 还有一个原因是 **CPU 频率不是固定的**。现代 CPU 的频率会受到很多东西影响：Turbo Boost、温度、功耗、AVX / AVX-512、线程数量，CPU 调度等等。

  所以比较两个程序时不能假定 CPU 一定运行在同样频率。

* 还有一个更深一点的原因：**<font color=24b3aa>一条 instruction 并不代表相同的工作量。</font>**

  比如 `add rax, rbx` 是一条指令，而一条 SIMD 指令，例如 `vaddps ymm0, ymm1, ymm2` 也是“一条指令”，但可能一次完成 8 个 float 加法，所以单纯 instructions 数量并不等于实际完成的业务工作量。

  比如一个向量化程序 IPC = 1.5，可能比另一个普通标量程序 IPC = 3.0 快很多，因为前者每条指令处理的数据更多。

### 1.2 分支预测

主要看：

1. <font color=blur>`branches`：执行了多少条分支指令。</font>

   例如 `1,200,000,000 branches`

   包括：

   ```cpp
   if (...)
   for (...)
   while (...)
   switch (...)
   ```

   以及部分函数调用/跳转。

2. <font color=blur>`branch-misses`：CPU 分支预测失败次数。</font>

   现代 CPU 遇到分支（比如 `if (x > 10)`）不会不会傻等条件计算完成，而是会提前猜我猜你会走 if，如果猜对继续执行；如果猜错：pipeline 中已经做的部分工作作废并重新执行。这会产生性能损失。

比如：

```text
1,000,000 branches
10,000 branch-misses
```

就是 1% branch miss。

而 `500,000 branch-misses` 就是 50%。

所以看到 branchs 和 branchs-misses，最好不要只看绝对数量，而看 branch miss rate。

### 1.3 Cache / 内存

如果运行：

```bash
perf stat -d ./a.out
```

经常会看到：

1. <font color=blur>`cache-references`：CPU cache 相关访问数量。</font>
2. <font color=blur>`cache-misses`：缓存访问没有命中。</font>

我们应该主要关心 cache miss rate（cache-misses/cache-references），例如：

```text
100,000,000 cache-references
  5,000,000 cache-misses
```

就是 5% cache miss rate。

不过这里有一个很重要的注意点：`cache-references` 和 `cache-misses` 的具体硬件含义与 CPU 架构有关。

它们不像 `instructions` 那么容易跨机器直接比较。

如果要认真研究 cache，一般进一步看：

```bash
perf stat -d
```

甚至：

```bash
perf stat -d -d
perf stat -d -d -d
```

以及 CPU 特定的：

```bash
perf list
```

### 1.4 操作系统层面的指标

这几个也很常见：

1. **<font color=blur>task-clock：程序真正占用 CPU 的时间。</font>**

   例如 `2345.67 msec task-clock` 大约说明 CPU execution time ≈ 2.35 秒。如果多线程程序运行 1 秒，但用了 8 个 CPU core，那么 `task-clock` 可能接近 8000 ms，所以它不是简单等于 wall-clock 时间。

2. **<font color=blur>context-switches：上下文切换次数。</font>**

   例如 `12345 context-switches`，意思是操作系统频繁切换线程，上下文切换本身有成本。如果它特别高，可能涉及：

   * 线程数量太多
   * mutex / condition_variable
   * 频繁 sleep/wakeup
   * I/O
   * 线程争抢 CPU

   所以多线程程序性能异常时，这个指标值得看。

3. **<font color=blur>cpu-migrations：线程被 Linux scheduler 从一个 CPU core 移到另一个 core 的次数。</font>**

   频繁迁移可能影响 CPU cache locality、NUMA locality。

   特别是 NUMA 服务器、实时程序、高性能计算场景。

4. **<font color=blur>page-faults：缺页异常次数。</font>**

   `page-faults` 很正常。真正昂贵的是涉及磁盘 I/O 的 major page fault。

   如果需要更细可以看：

   ```bash
   perf stat -e page-faults,minor-faults,major-faults ./a.out
   ```

   一般 `minor-faults` 不需要读取磁盘。而 `major-faultmajor-faults` 通常更值得警惕。

### 1.5 最下面三个时间

通常还有：

* <font color=blur>**2.41 seconds `time elapsed`：**</font>墙上时间，Wall Clock Time。就是从程序从开始到结束的时间，这是用户最直接感受到的运行时间。
* **<font color=blur>2.30 seconds `user`：</font>**CPU 在用户态代码中运行的时间。通常是一些计算任务。
* <font color=blur>**0.08 seconds `sys`：**</font>CPU 在内核态花的时间。例如程序频繁执行系统调用 `open()/read()/write()/mmap()/socket()`，可能导致 `sys` 时间上升。

所以 user 很高更像是在做计算。而 sys 很高则可能需要关注：

* system calls
* I/O
* 网络
* 内存管理

### 1.6 实际分析时最值得看的几个组合

不要孤立地看指标。

我一般先看这几个关系。

**<font color=blur>情况 A：低 IPC + 高 cache miss</font>**

很可能 CPU 大量时间在等内存。

典型代码：

```cpp
sum += data[random_index];
```

可以检查：

* cache locality
* 数据布局
* 连续内存
* 减少随机访问

**<font color=blur>情况 B：低 IPC + 高 branch miss</font>**

例如：

```text
IPC              0.7
branch-misses    20%
```

可能 branch predictor 经常预测失败。

**<font color=blur>情况 C：高 context switches</font>**

可能要检查：

* mutex
* 线程池
* 线程数量
* condition variable
* blocking I/O

**<font color=blur>情况 D：cycles 和 instructions 都很高，但 IPC 正常</font>**

例如 IPC = 2.5，但程序还是慢。那么 CPU 本身可能没有明显 stall。可能单纯是**你做的工作太多了。**例如算法复杂度 $O(N²)$

这种情况下 `perf report` 往往更重要，因为你需要找到到底哪个函数执行了这么多工作？

### 1.7 速查表

| 指标               | 含义            | 重点                  |
| ------------------ | --------------- | --------------------- |
| `task-clock`       | CPU 使用时间    | 多线程尤其有用        |
| `cycles`           | CPU 时钟周期    | 和 instructions 配合  |
| `instructions`     | 执行的指令数    | 算 IPC                |
| `branches`         | 分支数量        | 和 branch-misses 配合 |
| `branch-misses`    | 分支预测失败    | 看失败率              |
| `cache-references` | cache 访问      | 和 misses 配合        |
| `cache-misses`     | cache 未命中    | 内存瓶颈线索          |
| `context-switches` | 上下文切换      | 多线程/锁             |
| `cpu-migrations`   | CPU 核迁移      | cache/NUMA            |
| `page-faults`      | 缺页异常        | 内存映射              |
| `time elapsed`     | 实际运行时间    | 最终性能              |
| `user`             | 用户态 CPU 时间 | 计算                  |
| `sys`              | 内核态 CPU 时间 | syscall/I/O           |

## 02 `perf top`：实时性能监控

类似于 Linux 的 `top` 命令，但 `perf top` 监控的是 **<font color=blue>函数级别</font>** 的 CPU 占用率。非常适合快速定位当前系统或某个进程的“热点（Hotspots）”函数。

**常见用法：**

```bash
# 监控整个系统的实时热点（需要 root 权限）
sudo perf top

# 仅监控特定 PID 的进程
sudo perf top -p <PID>
```

<font color=blur>在界面中，你可以按 `?` 查看快捷键，或者选中某个函数按 `Enter` -> `Annotate` 查看具体是哪行汇编代码消耗了 CPU。</font>

## 03 `perf record`：离线数据采集（Sampling）

<font color=blur>`stat` 告诉你“发生了什么”，而 `record` 能告诉你“具体发生在哪段代码里”。</font>它会以固定的频率（默认约 4000Hz）对 CPU 进行采样，记录当时的指令指针和函数调用栈，并将数据保存到当前目录的 `perf.data` 文件中。

**高频用法：**

```bash
# 采集特定程序的调用栈信息（-g 开启调用图记录）
perf record -g ./your_program

# 采集特定 PID，持续 10 秒
perf record -p <PID> -g -- sleep 10

# 采集整个系统 10 秒钟的数据
sudo perf record -a -g -- sleep 10
```

> **重要提示：** 为了让 `perf` 能够准确抓取程序的函数调用栈，编译你的代码（C/C++/Rust 等）时，强烈建议加上 `-g`（保留调试信息）和 `-fno-omit-frame-pointer`（保留帧指针）编译选项。

## 04 `perf report`：解析与可视化

有了 `perf.data` 文件后，使用 `report` 命令来交互式地分析采样结果。

**基本用法：**

```bash
# 读取当前目录下的 perf.data 并展示分析界面
perf report

# 如果采集时用了 -g，报告也会包含详细的调用栈缩进
perf report -g
```

你可以展开调用树，清晰地看到是哪个函数调用了哪个子函数，最终导致了高 CPU 占用。

## 05 应用

```cpp
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#if defined(__GNUC__) || defined(__clang__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

volatile uint64_t g_sink = 0;


// ============================================================
// 1. CPU-heavy
//    基本不访问大块内存，主要让 CPU 做整数运算
// ============================================================

NOINLINE uint64_t cpu_heavy(int scale)
{
    uint64_t x = 0x123456789abcdefULL;

    const uint64_t iterations =
        150'000'000ULL * static_cast<uint64_t>(scale);

    for (uint64_t i = 0; i < iterations; ++i) {
        // xorshift + 整数运算
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;

        x += i * 0x9e3779b97f4a7c15ULL;
    }

    return x;
}


// ============================================================
// 2. Memory-heavy
//    创建一个远大于 L1/L2 cache 的数组，然后随机访问
// ============================================================

NOINLINE uint64_t memory_heavy(int scale)
{
    constexpr size_t N = 1ULL << 24;   // 16M 个 uint64_t
                                        // 大约 128 MB

    std::vector<uint64_t> data(N);
    std::vector<uint32_t> indices(N);

    // 初始化数据
    for (size_t i = 0; i < N; ++i) {
        data[i] = i * 0x9e3779b97f4a7c15ULL;
    }

    // 构造伪随机访问序列
    uint32_t x = 123456789;

    for (size_t i = 0; i < N; ++i) {
        x = x * 1664525u + 1013904223u;
        indices[i] = x & (N - 1);
    }

    uint64_t sum = 0;

    // 重复进行随机内存访问
    const int rounds = 3 * scale;

    for (int r = 0; r < rounds; ++r) {
        for (size_t i = 0; i < N; ++i) {
            sum += data[indices[i]];
        }
    }

    return sum;
}


// ============================================================
// 3. Branch-heavy
//    通过随机数据制造难以预测的分支
// ============================================================

NOINLINE uint64_t branch_true(uint64_t x)
{
    return x * 3 + 1;
}

NOINLINE uint64_t branch_false(uint64_t x)
{
    return x * 7 + 3;
}

NOINLINE uint64_t branch_heavy(int scale)
{
    constexpr size_t N = 1ULL << 24;

    std::vector<uint8_t> data(N);

    uint32_t x = 123456789;

    // 生成近似随机的 0/1
    for (size_t i = 0; i < N; ++i) {
        x = x * 1664525u + 1013904223u;
        data[i] = static_cast<uint8_t>(x >> 31);
    }

    uint64_t sum = 0;

    const int rounds = 8 * scale;

    for (int r = 0; r < rounds; ++r) {
        for (size_t i = 0; i < N; ++i) {

            // 大约 50/50，分支预测器比较难预测
            if (data[i]) {
                sum += branch_true(i);
            } else {
                sum += branch_false(i);
            }
        }
    }

    return sum;
}


// ============================================================
// 辅助函数：测量每个 workload 的执行时间
// ============================================================

template <typename F>
void run_test(const std::string& name, F func)
{
    std::cout << "\n=== " << name << " ===\n";

    auto begin = std::chrono::steady_clock::now();

    uint64_t result = func();

    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed = end - begin;

    g_sink = g_sink ^ result;

    std::cout
        << "elapsed: "
        << elapsed.count()
        << " seconds\n";
}


// ============================================================

int main(int argc, char** argv)
{
    std::string mode = "all";
    int scale = 1;

    if (argc >= 2) {
        mode = argv[1];
    }

    if (argc >= 3) {
        scale = std::atoi(argv[2]);

        if (scale < 1) {
            scale = 1;
        }
    }

    std::cout << "mode  = " << mode << '\n';
    std::cout << "scale = " << scale << '\n';

    if (mode == "cpu" || mode == "all") {
        run_test("CPU heavy", [&] {
            return cpu_heavy(scale);
        });
    }

    if (mode == "memory" || mode == "all") {
        run_test("Memory heavy", [&] {
            return memory_heavy(scale);
        });
    }

    if (mode == "branch" || mode == "all") {
        run_test("Branch heavy", [&] {
            return branch_heavy(scale);
        });
    }

    std::cout << "\nsink = " << g_sink << '\n';

    return 0;
}
```

编译指令：不要直接 `-O0`，因为我们希望分析一个比较接近真实 Release 程序的情况：

```bash
g++ \
    -std=c++17 \
    -O2 \
    -g \
    -fno-omit-frame-pointer \
    perf_demo.cpp \
    -o perf_demo
```

* `-O2`：保留优化，模拟真实程序
* `-g`：保留符号信息，让 perf 能看到函数名、源码
* `-fno-omit-frame-pointer`：让 perf 更容易恢复调用栈

```bash
./perf_demo all
```

会依次运行：

```
CPU heavy
Memory heavy
Branch heavy
```

也可以单独运行：

```bash
./perf_demo cpu
./perf_demo memory
./perf_demo branch
```

第二个参数可以增加工作量：

```bash
./perf_demo all 5
```

`5` 大概就是把主要 workload 增加到原来的 5 倍，方便测试 `perf top`。

### 5.1 第一组实验 `perf_stat`

先执行：

```bash
perf stat ./perf_demo all
```

然后推荐：

```bash
perf stat -d ./perf_demo all
```

`-d` 会显示更多硬件性能指标。

你应该会看到类似：

```
Performance counter stats for './perf_demo all':


    8,123,456,789      cycles
    9,876,543,210      instructions
      123,456,789      branches
       12,345,678      branch-misses
       ...
```

#### CPU workload

```bash
perf stat -d ./perf_demo cpu
```

重点观察：

```
cycles
instructions
instructions per cycle
```

它主要是在不停执行：

```
x ^= x << 13;
x ^= x >> 7;
x ^= x << 17;
```

所以属于：

```
CPU
 ↓
ALU / integer operations
 ↓
instructions
cycles
```

#### Memory workload

```bash
perf stat -d ./perf_demo memory
```

重点观察：

```
cache-references
cache-misses
```

因为里面有：

```cpp
sum += data[indices[i]];
```

而：

```cpp
indices[i]
```

基本是随机的。

所以访问模式类似：

```
data[100]
data[873429]
data[1283812]
data[33]
data[9128372]
...
```

CPU 硬件预取器很难预测下一次访问哪里。

因此可能出现：

```
CPU
 │
 ├─ L1 miss
 │
 ├─ L2 miss
 │
 ├─ L3 miss
 │
 └─ DRAM
```

这个实验很适合理解：

> **为什么 CPU 很快，但程序依然可能被内存拖慢。**

#### Branch workload

运行：

```bash
perf stat -e \
cycles,instructions,branches,branch-misses \
./perf_demo branch
```

重点看：

```
branches
branch-misses
```

因为程序不断执行：

```cpp
if (data[i]) {
    ...
} else {
    ...
}
```

而 `data[i]` 接近随机的：

```
1
0
1
1
0
0
1
0
...
```

CPU 很难预测：

```
下一次到底走 if
还是走 else？
```

所以应该能看到比较明显的：

```
branch-misses
```

### 5.3 第二组实验：`perf record`

这是更重要的一组。

执行：

```bash
perf record -g ./perf_demo all
```

结束后当前目录会出现：

```
perf.data
```

然后：

```bash
perf report
```

你应该能够看到类似：

```
Overhead  Command    Shared Object    Symbol


  35.xx%  perf_demo  perf_demo        branch_heavy(int)
  30.xx%  perf_demo  perf_demo        memory_heavy(int)
  25.xx%  perf_demo  perf_demo        cpu_heavy(int)
   ...
```

具体百分比会取决于你的 CPU。

这里就开始体现 `perf report` 的真正价值了：

```
程序运行 5 秒


你只知道：
    "程序慢"


            ↓ perf record


perf 告诉你：


branch_heavy   40%
memory_heavy   35%
cpu_heavy      20%
其他            5%
```

于是热点一下就出来了。

#### 展开调用栈

因为我们用了：

```
perf record -g
```

所以进入：

```
perf report
```

后可以展开调用链。

例如 branch 部分可能看到：

```
main
└── run_test
    └── branch_heavy
        ├── branch_true
        └── branch_false
```

memory 部分：

```
main
└── run_test
    └── memory_heavy
```

CPU 部分：

```
main
└── run_test
    └── cpu_heavy
```

这就是以后分析真实 C++ 项目最重要的能力之一：

> **不仅要知道哪个函数慢，还要知道是谁调用它导致它慢。**

#### 再试试 `perf annotate`

先：

```bash
perf record ./perf_demo cpu
```

然后：

```bash
perf annotate
```

你可以进一步看到某个函数内部：

```
cpu_heavy()
```

到底哪几条机器指令最热。

也就是说分析粒度从：

```
程序
 ↓
函数
```

继续下降到：

```
程序
 ↓
函数
 ↓
源代码
 ↓
汇编指令
```

这是 `perf` 很强的地方。

#### `perf top` 也可以测试

因为程序默认可能执行得不够久，所以把 workload 放大：

```bash
./perf_demo all 20 &
```

拿到 PID：

```bash
PID=$!
```

然后：

```bash
perf top -p $PID
```

或者看调用栈：

```bash
perf top -g -p $PID
```

你就能实时看到：

```
Overhead
   │
   ├── cpu_heavy
   ├── memory_heavy
   ├── branch_heavy
   └── ...
```

随着程序进入不同阶段，最热的函数也会发生变化。

## 06 火焰图

你已经知道：

```bash
perf record -g ./perf_demo
perf report
```

`perf report` 可以给你调用树，例如：

```
main
└── process()
    ├── update()
    │   └── calculate()
    │       └── memcpy()
    └── search()
        └── compare()
```

问题是，真实大型程序可能有：

* 几百个线程
* 几千个函数
* 几万条调用路径
* 几十万甚至几百万个 stack samples

这时候 `perf report` 的树会非常难读。

火焰图就是把大量 stack sample 变成：

```
               ┌── memcpy ────────┐
        ┌──── calculate ───────────┐
        │                          │
   ┌────── update ────────────────┐│
   │                              ││
   │        ┌── compare ─────┐    ││
   │   ┌────── search ───────┐    ││
┌──────────── process ────────────────┐
┌──────────────── main ───────────────┐
```

然后你主要观察 **哪些“火焰”最宽。**

因为宽度代表这个调用路径在采样数据中出现得更多。

### 6.1 火焰图三个维度

***<font color=blue>Y 轴：调用栈深度</font>***

经典 CPU 火焰图：

```
↑
│           foo()
│      calculate()
│        update()
│       process()
│         main()
└────────────────────→
```

从下往上表示：下面是 caller，上面是 callee。

<font color=blur>所以，Y 轴只表示 stack depth，**不代表执行时间。**</font> 因此很高但是很窄的塔只是说明调用层级很深，不代表它特别耗 CPU。

***<font color=blue>X 轴：最容易被误解</font>***

很多人第一次看火焰图，会以为左边是程序开始，右边是程序结束。这是错误的。<font color=blur>经典 Flame Graph 的 X 轴**不是时间轴。**</font>

<font color=blur>调用栈通常会被排序并合并，相似调用路径放在一起，所以左右位置本身没有“先后执行”的意义。</font>

比如：

```
|---- foo ----|------ bar ------|
```

不能解释成先运行 `foo`，然后运行 `bar`。它只是所有 foo 相关 stack 放在一起，所有 bar 相关 stack 放在一起。

如果你真正想看：

```
10:00 运行什么
10:01 运行什么
10:02 运行什么
```

那是 **<font color=blur>Flame Chart / 时间序列分析</font>**，不是经典 Flame Graph。

***<font color=blue>最重要的是宽度</font>***

假设：

```
┌────────────── main ───────────────┐
┌──────────── process ───────────────┐
│                                      │
├─── calculate ────┐ ┌─ search ┐ ┌ IO ┐
│                   │ │         │ │    │
```

其中 `calculate` 占整张图 50%，大致表示约 50% 的采样调用栈包含了 `calculate()`。所以函数框越宽表示出现于采样调用栈中的比例越高。<font color=blur>对于普通 **CPU cycles/on-CPU sampling** 来说，它通常可以近似理解成占用 CPU 时间越多。</font>

但这里有一个非常重要的细节。<font color=blur>宽不等于这个函数自己慢。</font>

假设火焰图：

```
                 ┌────── foo ───────┐
                 │                   │
        ┌────── calculate ──────────┐
        │                            │
┌────────────── process ──────────────────────────┐
┌────────────── main ─────────────────────────────┐
```

`process()` 很宽，但不能说 `process()` 自己占用了大量 CPU，因为它可能只是：

```cpp
void process() {
    calculate();
}
```

真正 CPU 消耗是在

```
calculate()
   ↓
 foo()
```

火焰图中一个框的总宽度更接近 **inclusive cost**：自己和所有子调用。

所以：

```
process
██████████████████████


calculate
██████████████████


foo
███████████████
```

说明：大量 CPU 工作发生在 process 这条调用链下面，而不意味着 `process()` 本身那些指令特别慢。

### 6.2 怎么看函数自己消耗 CPU

看它的**顶部裸露区域**。

例如：

```
             ┌──── foo ────┐
             │             │
┌──────────── calculate ──────────────────┐
```

`calculate`：

```
总宽度
██████████████████████████████
```

但它的上面：

```
             ███████████
```

只有一部分被 `foo` 覆盖。

没有 child 覆盖的：

```
████████████         █████████
```

这些区域对应采样时 `calculate()`，自己就是调用栈顶部，也就是 CPU 当时直接执行在 `calculate()` 里面。

因此一个非常好的阅读习惯是：**先找顶部最宽的“平顶”，再往下追它的调用来源。**

Brendan Gregg 对经典 CPU flame graph 的描述也是：最上缘表示实际被采样到正在 CPU 上运行的函数，下面则是它的调用 ancestry。

### 6.3 例子

假设你的程序：

```cpp
int main() {
    while (...) {
        process();
    }
}
```

里面：

```cpp
void process() {
    filter();
    search();
    update();
}
```

火焰图：

```
                       ┌─ memcpy ───────────┐
                 ┌────── copyData ──────────┐
                 │                           │
       ┌──────────── search ───────────────┐
       │                                     │
       │                       ┌─ update  ─┐ │
       │                       │           │ │
┌──────────────── process ────────────────────────┐
┌───────────────── main ──────────────────────────┐
```

第一反应不要是 `process()` 最宽，所以优化 `process()`。而应该看最宽的顶部是谁？

发现：

```
memcpy
██████████████████
```

然后往下看：

```
main
 ↓
process
 ↓
search
 ↓
copyData
 ↓
memcpy
```

于是得到非常重要的信息：大量 CPU 时间花在 `search()` 路径下的数据复制。

那么你接下来可能去查：

```cpp
std::vector<Data> temp = data;
```

或者：

```cpp
memcpy(...)
```

或者：

```cpp
return big_vector;
```

而不是漫无目的地优化 `process()`。