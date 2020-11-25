#include "header.h"

int main(int argc, char *argv[]) {

    //printf ("Reader pid is %d\n\n", getpid());
    key_t key = ftok ("src.c", 0);

    int semid = semget (key, 6, 0666 | IPC_CREAT);

    //printf ("Reader: before all initializations\n");
    //GetAllSemsInfo(semid);

    {
        struct sembuf ops[2];

        ops[0].sem_num = REXIST;
        ops[0].sem_op = 0;
        ops[0].sem_flg = 0;

        ops[1].sem_num = REXIST;
        ops[1].sem_op = 1;
        ops[1].sem_flg = SEM_UNDO;

        if (semop(semid, ops, 2) < 0) {
            perror("Checking REXIST: semop: ");
            exit(EXIT_FAILURE);
        }
    }

    {
        struct sembuf op;

        op.sem_num = PAIR;
        op.sem_op = 0;
        op.sem_flg = 0;

        if (semop(semid, &op, 1) < 0) {
            perror ("Reader: PAIR fail\n");
            exit(EXIT_FAILURE);
        }
    }

    // ??????? may be in writer.c check REXIST ?????
    semctl(semid, MEMORY, SETVAL, 2);

    {
        struct sembuf ops[2];

        ops[0].sem_num = REXIST;
        ops[0].sem_op = 1;
        ops[0].sem_flg = SEM_UNDO;

        ops[1].sem_num = MEMORY;
        ops[1].sem_op = -1;
        ops[1].sem_flg = SEM_UNDO;

        if (semop(semid, ops, 2) < 0) {
            printf ("Reader: decrement MEMORY failed\n");
            exit(EXIT_FAILURE);
        }
    }

    {
        struct sembuf ops[3];

        ops[0].sem_num = WEXIST;
        ops[0].sem_op = -2;
        ops[0].sem_flg = 0;

        ops[1].sem_num = WEXIST;
        ops[1].sem_op = 2;
        ops[1].sem_flg = 0;

        ops[2].sem_num = PAIR;
        ops[2].sem_op = 1;
        ops[2].sem_flg = SEM_UNDO;

        if (semop(semid, ops, 3) < 0) {
            printf ("Reader: Waiting failed\n");
            exit(EXIT_FAILURE);
        }
    }

    int shmid = shmget (key, PAGESIZE * sizeof(char), 0666 | IPC_CREAT);
    char *shmbuf = (char*) shmat(shmid, NULL, 0);

    while (1) {

        {
            // may be check PAIR == 2 ( semctl (GETVAL) );
            struct sembuf ops[4];

            ops[0].sem_num = PAIR;
            ops[0].sem_op = -2;
            ops[0].sem_flg = IPC_NOWAIT;

            ops[1].sem_num = PAIR;
            ops[1].sem_op = 2;
            ops[1].sem_flg = 0;

            ops[2].sem_num = PRINT;
            ops[2].sem_op = -1;
            ops[2].sem_flg = 0;

            ops[3].sem_num = FINISH;
            ops[3].sem_op = 0;
            ops[3].sem_flg = IPC_NOWAIT;


            if (semop(semid, ops, 4) < 0) {
                //perror("semop in reader: ");
                //printf("Reader: writer died or finished\n");
                write (STDOUT_FILENO, "\0", 1);
                break;
            }
        }

        //printf (">>>> \n");
        int write_ret_val = write (STDOUT_FILENO, shmbuf, PAGESIZE);
        //printf ("<<<<\n");

        if (write_ret_val < 0) {
            perror("write from shm ():");
            semctl(semid, 0, IPC_RMID);
            shmdt(shmbuf);
            shmctl (shmid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }

        V (semid, MEMORY, 0);
    }

    {
        struct sembuf ops[2];

        ops[0].sem_num = PAIR;
        ops[0].sem_op = -1;
        ops[0].sem_flg = SEM_UNDO;

        ops[1].sem_num = REXIST;
        ops[1].sem_op = -2;
        ops[1].sem_flg = SEM_UNDO;

        semop(semid, ops, 2);
    }

    return 0;
}
