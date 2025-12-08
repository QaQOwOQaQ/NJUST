# V1: 简易单任务队列线程池

https://github.com/progschj/ThreadPool/tree/master

用现代的C++标准库（线程+锁+条件变量）实现一个单任务队列的线程池非常简单。就像之前那篇博文里面讲的一样，原理非常简单，对新手而言最复杂的其实就是 C++11 众多的新语言特性罢了。

一个简易的单任务队列线程池的实现思路：在线程池构造时初始化线程数，在析构时停止线程池。对外也只需要提供提交任务的接口就够了。

## Q1. 为什么对 `stop_` 的访问要加锁？

你观察得非常仔细！没错，在 **Worker 线程（消费者）** 的代码里，访问 `stop_` 确实是在 `lock` 保护下的：

```CPP
// Worker 线程
std::unique_lock<std::mutex> lock(queue_mtx_); // 1. 这里加锁了
cond_.wait(lock, [this]{return stop_ || !tasks_.empty();}); // 2. 这里在锁的保护下读取 stop_
```

但是，多线程编程中有一个核心原则：**锁必须是“读”和“写”双方共同遵守的协定。**

如果 **Worker 线程（读方）** 加了锁，而 **主线程析构函数（写方）** 不加锁，那么 Worker 的锁就形同虚设。

#### 技术逻辑：为什么写端必须也要加锁？

在 C++ 中，`std::mutex` 只有在**所有**访问共享变量的线程都去 `lock` 它的时候才起作用。

如果你在析构函数里写成这样（错误的写法）：

```CPP
// ❌ 错误：析构函数不加锁
ThreadPool::~ThreadPool() {
    stop_ = true;     // 1. 无视 Worker 的锁，直接写内存
    cond_.notify_all(); 
    // ...
}
```

这里会发生严重的逻辑漏洞（即**丢失唤醒**），请看这个致命的时序：

1. **Worker 线程**：抢到了锁，检查 `stop_` 是 `false`。
2. **Worker 线程**：准备调用底层 API 进入睡眠（Wait），但在真正睡着之前，CPU 发生了上下文切换。
   - *注意：此时 Worker 即使持有逻辑上的锁，但在进入 wait 的临界瞬间，状态是脆弱的。*
3. **主线程（析构）**：**没有去拿锁**，直接把 `stop_` 改成了 `true`，并大喊一声 `notify_all`。
   - 因为主线程没拿锁，它不需要等 Worker 释放锁，它直接就执行了。
4. **Worker 线程**：切回执行。它还停留在“准备去睡”的指令上。它完全不知道刚才有人喊过它是要退出了。于是它**真的睡着了**。
5. **结果**：主线程在 `join` 等 Worker，Worker 在睡大觉。**死锁。**

#### 正确的“加锁”如何防止上述问题？

如果在析构函数里加了锁：

```CPP
// ✅ 正确：析构函数加锁
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mtx_); // 尝试拿锁
        stop_ = true; 
    } // 释放锁
    cond_.notify_all();
    // ...
}
```

此时的时序变成了：

1. **Worker 线程**：拿着锁，准备去睡。
2. **主线程（析构）**：尝试 `lock(queue_mtx_)`。
   - **被阻塞！** 因为 Worker 还拿着锁。主线程被迫等待。
3. **Worker 线程**：调用 `wait`，进入睡眠状态，**同时释放锁**。
4. **主线程**：终于拿到了锁。修改 `stop_ = true`，释放锁，`notify_all`。
5. **Worker 线程**：被唤醒，重新拿锁，检查 `stop_`，发现是 `true`，退出。

#### 总结

你在 Worker 里的加锁是**完全正确且必要**的。 我之所以强调在**析构函数**里也要加锁，是因为：

**锁的作用是“互斥”。如果只有一个人去抢锁，另一个人无视锁直接操作内存，那就根本不存在“互斥”，锁也就失效了。**

## Q2. 为什么构造函数和析构函数要加 inline

给这两个函数加上 `inline` 关键字时，不仅仅是建议编译器进行内联优化，更重要的是**修改了链接规则**。

`inline` 告诉链接器：“这个函数可能会在多个目标文件中被定义多次。这是被允许的，请不要报错。你只需要随便选其中一个保留，把其他的丢掉就可以了。”

