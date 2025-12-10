#pragma once
#include <optional>
#include <string>
#include <utility>

/**
 * @file
 * @brief 轻量 Result<T>
 * @ingroup Util
 */

namespace demo::util {

/** @brief 成功时含值，失败时含错误消息 */
template<class T>
class Result {
public:
  static Result ok(T v) { return Result(std::move(v), {}); }
  static Result err(std::string e) { return Result({}, std::move(e)); }

  bool has_value() const noexcept { return val_.has_value(); }
  T& value() { return *val_; }
  const std::string& error() const { return err_; }

private:
  std::optional<T> val_;
  std::string err_;
  Result(std::optional<T> v, std::string e): val_(std::move(v)), err_(std::move(e)) {}
};

} // namespace demo::util
