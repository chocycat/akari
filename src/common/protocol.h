#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace Protocol {
using MessageId = uint32_t;
inline const char *SOCKET_ADDR = "/tmp/akari.sock";

enum class MessageType : uint16_t {
  // Reserved

  NIL = 0,

  // Registration

  REGISTER = 128,
  UNREGISTER,

  // Window

  WINDOW_CREATE = 256,
  WINDOW_DESTROY,
  WINDOW_DELTA,

  // Input

  INPUT_KEY = 512,
  INPUT_BUTTON,
  INPUT_MOTION,
};

enum class MessageStatus : uint8_t {
  NONE,
  OK,
  ERROR,
};

struct MessageHeader {
  MessageType type;                    // Message type
  uint32_t size;                       // Payload size
  std::optional<MessageStatus> status; // Status

  MessageId id; // Message id
  std::optional<MessageId>
      reply_to; // References another message it's replying to
} __attribute__((packed));

struct Message {
  MessageHeader header;
  std::vector<uint8_t> payload;

  template <typename T>
  Message create(MessageType type, MessageStatus status, const T &payload);
  template <typename T> std::optional<T> get_data() const;

  uint32_t new_id();
} __attribute__((packed));

} // namespace Protocol
