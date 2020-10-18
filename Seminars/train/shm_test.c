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
    struct shmid_ds shm_info;

    if (shmctl (shmid, IPC_STAT, &shm_info) < 0) {
        perror ("shmctl():");
        exit(EXIT_FAILURE);
    }

    printf ("number of attached processes is %ld\n", shm_info.shm_nattch);

    shmdt (shmbuf);
    if (shmctl (shmid, IPC_RMID, NULL) < 0) {
        perror ("rm():");
    }

    return 0;
}