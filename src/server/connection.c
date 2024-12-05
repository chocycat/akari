#include "connection.h"
#include "../common/message.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define INITIAL_CONNECTIONS 8 // This should be promptly resized during runtime
#define MAX_CONNECTIONS                                                        \
  1024 // Realistically, no one is gonna run 1024 GUI clients

struct ConnectionManager *cman_new(void) {
  struct ConnectionManager *cman = malloc(sizeof(*cman));
  if (!cman)
    return NULL;

  cman->connections = malloc(sizeof(struct Connection *) * INITIAL_CONNECTIONS);
  if (!cman->connections) {
    free(cman);
    return NULL;
  }

  cman->count = 0;
  cman->capacity = INITIAL_CONNECTIONS;
  cman->next_id = 1;

  return cman;
}

void cman_destroy(struct ConnectionManager *cman) {
  if (!cman)
    return;

  for (size_t i = 0; i < cman->count; i++) {
    conn_close(cman, cman->connections[i]);
  }

  free(cman->connections);
  free(cman);
}

static bool grow_cman(struct ConnectionManager *cman) {
  if (cman->capacity >= MAX_CONNECTIONS)
    return false;

  size_t ncapacity = cman->capacity * 2;
  if (ncapacity > MAX_CONNECTIONS)
    ncapacity = MAX_CONNECTIONS;

  struct Connection **narr =
      realloc(cman->connections, sizeof(struct Connection *) * ncapacity);
  if (!narr)
    return false;

  cman->connections = narr;
  cman->capacity = ncapacity;
  return true;
}

bool conn_append(struct ConnectionManager *cman, struct Connection *conn) {
  if (!cman || !conn)
    return false;

  // check if we're at capacity
  if (cman->count >= cman->capacity) {
    if (!grow_cman(cman))
      return false;
  }

  if (cman->count >= MAX_CONNECTIONS)
    return false;

  conn->id = cman->next_id++;

  // wraparound
  if (cman->next_id == 0)
    cman->next_id = 1;

  cman->connections[cman->count++] = conn;
  conn->state = CONN_STATE_OPEN;

  return true;
}

struct Connection *conn_new(int fd) {
  struct Connection *conn = malloc(sizeof(*conn));
  if (!conn)
    return NULL;

  conn->fd = fd;
  conn->type = CLIENT_TYPE_UNKNOWN; // yet to register
  conn->state = CONN_STATE_WAITING;
  conn->pid = -1; // yet to set one
  conn->id = 0;   // will be set when added to a cman

  // create buffers
  conn->recv_buffer = buffer_new();
  conn->send_buffer = buffer_new();

  if (!conn->recv_buffer || !conn->send_buffer) {
    buffer_destroy(conn->recv_buffer);
    buffer_destroy(conn->send_buffer);
    free(conn);
    return NULL;
  }

  return conn;
}

void conn_close(struct ConnectionManager *cman, struct Connection *conn) {
  if (!cman || !conn)
    return;

  // remove from cman
  for (size_t i = 0; i < cman->count; i++) {
    if (cman->connections[i] == conn) {
      cman->count--;
      if (i < cman->count) {
        cman->connections[i] = cman->connections[cman->count];
      }
    }
  }

  close(conn->fd);
  buffer_destroy(conn->recv_buffer);
  buffer_destroy(conn->send_buffer);
  free(conn);
}

struct Message *conn_read(struct Connection *conn) {
  int err = 0;
  struct Message *msg = message_read(conn->fd, conn->recv_buffer, &err);

  // connection is closed
  if (err == -1) {
    conn->state = CONN_STATE_CLOSED;
    printf("[INF] Connection closed\n");
    return NULL;
  }

  return msg;
}
