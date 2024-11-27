#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHMEM_SIZE  4096          // Размер сегмента разделяемой памяти
#define SH_MESSAGE  "Hello World!\n"  // Сообщение для записи

#define SEM_KEY    2008      // Ключ для семафора
#define SHM_KEY    2008           // Ключ для разделяемой памяти

union semnum {
    int val;                      // Используется для установки значения семафора
    struct semid_ds *buf;         // Структура для управления семафорами
    unsigned short *array;        // Массив значений для инициализации семафоров
} sem_arg;

int main(void) {
    int shm_id, sem_id;           // Идентификаторы разделяемой памяти и семафора
    char *shm_buf;                // Указатель на сегмент разделяемой памяти
    struct shmid_ds ds;           // Структура для информации о сегменте памяти
    struct sembuf sb[1];          // Операции над семафорами
    unsigned short sem_vals[1];   // Массив для инициализации семафоров

    // Создание сегмента разделяемой памяти
    shm_id = shmget(SHM_KEY, SHMEM_SIZE, IPC_CREAT | IPC_EXCL | 0600);
    if (shm_id == -1) {
        perror("shmget() error");
        return 1;
    }

    // Создание семафора
    sem_id = semget(SEM_KEY, 1, 0600 | IPC_CREAT | IPC_EXCL);
    if (sem_id == -1) {
        perror("semget() error");
        shmctl(shm_id, IPC_RMID, NULL); // Удаляем сегмент памяти при ошибке семафора
        return 1;
    }

    printf("Semaphore ID: %d\n", sem_id);

    // Инициализация семафора значением 1
    sem_vals[0] = 1;
    sem_arg.array = sem_vals;
    if (semctl(sem_id, 0, SETALL, sem_arg) == -1) {
        perror("semctl() error");
        shmctl(shm_id, IPC_RMID, NULL);  // Удаляем память
        semctl(sem_id, 0, IPC_RMID);     // Удаляем семафор
        return 1;
    }

    // Присоединение сегмента разделяемой памяти
    shm_buf = (char *)shmat(shm_id, NULL, 0);
    if (shm_buf == (char *)-1) {
        perror("shmat() error");
        shmctl(shm_id, IPC_RMID, NULL);  // Удаляем память
        semctl(sem_id, 0, IPC_RMID);     // Удаляем семафор
        return 1;
    }

    // Проверка размера сегмента
    shmctl(shm_id, IPC_STAT, &ds);
    if (ds.shm_segsz < strlen(SH_MESSAGE) + 1) {
        fprintf(stderr, "Error: segment size is too small\n");
        shmdt(shm_buf);
        shmctl(shm_id, IPC_RMID, NULL);
        semctl(sem_id, 0, IPC_RMID);
        return 1;
    }

    // Записываем сообщение в разделяемую память
    strcpy(shm_buf, SH_MESSAGE);
    printf("Message written to shared memory: %s\n", shm_buf);

    // Операция на семафоре: захват ресурса
    sb[0].sem_num = 0;
    sb[0].sem_op = -1;  // Уменьшение значения семафора
    sb[0].sem_flg = SEM_UNDO;
    if (semop(sem_id, sb, 1) == -1) {
        perror("semop() error");
        shmdt(shm_buf);
        shmctl(shm_id, IPC_RMID, NULL);
        semctl(sem_id, 0, IPC_RMID);
        return 1;
    }

    // Освобождение ресурсов
    if (shmdt(shm_buf) == -1) {
        perror("shmdt() error");
    }

    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl(IPC_RMID) error");
    }

    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl(IPC_RMID) error");
    }

    return 0;
}