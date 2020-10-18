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

struct pid_table {
    pid_t *table;
    unsigned size;
};


int SetParentDeath (int msgid, pid_t ppid_bef_fork);
int RecieveAndDelayPid(int msgid, struct pid_table child_pids, struct pid_table *delayed);
long GetThisChildNumber(pid_t pid, struct pid_table child_pids);
int SendPermission(int msgid, long type, unsigned number);
int GetConfirmation(int msgid, pid_t pid);
long GetMyNumber (int msgid, pid_t mypid);
void DeleteMSQ (int msgid);