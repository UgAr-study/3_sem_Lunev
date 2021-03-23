//
// Created by artem on 17.03.2021.
//

#include <math.h>
#include "cpu_info.h"

static int find_in_array (const int* arr, int size, int elem) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == elem)
            return 1;
    }
    return 0;
}

static int add_to_array (int** arr, size_t* size, int elem, size_t position) {

    if (position >= *size) {
        *arr = (int*) realloc (*arr, (2 * position + 1) * sizeof (int));
        *size = 2 * position;
    }

    if (*arr == NULL)
        return -1;

    (*arr)[position] = elem;
    return 0;
}

int ncpus(void) {
    FILE *cmd = popen("grep '^processor' /proc/cpuinfo | wc -l", "r");

    if (cmd == NULL)
        return -1;

    unsigned nprocs;
    size_t n;
    char buff[8];

    if ((n = fread(buff, 1, sizeof(buff)-1, cmd)) <= 0)
        return -1;

    buff[n] = '\0';
    if (sscanf(buff, "%u", &nprocs) != 1)
        return -1;

    pclose(cmd);

    return nprocs;
}

struct cpu_info get_mycpu_info () {
    FILE *cmd = popen("grep \"core id\" /proc/cpuinfo", "r");
    char buff[14] = {0};
    int core_ids[32];
    int count = 0;
    short num_read_bytes = 13;

    while(fread(buff, sizeof(char), num_read_bytes, cmd) > 0) {
        core_ids[count++] = (int) strtol (buff + 10, NULL, 10);

        if (core_ids[count - 1] == 9)
            num_read_bytes++;

    }

    struct cpu_info res = {};

    size_t real = 0;
    size_t hyper = 0;
    for (int i = 0; i < count; ++i) {
        if (!find_in_array(core_ids, i, core_ids[i])) {
            add_to_array(&res.real_threads, &real, i, res.real_threads_size++);
        } else {
            add_to_array(&res.hyper_threads, &hyper, i, res.hyper_threads_size++);
        }
    }

    return res;
}

