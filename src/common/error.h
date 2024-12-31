#pragma once

#include <stdexcept>
#include <string>

class ProtocolError : public std::runtime_error {
public:
  explicit ProtocolError(const std::string &message)
      : std::runtime_error(message) {}

  explicit ProtocolError(const char *message) : std::runtime_error(message) {}
};
