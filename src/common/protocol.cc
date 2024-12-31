#include "protocol.h"
#include <atomic>

namespace Protocol {
template <typename T>
Message Message::create(MessageType type, MessageStatus status,
                        const T &payload) {
  Message msg;
  msg.header.type = type;
  msg.header.id = new_id();
  msg.header.status = status;
  msg.header.size = sizeof(T);

  msg.payload.resize(sizeof(T));
  memcpy(msg.payload.data(), &payload, sizeof(T));

  return msg;
}

template <typename T> std::optional<T> Message::get_data() const {

  if (payload.size() != sizeof(T))
    return std::nullopt;

  return *reinterpret_cast<const T *>(payload.data());
}

uint32_t Message::new_id() {
  static std::atomic<uint32_t> curr_id{0};
  return ++curr_id;
}
} // namespace Protocol
