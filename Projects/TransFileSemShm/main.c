#include "header.h"

enum { TEST = 0 };

int main(int argc, char *argv[]) {
    key_t key = ftok ("./header.h", 0);

    int semid = semget (key, 1, 0666 | IPC_CREAT); // all semaphores are 0

    if (semid < 0) {
        perror("3) semget: ");
        return 0;
    }

    if ((semctl (semid, TEST, SETVAL, 1)) < 0) {
        perror("semctl: ");
        semctl(semid, 0, IPC_RMID);
        return 0;
    }

    //TODO How to make them connected without initialization?

    struct sembuf sb[2] = {};

    sb[0].sem_num = TEST;
    sb[0].sem_op = 0;
    sb[0].sem_flg = IPC_NOWAIT;

    sb[1].sem_num = TEST;
    sb[1].sem_op = 1;
    sb[1].sem_flg = SEM_UNDO;

    if (semop (semid, sb, 2) < 0) {
        printf ("Failed\n");
        perror("W: semop():");
        semctl (semid, 0 , IPC_RMID);
        return -1;
    }

    printf ("Success\n");

    semctl (semid, 0 , IPC_RMID);
    return 0;
}

    Reader:
P (PRINT);
print to stdout;
V (WRITER);

    Writer:
P (WRITER);
scanf from file;
V (PRINT);

