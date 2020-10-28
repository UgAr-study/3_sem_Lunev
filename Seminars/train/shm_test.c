#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

    key_t key = ftok("main.c", 0);
    int shmid = shmget(key, 10 * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuf = (char *) shmat(shmid, NULL, 0);

    printf ("Hello, my pid is [%d] and I attached\n", getpid());

    struct shmid_ds shm_info;

    getchar();

    if (shmctl (shmid, IPC_STAT, &shm_info) < 0) {
        perror ("shmctl():");
        exit(EXIT_FAILURE);
    }

    printf ("number of attached processes is %ld\n", shm_info.shm_nattch);
    printf ("the last attached / detached pid is [%d]\n", shm_info.shm_lpid);

    shmdt (shmbuf);
    if (shmctl (shmid, IPC_RMID, NULL) < 0) {
        perror ("rm():");
    }

    return 0;
}