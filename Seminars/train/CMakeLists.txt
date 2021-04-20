cmake_minimum_required(VERSION 3.17)
project(train C)

set(CMAKE_C_STANDARD 99)

add_executable(
        Q5T1
        Q_5_task1.c
)

add_executable(
        main
        main.c
)

add_executable(in input.c select_test.c main.c semaphore_test.c shm_test.c check_process.c signal_test.c Q_5_task1.c ch_strtok.c segfault.c sigcont.c test.c)
add_executable(out output.c fork_test.c)