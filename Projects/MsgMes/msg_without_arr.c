#include "message_header.h"

int main(int argc, char* argv[]) {

    long num;
    int msgid;

    if (argc == 1) {
        num = N;
    } else {
        char *endptr;
        num = strtol(argv[1], &endptr, 10);
        if (*endptr != '\0') {
            printf("bad arg\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("parent pid is %d\n", getpid());

    if ((msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT)) < 0) {
        printf("Can't create msg\n");
        exit(EXIT_FAILURE);
    }

    for (unsigned i = 0; i < num; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            printf ("parent couldn't do fork...\n");
            DeleteMSQ(msgid);
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { //We are in the child

            if (Receive(msgid, i+1) < 0) {
                exit (EXIT_FAILURE);
            }

            printf ("%d\n", i + 1);
            fflush(stdout);

            if (Send(msgid, i + 1, 0) < 0) {
                exit(EXIT_FAILURE);
            }

            exit(EXIT_SUCCESS);
        }

    }


    for (unsigned i = 0; i < num; ++i) {
        Send (msgid, i + 1, 0);
        Receive(msgid, i + 1);
    }

    DeleteMSQ(msgid);
    return 0;
}

