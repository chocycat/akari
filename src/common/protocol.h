#pragma once

#include <stdbool.h>
#include <stdint.h>

/// Defines all message types used within the protocol.
enum MessageType {
  /// Client

  REGISTER_CLIENT = 0, // Client registers itself to server
  UNREGISTER_CLIENT,   // Client unregisters itself to server

  /// Manager

  REGISTER_MANAGER = 32, // Manager registers itself to server
  UNREGISTER_MANAGER,    // Manager unregisters itself to server

  /// Window

  CREATE_WINDOW = 64, // Actions that a window should be created
  CREATED_WINDOW, // Reports that a window has been created (usually dispatched
                  // by the server)
  DESTROY_WINDOW, // Actions that a window should be destroyed

  UPDATE_WINDOW_STATE,  // Actions that a window state should be updated
  UPDATED_WINDOW_STATE, // Reports that a window state has been changed

  FOCUS_WINDOW,   // Actions that a window should be focused
  FOCUSED_WINDOW, // Reports that a window has been focused

  /// Input

  INPUT_KEY_PRESS = 128, // Generic keyboard device press
  INPUT_KEY_RELEASE,     // Generic keyboard device release
  INPUT_BUTTON_PRESS,    // Mouse button press
  INPUT_BUTTON_RELEASE,  // Mouse button release
  INPUT_MOTION,          // Mouse motion
};

struct MessageHeader {
  uint16_t type;      // Message Type
  uint16_t length;    // Payload length
  uint32_t client_id; // Sender or recipient ID
} __attribute__((packed));

struct Message {
  struct MessageHeader header;
  uint8_t *payload; // Array
} __attribute__((packed));
