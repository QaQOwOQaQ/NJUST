// 严格符合 C++ 标准库实现的 scoped_lock
// 核心思路：一个主模板+两个特化（针对管理一个锁和无锁的情况）
//          使用 tuple 管理多个锁
//          构造时加锁，析构时解锁，提供 adopt_lock 版本的构造函数（无需加锁）
// 难点主要在于如何实现对多个锁的“原子”加锁，标准库提供了 std::lock
// 但我们肯定要自己实现，这里的思路也是仿照 std::lock，使用递归加锁
// 从第一个锁开始递归加锁，如果失败就全部回退（解锁）

#include <atomic>
#include <cassert>
#include <chrono>
#include <initializer_list>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

/*===================================================*/
/*                   Scoped lock                     */
/*===================================================*/
namespace my_std_like {
namespace detail {

template<int Index>
struct TryLockImpl {
    template<typename... Mutexes>
    static void DoTryLock(std::tuple<Mutexes&...> mtxes, int& index) {
        index = Index;
        auto lock = std::unique_lock(std::get<Index>(mtxes), std::try_to_lock);
        if(lock.owns_lock()) {
            if constexpr(Index + 1 < sizeof...(Mutexes)) {
                TryLockImpl<Index + 1>::DoTryLock(mtxes, index);
            }
            else {
                index = -1;
            }
            // 如果加锁成功，unique_lock 就在这里递归放弃对锁的持有权
            if(index == -1) {
                lock.release();
            }
        }
    }
};

template<typename Mutex1, typename Mutex2, typename... Mutex3>
void Lock(Mutex1& m1, Mutex2& m2, Mutex3&... m3) {
    while(true) {
        // 先获取第一把锁，再对后面的锁递归加锁
        std::unique_lock<Mutex1> first(m1);
        int index = 0;
        if constexpr (sizeof...(Mutex3)) {
            auto locks = std::tie(m2, m3...);
            TryLockImpl<0>::DoTryLock(locks, index);
        }
        // 如果只有两个锁，特殊处理：不需要递归
        else {
            auto lock = std::unique_lock<Mutex2>(m2, std::try_to_lock);
            if(lock.owns_lock()) {
                index = -1;
                lock.release();
            }
        }
        if(index == -1) {
            first.release();
            return ;
        }
    }
}
}  // namespace detail


// forward declaration
template<typename... Mutexes>
class scoped_lock;

// --- 0 mutex --- 
template<>
class scoped_lock<> {
public:
    scoped_lock() noexcept = default;
    ~scoped_lock() noexcept = default;

    scoped_lock(const scoped_lock&) = delete;
    scoped_lock& operator=(const scoped_lock&) = delete;
    scoped_lock(scoped_lock&&) = delete;
    scoped_lock& operator=(scoped_lock&&) = delete;
};

// --- 1 mutex ---
template<typename Mutex>
class scoped_lock<Mutex> {
public:
    using mutex_type = Mutex;

    explicit scoped_lock(Mutex &mtx) noexcept : mtx_(mtx) { mtx_.lock(); }
    scoped_lock(std::adopt_lock_t, Mutex &mtx) noexcept : mtx_(mtx) { }
    ~scoped_lock() { mtx_.unlock(); };

    scoped_lock(const scoped_lock&) = delete;
    scoped_lock& operator=(const scoped_lock&) = delete;
    scoped_lock(scoped_lock&&) = delete;
    scoped_lock& operator=(scoped_lock&&) = delete;
private:
    Mutex& mtx_;
};

// --- 2+ mutex  ---
template<typename... Mutexes>
class scoped_lock {
public:
    scoped_lock(Mutexes&... mtxes) : mtxes_(mtxes...) { 
        detail::Lock(mtxes...); 
    }
    scoped_lock(std::adopt_lock_t, Mutexes&... mtxes) : mtxes_(mtxes...) {}
    ~scoped_lock() { 
        std::apply([](auto&... ms) {
            (..., (void)ms.unlock());
        }, mtxes_);
    }

    scoped_lock(const scoped_lock&) = delete;
    scoped_lock& operator=(const scoped_lock&) = delete;
    scoped_lock(scoped_lock&&) = delete;
    scoped_lock& operator=(scoped_lock&&) = delete;

private:
    std::tuple<Mutexes&...> mtxes_;
};



}  // namespace my_std_like



/*===================================================*/
/*                      TEST                         */
/*===================================================*/
static void require(bool cond, const char* msg) {
    if (!cond) {
        std::cerr << "TEST FAILED: " << msg << "\n";
        std::abort();
    }
}

