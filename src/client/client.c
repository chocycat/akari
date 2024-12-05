#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
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

  client->recv_buffer = buffer_new();
  client->send_buffer = buffer_new();
  client->client_id = 0; // set after registration

  return client;
}
