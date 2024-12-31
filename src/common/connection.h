#pragma once

#include "protocol.h"
#include <functional>
#include <future>

namespace Protocol {
using MessageCallback = std::function<void(const Message &)>;

class Connection {
public:
  Connection();
  ~Connection();

  void connect();
  void disconnect();

  void send(const Message &msg);

  std::future<Message> request(const Message &msg);
  void request(const Message &msg, MessageCallback cb);

  void subscribe(MessageType type, MessageCallback cb);
  void unsubscribe(MessageType type);

  bool connected() const { return is_connected; }

private:
  void reader();
  void process_msg(const Message &msg);
  void reconnect();

  int fd = -1;
  uint32_t client_id;

  std::atomic<bool> is_connected = false;
  std::atomic<bool> should_run = false;

  std::thread reader_thread;

  std::mutex callbacks_mutex;
  std::unordered_map<MessageType, std::vector<MessageCallback>> subscribers;

  std::mutex pending_mutex;
  std::unordered_map<uint32_t, std::promise<Message>> pending_requests;
  std::unordered_map<uint32_t, MessageCallback> pending_callbacks;
};
} // namespace Protocol
