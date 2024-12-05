#include "../src/client/client.h"
#include "../src/common/message.h"
#include "../src/common/message_payload.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_ADDR "/tmp/nostrum.sock"

int main(int argc, char *argv[]) {
  struct Client *client = client_connect(SOCKET_ADDR);
  if (client == NULL) {
    printf("err: failed to create client\n");
    return 1;
  }

  struct MessageHeader *header = malloc(sizeof(*header));
  message_header(header, M_REGISTER_CLIENT,
                 sizeof(struct MessageRegisterClient));
  struct MessageRegisterClient msg_reg = {.pid = getpid()};

  if (!message_send(client->fd, client->send_buffer, header, &msg_reg,
                    sizeof(msg_reg))) {
    free(header);
    printf("err: failed to write message\n");
    return 1;
  }

  free(header);

  while (1) {
    struct Message *msg = message_read(client->fd, client->recv_buffer, NULL);
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
