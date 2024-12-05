#include "message.h"
#include "buffer.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

size_t message_size(uint16_t type, size_t size) {
  return sizeof(struct MessageHeader) + size;
}

void message_header(struct MessageHeader *header, uint16_t type,
                    uint16_t size) {
  header->type = type;
  header->size = size;
}

struct Message *message_read(int fd, struct CircularBuffer *buf, int *err) {
  uint8_t tbuf[8192];
  ssize_t n = read(fd, tbuf, sizeof(tbuf));

  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return NULL;

    printf("err: %s\n", strerror(errno));
    return NULL;
  }

  if (n == 0) {
    if (err != NULL)
      *err = -1;

    return NULL;
  }

  if (buffer_write(buf, tbuf, n) < 0) {
    return NULL;
  }

  if (buf->used < sizeof(struct MessageHeader)) {
    return NULL;
  }

  // read header
  struct MessageHeader header;
  buffer_read(buf, &header, sizeof(struct MessageHeader));

  // read entire message
  struct Message *msg = malloc(sizeof(struct Message));
  if (!msg) {
    printf("err: failed to allocate message\n");
    return NULL;
  }

  msg->header = header;

  if (header.size > 0) {
    // allocate enough resources for the content
    msg->payload = malloc(header.size);
    if (!msg->payload) {
      free(msg);
      printf("err: failed to allocate message payload\n");
      return NULL;
    }

    buffer_read(buf, msg->payload, header.size);
  } else {
    msg->payload = NULL;
  }

  return msg;
}

bool message_send(int fd, struct CircularBuffer *buf,
                  struct MessageHeader *header, const void *payload,
                  size_t size) {
  // write header
  if (buffer_write(buf, header, sizeof(*header)) < 0) {
    printf("err: failed to write header\n");
    return false;
  }

  // write payload
  if (size > 0) {
    if (buffer_write(buf, payload, size) < 0) {
      printf("err: failed to write payload\n");
      return false;
    }
  }

  size_t space = buf->capacity - buf->read_pos;
  size_t left = buf->used;

  if (left > space)
    left = space;

  ssize_t n = write(fd, buf->data + buf->read_pos, left);

  if (n < 0) {
    printf("err: failed to write to fd\n");
    return false;
  }

  buf->read_pos += n;
  buf->used -= n;

  if (buf->read_pos >= buf->capacity)
    buf->read_pos = 0;

  return true;
}