之所以不对 `push_task` 添加 inline，是因为 C++ 标准规定：**模板函数的定义允许多次出现（只要它们是一样的）**。链接器会自动处理模板的去重。所以模板函数通常写在头文件里且不需要显式加 `inline`。

## Q3. 为什么 `packaged_task` 要用 `make_shared`？

这是一个非常深刻的 C++ 细节问题。

简单来说，使用 `std::make_shared`（即使用 `std::shared_ptr`）的主要原因是：**为了解决“不可复制”与“需要复制”之间的矛盾。**

具体的矛盾在于以下两点：

1. **`std::packaged_task` 是不可复制的（Move-only）。**
2. **`std::function` 要求其包裹的对象必须是可复制的（CopyConstructible）。**

我们来详细拆解一下这个逻辑链条：

#### 矛盾源头：`std::packaged_task` 的特性

`std::packaged_task` 拥有一个共享状态（Shared State），用于在该任务执行结束后，通过 `std::future` 拿到返回值。 为了保证这个状态的所有权清晰，C++ 标准规定 **`std::packaged_task` 不能被复制，只能被移动（move）**。

```cpp
std::packaged_task<int()> task1([]{ return 1; });
std::packaged_task<int()> task2 = task1; // ❌ 编译错误！禁止复制
std::packaged_task<int()> task3 = std::move(task1); // ✅ 允许移动
```

#### 容器限制：`std::function` 的特性

你的线程池中定义了任务队列：

```cpp
std::queue<std::function<void()>> tasks_;
```

这里用到了 `std::function`。在 C++23 之前，**`std::function` 要求它所存储的可调用对象必须是“可复制构造”的**。即便你实际上只对其进行移动操作，`std::function` 的内部实现机制（类型擦除）仍然要求该类型具备复制的能力。

#### 如果不用 `shared_ptr` 会发生什么？

假设我们不使用 `shared_ptr`，直接把 `packaged_task` 放入 Lambda 表达式中：

```cpp
// 假设 task 是一个 std::packaged_task 对象
// 我们试图把它 move 进 lambda
auto lambda = [t = std::move(task)]() mutable { 
    t(); 
};
```

此时，这个 `lambda` 对象因为捕获了一个“不可复制”的 `task`，所以这个 `lambda` 自身也变成了**不可复制（Move-only）** 的。

当你尝试把这个 `lambda` 塞进 `std::function` 时：

```cpp
std::function<void()> func = std::move(lambda); // ❌ 编译错误
```

编译器会报错，因为 `std::function` 拒绝接收一个不可复制的 Lambda。

#### 解决方案：`std::shared_ptr`

为了绕过这个限制，我们使用 `std::shared_ptr` 来包裹 `packaged_task`。

- **`std::shared_ptr` 是可以复制的**（复制它只是增加引用计数，开销极小）。
- 当我们把 `shared_ptr` 捕获进 Lambda 时，Lambda 捕获的是指针的副本。
- 因此，这个 **Lambda 也就变成了可复制的**。
- `std::function` 看到 Lambda 可复制，就很高兴地接受了它。

```cpp
// 1. 在堆上创建 task，用 shared_ptr 管理
auto task_ptr = std::make_shared<std::packaged_task<int()>>(...);

// 2. Lambda 捕获的是 shared_ptr（按值复制指针）
// 这个 Lambda 依然是可复制的！
auto lambda = [task_ptr](){ 
    (*task_ptr)(); 
};

// 3. 放入 std::function，编译通过 ✅
tasks_.emplace(lambda);
```

#### 为什么用 `make_shared` 而不是 `new`？

虽然 `std::shared_ptr<T>(new T(...))` 也能解决上述的“复制”问题，但推荐使用 `std::make_shared`，原因有两个：

1. **性能优化**：
   - `new` 写法：需要两次内存分配（一次分配对象 `T`，一次分配 `shared_ptr` 的控制块）。
   - `make_shared` 写法：只需要**一次内存分配**（它把对象和控制块分配在同一块连续内存中）。
2. **异常安全**：
   - 防止在分配内存和创建智能指针之间发生异常导致内存泄漏（虽然在这个特定场景下不太明显，但这是通用最佳实践）。

#### 总结

