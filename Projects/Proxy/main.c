#include "header.h"

int child_deaths = 0;

int main(int argc, char *argv[]) {

    int N = N_DEF;
    if (argc == 1) {
        printf ("too few arguments\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 3) {
        N = strtol(argv[2], NULL, 10);
    }

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = child_handler;

    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        printf ("Can't do signal handler\n");
        exit(EXIT_FAILURE);
    }

    struct Channel *channels = (struct Channel*) calloc (N, sizeof (struct Channel));

    if (channels == NULL) {
        printf ("Error: lack of memory\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; ++i) {

        int fds_to[2] = {0};
        int fds_from[2] = {0};

        pipe(fds_to);
        pipe(fds_from);

        pid_t ppid_before_fork = getpid();
        pid_t pid = fork();

        if (pid < 0) {
            printf ("parent can't do fork\n");
            free_all(channels, i + 1);
            perror ("fork: ");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {

            for (int j = 0; j < i; ++j) {
                close (channels[j].fd_from);
                close (channels[j].fd_to);
            }


            if (SetParentDeath(ppid_before_fork) < 0) {
                printf ("parent died\n");
                fflush(stdout);
                exit(EXIT_FAILURE);
            }

            int fd_from = 0, fd_to = 0;

            if (i == 0) {
                close(fds_to[0]);
                close(fds_to[1]);
                close(fds_from[0]);

                char *filepath = argv[1];
                fd_from = open (filepath, O_RDONLY);

                if (fd_from <= 0) {
                    printf ("Can't open a file\n");
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                }

                fd_to = fds_from[1];
            } else {
                close (fds_from[0]);
                close (fds_to[1]);

                fd_from = fds_to[0];
                fd_to = fds_from[1];
            }

            int spl_ret_val;

            while(1) {

                if ((spl_ret_val = splice(fd_from, NULL, fd_to, NULL, PAGE, SPLICE_F_MOVE)) == 0) { //кс за pipe
                    close (fd_to);
                    close (fd_from);

                    exit(EXIT_SUCCESS);
                }

                if (spl_ret_val < 0) {
                    printf ("child[%d]: splice failed\n", i);
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                }
            }

        } else {

            if (i == 0) {
                close (fds_to[0]);
                close (fds_to[1]);
                close (fds_from[1]);

                channels[i].fd_from = fds_from[0];
                channels[i].is_can_read = 1;

                if (N == 1) {
                    channels[i].fd_to = STDOUT_FILENO;
                    channels[i].is_can_write = 1;
                }

                else channels[i].fd_to = -1;

            } else if (i == N - 1) {
                close (fds_to[0]);
                close (fds_from[1]);

                channels[i - 1].fd_to = fds_to[1];
                channels[i - 1].is_can_write = 1;

                channels[i].fd_from = fds_from[0];
                channels[i].is_can_read = 1;

                channels[i].fd_to = STDOUT_FILENO;
                channels[i].is_can_write = 1;

            } else {
                close (fds_to[0]);
                close (fds_from[1]);

                channels[i - 1].fd_to = fds_to[1];
                channels[i - 1].is_can_write = 1;

                channels[i].fd_from = fds_from[0];
                channels[i].is_can_read = 1;
            }

        }
    }

    for (int i = 0; i < N; ++i) {
        channels[i].size = (size_t)pow(3, N - i) * 1024;
        channels[i].buffer = (char*) calloc (channels[i].size, sizeof(char));

        if (channels[i].buffer == NULL) {
            printf ("Error: lack of memory\n");
            free_all(channels, i);
            exit(EXIT_FAILURE);
        }

        channels[i].offset_for_read = channels[i].buffer;
        channels[i].offset_for_write = channels[i].buffer;
        channels[i].empty = channels[i].size;
        channels[i].full = 0;
    }


    for (int i = 0; i < N; ++i) {
        int res;
        res = fcntl(channels[i].fd_from, F_SETFL, O_NONBLOCK);

        if (res < 0) {
            perror("fcntl read: ");
            exit(EXIT_FAILURE);
        }

        res = fcntl(channels[i].fd_to, F_SETFL, O_NONBLOCK);

        if (res < 0) {
            perror("fcntl write: ");
            exit(EXIT_FAILURE);
        }
    }

    fd_set rfds;
    fd_set wfds;


    while (child_deaths < N) {

        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        int max = 0;
        for (int i = 0; i < N; ++i) {

            if (channels[i].is_can_read && channels[i].empty != 0) {
                FD_SET (channels[i].fd_from, &rfds);

                if (channels[i].fd_from > max)
                    max = channels[i].fd_from;

            }

            if (channels[i].is_can_write && channels[i].full != 0) {
                FD_SET (channels[i].fd_to, &wfds);

                if (channels[i].fd_to > max)
                    max = channels[i].fd_to;

            }

        }

        int ret_sel_val = select(max + 1, &rfds, &wfds, NULL, NULL);

        if (ret_sel_val < 0) {

            if (errno == EINTR)
                continue;

            perror ("select: ");
            exit(EXIT_FAILURE);
        }

        for (int i = child_deaths; i < N; ++i) {

            if (FD_ISSET(channels[i].fd_from, &rfds)) {
                //can read from it
                int num_read = PutInBuffer(channels, i, N); //кс за pipe родитель-ребенок

                if (num_read == 0) {
                    close(channels[i].fd_from);
                    channels[i].is_can_read = 0;
                } //конец этой кс за pipe

                if (num_read < 0) {
                    printf ("smth strange fro read side\n");
                    continue;
                }
            }

            if (FD_ISSET(channels[i].fd_to, &wfds)) {
                //can write to it
                int num_write = GetFromBuffer(channels, i, N);

                if (num_write < 0) {
                    //fd_to pipe has no enough space
                    continue;
                }
            }

            if (channels[i].is_can_read == 0 && channels[i].full == 0) {

                if (i != child_deaths) {
                    printf ("i = %d; some child died too early; ch_deaths = %d\n", i, child_deaths);
                    free_all(channels, N);
                    exit(EXIT_FAILURE);
                }

                close(channels[i].fd_to);
                channels[i].is_can_write = 0;
                child_deaths++;
            }
        }
    }

    free_all(channels, N);

    return 0;
}


void child_handler(int s) {
    if (s == SIGCHLD) {
        int state = 0;
        wait(&state);

        if (WIFEXITED(state)) {
            if (WEXITSTATUS(state) == EXIT_SUCCESS) {
                return;
            }
        }

        printf ("One of the children was killed\n");
        exit(EXIT_FAILURE);
    }
}