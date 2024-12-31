#pragma once

#include <string>

namespace Logger {
enum Level { INFO, WARN, ERROR };

template <typename... Args>
void out(Level level, std::string &format, Args &&...args);

template <typename... Args> void inf(const std::string &fmt, Args &&...args);
template <typename... Args> void warn(const std::string &fmt, Args &&...args);
template <typename... Args> void err(const std::string &fmt, Args &&...args);
} // namespace Logger
