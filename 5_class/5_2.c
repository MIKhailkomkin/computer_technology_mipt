#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

// Основная функция
int main(int argc, char *argv[], char *envp[])
{
    int semid;                   // Идентификатор семафора
    char pathname[] = "5_2.c"; // Путь к существующему файлу для генерации ключа
    key_t key;                   // Ключ IPC
    struct sembuf mybuf;         // Структура для операции над семафором
    
    // Генерация ключа на основе файла и уникального идентификатора
    key = ftok(pathname, 0);
    if (key == -1) {
        perror("ftok() error");
        exit(-1);
    }

    // Создание или получение идентификатора семафора
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
        perror("semget() error");
        exit(-1);
    }

    // Устанавливаем значение семафора на 1 (освобождение ресурса)
    mybuf.sem_num = 0;  // Номер семафора в наборе (если несколько семафоров)
    mybuf.sem_op  = 1;  // Увеличиваем значение семафора на 1
    mybuf.sem_flg = 0;  // Флаг, управляющий поведением операции

    // Выполняем операцию над семафором
    if (semop(semid, &mybuf, 1) < 0) {
        perror("semop() error");
        exit(-1);
    }  
    
    printf("The condition is present\n");
    return 0;
}