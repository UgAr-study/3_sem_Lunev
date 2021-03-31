//
// Created by artem on 17.03.2021.
//
#pragma once

#include <stdio.h>
#include <stdlib.h>

struct cpu_info {
    struct cpu* cpus;
    size_t n_cpu;
};

struct cpu {
    int* processors;
    size_t n_proc;
    size_t capacity;
};

struct cpu_info get_mycpu_info ();

int ncpus(void);