#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

struct Client *client_connect(const char *socket_addr) {
  struct Client *client = malloc(sizeof(*client));
  if (client == NULL) {
    printf("err: failed to allocate client\n");
    free(client);
    return NULL;
  }

  client->fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (client->fd == -1) {
    printf("err: failed to open socket\n");
    free(client);
    return NULL;
  }

  struct sockaddr_un saddr = {.sun_family = AF_UNIX};
  strcpy(saddr.sun_path, socket_addr);

  if (connect(client->fd, (struct sockaddr *)&saddr,
              sizeof(struct sockaddr_un)) < 0) {
    printf("err: failed to connect to socket\n");
    close(client->fd);
    free(client);
    return NULL;
  }

  client->recv_buffer = *buffer_new();
  client->send_buffer = *buffer_new();
  client->client_id = 0; // set after registration

  struct MessageRegisterClient msg_reg = {.pid = getpid(), .flags = 0};

  if (!client_write(client, REGISTER_CLIENT, &msg_reg, sizeof(msg_reg))) {
    printf("err: failed to write message\n");
    return NULL;
  }

  return client;
}

bool client_write(struct Client *client, uint32_t type, const void *payload,
                  size_t size) {
  struct MessageHeader *header = malloc(sizeof(*header));
  message_header(header, type, size, client->client_id);

  // write header
  if (buffer_write(&client->send_buffer, header, sizeof(*header)) < 0) {
    printf("err: failed to write header\n");
    return false;
  }

  // write payload
  if (size > 0) {
    if (buffer_write(&client->send_buffer, payload, size) < 0) {
      printf("err: failed to write payload\n");
      return false;
    }
  }

  // flush to socket
  size_t space = client->send_buffer.capacity - client->send_buffer.read_pos;
  size_t left = client->send_buffer.used;

  if (left > space)
    left = space;

  ssize_t n =
      write(client->fd, client->send_buffer.data + client->send_buffer.read_pos,
            left);

  if (n < 0) {
    printf("err: failed to write to fd\n");
    return false;
  }

  client->send_buffer.read_pos += n;
  client->send_buffer.used -= n;

  if (client->send_buffer.read_pos >= client->send_buffer.capacity)
    client->send_buffer.read_pos = 0;

  return true;
}
