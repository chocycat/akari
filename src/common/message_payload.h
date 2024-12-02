#pragma once

#include <sched.h>
#include <stdint.h>
#include <sys/cdefs.h>

struct MessageRegisterClient {
  pid_t pid;
} __attribute__((packed));

struct MessageRegisterClientAck {
  uint32_t client_id;
} __attribute__((packed));
