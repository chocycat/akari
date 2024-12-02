#pragma once

#include <sched.h>
#include <stdint.h>
#include <sys/cdefs.h>

struct MessageRegisterClient {
  pid_t pid;
  uint32_t flags;
} __attribute__((packed));
