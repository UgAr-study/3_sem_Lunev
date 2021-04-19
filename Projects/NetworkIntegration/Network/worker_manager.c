#include "worker_manager.h"

void* work_handler (void* arg) {

}


/**
 * defines work for every worker by given number of threads
 * @param n_threads number of threads to compute on
 * @param begin - beginning of the interval
 * @param end - end of the interval
 * @return {@code tasks_for_workers structure} with array of
 * {@code worker_info structures} and its size
 */
struct tasks_for_workers*
divide_work (unsigned n_machines, unsigned n_threads, const double begin, const double end) {

    if (n_machines == 0 || n_threads == 0)
        return NULL;

    const unsigned n_cpus = get_nprocs();
    const double interval_per_thread = (end - begin) / n_threads;

    unsigned tmp = n_threads / n_cpus;
    unsigned n_workers = n_threads % n_cpus == 0 ? tmp : tmp + 1;
    n_workers = n_machines < n_workers ? n_machines : n_workers;

    struct tasks_for_workers*
    res = (struct tasks_for_workers*) malloc (1 * sizeof (struct tasks_for_workers));

    res->size = n_workers;
    res->tasks = (struct worker_info*) malloc (n_workers * sizeof (struct worker_info));

    for (int i = 0; i < n_workers - 1; ++i) {

    }


    return res;
}

/**
 * send one broadcast hello message
 * @return on success, 0 is returned. On error, -1 is returned
 */
int send_hello_message () {

    struct sockaddr_in bc_addr;

    int sk = socket(AF_INET, SOCK_DGRAM, 0);

    if (sk == -1) {
        return -1;
    }

    int true = 1;
    if (setsockopt(sk, SOL_SOCKET, SO_BROADCAST, &true, sizeof true) < 0) {
        close(sk);
        return -1;
    }

    memset(&bc_addr, 0, sizeof bc_addr);

    bc_addr.sin_family = AF_INET;
    bc_addr.sin_port = htons(UDP_PORT);
    bc_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    int msg = TCP_PORT;
    if (sendto(sk, &msg, sizeof msg, 0, (struct sockaddr*)&bc_addr, sizeof bc_addr) < 0) {
        close (sk);
        return -1;
    }

    close (sk);
    return 0;
}

/**
 * wait for hello messages
 * @param error pointer to store exit status (if it is NULL, then it is ignored)
 * @return on success, valid {@code struct sockaddr_in} for further tcp connection is returned
 * and if {@code error} is not NULL, it is set to 0. On error, {@code error} is set to -1, and
 * return value is undefined
 */
struct sockaddr_in get_hello_message (int* error) {

    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof peer_addr;

    memset(&peer_addr, 0, sizeof peer_addr);

    struct sockaddr_in sock_addr;

    memset(&sock_addr, 0, sizeof sock_addr);

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(UDP_PORT);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sk = socket(AF_INET, SOCK_DGRAM, 0);

    if (sk == -1) {

        if (error)
            *error = -1;

        return peer_addr;
    }

    if (bind (sk, (struct sockaddr*)&sock_addr, sizeof sock_addr) == -1) {

        close (sk);

        if (error)
            *error = -1;

        return peer_addr;
    }

    int server_tcp_port = 0;

    int i = 0;
    while (server_tcp_port != TCP_PORT) {
        //FIXME: debug
        printf("Client: [%d] attempt\n", i);

        server_tcp_port = 0;

        recvfrom(sk, &server_tcp_port, sizeof(int),0,
                 (struct sockaddr *)&peer_addr, &peer_addr_len);

        ++i;
    }

    //FIXME: debug
    printf("the host of server is [%s]\n", inet_ntoa(peer_addr.sin_addr));

    struct sockaddr_in res;

    memset(&res, 0, sizeof res);

    res.sin_family = AF_INET;
    res.sin_port = htons(TCP_PORT);
    res.sin_addr = peer_addr.sin_addr;

    if (error)
        *error = 0;

    return res;
}

/**
 * establishing the tcp connection to main computer
 * @param serv_addr address of server
 * @return on success, file descriptor of the connection is returned,
 * otherwise, -1 is returned
 */
int connect_to_server(struct sockaddr_in serv_addr) {

    int sk;
    struct sockaddr_in server_addr = serv_addr;

    if ((sk = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket: ");
        exit(EXIT_FAILURE);
    }

    //FIXME: debug
    printf ("------Connecting to the server---------------\n");

    if (connect(sk, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sk);
        return -1;
    }

    // FIXME: debug
    int n_threads = 0;
    size_t r = read (sk, &n_threads, sizeof n_threads);
    if (r != 0) {
        printf("client: number of threads is %d\n", n_threads);
        return sk;
    } else {
        fprintf(stderr, "broken connection in read\n");
        return -1;
    }
    //FIXME: till here
}


int get_tcp_connections () {

    int serv_sock, new_sock;
    struct sockaddr_in serv_addr, new_addr;
    socklen_t new_addr_len = sizeof new_addr;


    // Creating socket file descriptor
    if ((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        return -1;
    }


    memset(&serv_addr, 0, sizeof serv_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(TCP_PORT);

    if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof serv_addr) < 0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    if (listen(serv_sock, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    while (1) {

        //FIXME: debug
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");

        if ((new_sock = accept(serv_sock, (struct sockaddr *) &serv_addr, &new_addr_len)) < 0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        if (!fork()) {

            //we are in the child, so work with new_sock
            close(serv_sock);

            //FIXME: debug
            int n_threads = 6;
            if (write(new_sock, &n_threads, sizeof n_threads) == -1) {
                perror("write");
                close(new_sock);
                exit(EXIT_FAILURE);
            }

            close(new_sock);
            exit(EXIT_SUCCESS);
        } else {
            close(new_sock);
        }
    }
}

int get_result () {
    return 0;
}