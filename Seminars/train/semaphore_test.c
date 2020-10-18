#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

int main () {
    const char *file = "semaphore_test.c";
    key_t key = ftok (file, 0);
    int semid;
    if ((semid = semget (key, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
        perror ("semget():");
        return 0;
    }
    struct semid_ds infobuf;
    union semun {
        int val;
        struct semid_ds *buf;
    } arg;
    arg.buf = &infobuf;
    if (semctl (semid, 0, IPC_STAT, arg.buf) < 0) {
        perror("stat():");
    }
    printf ("sem_otime before op is %ld\n", arg.buf->sem_otime);

    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);

    if (semctl (semid, 0, IPC_STAT, arg.buf) < 0) {
        perror ("stat():");
    }

    printf ("sem_otime after op is %ld\n", arg.buf->sem_otime);
    if (semctl(semid, 0, IPC_RMID) < 0) {
        perror("rm():");
    }

    return 0;
}
