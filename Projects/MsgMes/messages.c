#include "message_header.h"

int Send(int msgid, long type, unsigned number) {
    struct msgbuf sndbuf;
    sndbuf.mtype = type;
    sndbuf.number = number;
    int len = sizeof(unsigned);

    if (msgsnd(msgid, &sndbuf, len, 0) < 0) {
        return -1;
    }
    return 0;
}

int Receive(int msgid, long type) {
    struct msgbuf rcvbuf;
    if (msgrcv(msgid, &rcvbuf, sizeof(unsigned), type, 0) < 0) {
        return -1;
    }
    return 0;
}


void DeleteMSQ (int msgid) {
    if (msgctl(msgid, IPC_RMID, NULL) < 0 && errno != EINVAL) {
        printf("[%d]: Can't remove the msg queue\n", getpid());
        perror ("msgctl():");
        exit(EXIT_FAILURE);
    }
}
