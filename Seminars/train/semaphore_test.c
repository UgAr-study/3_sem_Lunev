#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "../../Projects/TransFileSemShm/header.h"
#include <errno.h>

int main () {
    const char *file = "semaphore_test.c";
    key_t key = ftok (file, 0);
    int semid;
    if ((semid = semget (key, 1, 0666 | IPC_CREAT )) < 0) {
        perror ("semget():");
        return 0;
    }


    printf ("sen is %d\n", GetVal(semid, 0));
    V (semid, 0, 0);
    getchar();
    printf ("sen is %d\n", GetVal(semid, 0));

    semctl(semid, 0, IPC_RMID);
    return 0;
}
