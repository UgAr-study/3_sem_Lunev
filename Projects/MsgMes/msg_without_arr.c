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
/*

    struct pid_table child_pids;
    child_pids.table = (pid_t *) calloc(num, sizeof(pid_t));
    child_pids.size = num;
*/

    if ((msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT)) < 0) {
        printf("Can't create msg\n");
        exit(EXIT_FAILURE);
    }

#ifdef PROTECT
    pid_t ppid_bef_fork = getpid();
#endif
    for (unsigned i = 0; i < num; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            printf ("parent couldn't do fork...\n");
            DeleteMSQ(msgid);
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { //We are in the child

#ifdef PROTECT
            if (SetParentDeath(msgid, ppid_bef_fork) < 0) {
                printf ("Can't do prctl\n");
                kill(getpid(), SIGKILL);
            }
#endif

            //pid_t mypid = getpid();
            /*if (Send (msgid, 1, i) < 0) {
                exit (EXIT_FAILURE);
            }
            */
            if (Send(msgid, 1, i) < 0) {
                //sending to parent went wrong
                exit(EXIT_FAILURE);
            }

            //long mynumber = GetMyNumber (msgid, mypid);
            GetMyNumber(msgid, i);
            /*if (mynumber < 0) {
                //parent didn't answer
                exit(EXIT_FAILURE);
            }*/

            printf ("%d\n", i);
            fflush(stdout);

            if (Send(msgid, i, 0) < 0) {
                //sending confirmation to parent went wrong
                exit(EXIT_FAILURE);
            }

            exit(EXIT_SUCCESS);
        }

        //Wa are in the parent
        //child_pids.table[i] = pid;
    }

    //exit(0);
/*

    struct pid_table delayed_children;
    delayed_children.table = (pid_t *) calloc(num, sizeof(pid_t));
    delayed_children.size = num;
*/

    /*
    for (unsigned i = 0; i < num; ++i)
        if (RecieveAndDelayPid (msgid, child_pids, &delayed_children) < 0) {
            DeleteMSQ(msgid);
            exit(EXIT_FAILURE);
        }
    */
    for (unsigned i = 0; i < num; ++i) {
        //pid_t pid = delayed_children.table[i];
        Send (msgid, i, 0);
        GetConfirmation(msgid, i);
        /*
        if (pid > 0) {
            if (Send(msgid, pid, i) < 0) {
                //permission hasn't sent
                exit(EXIT_FAILURE);
            }
            if (GetConfirmation(msgid, pid) < 0) {
                //child didn't answer
                exit(EXIT_FAILURE);
            }
            continue;
        }*/

        //child is missed


        //exit(EXIT_FAILURE);
    }

    //DeleteMSQ(msgid);
    return 0;
}

