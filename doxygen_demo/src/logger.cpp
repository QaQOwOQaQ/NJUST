#include "core/logger.hpp"
#include <iostream>
#include <mutex>
#include <ctime>

namespace demo::core {

struct Logger::Impl {
  int level = 1;
  mutable std::mutex m;
  void log(const char* tag, const std::string& msg) const {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "[" << tag << "] " << msg << "\n";
  }
};

Logger::Logger(): p_(std::make_unique<Impl>()) {}
Logger::~Logger() = default;
Logger::Logger(Logger&& o) noexcept = default;
Logger& Logger::operator=(Logger&& o) noexcept = default;

Logger& Logger::instance() {
  static Logger g;
  return g;
}

void Logger::set_level(int level){ p_->level = level; }
void Logger::info(const std::string& msg) const { if (p_->level <= 1) p_->log("INFO", msg); }
void Logger::error(const std::string& msg) const { p_->log("ERROR", msg); }

} // namespace demo::core
