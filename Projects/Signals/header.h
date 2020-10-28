#pragma once

//#define _POSIX_C_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <errno.h>
#include <signal.h>

struct Bits {
    short bits[8 * sizeof(char)];
};

struct Bits ToBits(char c);
void PrintBit (struct Bits bits);
char ToChar (struct Bits bit);
int SetParentDeath (pid_t ppid_bef_fork);
void SetBitsEmpty (struct Bits* bit);
//void handler (int signo);