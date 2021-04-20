//
// Created by artem on 20.04.2021.
//
#include "common.h"
#include "Network/worker.h"
#include "Network/worker_manager.h"
#include "Integration/calc.h"

double foo (double x) {
    return x * x;
}


int run_worker () {

    int error = SUCCESS;

    struct sockaddr_in serv_addr = get_hello_message (&error);

    if (error != SUCCESS) {
        p_error (error);
        return -1;
    }

    int serv_socket = connect_to_server (serv_addr, &error);

    if (error != SUCCESS) {
        p_error (error);
        return -1;
    }

    error = do_computation (serv_socket);

    if (error != SUCCESS) {
        p_error (error);
        return -1;
    }

    return 0;
}


/**
 * wait for hello messages
 * @param error pointer to store exit status (if it is NULL, then it is ignored)
 * @return on success, valid {@code struct sockaddr_in} for further tcp connection is returned
 * and if {@code error} is not NULL, it is set to error.SUCCESS.
 * On error, if {@code error} is not NULL, it is set to an appropriate value,
 * and return value is undefined
 */
struct sockaddr_in get_hello_message (int *const error) {

    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof peer_addr;

    memset (&peer_addr, 0, sizeof peer_addr);

    struct sockaddr_in sock_addr;

    memset (&sock_addr, 0, sizeof sock_addr);

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons (UDP_PORT);
    sock_addr.sin_addr.s_addr = htonl (INADDR_ANY);

    int sk = socket (AF_INET, SOCK_DGRAM, 0);

    if (sk == -1) {

        if (error)
            *error = E_SOCK;

        return peer_addr;
    }

    if (bind (sk, (struct sockaddr *) &sock_addr, sizeof sock_addr) == -1) {

        close (sk);

        if (error)
            *error = E_SOCK;

        return peer_addr;
    }

    int server_tcp_port = 0;

    int i = 0;
    while (server_tcp_port != TCP_PORT) {
        //FIXME: debug
        printf ("Client: [%d] attempt\n", i);

        server_tcp_port = 0;

        recvfrom (sk, &server_tcp_port, sizeof (int), 0,
                  (struct sockaddr *) &peer_addr, &peer_addr_len);

        ++i;
    }

    //FIXME: debug
    printf ("the host of server is [%s]\n", inet_ntoa (peer_addr.sin_addr));

    struct sockaddr_in res;

    memset (&res, 0, sizeof res);

    res.sin_family = AF_INET;
    res.sin_port = htons (TCP_PORT);
    res.sin_addr = peer_addr.sin_addr;

    if (error)
        *error = SUCCESS;

    return res;
}



/**
 * establishing the tcp connection to main computer
 * @param serv_addr - address of server
 * @param error - pointer to variable, where error status will be put
 * @return on success, valid file descriptor of the connection is returned
 * and if {@code error} is not NULL, {@code SUCCESS} status will be set.
 * On error, -1 is returned and if {@code error} is not NULL,
 * an appropriate {@code error} value will be set in it.
 */
int connect_to_server (struct sockaddr_in serv_addr, int *const error) {

    int sk;
    struct sockaddr_in server_addr = serv_addr;

    int err = SUCCESS;

    if ((sk = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        //FIXME: debug
        perror ("socket: ");
        err = E_SOCK;
        goto exit;
    }

    //FIXME: debug
    printf ("------Connecting to the server---------------\n");

    if (connect (sk, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
        //FIXME: debug
        perror ("connect");
        close (sk);
        err = E_CONN;
    }

exit:

    if (error)
        *error = err;

    if (err != SUCCESS)
        return -1;

    return sk;

    /*
    // FIXME: debug
    int n_threads = 0;
    size_t r = read (sk, &n_threads, sizeof n_threads);
    if (r != 0) {
        printf ("client: number of threads is %d\n", n_threads);
        return sk;
    } else {
        fprintf (stderr, "broken connection in read\n");
        return -1;
    }
    //FIXME: till here*/
}

int do_computation (int socket) {

    if (socket < 0)
        return E_INVAL;

    struct worker_info init_info;

    if (read (socket, &init_info, sizeof init_info) != sizeof init_info) {
        //FIXME: debug
        perror ("partial read from socket");
        return E_CONN;
    }

    double res = 0;
    int check = Integrate (init_info.n_threads, init_info.begin, init_info.end, foo, &res);

    if (check != SUCCESS) {
        return check;
    }

    if (write (socket, &res, sizeof res) != sizeof res) {
        return E_CONN;
    }

    return SUCCESS;
}
