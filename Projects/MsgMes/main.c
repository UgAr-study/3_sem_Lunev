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

    struct pid_table child_pids;
    child_pids.table = (pid_t *) calloc(num, sizeof(pid_t));
    child_pids.size = num;

    if ((msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT)) < 0) {
        printf("Can't create msg\n");
        exit(EXIT_FAILURE);
    }


    pid_t ppid_bef_fork = getpid();
    for (unsigned i = 0; i < num; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            printf ("parent couldn't do fork...\n");
            DeleteMSQ(msgid);
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { //We are in the child

            if (SetParentDeath(msgid, ppid_bef_fork) < 0) {
                printf ("Can't do prctl\n");
                kill(getpid(), SIGKILL);
            }

            pid_t mypid = getpid();
            if (SendPermission(msgid, 1, mypid) < 0) {
                //sending to parent went wrong
                exit(EXIT_FAILURE);
            }

            long mynumber = GetMyNumber (msgid, mypid);
            if (mynumber < 0) {
                //parent didn't answer
                exit(EXIT_FAILURE);
            }
            printf ("%ld\n", mynumber);
            fflush(stdout);

            if (SendPermission(msgid, mypid, 0) < 0) {
                //sending confirmation to parent went wrong
                exit(EXIT_FAILURE);
            }

            exit(EXIT_SUCCESS);
        }

        //Wa are in the parent
        child_pids.table[i] = pid;
    }

    //exit(0);

    struct pid_table delayed_children;
    delayed_children.table = (pid_t *) calloc(num, sizeof(pid_t));
    delayed_children.size = num;

    for (unsigned i = 0; i < num; ++i)
        if (RecieveAndDelayPid (msgid, child_pids, &delayed_children) < 0) {
            DeleteMSQ(msgid);
            exit(EXIT_FAILURE);
        }

    for (unsigned i = 0; i < num; ++i) {
        pid_t pid = delayed_children.table[i];
        if (pid != 0) {
            if (SendPermission(msgid, pid, i) < 0) {
                //permission hasn't sent
                exit(EXIT_FAILURE);
            }
            if (GetConfirmation(msgid, pid) < 0) {
                //child didn't answer
                exit(EXIT_FAILURE);
            }
            continue;
        }

        //child is missed

        DeleteMSQ(msgid);
        exit(EXIT_FAILURE);
    }

    return 0;
}