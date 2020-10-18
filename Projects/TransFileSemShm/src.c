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

int CreateAndInitSems (key_t key) {
    int semid;
    if ((semid = semget (key, 6, 0666 | IPC_CREAT | IPC_EXCL)) >= 0) {
        //int vals[5] = { 0, 0, 1, 0, 0};

        if (semctl (semid, WEXIST, SETVAL, 1) < 0) {
            perror ("semctl():");
            return -1;
        }
        if (semctl (semid, REXIST, SETVAL, 1) < 0) {
            perror ("semctl():");
            return -1;
        }
        if (semctl (semid, MUTEX, SETVAL, 1) < 0) {
            perror ("semctl():");
            return -1;
        }
        if (semctl (semid, EMPTY, SETVAL, 1) < 0) {
            perror ("semctl():");
            return -1;
        }
        if (semctl (semid, FULL, SETVAL, 0) < 0) {
            perror ("semctl():");
            return -1;
        }
        if (semctl (semid, REMOVE, SETVAL, 1) < 0) {
            perror ("semctl():");
            return -1;
        }

    } else if (errno == EEXIST) {
        semid = semget (key, 4, 0666 | IPC_CREAT);
        struct semid_ds infobuf;
        semctl (semid, 0, IPC_STAT, &infobuf);
        printf ("otime is %ld\n", infobuf.sem_otime);
        while (infobuf.sem_otime == 0)
            semctl (semid, 0, IPC_STAT, &infobuf);
    } else {
        perror ("W: semget():");
        return -1;
    }

    return semid;
}

void GetAllSemsInfo (int semid) {
    printf ("semid is %d\n", semid);
    printf ("WEXIST is %d\n", GetVal(semid, WEXIST));
    printf ("REXIST is %d\n", GetVal(semid, REXIST));
    printf ("FULL is %d\n", GetVal(semid, FULL));
    printf ("EMPTY is %d\n", GetVal(semid, EMPTY));
    printf ("MUTEX is %d\n", GetVal(semid, MUTEX));
    printf ("REMOVE is %d\n", GetVal(semid, REMOVE));
}