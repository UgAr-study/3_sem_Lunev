
#include "common.h"
#include "Network/worker_manager.h"

void *work_handler (void *arg) {

    double *res = (double*) malloc (1 * sizeof (double));

    if (res == NULL) {
        goto exit_without_free;
    }

    struct handler_info* h_info = arg;

    if (write (h_info->socket, &h_info->w_info, sizeof h_info->w_info) != sizeof h_info->w_info) {
        //FIXME: debug
        perror ("partial write to socket");
        goto exit_free_res;
    }

    if (read (h_info->socket, res, sizeof (double)) != sizeof (double)) {
        //FIXME: debug
        perror ("read from socket");
        goto exit_free_res;
    }

    goto exit_without_free;

exit_free_res:

    free (res);
    res = NULL;

exit_without_free:

    return res;
}


/**
 * defines work for every worker by given number of threads
 * @param n_threads number of threads to compute on
 * @param begin - beginning of the interval
 * @param end - end of the interval
 * @param error - pointer for value to set if error occurs
 * @return On success, pointer to {@code tasks_for_workers} structure with array of
 * {@code handler_info} structures and its size is returned
 * On error, NULL is returned and if {@code error} is not NULL,
 * it is set to an appropriate value
 */
struct tasks_for_workers *
divide_work (unsigned n_machines, unsigned n_threads,
             double begin, const double end, int *const error) {

    if (n_machines == 0 || n_threads == 0) {

        if (error)
            *error = E_INVAL;

        return NULL;
    }

    const unsigned n_cpus = get_nprocs ();
    const double interval_per_thread = (end - begin) / n_threads;

    unsigned tmp = n_threads / n_cpus;
    unsigned n_workers = n_threads % n_cpus == 0 ? tmp : tmp + 1;
    n_workers = n_machines < n_workers ? n_machines : n_workers;

    struct tasks_for_workers *
            res = (struct tasks_for_workers *) malloc (1 * sizeof (struct tasks_for_workers));

    if (!res)
        goto exit_without_free;

    res->size = n_workers;
    res->task = (struct handler_info *) calloc (n_workers, sizeof (struct handler_info));

    if (!res->task)
        goto exit_free_res;

    for (int i = 0; i < n_workers - 1; ++i) {

        res->task[i].w_info.n_threads = n_threads / n_workers;
        res->task[i].w_info.begin = begin;

        begin += interval_per_thread * (double) res->task[i].w_info.n_threads;

        res->task[i].w_info.end = begin;
    }

    res->task[n_workers - 1].w_info.n_threads = n_threads - (n_workers - 1) * res->task[0].w_info.n_threads;
    res->task[n_workers - 1].w_info.begin = begin;
    res->task[n_workers - 1].w_info.end = end;

    return res;

exit_free_res:

    free (res);

exit_without_free:

    if (error)
        *error = E_INVAL;

    return NULL;
}

/**
 * dump the result of {@code divide_work()} function
 * @param tasks - return value of {@code divide_work()} function
 */
void dump_tasks (struct tasks_for_workers *const tasks) {

    if (tasks == NULL) {
        fprintf (stderr, "tasks is NULL\n");
        return;
    }

    for (int i = 0; i < tasks->size; ++i) {
        printf ("task[%d]:\nn_threads: %zu\nbegin: %f\nend: %f\n",
                i, tasks->task[i].w_info.n_threads,
                tasks->task[i].w_info.begin, tasks->task[i].w_info.end);
    }
}


/**
 * send one broadcast hello message
 * @return on success, {@code error.SUCCESS} is returned.
 * On error, one of the {@code error}'s values is returned
 */
int send_hello_message () {

    struct sockaddr_in bc_addr;

    int sk = socket (AF_INET, SOCK_DGRAM, 0);

    if (sk == -1) {
        return E_SOCK;
    }

    int true = 1;
    if (setsockopt (sk, SOL_SOCKET, SO_BROADCAST, &true, sizeof true) < 0) {
        close (sk);
        return E_SOCK;
    }

    memset (&bc_addr, 0, sizeof bc_addr);

    bc_addr.sin_family = AF_INET;
    bc_addr.sin_port = htons (UDP_PORT);
    bc_addr.sin_addr.s_addr = htonl (INADDR_BROADCAST);

    int msg = TCP_PORT;
    if (sendto (sk, &msg, sizeof msg, 0, (struct sockaddr *) &bc_addr, sizeof bc_addr) < 0) {
        close (sk);
        return E_SOCK;
    }

    close (sk);
    return SUCCESS;
}


