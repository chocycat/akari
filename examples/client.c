#include "../src/client/client.h"
#include "../src/common/message.h"
#include "../src/common/message_payload.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define SOCKET_ADDR "/tmp/nostrum.sock"

int main(int argc, char *argv[]) {
  struct Client *client = client_connect(SOCKET_ADDR);
  if (client == NULL) {
    printf("err: failed to create client\n");
    return 1;
  }

  while (1) {
    struct Message *msg = client_read(client);
    if (msg == NULL)
      continue;

    switch (msg->header.type) {
    case M_REGISTER_CLIENT_ACK: {
      struct MessageRegisterClientAck *res =
          (struct MessageRegisterClientAck *)msg->payload;

      client->client_id = res->client_id;
      printf("I am client %d!\n", client->client_id);

      break;
    }
    }
  }

  return 0;
}
