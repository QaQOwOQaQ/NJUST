#pragma once 

#include <mutex>
#include <tuple>
#include <utility>
#include <type_traits>
#include <thread>

namespace my_sync_lib::detail {

    template<typename Tuple, std::size_t Index, std::size_t N>
    struct TryLockRecursiveImpl {
        static bool do_try(Tuple& t) {
            using Mutex = std::remove_reference_t<
                std::tuple_element_t<Index, Tuple>
            >;

            std::unique_lock<Mutex> first(std::get<Index>(t), std::try_to_lock);
            if(!first.owns_lock()) {
                return false;
            }

            if constexpr (Index + 1 < N) {
                if(!TryLockRecursiveImpl<Tuple, Index + 1, N>::do_try(t)) {
                    return false;
                }
            }
            first.release();
            return true;
        }
    };

    template<typename Mutex1, typename... Mutexes>
    void lock_all(Mutex1 &m1, Mutexes&... ms) {
        for(;;) {
            std::unique_lock<Mutex1> first(m1);
            if constexpr (sizeof...(ms) == 0) {
                first.release();
                return ;
            }
            else {
                auto others = std::tie(ms...);
                constexpr size_t N = sizeof...(ms);
                bool success = TryLockRecursiveImpl<decltype(others), 0, N>::do_try(others);
                if(success) {
                    first.release();
                    return ;
                }
            }
            std::this_thread::yield();
        }
    }

} // namespace my_sync_lib::detail


template<typename... MutexTypes>
class ScopedLock {
    static_assert(sizeof...(MutexTypes) >= 2, 
        "ScopedLock primary template is for 2 or more mutexes; use ScopedLock<Mutex> or Scoped<> instead"
    );
public:
    explicit ScopedLock(MutexTypes&... mtxes) noexcept : mtxes_(mtxes...) { my_sync_lib::detail::lock_all(mtxes...); }
    explicit ScopedLock(std::adopt_lock_t, MutexTypes&... mtxes) noexcept : mtxes_(mtxes...) {}
    ~ScopedLock() noexcept { unlock_all(std::index_sequence_for<MutexTypes...>{}); }

    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;

private:
    std::tuple<MutexTypes&...> mtxes_;
    
    template<std::size_t... I>
    void unlock_all(std::index_sequence<I...>) noexcept {
        (std::get<I>(mtxes_).unlock(), ...);
    }
};

// template specialization for 1 mutex
template<typename MutexType>
class ScopedLock<MutexType> {
public:
    explicit ScopedLock(MutexType &mtx) noexcept : mtx_(mtx), own_(true) { mtx_.lock(); }
    ScopedLock(std::adopt_lock_t, MutexType &mtx) noexcept : mtx_(mtx), own_(true) {}
    ~ScopedLock() noexcept { if(own_) mtx_.unlock(); }

    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
private:
    MutexType& mtx_;
    bool own_;
};

// template specialization for 0 mutex
template<>
class ScopedLock<> {
public:
    ScopedLock() noexcept = default;
    explicit ScopedLock(std::adopt_lock_t) noexcept {};
};