//
// Created by artem on 20.04.2021.
//

#include "common.h"

void p_error (enum error err) {

    switch (err) {

        case SUCCESS:
            fprintf (stderr, "Success\n");
            break;
        case E_INVAL:
            fprintf (stderr, "Invalid arguments were passed to function\n");
            break;
        case E_SOCK:
            fprintf (stderr, "Error while creating, binding or listening the socket\n");
            break;
        case E_CONN:
            fprintf (stderr, "Error while connecting, writing or reading from socket\n");
            break;
        case E_MEM:
            fprintf (stderr, "Lack of memory\n");
            break;
        case E_THREAD:
            fprintf (stderr, "Error while creating or joining the thread\n");
            break;
        default:
            fprintf (stderr, "Unknown error\n");
    }
}

