#include "header.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf ("few arguments\n");
        exit (EXIT_FAILURE);
    }

    int fdfrom = open (argv[1], O_RDONLY);

    if (fdfrom < 0) {
        printf ("file is not opened\n");
        exit(EXIT_FAILURE);
    }

    key_t key = ftok ("src.c", 0);

    int semid = semget (key, 6, 0666 | IPC_CREAT);

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
        struct sembuf op;

        op.sem_num = PAIR;
        op.sem_op = 0;
        op.sem_flg = 0;

        if (semop(semid, &op, 1) < 0) {
            perror ("Checking PAIR: semop: ");
            exit(EXIT_FAILURE);
        }
    }

    semctl(semid, PRINT, SETVAL, 1);

    {
        struct sembuf ops[2];

        ops[0].sem_num = WEXIST;
        ops[0].sem_op = 1;
        ops[0].sem_flg = SEM_UNDO;

        ops[1].sem_num = PRINT;
        ops[1].sem_op = -1;
        ops[1].sem_flg = SEM_UNDO;

        if (semop(semid, ops, 2) < 0) {
            perror("Set PRINT: semop: ");
            exit(EXIT_FAILURE);
        }
    }

    {
        struct sembuf ops[3];

        ops[0].sem_num = REXIST;
        ops[0].sem_op = -2;
        ops[0].sem_flg = 0;

        ops[1].sem_num = REXIST;
        ops[1].sem_op = 2;
        ops[1].sem_flg = 0;

        ops[2].sem_num = PAIR;
        ops[2].sem_op = 1;
        ops[2].sem_flg = SEM_UNDO;

        if (semop(semid, ops, 3) < 0) {
            printf ("Writer: Waiting reader failed\n");
            exit(EXIT_FAILURE);
        }
    }


    int shmid = shmget (key, sizeof(struct buffer), 0666 | IPC_CREAT);

    if (shmid < 0) {
        perror("shmget: ");
        exit(EXIT_FAILURE);
    }

    char *shmbuf = (char*) shmat(shmid, NULL, 0);

    while (1) {

        {
            struct sembuf ops[3];

            ops[0].sem_num = PAIR;
            ops[0].sem_op = -2;
            ops[0].sem_flg = IPC_NOWAIT;

            ops[1].sem_num = PAIR;
            ops[1].sem_op = 2;
            ops[1].sem_flg = 0;

            ops[2].sem_num = MEMORY;
            ops[2].sem_op = -1;
            ops[2].sem_flg = 0;

            if (semop(semid, ops, 3) < 0) {
                printf("Writer: reader died\n");
                break;
            }
        }

        struct buffer buffer;
        buffer.size = read (fdfrom, buffer.buf, PAGESIZE);

        if (buffer.size == 0) {

            {
                struct sembuf ops[2];

                ops[0].sem_num = PRINT;
                ops[0].sem_op = 1;
                ops[0].sem_flg = 0;

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

        if (buffer.size < 0) {
            perror ("read from ():");
            semctl (semid, 0, IPC_RMID);
            shmdt(shmbuf);
            shmctl (shmid, IPC_RMID, NULL);
            exit (EXIT_FAILURE);
        }

        memcpy (shmbuf, &buffer, sizeof(buffer));

        V (semid, PRINT, 0);
    }

    {
        struct sembuf ops[2];

        ops[0].sem_num = PAIR;
        ops[0].sem_op = -1;
        ops[0].sem_flg = SEM_UNDO;

        ops[1].sem_num = WEXIST;
        ops[1].sem_op = -2;
        ops[1].sem_flg = SEM_UNDO;

        semop(semid, ops, 2);
    }

    close (fdfrom);

    return 0;
}

