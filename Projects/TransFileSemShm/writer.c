#include "header.h"

int main(int argc, char *argv[]) {

    printf ("Writer pid is %d\n\n", getpid());
    const char *file = "test.txt";
    int fdfrom = open (file, O_RDONLY);
    printf ("fd from is %d\n", fdfrom);

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

    if (CheckShit(semid, EMPTY) <= 0) {
        if (InitAllSems(semid) < 0) {
            perror ("writer: InitAllSems():");
            semctl (semid, 0 , IPC_RMID);
        }
    }

    P (semid, WEXIST, SEM_UNDO);
    W (semid, REXIST, 0);

    printf ("a P EX\n");

    int shmid = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuf = (char*) shmat(shmid, NULL, 0);

    for (int i = 0; i < PAGESIZE; ++i)
        shmbuf[i] = '\0';


    int read_ret_val, j = 1;
    while (1) {

        GetAllSemsInfo(semid);

        V (semid, WCRITSEC, SEM_UNDO);
        if (W (semid, REXIST, IPC_NOWAIT) < 0) {
            printf ("Writer: checking shit...\n");
            if (W (semid, EMPTY, IPC_NOWAIT) == 0) {
                printf ("I think reader died\n");
                break;
            }
        }

        P (semid, EMPTY, 0);
        P (semid, MUTEX, SEM_UNDO);

        for (int i = 0; i < PAGESIZE; ++i)
            shmbuf[i] = '\0';

        if (j >= 2) {

            struct shmid_ds shm_info;
            if (shmctl (shmid, IPC_STAT, &shm_info) < 0) {
                perror ("shmctl():");
                exit(EXIT_FAILURE);
            }
            if (shm_info.shm_nattch == 1) {
                printf ("Writer finish\n");
                shmdt (shmbuf);
                V (semid, MUTEX, SEM_UNDO);
                V (semid, EMPTY, 0);
                break;
            }

        }

        read_ret_val = read (fdfrom, shmbuf, PAGESIZE);

        if (read_ret_val == 0) {
            printf ("Writer finish\n");
            shmdt (shmbuf);
            V (semid, MUTEX, SEM_UNDO);
            V (semid, FULL, 0);
            break;
        }

        if (read_ret_val < 0) {
            perror ("read from ():");
            semctl (semid, 0, IPC_RMID);
            shmdt(shmbuf);
            shmctl (shmid, IPC_RMID, NULL);
            exit (EXIT_FAILURE);
        }

        V (semid, MUTEX, SEM_UNDO);
        V (semid, FULL, 0);
        P (semid, WCRITSEC, SEM_UNDO);
        W (semid, RCRITSEC, 0);

//#ifdef BREAK
        ++j;
        if (j == 2) {
            printf ("\nWating while you kill reader\n");
            GetAllSemsInfo(semid);
            sleep (10);
            printf ("I waked up\n");
        }
// #endif
    }


    //P (semid, WCRITSEC);
    shmctl (shmid, IPC_RMID, NULL);
    V (semid, WEXIST, SEM_UNDO);

    //semctl (semid, 0 , IPC_RMID);

    return 0;
}

