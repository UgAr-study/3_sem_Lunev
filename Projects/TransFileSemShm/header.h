#pragma once

#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>


enum {WEXIST = 0, REXIST = 1, FULL = 2, EMPTY = 3, MUTEX = 4, PAGESIZE = 10};

int P (int semid, int n, int flags);
int V (int semid, int n, int flags);
int W (int semid, int n, int flags);
int GetVal (int semid, int n);
int InitAllSems (int semid);
void GetAllSemsInfo (int semid);
int IsExist (int semid, int num, int w_f);
void InitSemWith1 (int semid, int num);
int IsAlive (int semid, int who, int what);
void WaitOppenent (int semid, int num);
