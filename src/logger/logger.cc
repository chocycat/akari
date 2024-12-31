#include "logger.h"

#include <chrono>
#include <fmt/core.h>
#include <iomanip>
#include <sstream>
#include <string>

namespace Logger {
const char *PREFIX = "SRV";

std::string time() {
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);
  std::tm now_tm = *std::localtime(&now_c);

  std::ostringstream oss;
  oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

template <typename... Args>
void out(Level level, std::string &format, Args &&...args) {
  std::string level_str;
  switch (level) {
  case Level::INFO:
    level_str = "(INF)";
    break;
  case Level::WARN:
    level_str = "(WRN)";
    break;
  case Level::ERROR:
    level_str = "(ERR)";
    break;
  };

  auto msg = fmt::format(format, std::forward<Args>(args)...);
  fmt::print("[{}] {} {}: {}\n", time(), PREFIX, level_str, msg);
}

template <typename... Args> void inf(const std::string &fmt, Args &&...args) {
  out(fmt, Level::INFO, std::forward<Args>(args)...);
}

template <typename... Args> void warn(const std::string &fmt, Args &&...args) {
  out(fmt, Level::WARN, std::forward<Args>(args)...);
}

template <typename... Args> void err(const std::string &fmt, Args &&...args) {
  out(fmt, Level::ERROR, std::forward<Args>(args)...);
}
} // namespace Logger