- **问题**：`std::packaged_task` 不能复制，但 `std::function` 需要可复制的对象。
- **解决**：使用 `std::shared_ptr` 把“所有权管理”变成了“引用计数管理”，使得包裹它的 Lambda 变成可复制的，从而能安全地存入 `std::queue<std::function<void()>>` 中。
- **注**：如果你使用的是 **C++23**，引入了 `std::move_only_function`，那么就不再需要 `shared_ptr` 了，可以直接移动 `packaged_task`。但对于 C++11/14/17/20，`make_shared` 是标准解法。

# V2: 重构 V1，分离队列代码，编写线程安全任务队列

在 V1 中，对任务队列的并发读写保护逻辑是在线程池内部实现的，因此我们可以考虑将这部分代码拿出来，封装到一个“线程安全任务队列”，这样线程池的实现就会更简洁。

# V3: 简易多任务队列线程池

在单任务队列线程池中，所有线程共用一个任务队列。而多任务队列线程池就是每个线程对应着一个自己的任务队列。

在这里我们继续使用之前编写的线程安全（阻塞）任务队列。

对于提交的任务，我们使用轮询调度分发任务，具体的，我们使用一个游标去记录下一个任务应该交给哪个任务队列，为了并发安全和访问效率，这个游标需要设置成原子变量。

另外需要注意的是，由于我们的 `SafeQueue` 数据成员包含锁和条件变量（不可移动和赋值），因此该类也是不可移动和复制的。而 `vector` 有要求所存储元素必须是可移动或可复制的（动态扩容），因此不能直接用 `vector` 存储 `SafeQueue`，意一个简单的解决办法就是使用指针。

# V4: 功能完备的线程池

