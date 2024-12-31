#include "src/common/protocol.h"
#include "src/logger/logger.h"

#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using Protocol::SOCKET_ADDR;
const int BACKLOG_SIZE = 2048;

int main(int argc, char *argv[]) {
  int server_fd = -1;

  // Clean up previous socket
  remove(SOCKET_ADDR);

  server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (server_fd == -1) {
    Logger::err("Failed to open socket: %s", strerror(errno));
    return 1;
  }

  sockaddr_un server_addr{.sun_family = AF_UNIX};
  strcpy(server_addr.sun_path, SOCKET_ADDR);

  if (bind(server_fd, reinterpret_cast<sockaddr *>(&server_addr),
           sizeof(server_addr)) == -1) {
    Logger::err("Failed to bind socket: %s", strerror(errno));
    return 1;
  }

  if (listen(server_fd, BACKLOG_SIZE) == -1) {
    Logger::err("Failed to start listener: %s", strerror(errno));
    return 1;
  }

  Logger::inf("Server is listening on: %s", SOCKET_ADDR);

  while (true) {
    sockaddr_un client_addr{};
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(
        server_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_len);
    if (client_fd == -1) {
      Logger::err("Failed to accept a client: %s", strerror(errno));
      continue;
    }

    Logger::inf("Accepted a connection");
  }

  close(server_fd);
  return 0;
}
