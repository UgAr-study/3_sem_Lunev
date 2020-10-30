#pragma once

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

enum { N = 10 };

struct msgbuf {
    long mtype;
    unsigned number;
};

int Send (int msgid, long type, unsigned number);
int Receive(int msgid, long type);
void DeleteMSQ (int msgid);