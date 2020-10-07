#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//int var = 0;

main()
{
    pid_t pid;
    for (int i = 0; i < 3; ++i) {
        int var = 0;
        int* pvar = &var;
        switch (pid = fork()) {
            case -1:
                perror("fork"); /* произошла ошибка */
                exit(1); /*выход из родительского процесса*/
            case 0:
//                printf(" CHILD: Это процесс-потомок!\n");
//                printf(" CHILD: Мой PID -- %d\n", getpid());
//                printf(" CHILD: PID моего родителя -- %d\n",
//                       getppid());
                //printf(" CHILD: Введите мой код возврата(как можно меньше):");
                (*pvar)++;
                printf(" CHILD: num [%p] %d --- pvar [%p] %d\n", &var, var, pvar, *pvar);
                exit(2);
            default:
//                printf("PARENT: Это процесс-родитель!\n");
//                printf("PARENT: Мой PID -- %d\n", getpid());
//                printf("PARENT: PID моего потомка %d\n", pid);
                //printf("PARENT: Я жду, пока потомок не вызовет exit()...\n");
                (*pvar)++;
                printf("PARENT: num [%p] %d --- pvar [%p] %d\n", &var, var, pvar, *pvar);
                wait(NULL);
        }
    }
}
