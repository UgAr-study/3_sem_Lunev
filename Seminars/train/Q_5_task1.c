#include "../../Projects/First_Pipe/fifo_header.h"

int main () {
    const char * const fifopath = "fifo";

    mkfifo(fifopath, 0666);
    int fd = open (fifopath, O_RDWR);

    write (fd, "aaa", 3);
    printf ("before senond open\n");
    fd = open (fifopath, O_RDONLY);
    printf  ("after second open\n");
    //close(fd);
    char buf[10] = {0};
    write (STDOUT_FILENO, buf, read (fd, buf, 3));
    close (fd);
    return 0;
}