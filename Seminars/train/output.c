#include "../../Projects/First_Pipe/fifo_header.h"

int main() {
    int fd;
    const char* filepath = "/home/artem/ProgProjects/ClionProjects/Lunev/Seminars/train/fifo";
    if (mkfifo(filepath, 0666) < 0 && errno != EEXIST)
        printf ("Ooops! output.c\n");
    fd = open (filepath, O_WRONLY);
    printf ("output fd = %d\n", fd);
    //sleep(5);
    //close (fd);
    /*
    fd_set r, w;
    FD_ZERO(&r);
    FD_ZERO(&w);
    FD_SET(fd, &r);
    FD_SET(fd, &w);

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int sel_ret_val = select(fd + 1,&r, &w, NULL, &tv);
    printf ("%d\n", sel_ret_val);
     */
    return 0;
}
