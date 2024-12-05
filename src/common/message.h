#pragma once

#include "buffer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// Defines all message types used within the protocol.
enum MessageType {
  /// Client

  M_REGISTER_CLIENT = 0, // Client registers itself to server
  M_REGISTER_CLIENT_ACK, // Server response
  M_UNREGISTER_CLIENT,   // Client unregisters itself to server

  /// Manager

  M_REGISTER_MANAGER = 32, // Manager registers itself to server
  M_UNREGISTER_MANAGER,    // Manager unregisters itself to server

  /// Window

  M_CREATE_WINDOW = 64, // Actions that a window should be created
  M_CREATED_WINDOW,     // Reports that a window has been created (usually
                        // dispatched by the server)
  M_DESTROY_WINDOW,     // Actions that a window should be destroyed

  M_UPDATE_WINDOW_STATE,  // Actions that a window state should be updated
  M_UPDATED_WINDOW_STATE, // Reports that a window state has been changed

  M_FOCUS_WINDOW,   // Actions that a window should be focused
  M_FOCUSED_WINDOW, // Reports that a window has been focused

  /// Input

  M_INPUT_KEY_PRESS = 128, // Generic keyboard device press
  M_INPUT_KEY_RELEASE,     // Generic keyboard device release
  M_INPUT_BUTTON_PRESS,    // Mouse button press
  M_INPUT_BUTTON_RELEASE,  // Mouse button release
  M_INPUT_MOTION,          // Mouse motion
};

struct MessageHeader {
  uint16_t type; // Message Type
  uint16_t size; // Payload size
} __attribute__((packed));

struct Message {
  struct MessageHeader header;
  uint8_t *payload; // Array
} __attribute__((packed));

size_t message_size(uint16_t type, size_t size);

void message_header(struct MessageHeader *header, uint16_t type, uint16_t size);

struct Message *message_read(int fd, struct CircularBuffer *buf, int *err);
bool message_send(int fd, struct CircularBuffer *buf,
                  struct MessageHeader *header, const void *payload,
                  size_t size);
