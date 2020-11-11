#include "header.h"

int main(int argc, char *argv[]) {

    printf ("Writer pid is %d\n\n", getpid());
    const char *file = "test.txt";
    int fdfrom = open (file, O_RDONLY);
    printf ("fd from is %d\n", fdfrom);

    key_t key = ftok ("src.c", 0);

    int semid = semget (key, 5, 0666 | IPC_CREAT);

    printf ("Writer: before all initializations\n");
    GetAllSemsInfo(semid);

    if (IsExist(semid, WEXIST, IPC_NOWAIT) == 1) {
        IsExist(semid, WEXIST, 0);
    }

    WaitOppenent(semid, REXIST);

    InitSemWith1(semid, EMPTY);
    InitSemWith1(semid, MUTEX);

    int shmid = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuf = (char*) shmat(shmid, NULL, 0);

    for (int i = 0; i < PAGESIZE; ++i)
        shmbuf[i] = '\0';

    P (semid, FULL, IPC_NOWAIT);

    int read_ret_val, j = 1;
    while (1) {

        printf ("Writer: before P(EMPTY)\n");
        GetAllSemsInfo(semid);

        if (IsAlive(semid, REXIST, EMPTY) != 1) {
            printf ("Writer: Reader died\n");
            shmdt (shmbuf);
            //V (semid, MUTEX, SEM_UNDO);
            //V (semid, EMPTY, 0);
            break;
        }

        P (semid, MUTEX, SEM_UNDO);

        for (int i = 0; i < PAGESIZE; ++i)
            shmbuf[i] = '\0';

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

#ifdef BREAK
        ++j;
        if (j == 2) {
            printf ("\nWating while you kill reader\n");
            GetAllSemsInfo(semid);
            sleep (10);
            printf ("I waked up\n");
        }
#endif
    }

    //semctl (semid, 0 , IPC_RMID);
    shmctl (shmid, IPC_RMID, NULL);
    P (semid, WEXIST, SEM_UNDO);

    return 0;
}

