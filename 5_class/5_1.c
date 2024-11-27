#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h> // Для exit()

int main(int argc, char *argv[], char *envp[])
{
    int semid;
    char pathname[] = "5_1.c";
    key_t key;
    struct sembuf mybuf;
    
    // Генерация ключа
    key = ftok(pathname, 0);
    if (key == -1) {
        perror("ftok() error");
        exit(-1);
    }

    // Создание семафора
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
        perror("semget() error");
        exit(-1);
    }

    // Инициализация семафора значением 1
    if (semctl(semid, 0, SETVAL, 1) < 0) {
        perror("semctl() error");
        exit(-1);
    }

    // Подготовка операции на семафоре
    mybuf.sem_num = 0;
    mybuf.sem_op  = -1;  // Уменьшаем значение семафора
    mybuf.sem_flg = 0;

    // Ожидание семафора
    if (semop(semid, &mybuf, 1) < 0) {
        perror("semop() error");
        exit(-1);
    }

    printf("The condition is present\n");

    return 0;
}