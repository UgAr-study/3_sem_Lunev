#include "header.h"

void free_all(struct Channel *channels, size_t size) {
    for (int i = 0; i < size; ++i) {
        char *buff = NULL;
        if ((buff = channels[i].buffer)) {
            free (buff);
        }
    }
}

void GetFromBuffer (struct Channel *channels, int i) {

    int write_ret_val = 0;

    if (channels[i].offset_for_read - channels[i].buffer <= channels[i].size - channels[i].full) {
        write_ret_val = write(channels[i].fd_to, channels[i].offset_for_read, channels[i].full);
    } else {
        size_t n = (size_t) (channels[i].buffer + channels[i].size - channels[i].offset_for_read);
        write_ret_val = write(channels[i].fd_to, channels[i].offset_for_read, n);
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
}

void PutInBuffer (struct Channel *channels, int i) {
    int read_ret_val = 0;

    if (channels[i].offset_for_write - channels[i].buffer <= channels[i].size - channels[i].empty) {
        read_ret_val = read(channels[i].fd_from, channels[i].offset_for_write, channels[i].empty);

    } else {
        size_t n = (size_t) (channels[i].buffer + channels[i].size - channels[i].offset_for_write);
        read_ret_val = read(channels[i].fd_from, channels[i].offset_for_write, n);
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
}

int SetParentDeath (pid_t ppid_bef_fork) {

    if (prctl(PR_SET_PDEATHSIG, SIGKILL) < 0) {
        printf ("Can't do prctl\n");
        return -1;
    }

    if (getppid() != ppid_bef_fork) {
        printf ("My parent has been changed\n");
        printf ("My new parent pid is [%d]\nI'm finish", getppid());
        return -1;
    }

    return 0;
}