之前我们通过 V1、V2、V3 了解了设计一个线程池的基本思路，接下来我们就要为线程池拓展各种各样的功能了。这里我们将实现一个基于 C++11，功能详备的线程池。代码思路参考自：[github.com/jencoldeng](https://github.com/jencoldeng/ThreadPool/tree/master)。

## Q1: `priority_queue::top` 返回的是 const reference

`std::priority_queue::top()` 返回的是 **const 引用** 

更精确一点：

```cpp
const_reference top() const;
```

也就是说：

- 你拿到的是 `const T&`
- **不能通过这个引用去改里面的元素**

这所以这样设计，是因为 `priority_queue` 内部是个堆结构，它有个重要不变量：

* 堆顶元素（`top()`）永远是“最大/最优”的那个。

如果允许你通过 `top()` 拿到一个 **可写引用**，你可以这样干：

```cpp
auto& x = pq.top();
x = something_smaller; // 直接改元素的值
```

这样堆就乱了：内部没有重新做调整（heapify），但元素值变了，**结构还是原来的结构**，堆的性质就被你破坏了。

所以标准库直接禁止这事：

* 顶元素只读，想改就先 `pop()`，然后重新 `push()` 一个新的。

因此，我们不可以这样编写代码：

``` cpp
// 源代码
if(!delay_tasks_.empty() && delay_tasks_.top().exec_tm_ <= now) {
    TimeTask top_copy = delay_tasks_.top();
    delay_tasks_.pop();
    out_task = std::move(top_copy.task_);
    return PopResult::OK;
}

// 错误的优化
if(!delay_tasks_.empty() && delay_tasks_.top().exec_tm_ <= now) {
	out_task = std::move(delay_tasks_.top().task_); // 实际上执行 copy
    delay_tasks_.pop();
    return PopResult::OK;
}
```

## Q2: `ThreadPool::clean_inactive_threads` 中为什么使用 `try_to_lock`？

核心原因一句话：**这是一个“打扫卫生”的辅助函数，不值得为它在锁上阻塞，更不想跟 stop() 那种关键路径互相卡住，甚至死锁。**

我拆开讲下用 `try_to_lock` 的几个目的——你会更有感觉：

### 1. 清理线程不是“必须立刻完成”的事

`clean_inactive_threads()` 做的是：

> 把 `dead_workers_` 里的 `std::thread` 挪出来，然后在不持锁的情况下 `join()` 一下。

这类操作的特点：

- 不做它，程序短时间内也没问题（只是迟点回收资源）
- 做它可能会比较“重”（`join` 有时会等一阵）

因此它适合作为**尽力而为（best-effort）**的后台维护逻辑，而不是强同步点。

用 `std::try_to_lock` 的语义是：

```cpp
std::unique_lock<std::mutex> lock(thread_mutex_, std::try_to_lock);
if (!lock.owns_lock()) {
    // 锁忙着呢，那算了，我先不打扰
    return;
}
```

也就是说：

> “我先试着拿一下锁，要是拿不到就算了，下次再清。”

这样就不会因为“打扫卫生”而把其他重要操作堵在锁前面。

### 2. 避免与 stop() 等关键路径长时间锁竞争甚至死锁

典型 ThreadPool 里，`thread_mutex_` 可能在这些地方使用：

- 新建 worker 线程
- 把退出的线程放进 `dead_workers_`
- `stop()`/`shutdown()` 中遍历 worker 状态、发退出信号等
- 你的 `clean_inactive_threads()` 清理线程对象

如果 `clean_inactive_threads()` 用的是阻塞式加锁，比如：

```cpp
std::unique_lock<std::mutex> lock(thread_mutex_);
```

那可能发生这种情况：

1. 某线程在执行 `stop()`，持有 `thread_mutex_`，里面做一堆操作，耗时较长
2. 其他线程调用 `clean_inactive_threads()`，在锁上**阻塞**
3. 这会导致：
   - stop() 想完成 shutdown，但有别的逻辑卡在锁请求中，可能相互牵扯
   - 如果某个 worker 退出前还会触发清理逻辑再拿同一把锁，甚至有死锁风险

而用 `try_to_lock`：

- 如果此刻 `stop()` 正拿着锁在干“正事”，`clean_inactive_threads()` 发现拿不到锁就直接返回
- 清理工作可以等下一次再做，不会妨碍 stop() 收尾

### 3. 避免拿着锁去做可能很慢的操作（join）

你的代码已经做了一个很好的拆分：

1. 持锁：

   ```cpp
   local_dead.splice(local_dead.begin(), dead_workers_);
   ```

2. 解锁后：

   ```cpp
   for (auto& t : local_dead)
       if (t.joinable()) t.join();
   ```

也就是说：**你已经意识到 join 不应该在持锁状态下做**。

再加上 `try_to_lock`，进一步做到：

- 如果锁现在被别的关键逻辑占用（比如 stop），你连 “拿锁 + splice” 这点小工作都先不做，干脆让别人先忙完。

这是一种“锁使用最小打扰”的设计策略：

> 能不抢就不抢，能快点干完就快点走，长时间的事情就在锁外慢慢做。

### 4. 设计上的取舍：用 try_to_lock 的前提

使用 `try_to_lock` 的前提是：**你能接受这次什么都不做**。

换句话说，你自己要认同这件事：

> “清一次 dead_workers_ 不成问题，如果没清，这些线程对象晚一点再清也行。”

如果在你这个 ThreadPool 的场景里：

- `dead_workers_` 不清理不会导致功能错误，只是资源延迟回收
- `clean_inactive_threads()` 大多是定期/偶尔被调用的维护函数

那用 `try_to_lock` 就非常合适。

反之，如果这是一个“必须保证本次一定完成”的操作，那就不应该用 `try_to_lock`，而应该老老实实阻塞拿锁。

### 5. 总结成一句话

> 用 `try_to_lock` 是为了让 **清理死线程这个“辅助性操作”始终是非侵入/不打扰的**：拿到锁就顺手清一下，拿不到锁就立刻返回，不会跟 stop() 等关键路径抢锁，更不会因为清理工作导致长时间阻塞或潜在的死锁。

如果你把 `stop()` 的代码也贴出来，我可以帮你一起看看：

- 现在的锁策略是否完全安全
- 有哪些场景下确实会跟 `clean_inactive_threads()` 产生竞争，进而佐证用 `try_to_lock` 是合理的。

# TODO

[1] [BS::thread_pool](https://github.com/bshoshany/thread-pool)

[2] [CThreadPool](https://github.com/ChunelFeng/CThreadPool?tab=readme-ov-file)

[3] [CGraph](https://github.com/ChunelFeng/CGraph)

[4] [手撸一款简单高效的线程池（一）](https://mp.weixin.qq.com/s?__biz=MzIxNjA5ODQ0OQ==&mid=2654703194&idx=1&sn=5b411b7d8a3a552e41a6a5a97fa4eeec&chksm=8c411da4bb3694b288625a0baa18d88bdff36a3136f9d0d7fb1d1a43e13ca00dfa053765ac23&scene=178&cur_album_id=1384528806857539584&search_click_id=#rd)