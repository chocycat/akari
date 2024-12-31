#include "connection.h"
#include "src/common/error.h"
#include "src/common/protocol.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

namespace Protocol {
Connection::Connection() {}
Connection::~Connection() { disconnect(); }

void Connection::connect() {
  should_run = true;

  reconnect();
  reader_thread = std::thread(&Connection::reader, this);
}

void Connection::disconnect() {
  should_run = false;

  if (fd != -1) {
    close(fd);
    fd = -1;
  }

  if (reader_thread.joinable()) {
    reader_thread.join();
  }

  is_connected = false;
}

void Connection::send(const Message &msg) {
  if (!is_connected)
    throw ProtocolError("Not connected");

  if (::send(fd, &msg.header, sizeof(MessageHeader), 0) !=
      sizeof(MessageHeader))
    throw ProtocolError("Failed to send message header");

  if (msg.header.size > 0) {
    if (::send(fd, msg.payload.data(), msg.header.size, 0) != msg.header.size)
      throw ProtocolError("Failed to send message payload");
  }
}

std::future<Message> Connection::request(const Message &msg) {
  std::promise<Message> promise;
  auto future = promise.get_future();

  {
    std::lock_guard<std::mutex> lock(pending_mutex);
    pending_requests[msg.header.id] = std::move(promise);
  }

  send(msg);
  return future;
}

void Connection::request(const Message &msg, MessageCallback cb) {
  {
    std::lock_guard<std::mutex> lock(pending_mutex);
    pending_callbacks[msg.header.id] = cb;
  }

  send(msg);
}

void Connection::subscribe(MessageType type, MessageCallback cb) {
  std::lock_guard<std::mutex> lock(callbacks_mutex);
  subscribers[type].push_back(cb);
}

void Connection::unsubscribe(MessageType type) {
  std::lock_guard<std::mutex> lock(callbacks_mutex);
  subscribers.erase(type);
}

void Connection::reader() {
  while (should_run) {
    if (!is_connected) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    MessageHeader header;
    ssize_t rec = recv(fd, &header, sizeof(MessageHeader), 0);

    if (rec <= 0) {
      is_connected = false;
      reconnect();
      continue;
    }

    Message msg;
    msg.header = header;

    if (header.size > 0) {
      msg.payload.resize(header.size);
      rec = recv(fd, msg.payload.data(), header.size, 0);

      if (rec != header.size) {
        is_connected = false;
        reconnect();
        continue;
      }
    }

    process_msg(msg);
  }
}

void Connection::process_msg(const Message &msg) {
  if (msg.header.reply_to) {
    std::lock_guard<std::mutex> lock(pending_mutex);

    // check if there are promise-based requests
    auto it_promise = pending_requests.find(*msg.header.reply_to);
    if (it_promise != pending_requests.end()) {
      it_promise->second.set_value(msg);
      pending_requests.erase(it_promise);
      return;
    }

    // check if there are callback-based requests
    auto it_callback = pending_callbacks.find(*msg.header.reply_to);
    if (it_callback != pending_callbacks.end()) {
      it_callback->second(msg);
      pending_callbacks.erase(it_callback);
      return;
    }
  }

  // subscriptions
  std::lock_guard<std::mutex> lock(callbacks_mutex);
  auto it = subscribers.find(msg.header.type);
  if (it != subscribers.end()) {
    for (const auto &callback : it->second) {
      callback(msg);
    }
  }
}

void Connection::reconnect() {
  while (should_run && !is_connected) {
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }

    sockaddr_un addr{.sun_family = AF_UNIX};
    strcpy(addr.sun_path, Protocol::SOCKET_ADDR);

    if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0) {
      is_connected = true;
      return;
    }

    close(fd);
    fd = -1;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
} // namespace Protocol
