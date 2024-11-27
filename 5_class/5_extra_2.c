#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SEM_KEY 2007  // Ключ семафора
#define SHM_KEY 2007  // Ключ общей памяти

int main() {
    int shm_id, sem_id;
    int *shared_value; // Указатель на разделяемую память
    struct sembuf sb;  // Операция семафора

    // Подключение к сегменту общей памяти
    shm_id = shmget(SHM_KEY, sizeof(int), 0666);
    if (shm_id == -1) {
        perror("shmget error");
        exit(1);
    }

    shared_value = (int *)shmat(shm_id, NULL, 0);
    if (shared_value == (int *)-1) {
        perror("shmat error");
        exit(1);
    }

    // Подключение к семафору
    sem_id = semget(SEM_KEY, 1, 0666);
    if (sem_id == -1) {
        perror("semget error");
        exit(1);
    }

    while (1) {
        // semafor
        sb.sem_num = 0;
        sb.sem_op = -1;  
        sb.sem_flg = 0;

        if (semop(sem_id, &sb, 1) == -1) {
            perror("semop error");
            exit(1);
        }

        // память
        if (*shared_value <= 0) {
            printf("0, exit\n");
            (*shared_value)--;
            break;
        }
        printf("Process %d: %d\n", getpid(), *shared_value);
        (*shared_value)--;

        sleep(1);  

        // free
        sb.sem_op = 1;  
        if (semop(sem_id, &sb, 1) == -1) {
            perror("semop error");
            exit(1);
        }
    }

    
    shmdt(shared_value);
    printf("end of task");

    return 0;
}