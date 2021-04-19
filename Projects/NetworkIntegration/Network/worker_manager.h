#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


#define UDP_PORT 9090
#define TCP_PORT 8011

struct worker_info {
    double begin;
    double end;
    unsigned n_threads;
};

struct tasks_for_workers {
    struct worker_info* tasks;
    unsigned size;
};

void* work_handler(void*);

struct tasks_for_workers*
divide_work (unsigned n_machines, unsigned n_threads, const double begin, const double end);

int send_hello_message();

struct sockaddr_in
get_hello_message(int* error);

int connect_to_server(struct sockaddr_in serv_addr);

int get_tcp_connections();

int get_result();
