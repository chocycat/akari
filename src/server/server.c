#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "../common/message.h"
#include "connection.h"

#define SOCKET_ADDR "/tmp/nostrum.sock"
#define BACKLOG_SIZE 2048

struct HandleClientArgs {
  int *fd;
  struct ConnectionManager *cman;
};

void *handle_client(void *arg) {
  struct HandleClientArgs *args = (struct HandleClientArgs *)arg;
  int fd = *(int *)args->fd;
  struct Connection *conn = conn_new(fd);
  conn_append(args->cman, conn);

  while (1) {
    if (conn->state == CONN_STATE_CLOSED) {
      printf("Closed");
      break;
    } else if (conn->state == CONN_STATE_WAITING) {
      printf("[DEB] Waiting\n");
      continue;
    }

    struct Message *msg = conn_read(conn);
    if (msg == NULL)
      continue;

    printf("Got message: %d\n", msg->header.type);
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  /// The socket file descriptors
  int sfd = -1, cfd = -1, *nfd;

  /// Client socket length
  socklen_t clen;

  // remove previous session
  remove(SOCKET_ADDR);
  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1) {
    // socket failed creation
    printf("[ERR] Failed to open socket: %s\n", strerror(errno));
    return 1;
  }

  // define sockaddr
  struct sockaddr_un saddr = {.sun_family = AF_UNIX};
  strcpy(saddr.sun_path, SOCKET_ADDR);

  // bind the socket
  if (bind(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_un)) == -1) {
    printf("[ERR] Failed to bind socket: %s\n", strerror(errno));
    return 1;
  }

  // TODO: determine if the backlog '2048' is acceptable or not
  //       in theory, it should be fine (2048 processes shouldn't open at the
  //       same time)
  if (listen(sfd, BACKLOG_SIZE) == -1) {
    printf("[ERR] Failed to start listening on socket: %s\n", strerror(errno));
    return 1;
  }

  printf("[INF] Server is listening on %s\n", SOCKET_ADDR);

  struct ConnectionManager *cman = cman_new();

  // main loop
  while (1) {
    clen = sizeof(struct sockaddr_un);
    nfd = malloc(sizeof(int));

    if ((*nfd = accept(sfd, (struct sockaddr *)&cfd, &clen)) == -1) {
      printf("[WRN] Failed to accept a client: %s\n", strerror(errno));
      free(nfd);
      continue;
    }

    printf("[INF] Accepted a connection\n");

    // create a new thread for the client
    pthread_t tid;
    struct HandleClientArgs args = {nfd, cman};

    if (pthread_create(&tid, NULL, handle_client, (void *)&args) != 0) {
      printf("[WRN] Failed to spawn thread for a client: %s\n",
             strerror(errno));
      free(nfd);
    } else {
      pthread_detach(tid);
    }
  }

  close(sfd);
  return 0;
}
