#pragma once

#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
//#include <sys/msg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <errno.h>
#include <signal.h>
#include <math.h>
#include <poll.h>
#include <assert.h>

enum { N = 10, PAGE = 10000 };
//static int child_deaths = 0;

struct Channel {
    int fd_from, fd_to;

    char *buffer;

    char* offset_for_write;
    char* offset_for_read;

    size_t size;
    size_t empty;
    size_t full;
};

void free_all(struct Channel *channels, size_t size);
int SetParentDeath (pid_t ppid_bef_fork);
void child_handler(int s);
void GetFromBuffer (struct Channel *channels, int i);
void PutInBuffer (struct Channel *channels, int i);