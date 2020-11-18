#include "header.h"

int main() {

    end = 0;

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = child_handler;

    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        printf ("Can't do signal handler\n");
        exit(EXIT_FAILURE);
    }

    printf ("Parent pid is %d\n", getpid());

    struct Channel *channels = (struct Channel*) calloc (N, sizeof (struct Channel));

    struct pollfd *poll_read_fds = (struct pollfd*) calloc (N, sizeof (struct pollfd));
    struct pollfd *poll_write_fds = (struct pollfd*) calloc (N, sizeof (struct pollfd));

    for (int i = 0; i < N; ++i) {

        int fds_to[2] = {0};
        int fds_from[2] = {0};

        pipe(fds_to);
        pipe(fds_from);

        printf ("\n\n>>>>Pipe[%d]::fds_to[0]   = %d\n"
                ">>>>Pipe[%d]::fds_to[1]   = %d\n"
                ">>>>Pipe[%d]::fds_from[0] = %d\n"
                ">>>>Pipe[%d]::fds_from[1] = %d\n\n\n",
                i, fds_to[0], i, fds_to[1], i, fds_from[0], i, fds_from[1]);

        pid_t ppid_before_fork = getpid();
        pid_t pid = fork();

        if (pid < 0) {
            printf ("parent can't do fork\n");
            free_all(channels, i + 1);
            perror ("fork: ");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {

            if (SetParentDeath(ppid_before_fork) < 0) {
                printf ("parent died\n");
                exit(EXIT_FAILURE);
            }

            int fd_from = 0, fd_to = 0;
            close (fds_to[1]);
            close (fds_from[0]);

            if (i == 0) {
                char *filepath = "test.txt";

               // sleep(5);

                fd_from = open (filepath, O_RDONLY);

                printf ("\nfile descriptor is %d\n", fd_from);

                if (fd_from <= 0) {
                    printf ("Can't open a file\n");
                    exit(EXIT_FAILURE);
                }

                fd_to = fds_from[1];

            } else {
                fd_from = fds_to[0];
                fd_to = fds_from[1];
            }

            while(1) {

                if (i == 100 * N/*some condition*/) {
                    break;
                }

                printf ("\nChild[%d]: splice from %d to %d\n\n", i, fd_from, fd_to);

                if (splice(fd_from, NULL, fd_to, NULL, PAGE, SPLICE_F_MOVE) <= 0) {
                    printf ("child №%d finished\n", i);
                    exit(EXIT_SUCCESS);
                }

            }

        } else {

            if (i == 0) {
                close(fds_to[0]);
                close (fds_to[1]);

                channels[i].size = (size_t)pow(3, N - i) * 1024;
                channels[i].offset_for_read = NULL;
                channels[i].offset_for_write = NULL;
                channels[i].empty = channels[i].size;
                channels[i].full = 0;

                channels[i].fd_from = fds_from[0];
                channels[i].buffer = (char*) calloc (channels[i].size, sizeof(char));

            } else if (i == N - 1) {
                channels[i].size = (size_t)pow(3, N - i) * 1024;
                channels[i].offset_for_read = NULL;
                channels[i].offset_for_write = NULL;
                channels[i].empty = channels[i].size;
                channels[i].full = 0;

                channels[i - 1].fd_to = fds_to[1];
                channels[i].fd_from = fds_from[0];
                channels[i].fd_to = STDOUT_FILENO;
                channels[i].buffer = (char*) calloc (channels[i].size, sizeof(char));

            } else {
                channels[i].size = (size_t)pow(3, N - i) * 1024;
                channels[i].offset_for_read = NULL;
                channels[i].offset_for_write = NULL;
                channels[i].empty = channels[i].size;
                channels[i].full = 0;

                channels[i - 1].fd_to = fds_to[1];
                channels[i].fd_from = fds_from[0];
                channels[i].buffer = (char*) calloc (channels[i].size, sizeof(char));

            }

            printf ("pid[%d] = %d\n", i, pid);
            if (i == 0) {
                close (fds_from[1]);
                continue;
            }

            close (fds_from[1]);
            close (fds_to[0]);
        }
    }

    for (int i = 0; i < N; ++i) {
        // the first element is never used, cause writing to the first child (but he reads from file)
        poll_write_fds[i].fd = channels[i].fd_to;
        poll_write_fds[i].events = POLLOUT;
        poll_write_fds[i].revents = 0;

        poll_read_fds[i].fd = channels[i].fd_from;
        poll_read_fds[i].events = POLLIN;
        poll_read_fds[i].revents = 0;
    }

    for (int i = 0; i < N; ++i) {
        printf ("channels[%d].fd_from = %d\n", i, channels[i].fd_from);
        printf ("channels[%d].fd_to   = %d\n\n", i, channels[i].fd_to);
    }


    int ready_to_read = 0, ready_to_write = 0;
    while (1) {

        ready_to_read = poll (poll_read_fds, N, 0);
        ready_to_write = poll (poll_write_fds + 1, N - 1, 0);

        if (ready_to_write == 0 && ready_to_read == 0)
            break;

        for (int i = 0; i < N; ++i) {

            if (poll_write_fds[i].events == poll_write_fds[i].revents) {
                // i != 0
                //write from buffer to pipe (check if buffer is not empty: empty != size)
                // don't forget to update offset (by write() returned value)
                // if offset == size -> offset = 0

                if (channels[i].full != 0) {

                    int write_ret_val = 0;

                    if (channels[i].offset_for_read - channels[i].buffer <= channels[i].size - channels[i].full) {
                        write_ret_val = write(channels[i].fd_to, channels[i].offset_for_read, channels[i].full);

                    } else {
                        size_t n = (size_t) (channels[i].buffer + channels[i].size - channels[i].offset_for_read);
                        write_ret_val = write (channels[i].fd_to, channels[i].offset_for_read, n);
                    }

                    if (write_ret_val < 0) {
                        perror("Parent: write: ");
                        free_all(channels, N);
                        exit(EXIT_FAILURE);
                    }

                    channels[i].full -= write_ret_val;
                    channels[i].offset_for_read += write_ret_val;
                    channels[i].empty += write_ret_val;

                    assert (channels[i].empty + channels[i].full == channels[i].size);

                    if (channels[i].offset_for_read - channels[i].buffer == channels[i].size) {
                        //we read up to end;
                        channels[i].offset_for_read = channels[i].buffer;
                    }

                    //continue;
                }
            }

            if (poll_read_fds[i].events == poll_read_fds[i].revents) {
                //read from pipe to buffer (check if buffer is available: empty != 0 or offset != 0)
                // don't forget to update empty

                printf ("Parent: read from %d\n", channels[i].fd_from);

                if (channels[i].empty != 0) {

                    int read_ret_val = 0;

                    if (channels[i].offset_for_write - channels[i].buffer <= channels[i].size - channels[i].empty) {
                        read_ret_val = read (channels[i].fd_from, channels[i].offset_for_write, channels[i].empty);

                    } else {
                        size_t n = (size_t) (channels[i].buffer + channels[i].size - channels[i].offset_for_write);
                        read_ret_val = read (channels[i].fd_from, channels[i].offset_for_write, n);
                    }

                    if (read_ret_val < 0) {
                        perror("Parent: read: ");
                        free_all(channels, N);
                        exit(EXIT_FAILURE);
                    }

                    channels[i].empty -= read_ret_val;
                    channels[i].offset_for_write += read_ret_val;
                    channels[i].full += read_ret_val;

                    assert (channels[i].empty + channels[i].full == channels[i].size);

                    if (channels[i].offset_for_write - channels[i].buffer == channels[i].size) {
                        //we wrote up to end;
                        channels[i].offset_for_write = channels[i].buffer;
                    }

                    break;
                }
            }


        }
    }

    return 0;
}
