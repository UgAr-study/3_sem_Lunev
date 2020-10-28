#include "message_header.h"

int RecieveAndDelayPid(int msgid, struct pid_table child_pids, struct pid_table *delayed) {
    struct msgbuf rcvbuf;
    size_t length = sizeof(unsigned);

    if (msgrcv(msgid, &rcvbuf, length, 0, 0) < 0) {
        perror("Recieve&Delay msgrcv()");
        DeleteMSQ(msgid);
        exit(EXIT_FAILURE);
    }

    pid_t rcvpid = rcvbuf.number;
    long position = GetThisChildNumber(rcvpid, child_pids);
    if (position < 0) {
        printf("unknown child\n");
        return -1;
    }

    delayed->table[position] = rcvpid;
    return 0;
}

long GetThisChildNumber(pid_t pid, struct pid_table child_pids) {
    for (unsigned i = 0; i < child_pids.size; ++i)
        if (child_pids.table[i] == pid)
            return i;

    return -1;
}

int Send(int msgid, long type, unsigned number) {
    struct msgbuf sndbuf;
    sndbuf.mtype = type;
    sndbuf.number = number;
    int len = sizeof(sndbuf) - sizeof(long);

    if (msgsnd(msgid, &sndbuf, sizeof(unsigned), 0) < 0) {
        perror("SendPerm msgsnd(): ");
        DeleteMSQ(msgid);
        return -1;
    }
    return 0;
}



int GetConfirmation(int msgid, pid_t pid) {
    struct msgbuf rcvbuf;

    if (msgrcv(msgid, &rcvbuf, sizeof(unsigned), pid, 0) < 0) {
        perror("GetConfirm msgrcv()");
        DeleteMSQ(msgid);
        return -1;
    }

    return 0;
}

long GetMyNumber (int msgid, pid_t mypid) {
    struct msgbuf rcvbuf;
    size_t length = sizeof(unsigned);

    if (msgrcv(msgid, &rcvbuf, length, mypid, 0) < 0) {
        perror("GetMyNUmber msgrcv()");
        DeleteMSQ(msgid);
        return -1;
    }

    return rcvbuf.number;
}

int SetParentDeath (int msgid, pid_t ppid_bef_fork) {

    if (prctl(PR_SET_PDEATHSIG, SIGKILL) < 0) {
        printf ("Can't do prctl\n");
        DeleteMSQ(msgid);
        return -1;
    }

    if (getppid() != ppid_bef_fork) {
        printf ("My parent has been changed\n");
        printf ("My new parent pid is [%d]\nI'm finish", getppid());
        DeleteMSQ(msgid);
        kill(getpid(), SIGKILL);
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
