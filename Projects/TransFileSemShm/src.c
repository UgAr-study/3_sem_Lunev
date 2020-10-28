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

int InitAllSems (int semid) {

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
    if (semctl (semid, WCRITSEC, SETVAL, 0) < 0) {
        perror ("semctl():");
        return -1;
    }
    if (semctl (semid, RCRITSEC, SETVAL, 1) < 0) {
        perror ("semctl():");
        return -1;
    }

    return 1;
}

int CreateAndInitSems (key_t key) {
    int semid;
    if ((semid = semget (key, 7, 0666 | IPC_CREAT | IPC_EXCL)) >= 0) {
        //int vals[5] = { 0, 0, 1, 0, 0};

        if (InitAllSems(semid) < 0) {
            perror("InitAllSems():");
            return -1;
        }

    } else if (errno == EEXIST) {
        semid = semget (key, 0, 0666 | IPC_CREAT);
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
    printf ("\n\nSemaphore info:[\n");
    printf ("semid is %d\n", semid);
    printf ("WEXIST is %d\n", GetVal(semid, WEXIST));
    printf ("REXIST is %d\n", GetVal(semid, REXIST));
    printf ("FULL is %d\n", GetVal(semid, FULL));
    printf ("EMPTY is %d\n", GetVal(semid, EMPTY));
    printf ("MUTEX is %d\n", GetVal(semid, MUTEX));
    printf ("WCRITSEC is %d\n", GetVal(semid, WCRITSEC));
    printf ("RCRITSEC is %d\n", GetVal(semid, RCRITSEC));
    printf ("]\n\n");
}


// return 1 if P(semid, n) will not block for infinity
// return -1 if some process shit to semaphore
// return 0 if something unexpected occurred

int CheckShit (int semid, int n) {
    pid_t lpid = semctl (semid, n, GETPID);
    char filepath[20] = "/proc/";
    sprintf (filepath + 6, "%d", lpid);
    printf ("dir with current process is %s\n", filepath);

    DIR* dir = opendir (filepath);
    printf ("ptr dir is %p\n", dir);
    if (dir == NULL) {
        if (errno == ENOENT) {
            if (W (semid, n, IPC_NOWAIT) == 0) {
                printf("this process [%d] shit to semaphore and died...\n", lpid);
                return -1;
            }
            return 1;
        }
        if (errno == ENOTDIR) {
            int fd = open (filepath, O_RDONLY);
            if (fd > 0) {
                printf ("Wow! The proc can be a file!\n");
                close(fd);
                return 1;
            }
            if (fd < 0 && errno == ENOENT) {
                if (W (semid, n, IPC_NOWAIT) == 0) {
                    printf("this process [%d] shit to semaphore and died...\n", lpid);
                    return -1;
                }
                return 1;
            }
        }
        perror("opendir():");
        return 0;
    }

    closedir(dir);
    return 1;
}