/**
 * establishes tcp connections with workers
 * @param tasks - pointer to struct with array of {@code handler_info}s
 * where connected socket file descriptors will be set
 * @return On success, {@code error.SUCCESS} is returned
 * and {@code tasks} is filled with valid file descriptors.
 * On error, all file descriptors in {@code tasks} are not valid
 * and an appropriate {@code error} is returned
 */
int get_tcp_connections (struct tasks_for_workers* tasks) {

    if (tasks == NULL) {
        return E_INVAL;
    }

    int error = SUCCESS;

    int serv_sock, new_sock;
    struct sockaddr_in serv_addr, new_addr;
    socklen_t new_addr_len = sizeof new_addr;

    // Creating server socket file descriptor
    if ((serv_sock = socket (AF_INET, SOCK_STREAM, 0)) == 0) {
        //FIXME: debug
        perror ("In socket");
        error = E_SOCK;
        goto exit_without_closing;
    }

    memset (&serv_addr, 0, sizeof serv_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons (TCP_PORT);


    if (bind (serv_sock, (struct sockaddr *) &serv_addr, sizeof serv_addr) < 0) {
        //FIXME: debug
        perror ("In bind");
        error = E_SOCK;
        goto exit_without_closing;
    }

    if (listen (serv_sock, 10) < 0) {
        //FIXME: debug
        perror ("In listen");
        error = E_SOCK;
        goto exit_without_closing;
    }

    int n_connected_sockets = 0;
    for (; n_connected_sockets < tasks->size; ++n_connected_sockets) {

        //FIXME: debug
        printf ("\n+++++++ Waiting for new connection ++++++++\n\n");

        if ((new_sock = accept (serv_sock, (struct sockaddr *) &serv_addr, &new_addr_len)) < 0) {
            //FIXME: debug
            perror ("In accept");
            error = E_CONN;
            goto exit_close_sockets;
        }

        tasks->task[n_connected_sockets].socket = new_sock;
    }

    close (serv_sock);
    goto exit_without_closing;

exit_close_sockets:

    for (int j = 0; j < n_connected_sockets; ++j)
        close (tasks->task[j].socket);

    close (serv_sock);

exit_without_closing:

    return error;
}

/**
 * does computing on cluster and sets the result to {@code res}.
 * This function doesn't close sockets!
 * @param tasks - pointer to {@code tasks_for_workers} with array of {@code handler_info}'s structures,
 * which are passed into worker_handler function when creating new thread.
 * @param res - pointer to variable, where the result will be put
 * @return On success, {@code error.SUCCESS} is returned and
 * {@code res} contains the result of computation.
 * On error, an appropriate {@code error} value is returned and
 * value of {@code res} is undefined
 */
int get_result (struct tasks_for_workers *const tasks, double *const res) {

    if (!tasks || !res)
        return E_INVAL;

    int error = SUCCESS;

    pthread_t *threads = (pthread_t *) calloc(tasks->size, sizeof(pthread_t));

    if (!threads) {
        error = E_MEM;
        goto exit_without_free;
    }

    int check = 0;

    int i = 0;
    for (; i < tasks->size; ++i) {

        check = pthread_create(&threads[i], NULL, work_handler, (void *) &tasks->task[i]);

        if (check != 0) {
            //FIXME: debug
            perror("pthread_create work_handler:");
            error = E_THREAD;
            goto exit_free_all;
        }
    }

    double result = 0;
    double *part = NULL;

    for (int j = 0; j < tasks->size; ++j) {

        check = pthread_join(threads[j], (void**) &part);

        if (check != 0 || !part) {

            if (check != 0) {
                fprintf (stderr, "check = %d\n", check);
            }

            if (part == NULL) {
                fprintf (stderr, "part is NULL\n");
            }

            perror("pthread_join work_handler");
            error = E_THREAD;
            goto exit_free_all;
        }

        result += *part;
        free (part);
    }

    *res = result;

    goto exit_free_threads;

exit_free_all:

    for (int j = 0; j < i; ++j) {
        pthread_cancel (threads[j]);
    }

exit_free_threads:

    free (threads);

exit_without_free:

    return error;
}