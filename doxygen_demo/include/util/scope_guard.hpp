#pragma once
#include <utility>

/**
 * @file
 * @brief 作用域守卫（RAII）
 * @ingroup Util
 */

namespace demo::util {

/**
 * @brief 作用域守卫：确保作用域退出时执行清理
 */
class ScopeGuard {
public:
  template<class F>
  explicit ScopeGuard(F&& f) noexcept : fn_(new Model<F>(std::forward<F>(f))) {}

  // 不可拷贝，可移动
  ScopeGuard(const ScopeGuard&) = delete;
  ScopeGuard& operator=(const ScopeGuard&) = delete;
  ScopeGuard(ScopeGuard&& other) noexcept : fn_(other.fn_) { other.fn_ = nullptr; }
  ScopeGuard& operator=(ScopeGuard&& other) noexcept {
    if (this != &other) { delete fn_; fn_ = other.fn_; other.fn_ = nullptr; }
    return *this;
  }

  ~ScopeGuard() { if (fn_) fn_->call(); }

private:
  struct Concept { virtual ~Concept() = default; virtual void call() noexcept = 0; };
  template<class F> struct Model : Concept {
    F f; explicit Model(F&& ff): f(std::forward<F>(ff)) {}
    void call() noexcept override { f(); }
  };
  Concept* fn_ = nullptr;
};

} // namespace demo::util
