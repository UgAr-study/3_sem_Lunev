#include "header.h"

int main(int argc, char *argv[]) {

    printf ("Reader pid is %d\n\n", getpid());
    key_t key = ftok ("src.c", 0);

#ifdef REMOVEALL
    int shmad = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuff = (char*) shmat(shmad, NULL, 0);
    int semid = semget (key, 4, 0666);
    printf ("semid is %d\n", semid);
    semctl (semid, 0, IPC_RMID);
    shmdt(shmbuff);
    shmctl (shmad, IPC_RMID, NULL);
    exit(0);
#endif

#ifndef REMOVEALL
    int semid = semget (key, 5, 0666 | IPC_CREAT);
#endif

    printf ("Reader: before all initializations\n");
    GetAllSemsInfo(semid);

    if (IsExist(semid, REXIST, IPC_NOWAIT) == 1) {
        IsExist(semid, REXIST, 0);
    }

    WaitOppenent(semid, WEXIST);

    int shmid = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuf = (char*) shmat(shmid, NULL, 0);

    int write_ret_val, i = 1;
    while (1) {

        printf ("Reader: before P(FULL)\n");
        GetAllSemsInfo(semid);

        if (IsAlive(semid, WEXIST, FULL) != 1) {
            printf ("Reader: Writer died\n");
            shmdt (shmbuf);
            //V (semid, MUTEX, SEM_UNDO);
            //V (semid, EMPTY, 0);
            break;
        }

        P (semid, MUTEX, SEM_UNDO);

        printf (">>>> \n");
        write_ret_val = write (STDOUT_FILENO, shmbuf, PAGESIZE);
        printf ("<<<<\n");

        if (write_ret_val < 0) {
            perror("write from shm ():");
            semctl(semid, 0, IPC_RMID);
            shmdt(shmbuf);
            shmctl (shmid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }

        /*
/////////////////////
        ++i;
        if (i == 2) {
            printf ("\nkill me at all!!\n");
            sleep(5);
            printf ("you missed...\nCurrent state is\n");
            GetAllSemsInfo(semid);
        }
////////////////////
         */

        V (semid, MUTEX, SEM_UNDO);
        V (semid, EMPTY, 0);

//#ifdef BREAK
        /*
        ++i;
        if (i == 2) {
            printf ("\nkill me at all!!\n");
            sleep(5);
            printf ("you missed...\nCurrent state is\n");
            GetAllSemsInfo(semid);
        }
         */
//#endif
    }

    //semctl (semid, 0 , IPC_RMID);
    shmctl (shmid, IPC_RMID, NULL);
    P (semid, REXIST, SEM_UNDO);

    return 0;
}
