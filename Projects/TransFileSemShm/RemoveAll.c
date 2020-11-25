#include "header.h"

int main () {
    key_t key = ftok ("src.c", 0);

    int shmad = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    if (shmad < 0) {
        perror("shmget: ");
        exit(EXIT_FAILURE);
    }
    char *shmbuff = (char*) shmat(shmad, NULL, 0);
    int semid = semget (key, 6, 0666);
    printf ("semid is %d\n", semid);
    semctl (semid, 0, IPC_RMID);
    shmdt(shmbuff);
    shmctl (shmad, IPC_RMID, NULL);
    return 0;
}
