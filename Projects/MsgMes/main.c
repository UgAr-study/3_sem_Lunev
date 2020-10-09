#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <errno.h>
#include <signal.h>

enum { N = 10 };

int MsgQContainer (int id) {
    static int msgid;
    if (id != 0) {
        msgid = id;
    }
    return msgid;
}

void handler (int sig) {
    if (sig == 0) {
        printf ("My parent died, so I'm too\n");
        kill(getpid(), SIGKILL);
        return;
    }
    if (msgctl(MsgQContainer(0), IPC_RMID, NULL) < 0 && errno != EINVAL) {
        printf ("Can't remove queue in handler");
        exit(EXIT_FAILURE);
    }
    printf ("Undefined sidnal [%d]", sig);
}

struct msgbuf {
    long mtype;
    unsigned number;
};

int main(int argc, char* argv[]) {

    int num;
    int msgid;

    if (argc == 1) {
        num = N;
    } else {
        char *endptr;
        num = strtol (argv[1], &endptr, 10);
        if (*endptr != '\0') {
            printf ("bad arg\n");
            exit (EXIT_FAILURE);
        }
    }

    printf ("parent pid is %d\n", getpid());

    pid_t *table_chpids = (pid_t*) calloc (num, sizeof(pid_t));

    if ((msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT)) < 0) {
        printf ("Can't create msg\n");
        exit (EXIT_FAILURE);
    }

    if (MsgQContainer(msgid) != msgid) {
        printf ("Container does wrong\n");
        if (msgctl(msgid, IPC_RMID, NULL) < 0) {
            printf("Can't remove the msg queue\n");
            perror("parent msgctl()");
        }
        exit(EXIT_FAILURE);
    }

    for (unsigned i = 0; i < num; ++i) {

        pid_t ppid_bef_fork = getpid();
        pid_t pid = fork();

        table_chpids[i] = pid;

        if (pid < 0) {
            printf ("Can't do fork on %u iteration\n", i);
            for (int j = 0; j < i; ++j)
                kill(table_chpids[j], SIGKILL);

            if (msgctl(msgid, IPC_RMID, NULL) < 0) {
                printf("Can't remove the msg queue\n");
                perror("parent msgctl()");
            }

            exit(EXIT_FAILURE);
        }

        if (pid == 0) {

            if (prctl(PR_SET_PDEATHSIG, SIGUSR1) < 0) {
                printf ("Can't do prctl\n");

                if (msgctl(msgid, IPC_RMID, NULL) < 0) {
                    if (errno == EIDRM)
                        printf ("\n[%d]: queue is already removed\n", getpid());
                    if (errno == EINVAL)
                        printf ("\n[%d]: Invalid arg\n", getpid());

                    printf("[%d]: Can't remove the msg queue\n", getpid());
                    //perror ("child after prctl msgtl()");
                }

                exit(EXIT_FAILURE);
            }

            signal(SIGUSR1, handler);

            if (getppid() != ppid_bef_fork) {
                printf ("My parent has been changed\n");
                printf ("My new parent pid is [%d]\nI'm finish", getppid());
                if (msgctl(msgid, IPC_RMID, NULL) < 0 && errno != EINVAL) {
                    printf("[%d]: Can't remove the msg queue\n", getpid());
                    perror ("child after comparing the ppid msgctl()");
                }

                kill(getpid(), SIGKILL);
            }

            struct msgbuf rcvbuf;
            if (msgrcv(msgid, (struct msgbuf*) &rcvbuf, sizeof(unsigned), (long) getpid(), 0) < 0) {
                printf ("Can't receive\n");

                if (msgctl(msgid, IPC_RMID, NULL) < 0 && errno != EINVAL) {
                    printf("[%d]: Can't remove the msg queue\n", getpid());
                    perror ("child after comparing the ppid msgctl()");
                }

                exit(EXIT_FAILURE);
            }
            printf ("%u\n", rcvbuf.number);
            return 0;
        }

    }

    printf ("\nMy childs are:\n");
    for (int i = 0; i < num; ++i) {
        printf ("%d) [%d]\n", i, table_chpids[i]);
    }

    for (int i = 0; i < num; ++i) {
        struct msgbuf sndbuf;
        sndbuf.mtype = table_chpids[i];
        sndbuf.number = i;
        int len = sizeof(unsigned);

        if (msgsnd(msgid, (struct msgbuf*) &sndbuf, len, 0) < 0) {
            printf ("Can't send\n");

            if (msgctl(msgid, IPC_RMID, NULL) < 0 && errno != EIDRM) {
                printf("[%d]: Can't remove the msg queue\n", getpid());
                perror ("parent after send msgctl()");
            }

            exit(EXIT_FAILURE);
        }

        waitpid (table_chpids[i], NULL, 0);
    }

    return 0;
}
