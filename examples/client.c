#include <stdio.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_ADDR "/tmp/nostrum.sock"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  int sfd = -1;
  struct sockaddr_un saddr;

  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1) {
    // socket failed creation
    printf("[ERR] Failed to open socket: %s\n", strerror(errno));
    return 1;
  }

  // define sockaddr
  saddr.sun_family = AF_UNIX;
  strcpy(saddr.sun_path, SOCKET_ADDR);

  // connect
  if (connect(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_un)) ==
      -1) {

    printf("[ERR] Failed to connect to socket: %s\n", strerror(errno));
    return 1;
  }

  char buffer[BUFFER_SIZE];

  // send an init message
  strcpy(buffer, "init,example");
  write(sfd, buffer, strlen(buffer) + 1);

  close(sfd);
  return 0;
}
