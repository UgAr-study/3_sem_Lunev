#include <stdio.h>
#include "Network/worker_manager.h"
#include "Network/worker.h"
#include "common.h"

const double begin = 0.0;
const double end = 100.0;

#define N_MACHINES 1

void run_server(size_t n_threads);

void *send_hellos (void *arg) {

    int error = SUCCESS;
    while (1) {

        error = send_hello_message();

        if (error != SUCCESS) {
            p_error (error);
            pthread_exit (NULL);
        }
    }
}

void *get_tcps (void *arg) {

    struct tasks_for_workers* tasks = arg;

    int error = get_tcp_connections (tasks);

    if (error != SUCCESS) {
        p_error (error);
        fprintf (stderr, "Computation failed!\n");
    }

    return NULL;
}

int main (int argc, char *argv[]) {

    char *serv_flag = "-s";

    if (argc == 2) {
        fprintf (stderr, "Not all args\n");
        return 0;
    }

    int n_threads = 0;
    int server = 0;

    if (argc == 3) {

        if (!strcmp (argv[1], serv_flag)) {

            server = 1;
            n_threads = (int) strtol (argv[2], NULL, 10);

        } else if (!strcmp (argv[2], serv_flag)) {

            server = 1;
            n_threads = (int) strtol (argv[1], NULL, 10);
        }
    }

    if (!server) {

        if (run_worker () == -1)
            fprintf (stderr, "Computation failed!\n");

        return 0;
    }

    run_server (n_threads);

    //struct tasks_for_workers *tasks = divide_work (4, 23, begin, end, NULL);
    return 0;
}

void run_server(size_t n_threads) {

    int error = SUCCESS;
    struct tasks_for_workers *tasks;

    tasks = divide_work (N_MACHINES, n_threads, begin, end, &error);
    dump_tasks (tasks);

    if (error != SUCCESS) {
        p_error (error);
        fprintf (stderr, "Computation failed!\n");
        return;
    }

    pthread_t th_sender, th_getter;

    if (pthread_create (&th_getter, NULL, get_tcps, tasks) != 0) {
        perror ("pthread_create getter:");
        return;
    }

    if (pthread_create (&th_sender, NULL, send_hellos, NULL) != 0) {
        perror ("pthread_create sender:");
        return;
    }

    if (pthread_join (th_getter, NULL) != 0) {
        perror ("pthread_join getter:");
        goto exit;
    }

    //TODO: why it crashes?
    //pthread_cancel (th_sender);

    double res = 0;
    error = get_result (tasks, &res);

    if (error != SUCCESS) {
        p_error (error);
        fprintf (stderr, "Computation failed!\n");
        goto exit;
    }

    printf ("result is %f\n", res);

exit:
    for (int i = 0; i < tasks->size; ++i) {
        close (tasks->task[i].socket);
    }
}

/*

void print_errors() {

    printf ("EDEADLK = %d\n", EDEADLK);
    printf ("EINVAL = %d\n", EINVAL);
    printf ("ESRCH = %d\n", ESRCH);
}*/
