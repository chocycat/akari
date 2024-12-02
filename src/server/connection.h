#pragma once

#include "../common/buffer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

enum ConnectionType {
  CLIENT_TYPE_UNKNOWN,
  CLIENT_TYPE_APP,
  CLIENT_TYPE_MANAGER,
};

enum ConnectionState { CONN_STATE_WAITING, CONN_STATE_OPEN, CONN_STATE_CLOSED };

struct Connection {
  uint32_t id;
  int fd;
  enum ConnectionType type;
  enum ConnectionState state;
  pid_t pid;
  struct CircularBuffer *recv_buffer;
  struct CircularBuffer *send_buffer;
};

struct ConnectionManager {
  struct Connection **connections;
  size_t count;
  size_t capacity;
  uint32_t next_id;
};

struct ConnectionManager *cman_new(void);
void cman_destroy(struct ConnectionManager *cman);
bool conn_append(struct ConnectionManager *cman, struct Connection *conn);
struct Connection *conn_new(int fd);
void conn_close(struct ConnectionManager *cman, struct Connection *conn);
struct Message *conn_read(struct Connection *conn);
void conn_write(struct Connection *conn);
