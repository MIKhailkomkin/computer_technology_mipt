#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SEM_KEY 2007  // Ключ семафора
#define SHM_KEY 2007 // Ключ общей памяти

int main() {
    int shm_id, sem_id;
    int *shared_value; // Указатель на разделяемую память
    struct sembuf sb = {0, -1, 0};  

    
    shm_id = shmget(SHM_KEY, sizeof(int), 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget error");
        exit(1);
    }

    shared_value = (int *)shmat(shm_id, NULL, 0);
    if (shared_value == (int *)-1) {
        perror("shmat error");
        exit(1);
    }

    sem_id = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
    if (sem_id == -1) {
        perror("semget error");
        exit(1);
    }

    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("semctl error");
        exit(1);
    }

    // start number
    printf("start number: ");
    scanf("%d", shared_value);

    printf("run 2 prog\n");

    // free
    shmdt(shared_value);

    return 0;
}