#include <stdio.h>     
#include <string.h>    
#include <sys/shm.h>   

// Размер разделяемой памяти (в байтах)
#define SHMEM_SIZE 4096

// Сообщение, которое будет записано в разделяемую память
#define SH_MESSAGE "Poglad Kota!\n"

int main (void)
{
    int shm_id;         // Идентификатор сегмента разделяемой памяти
    char *shm_buf;      // Указатель на разделяемую память
    int shm_size;       // Переменная для хранения размера сегмента разделяемой памяти
    struct shmid_ds ds; // Структура для хранения информации о сегменте памяти

    // Создание сегмента разделяемой памяти с указанным размером и правами доступа
    shm_id = shmget(IPC_PRIVATE, SHMEM_SIZE, IPC_CREAT | IPC_EXCL | 0600);
    
    // Проверка на ошибку при создании сегмента
    if (shm_id == -1) 
    {
        fprintf(stderr, "shmget() error\n");
        return 1;
    }

    // Присоединение сегмента разделяемой памяти к адресному пространству процесса
    shm_buf = (char *) shmat(shm_id, NULL, 0);
    
    // Проверка на ошибку при присоединении
    if (shm_buf == (char *) -1)
    {
        fprintf(stderr, "shmat() error\n");
        return 1;
    }

    // Получение информации о сегменте разделяемой памяти
    shmctl(shm_id, IPC_STAT, &ds);

    // Сохранение фактического размера сегмента
    shm_size = ds.shm_segsz;
    
    // Проверка, что размер сегмента достаточно велик для записи сообщения
    if (shm_size < strlen(SH_MESSAGE)) 
    {
        fprintf(stderr, "error: segsize=%d\n", shm_size);
        return 1;
    }

    // Копирование сообщения в разделяемую память
    strcpy(shm_buf, SH_MESSAGE);

    // Вывод идентификатора сегмента разделяемой памяти
    printf("ID: %d\n", shm_id);

    // Ожидание нажатия клавиши Enter для завершения программы
    printf("Press <Enter> to exit...");
    fgetc(stdin);

    // Отключение сегмента разделяемой памяти от процесса
    shmdt(shm_buf);

    // Удаление сегмента разделяемой памяти
    //shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
