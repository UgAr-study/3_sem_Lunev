//
// Created by artem on 17.03.2021.
//

#include <math.h>
#include "cpu_info.h"

static int find_in_array (const int* arr, int size, int elem) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == elem)
            return i;
    }
    return -1;
}

static int add_to_cpuinfo_array (struct cpu** arr, size_t* size, struct cpu elem, size_t position) {

    if (position >= *size) {
        *arr = (struct cpu*) realloc (*arr, (2 * position + 1) * sizeof (struct cpu));
        *size = 2 * position;
    }

    if (*arr == NULL)
        return -1;

    (*arr)[position] = elem;
    return 0;
}

static int add_to_cpu_array (int** arr, size_t* size, int elem, size_t position) {

    if (position >= *size) {
        *arr = (int*) realloc (*arr, (2 * position + 1) * sizeof (int));
        *size = 2 * position;
    }

    if (*arr == NULL)
        return -1;

    (*arr)[position] = elem;
    return 0;
}

struct cpu_info get_mycpu_info () {
    FILE *cmd = popen("grep \"core id\" /proc/cpuinfo", "r");
    char buff[14] = {0};
    int core_ids[256];
    int count = 0;
    short num_read_bytes = 13;

    while(fread(buff, sizeof(char), num_read_bytes, cmd) > 0) {
        core_ids[count++] = (int) strtol (buff + 10, NULL, 10);

        if (core_ids[count - 1] == 9)
            num_read_bytes++;

    }

    struct cpu_info res = {
            .cpus = (struct cpu*) calloc(count, sizeof(struct cpu)),
            .n_cpu = count
    };

    if (res.cpus == NULL)
        exit(EXIT_FAILURE);


    size_t n_cpu = 0;
    for (int i = 0; i < count; ++i) {

        if (find_in_array(core_ids, i, core_ids[i]) == -1) {
            // if we haven't met this core_id yet
            struct cpu cpu = {};
            add_to_cpuinfo_array(&res.cpus, &res.n_cpu, cpu, n_cpu);
            add_to_cpu_array(&res.cpus[n_cpu].processors,
                             &res.cpus[n_cpu].capacity,
                             i,
                             res.cpus[n_cpu].n_proc++);
            n_cpu++;
        } else {
            // if we have already met such core_id
            add_to_cpu_array(&res.cpus[core_ids[i]].processors,
                             &res.cpus[core_ids[i]].capacity,
                             i,
                             res.cpus[core_ids[i]].n_proc++);
        }
    }

    res.n_cpu = n_cpu;

    return res;
}

