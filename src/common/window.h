#pragma once

#include <stdbool.h>
#include <stdint.h>

struct WindowGeometry {
  uint64_t x;
  uint64_t y;
  uint64_t width;
  uint64_t height;
};

struct WindowState {
  char *title;
  char *icon;
};

struct Window {
  struct WindowGeometry *geometry;
  struct WindowState *state;

  bool focused;

  /// Defines properties for managers to use
  uint64_t hints;
};
