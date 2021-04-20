//
// Created by artem on 20.04.2021.
//

#pragma once

#include <stdio.h>

#define UDP_PORT 9090
#define TCP_PORT 8011

enum error {
    SUCCESS = 0, E_INVAL, E_SOCK, E_CONN, E_MEM, E_THREAD
};

void p_error (enum error err);

