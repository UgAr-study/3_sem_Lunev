#include "header.h"



int main(int argc, char *argv[]) {
    const char *file = "test.txt";
    key_t key = ftok ("./main.c", 0);
    int shmid = shmget (key, sizeof(long), 0666 | IPC_CREAT);
    int semid;
    if ((semid = semget (key, 4, IPC_CREAT | IPC_EXCL)) >= 0) {
        //init this sem (1,3) with 1;
    } else if (errno == EEXIST) {
        semid = semget (key, 4, IPC_CREAT);
    } else {
        perror ("W: semget():");
    }
    char *shmbuf = (char*) shmat(shmid, NULL, 0);

    /*
    WEXIST, REXIST, WMEM, RMEM
    WEXIST = 1? (P(WEXIST)) {
        WEXIST = 0;
        DO CRITICAL SECTION;
        WEXIST = 1; (V(WEXIST));
    }
    */

    P (semid, 1);
    V (semid, 1);

    return 0;
}
