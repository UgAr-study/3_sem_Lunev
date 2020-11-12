#include "header.h"

int P (int semid, int n, int flags) {
    struct sembuf sb;
    sb.sem_num = n;
    sb.sem_op = -1;
    sb.sem_flg = flags;
    if (semop (semid, &sb, 1) < 0) {
        perror ("P: semop():");
        return -1;
    }
    return 0;
}

int V (int semid, int n, int flags) {
    struct sembuf sb;
    sb.sem_num = n;
    sb.sem_op = 1;
    sb.sem_flg = flags;
    if (semop (semid, &sb, 1) < 0) {
        perror ("V: semop():");
        return -1;
    }
    return 0;
}

int W (int semid, int n, int flags) {
    struct sembuf sb;
    sb.sem_num = n;
    sb.sem_op = 0;
    sb.sem_flg = flags;
    if (semop (semid, &sb, 1) < 0) {
        perror("W: semop():");
        return -1;
    }
    return 0;
}

int GetVal (int semid, int n) {
    return semctl (semid, n, GETVAL, 0);
}

void InitSemWith1 (int semid, int num) {
    struct sembuf sb[2] = {};

    sb[0].sem_num = num;
    sb[0].sem_op = 0;
    sb[0].sem_flg = IPC_NOWAIT;

    sb[1].sem_num = num;
    sb[1].sem_op = 1;
    sb[1].sem_flg = 0;

    if (semop (semid, sb, 2) < 0)
        printf ("This sem is already non zero\n");
}



// return 1 if num is already exist, 0 - if not
int IsExist (int semid, int num, int wait_flag) {
    struct sembuf sb[2] = {};

    sb[0].sem_num = num;
    sb[0].sem_op = 0;
    sb[0].sem_flg = wait_flag;

    sb[1].sem_num = num;
    sb[1].sem_op = 1;
    sb[1].sem_flg = SEM_UNDO;

    if (semop (semid, sb, 2) < 0) {
        printf ("Failed\n");
        return 1;
    }

    return 0;
}

// 1 if who is alive, 0 if not
int IsAlive (int semid, int who, int what) {
    struct sembuf sb[3] = {};

    sb[0].sem_num = who;
    sb[0].sem_op = -1;
    sb[0].sem_flg = IPC_NOWAIT;

    sb[1].sem_num = who;
    sb[1].sem_op = 1;
    sb[1].sem_flg = 0;

    sb[2].sem_num = what;
    sb[2].sem_op = -1;
    sb[2].sem_flg = 0;

    if (semop (semid, sb, 3) < 0) {
        return 0;
    }

    return 1;
}

void WaitOppenent (int semid, int num) {
    struct sembuf sb[2] = {};

    sb[0].sem_num = num;
    sb[0].sem_op = -1;
    sb[0].sem_flg = 0;

    sb[1].sem_num = num;
    sb[1].sem_op = 1;
    sb[1].sem_flg = 0;

    if (semop (semid, sb, 2) < 0) {
        printf ("Waiting fail\n");
    }
}

void GetAllSemsInfo (int semid) {
    printf ("\nSemaphore info:[\n");
    printf ("semid  is %d\n", semid);
    printf ("WEXIST is %d\n", GetVal(semid, WEXIST));
    printf ("REXIST is %d\n", GetVal(semid, REXIST));
    printf ("PRINT  is %d\n", GetVal(semid, PRINT));
    printf ("MEMORY is %d\n", GetVal(semid, MEMORY));
    printf ("PAIR   is %d\n", GetVal(semid, PAIR));
    printf ("FINISH is %d\n", GetVal(semid, FINISH));
    printf ("]\n\n");
}