struct SpinBarrier {
    explicit SpinBarrier(int n) : total(n), arrived(0), go(false) {}
    void wait() {
        int prev = arrived.fetch_add(1, std::memory_order_acq_rel);
        if (prev + 1 == total) {
            go.store(true, std::memory_order_release);
        } else {
            while (!go.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
        }
    }
    int total;
    std::atomic<int> arrived;
    std::atomic<bool> go;
};

// A tiny mutex wrapper that records lock/unlock order (for single-threaded
// order testing). Not for concurrent correctness testing; it just forwards to
// std::mutex.
struct TracedMutex {
    explicit TracedMutex(int id, std::vector<int>* log) : id(id), log(log) {}
    void lock() {
        m.lock();
        if (log) log->push_back(+id);  // +id means lock
    }
    void unlock() noexcept {
        if (log) log->push_back(-id);  // -id means unlock
        m.unlock();
    }
    bool try_lock() {
        bool ok = m.try_lock();
        if (ok && log) log->push_back(+id);
        return ok;
    }
    std::mutex m;
    int id;
    std::vector<int>* log;
};

// ================================================================
// 3) Tests
// ================================================================
static void test_compile_shape() {
    // No runtime needed; just ensuring basic instantiation works.
    std::mutex m;
    my_std_like::scoped_lock<> a;
    my_std_like::scoped_lock b(m);  // CTAD: scoped_lock<mutex>
    (void)a;
    (void)b;
}

static void test_single_mutex_exclusion() {
    std::mutex m;
    int counter = 0;

    constexpr int threads = 8;
    constexpr int iters = 20000;

    std::vector<std::thread> ts;
    ts.reserve(threads);

    for (int t = 0; t < threads; ++t) {
        ts.emplace_back([&] {
            for (int i = 0; i < iters; ++i) {
                my_std_like::scoped_lock lock(m);
                ++counter;
            }
        });
    }
    for (auto& th : ts) th.join();

    require(counter == threads * iters,
            "single mutex: counter mismatch (lost increments)");
}

static void test_multi_mutex_deadlock_avoidance() {
    std::mutex a, b;
    std::atomic<int> ok{0};

    // Two threads lock in opposite "argument orders" to stress deadlock
    // avoidance.
    std::thread t1([&] {
        for (int i = 0; i < 20000; ++i) {
            my_std_like::scoped_lock lock(a, b);
            ok.fetch_add(1, std::memory_order_relaxed);
        }
    });

    std::thread t2([&] {
        for (int i = 0; i < 20000; ++i) {
            my_std_like::scoped_lock lock(b, a);
            ok.fetch_add(1, std::memory_order_relaxed);
        }
    });

    t1.join();
    t2.join();

    require(ok.load() == 40000,
            "multi mutex: unexpected loop count (possible hang or missed)");
}

static void test_adopt_lock() {
    std::mutex a, b;

    a.lock();
    b.lock();
    {
        my_std_like::scoped_lock lock(std::adopt_lock, a, b);
        // should NOT deadlock; should just manage unlock on scope exit
    }
    // After scope, both should be unlocked; if still locked, try_lock would
    // fail.
    require(a.try_lock(), "adopt_lock: mutex a not unlocked");
    a.unlock();
    require(b.try_lock(), "adopt_lock: mutex b not unlocked");
    b.unlock();
}

static void test_contention_many_mutexes() {
    // Stress: 3 mutexes, many threads, randomized access.
    std::mutex a, b, c;
    std::atomic<long long> sum{0};

    constexpr int threads = 12;
    constexpr int iters = 15000;

    SpinBarrier bar(threads);
    std::vector<std::thread> ts;
    ts.reserve(threads);

    for (int t = 0; t < threads; ++t) {
        ts.emplace_back([&, t] {
            std::mt19937 rng(12345u + (unsigned)t);
            bar.wait();
            for (int i = 0; i < iters; ++i) {
                // Shuffle argument order to further stress std::lock path
                int r = (int)(rng() % 6);
                switch (r) {
                    case 0: {
                        my_std_like::scoped_lock lk(a, b, c);
                        break;
                    }
                    case 1: {
                        my_std_like::scoped_lock lk(a, c, b);
                        break;
                    }
                    case 2: {
                        my_std_like::scoped_lock lk(b, a, c);
                        break;
                    }
                    case 3: {
                        my_std_like::scoped_lock lk(b, c, a);
                        break;
                    }
                    case 4: {
                        my_std_like::scoped_lock lk(c, a, b);
                        break;
                    }
                    case 5: {
                        my_std_like::scoped_lock lk(c, b, a);
                        break;
                    }
                }
                // Under lock, update sum deterministically-ish
                sum.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    for (auto& th : ts) th.join();

    require(sum.load() == 1LL * threads * iters,
            "contention: sum mismatch (hang or missed)");
}

int main() {
    std::cout << "Running tests...\n";

    test_compile_shape();
    std::cout << "  [OK] basic instantiation\n";

    test_single_mutex_exclusion();
    std::cout << "  [OK] single mutex exclusion\n";

    test_multi_mutex_deadlock_avoidance();
    std::cout << "  [OK] multi mutex deadlock avoidance\n";

    test_adopt_lock();
    std::cout << "  [OK] adopt_lock\n";

    test_contention_many_mutexes();
    std::cout << "  [OK] stress with 3 mutexes\n";

    std::cout << "All tests passed.\n";
    return 0;
}
