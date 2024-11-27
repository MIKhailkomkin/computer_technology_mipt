#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

#define SEM_KEY 2008
#define SHM_KEY 2008 

int main(int argc, char **argv) {
    int shm_id, sem_id;         // Идентификаторы общей памяти и семафора
    char *shm_buf;              // Указатель на сегмент общей памяти
    struct sembuf sb[1];        // Структура для управления операциями над семафором

    shm_id = shmget(SHM_KEY, 0, 0600); 
    if (shm_id == -1) {
        perror("shmget() error");
        return 1;
    }

    // Получение идентификатора семафора
    sem_id = semget(SEM_KEY, 1, 0600); // Работа с уже существующим набором семафоров
    if (sem_id == -1) {
        perror("semget() error");
        return 1;
    }

    // Подключение сегмента общей памяти
    shm_buf = (char *)shmat(shm_id, NULL, 0); // Приведение к (char *) для строк
    if (shm_buf == (char *)-1) {
        perror("shmat() error");
        return 1;
    }

    // Печать сообщения из общей памяти
    printf("Message: %s\n", shm_buf);

    // Настройка операции на семафоре (увеличение семафора на 1)
    sb[0].sem_num = 0;  // Номер семафора
    sb[0].sem_op = 1;   // Увеличение значения семафора
    sb[0].sem_flg = SEM_UNDO; // SEM_UNDO автоматически отменяет операцию, если процесс завершится

    if (semop(sem_id, sb, 1) == -1) {
        perror("semop() error");
        shmdt(shm_buf); // Отключить память перед выходом
        return 1;
    }

    // Отключение сегмента общей памяти
    if (shmdt(shm_buf) == -1) {
        perror("shmdt() error");
        return 1;
    }

    return 0;
}