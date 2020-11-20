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

            while(1) {

                int spl_ret_val;
                if ((spl_ret_val = splice(fd_from, NULL, fd_to, NULL, PAGE, SPLICE_F_MOVE)) == 0) {
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
                if (N == 1) channels[i].fd_to = STDOUT_FILENO;
                else channels[i].fd_to = -1;

            } else if (i == N - 1) {
                close (fds_to[0]);
                close (fds_from[1]);

                channels[i - 1].fd_to = fds_to[1];
                channels[i].fd_from = fds_from[0];
                channels[i].fd_to = STDOUT_FILENO;

            } else {
                close (fds_to[0]);
                close (fds_from[1]);

                channels[i - 1].fd_to = fds_to[1];
                channels[i].fd_from = fds_from[0];
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

    struct pollfd *poll_read_fds = (struct pollfd*) calloc (N, sizeof (struct pollfd));
    struct pollfd *poll_write_fds = (struct pollfd*) calloc (N, sizeof (struct pollfd));

    if (poll_write_fds == NULL || poll_read_fds == NULL) {
        printf ("Error: lack of memory\n");
        free_all(channels, N);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; ++i) {
        poll_write_fds[i].fd = channels[i].fd_to;
        poll_write_fds[i].events = POLLWRNORM;

        poll_read_fds[i].fd = channels[i].fd_from;
        poll_read_fds[i].events = POLLRDNORM;
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

    while (1) {

        int ready_to_read = poll (poll_read_fds, N, 0);

        if (ready_to_read > 0) {

            int closed_read_fds = 0;
            for (int i = 0; i < N; ++i) {

                if (poll_read_fds[i].revents & POLLNVAL) {
                    closed_read_fds++;
                    continue;
                }

                if (poll_read_fds[i].revents & POLLRDNORM) {
                    if (channels[i].empty != 0) {
                        PutInBuffer(channels, i, N);
                    }
                } else if (poll_read_fds[i].revents & POLLHUP) {
                    close (channels[i].fd_from);
                }
            }

            if (closed_read_fds == N) {
                while (channels[N - 1].full)
                    GetFromBuffer(channels, N - 1, N);
                break;
            }

        }

        int ready_to_write = poll (poll_write_fds, N, 0);

        if (ready_to_write > 0) {

            for (int i = 0; i < N; ++i) {

                if (poll_write_fds[i].revents & POLLERR) {
                    close (channels[i].fd_to);
                    continue;
                }

                if (poll_write_fds[i].revents & POLLWRNORM) {

                    if (child_deaths == i + 1 && channels[i].full == 0) {
                        if (i != N - 1) {
                            close(channels[i].fd_to);
                        }
                    }

                    if (channels[i].full != 0) {
                        GetFromBuffer(channels, i, N);
                    }
                }
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
                child_deaths++;
                return;
            }
        }
        printf ("One of the children was killed\n");
        exit(EXIT_FAILURE);
    }
}