#pragma once

#include "../common/buffer.h"
#include "../common/message.h"
#include "../common/message_payload.h"

struct Client {
  int fd;
  struct CircularBuffer recv_buffer;
  struct CircularBuffer send_buffer;
  uint32_t client_id;
};

/// Client Handling

// Connect and register self to the socket
struct Client *client_connect(const char *socket_addr);

// Disconnect and remove self from the socket
void client_destroy(struct Client *client);

/// Message handling

// Read a message from the socket
struct Message *client_read(struct Client *client);

// Send a message to the socket
bool client_write(struct Client *client, uint32_t type, const void *payload,
                  size_t size);
