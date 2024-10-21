#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
    int i;

    // Вывод параметров среды
    for(i = 0; envp[i] != NULL; i++) 
        printf("envp[%d]: %s\n", i, envp[i]);

    // Вывод аргументов командной строки
    for(i = 0; i < argc; i++) 
        printf("argv[%d]: %s\n", i, argv[i]);

    return 0;
}