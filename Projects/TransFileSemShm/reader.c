#include "header.h"

int main(int argc, char *argv[]) {

    printf ("Reader pid is %d\n\n", getpid());
    key_t key = ftok ("src.c", 0);

#ifndef REMOVEALL
    int semid = CreateAndInitSems(key);
#endif

#ifdef REMOVEALL
    int shmad = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuff = (char*) shmat(shmad, NULL, 0);
    int semid = semget (key, 0, 0666);
    printf ("semid is %d\n", semid);
    semctl (semid, 0, IPC_RMID);
    shmdt(shmbuff);
    shmctl (shmad, IPC_RMID, NULL);
    exit(0);
#endif

    GetAllSemsInfo(semid);

    W (semid, WEXIST, 0);

    /*
    if (CheckShit(semid, FULL) <= 0) {
        if (InitAllSems(semid) < 0) {
            perror ("writer: InitAllSems():");
            semctl (semid, 0 , IPC_RMID);
        }
    }
    */

    P (semid, REXIST, SEM_UNDO);
    printf ("a P EX\n");
    //P (semid, WCRITSEC);

    int shmid = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuf = (char*) shmat(shmid, NULL, 0);

    int write_ret_val, i = 1;
    while (1) {

        GetAllSemsInfo(semid);
        V (semid, RCRITSEC, SEM_UNDO);
        if (i == 1)
            P (semid, RCRITSEC, 0);

        W (semid, WCRITSEC, 0);
        printf ("\nReader can entry\n");
        if (W (semid, WEXIST, IPC_NOWAIT) < 0) {
            printf ("Reader: checking shit...\n");
            if (W (semid, FULL, IPC_NOWAIT) == 0) {
                printf("I think writer died\n");
                break;
            }
        }

        printf ("Reader: try to P(FULL)\n");
        P (semid, FULL, 0);
        P (semid, MUTEX, SEM_UNDO);

        struct shmid_ds shm_info;
        if (shmctl (shmid, IPC_STAT, &shm_info) < 0) {
            perror ("shmctl():");
            exit(EXIT_FAILURE);
        }
        if (shm_info.shm_nattch == 1) {
            shmdt (shmbuf);
            V (semid, MUTEX, SEM_UNDO);
            V (semid, EMPTY, 0);
            //V (semid, WCRITSEC);
            break;
        }

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

/////////////////////
        ++i;
        if (i == 2) {
            printf ("\nkill me at all!!\n");
            sleep(5);
            printf ("you missed...\nCurrent state is\n");
            GetAllSemsInfo(semid);
        }
////////////////////

        V (semid, MUTEX, SEM_UNDO);
        V (semid, EMPTY, 0);
        P (semid, RCRITSEC, SEM_UNDO);

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

    V (semid, REXIST, SEM_UNDO);

    //semctl (semid, 0 , IPC_RMID);
    shmctl (shmid, IPC_RMID, NULL);

    return 0;
}
