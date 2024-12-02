#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/// A buffer that will wrap around continously
struct CircularBuffer {
  uint8_t *data;
  size_t capacity;
  size_t read_pos;
  size_t write_pos;
  size_t used;
};

struct CircularBuffer *buffer_new(void);
void buffer_destroy(struct CircularBuffer *buf);

/// Ensures there is space to write to a buffer
bool ensure_space(struct CircularBuffer *buf, size_t needed);

/// Writes data into a buffer
///
/// It will automatically wrap around when reaching the end
///
/// Returns the amount of bytes written
ssize_t buffer_write(struct CircularBuffer *buf, const void *data, size_t len);

/// Reads data from a buffer
///
/// It will automatically wrap around when reaching the end
///
/// Returns the amount of bytes read
ssize_t buffer_read(struct CircularBuffer *buf, void *data, size_t len);
