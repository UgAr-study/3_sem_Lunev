#include "header.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf ("few arguments\n");
        exit (EXIT_FAILURE);
    }

    //printf ("Writer pid is %d\n\n", getpid());
    //const char *file = "test.txt";
    int fdfrom = open (argv[1], O_RDONLY);
    //printf ("fd from is %d\n", fdfrom);

    if (fdfrom < 0) {
        printf ("file is not opened\n");
        exit(EXIT_FAILURE);
    }

    key_t key = ftok ("src.c", 0);

    int semid = semget (key, 6, 0666 | IPC_CREAT);

    //printf ("semid = %d\n", semid);
    //printf ("Writer: before all initializations\n");
    //GetAllSemsInfo(semid);

    {
        struct sembuf ops[2];

        ops[0].sem_num = WEXIST;
        ops[0].sem_op = 0;
        ops[0].sem_flg = 0;

        ops[1].sem_num = WEXIST;
        ops[1].sem_op = 1;
        ops[1].sem_flg = SEM_UNDO;

        if (semop(semid, ops, 2) < 0) {
            perror("Checking WEXIST: semop: ");
            exit(EXIT_FAILURE);
        }
    }


    {
        struct sembuf ops[2];

        ops[0].sem_num = REXIST;
        ops[0].sem_op = -1;
        ops[0].sem_flg = 0;

        ops[1].sem_num = REXIST;
        ops[1].sem_op = 1;
        ops[1].sem_flg = 0;

        if (semop(semid, ops, 2) < 0) {
            printf ("Writer: Waiting failed\n");
            exit(EXIT_FAILURE);
        }

        semctl(semid, MEMORY, SETVAL, 1);
    }


    {
        struct sembuf ops[3];

        ops[0].sem_num = PAIR;
        ops[0].sem_op = -1;
        ops[0].sem_flg = 0;

        ops[1].sem_num = PAIR;
        ops[1].sem_op = 1;
        ops[1].sem_flg = 0;

        ops[2].sem_num = PAIR;
        ops[2].sem_op = 1;
        ops[2].sem_flg = SEM_UNDO;

        if (semop (semid, ops, 3) < 0) {
            perror ("Writer: PAIR fail\n");
            exit(EXIT_FAILURE);
        }
    }

    int shmid = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuf = (char*) shmat(shmid, NULL, 0);

    for (int i = 0; i < PAGESIZE; ++i)
        shmbuf[i] = '\0';


    while (1) {

        //printf ("Writer in while\n");
        //GetAllSemsInfo(semid);

        {
            struct sembuf ops[3];

            ops[0].sem_num = REXIST;
            ops[0].sem_op = -1;
            ops[0].sem_flg = IPC_NOWAIT;

            ops[1].sem_num = REXIST;
            ops[1].sem_op = 1;
            ops[1].sem_flg = 0;

            ops[2].sem_num = MEMORY;
            ops[2].sem_op = -1;
            ops[2].sem_flg = 0;

            if (semop(semid, ops, 3) < 0) {
                printf("Writer: reader died\n");
                break;
            }
        }

        for (int i = 0; i < PAGESIZE; ++i)
            shmbuf[i] = '\0';

        int read_ret_val = read (fdfrom, shmbuf, PAGESIZE);

        if (read_ret_val == 0) {
    //        printf ("Writer finish\n");

            {
                struct sembuf ops[2];

                ops[0].sem_num = PRINT;
                ops[0].sem_op = 1;
                ops[0].sem_flg = SEM_UNDO;

                ops[1].sem_num = FINISH;
                ops[1].sem_op = 1;
                ops[1].sem_flg = SEM_UNDO;

                if (semop(semid, ops, 2) < 0) {
                    printf ("Writer: finish failed\n");
                    exit(EXIT_FAILURE);
                }
            }

            shmdt (shmbuf);
            break;
        }

        if (read_ret_val < 0) {
            perror ("read from ():");
            semctl (semid, 0, IPC_RMID);
            shmdt(shmbuf);
            shmctl (shmid, IPC_RMID, NULL);
            exit (EXIT_FAILURE);
        }

        V (semid, PRINT, 0);
    }

    //printf ("Writer: state after while\n");
    //GetAllSemsInfo(semid);

    {
        struct sembuf ops[2];

        ops[0].sem_num = PAIR;
        ops[0].sem_op = -1;
        ops[0].sem_flg = SEM_UNDO;

        semop(semid, ops, 1);

        ops[1].sem_num = PAIR;
        ops[1].sem_op = 0;
        ops[1].sem_flg = 0;

        semop(semid, ops + 1, 1);
    }

    {
        struct sembuf op;

        op.sem_num = WEXIST;
        op.sem_op = -1;
        op.sem_flg = SEM_UNDO;

        semop(semid, &op, 1);
    }

    //GetAllSemsInfo(semid);

    semctl(semid, 0, IPC_RMID);
    close (fdfrom);

    //printf ("Writer: success!\n");
    return 0;
}

