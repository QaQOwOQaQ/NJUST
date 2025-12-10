#pragma once
#include <memory>
#include <string>

/**
 * @file
 * @brief 线程安全 Logger（PImpl）
 * @ingroup Core
 */

namespace demo::core {

/**
 * @brief 线程安全的 Logger（PImpl）
 */
class Logger {
public:
  static Logger& instance();                  ///< 单例
  void set_level(int level);
  void info(const std::string& msg) const;
  void error(const std::string& msg) const;

  // 移动语义以便演示
  Logger(Logger&&) noexcept;
  Logger& operator=(Logger&&) noexcept;
  ~Logger();

  // 不可拷贝
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

private:
  struct Impl;
  std::unique_ptr<Impl> p_;
  Logger();                                   // 私有构造
};

} // namespace demo::core
