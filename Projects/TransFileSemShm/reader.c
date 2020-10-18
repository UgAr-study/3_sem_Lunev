#include "header.h"

int main(int argc, char *argv[]) {

    key_t key = ftok ("src.c", 0);

    int semid = CreateAndInitSems(key);

#ifdef REMOVEALL
    int shmad = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuff = (char*) shmat(shmad, NULL, 0);
    semctl (semid, 0, IPC_RMID);
    shmdt(shmbuff);
    shmctl (shmad, IPC_RMID, NULL);
    exit(0);
#endif

    GetAllSemsInfo(semid);

    P (semid, REXIST, SEM_UNDO);
    printf ("a P EX\n");
    //P (semid, REMOVE);

    int shmid = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuf = (char*) shmat(shmid, NULL, 0);

    int write_ret_val, i = 1;
    while (1) {
        P (semid, FULL, 0);
        P (semid, MUTEX, 0);

        struct shmid_ds shm_info;
        if (shmctl (shmid, IPC_STAT, &shm_info) < 0) {
            perror ("shmctl():");
            exit(EXIT_FAILURE);
        }
        if (shm_info.shm_nattch == 1) {
            shmdt (shmbuf);
            V (semid, MUTEX, 0);
            V (semid, EMPTY, 0);
            //V (semid, REMOVE);
            break;
        }

        write_ret_val = write (STDOUT_FILENO, shmbuf, PAGESIZE);

        if (write_ret_val < 0) {
            perror("write from shm ():");
            semctl(semid, 0, IPC_RMID);
            shmdt(shmbuf);
            shmctl (shmid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }


        V (semid, MUTEX, 0);
        V (semid, EMPTY, 0);

//#ifdef BREAK
        ++i;
        if (i == 2) {
            printf ("\nkill me at all!!\n");
            sleep(5);
            printf ("you missed...\n");
        }
//#endif
    }

    V (semid, REXIST, SEM_UNDO);

    //semctl (semid, 0 , IPC_RMID);
    shmctl (shmid, IPC_RMID, NULL);

    return 0;
}
