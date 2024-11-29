#include "buffer.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUFFER_SIZE (32 * 1024) // 32KB
#define MAX_BUFFER_SIZE                                                        \
  (16 * 1024 * 1024) // 16MB
                     // This shouldn't need a bump anytime soon.

struct CircularBuffer *buffer_new(void) {
  struct CircularBuffer *buf = malloc(sizeof(*buf));
  if (!buf)
    return NULL;

  // allocate initial size
  buf->data = malloc(INITIAL_BUFFER_SIZE);
  if (!buf->data) {
    free(buf);
    return NULL;
  }

  buf->capacity = INITIAL_BUFFER_SIZE;
  buf->read_pos = 0;
  buf->write_pos = 0;
  buf->used = 0;

  return buf;
}

void buffer_destroy(struct CircularBuffer *buf) {
  free(buf);
  free(buf->data);
  return;
}

bool ensure_space(struct CircularBuffer *buf, size_t needed) {
  if (!buf || needed > MAX_BUFFER_SIZE)
    return false;

  // check if we already have enough space
  if (buf->used + needed <= buf->capacity)
    return false;

  // we need to resize the buffer
  size_t ncapacity = buf->capacity * 2;
  while (ncapacity < (buf->used + needed))
    ncapacity *= 2;

  // make sure we don't go overboard
  if (ncapacity > MAX_BUFFER_SIZE)
    ncapacity = MAX_BUFFER_SIZE;

  if (buf->used + needed > ncapacity)
    return false;

  uint8_t *data = malloc(ncapacity);
  if (!data)
    return false;

  // copy the existing data, wrap-around
  if (buf->write_pos > buf->read_pos) {
    // simple, continuous data
    memcpy(data, buf->data + buf->read_pos, buf->used);
  } else {
    // should wrap
    size_t chunk = buf->capacity - buf->read_pos;
    memcpy(data, buf->data + buf->read_pos, chunk);
    memcpy(data + chunk, buf->data, buf->read_pos);
  }

  // assign the new data
  free(buf->data);
  buf->data = data;
  buf->capacity = ncapacity;
  buf->read_pos = 0;
  buf->write_pos = buf->used;

  return true;
}

ssize_t buffer_write(struct CircularBuffer *buf, const void *data, size_t len) {
  if (!buf || !data || len == 0)
    return -1;
  if (!ensure_space(buf, len))
    return -1;

  size_t space = buf->capacity - buf->write_pos;

  if (len <= space) {
    memcpy(buf->data + buf->write_pos, data, len);
    buf->write_pos += len;
  } else {
    memcpy(buf->data + buf->write_pos, data, space);
    memcpy(buf->data, (uint8_t *)data + space, len - space);
    buf->write_pos = len - space;
  }

  if (buf->write_pos >= buf->capacity)
    buf->write_pos = 0;

  buf->used += len;
  return len;
}

ssize_t buffer_read(struct CircularBuffer *buf, void *data, size_t len) {
  if (!buf || !data || len == 0)
    return 0;
  if (buf->used == 0)
    return 0;

  size_t space = buf->capacity - buf->read_pos;

  if (len <= space) {
    memcpy(data, buf->data + buf->read_pos, len);
    buf->read_pos += len;
  } else {
    memcpy(data, buf->data + buf->read_pos, space);
    memcpy((uint8_t *)data + space, buf->data, len - space);
    buf->read_pos = len - space;
  }

  if (buf->read_pos >= buf->capacity)
    buf->read_pos = 0;

  buf->used -= len;
  return len;
}
