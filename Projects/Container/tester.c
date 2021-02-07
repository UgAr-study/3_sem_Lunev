//
// Created by artem on 08.02.2021.
//
#include <fcntl.h>
#include "RBTree.h"

int* testFromFile (char *file) {

    int fd = open (file, O_RDONLY);

    if (fd <= 0) {
        fprintf (stderr, "Can't open the file\n");
        return NULL;
    }
}
