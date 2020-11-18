#pragma once

#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <errno.h>
#include <signal.h>
#include <math.h>
#include <poll.h>

enum { N = 5, PAGE = 10 };

struct Channel {
    char *buffer;
    int fd_from, fd_to;
};

void free_all(struct Channel *channels, size_t size);