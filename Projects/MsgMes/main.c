#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

enum { N = 1000000 };

struct msgbuf {
    long mtype;
    unsigned number;
};

int main() {

    int msgid;

    printf ("parent pid is %d\n", getpid());

    pid_t *table_chpids = (pid_t*) calloc (N, sizeof(pid_t));

    if ((msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT)) < 0) {
        printf ("Can't create msg\n");
        exit (EXIT_FAILURE);
    }

    for (unsigned i = 0; i < N; ++i) {
        pid_t pid = fork();

        table_chpids[i] = pid;

        if (pid < 0) {
            printf ("Can't do fork on %u iteration\n", i);
            msgctl(msgid, IPC_RMID, NULL);
            for (int j = 0; j < i; ++j)
                kill(table_chpids[j], SIGKILL);

            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            //getchar();
            struct msgbuf rcvbuf;
            if (msgrcv(msgid, (struct msgbuf*) &rcvbuf, sizeof(unsigned), (long) getpid(), 0) < 0) {
                printf ("Can't receive\n");
                msgctl(msgid, IPC_RMID, NULL);
                exit(EXIT_FAILURE);
            }
            printf ("%u\n", rcvbuf.number);
            return 0;
        }

    }

    for (int i = 0; i < N; ++i) {
        struct msgbuf sndbuf;
        sndbuf.mtype = table_chpids[i];
        sndbuf.number = i;
        int len = sizeof(unsigned);
        if (msgsnd(msgid, (struct msgbuf*) &sndbuf, len, 0) < 0) {
            printf ("Can't send\n");
            msgctl(msgid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }

        waitpid (table_chpids[i], NULL, 0);
    }

    return 0;
}
