#include "../src/client/client.h"
#include <stdio.h>

#define SOCKET_ADDR "/tmp/nostrum.sock"

int main(int argc, char *argv[]) {
  struct Client *client = client_connect(SOCKET_ADDR);
  if (client == NULL) {
    printf("err: failed to create client\n");
    return 1;
  }

  // pause
  getchar();

  client_write(client, UNREGISTER_CLIENT, NULL, 0);

  getchar();

  return 0;
}
