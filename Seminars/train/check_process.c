#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

int main (int argc, char* argv[]) {
    if (argc == 1)  {
        printf ("no file path\n");
        //return 0;
    }

    pid_t pid = getpid();
    char spid[10] = {};
    sprintf (spid, "%d", pid);
    printf ("my pid is %d, strpid is %s\n", pid, spid);
    char filepath[20] = "/proc/";
    sprintf (filepath + 6, "%d", pid);
    printf ("dir with current process is %s\n", filepath);

    DIR* dir = opendir (filepath);
    if (dir == NULL && errno == ENOENT || errno == ENOTDIR) {
        printf("dir is NULL\n");
        perror("opendir():");
        return 0;
    }

    printf ("dir ptr is %p\n", dir);
    closedir(dir);
    return 0;
}

