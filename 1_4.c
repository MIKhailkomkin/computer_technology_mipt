#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[])
{
    pid_t pid, ppid, chpid;
    int a = 0, result;

    chpid = fork();  // Порождаем новый процесс

    if (chpid == -1)
    { 
        // Ошибка при вызове fork()
        printf("Can't fork a child process\n");
        return -1;
    } 
    else if (chpid > 0)
    {
        // Родительский процесс
        a = a + 1;

        pid = getpid();   // Получаем PID текущего процесса (родителя)
        ppid = getppid(); // Получаем PID родительского процесса

        printf("Parent Process: pid = %d, ppid = %d, result = %d\n", (int)pid, (int)ppid, a);
    } 
    else 
    {
        // Дочерний процесс
        printf("Child Process is running 'cat' command\n");

        // Запускаем команду "cat имя_файла" через execle
        if (argc > 1) {
            result = execle("/bin/cat", "cat", argv[1], NULL, envp);
        } else {
            printf("No file argument provided for 'cat' command\n");
            exit(-1);
        }

        // Если execle вернула значение, значит произошла ошибка
        if (result < 0)
        {
            printf("Error on program start\n");
            exit(-1);
        }
    }

    return 0;
}