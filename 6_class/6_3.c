#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MESSAGE_LENGTH 255

struct mymsgbuf {
    long mtype;               // Тип сообщения (используется как номер получателя)
    char mtext[MESSAGE_LENGTH]; // Текст сообщения
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <receive_number> <send_number>\n", argv[0]);
        exit(-1);
    }

    // Получение номеров из аргументов командной строки
    long recv_number = atol(argv[1]); // Номер для получения сообщений
    long send_number = atol(argv[2]); // Номер для отправки сообщений

    int msqid;
    char pathname[] = "2_Q.c"; // Имя файла для ftok
    key_t key;
    struct mymsgbuf mybuf;

    // Создаем ключ для очереди
    key = ftok(pathname, 0);
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        perror("msgget");
        exit(-1);
    }

    // Разделение на два процесса
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(-1);
    }

    if (pid == 0) {
        // принимает сообщения 
        while (1) {
            ssize_t len;
            if ((len = msgrcv(msqid, &mybuf, MESSAGE_LENGTH, recv_number, 0)) < 0) {
                perror("msgrcv");
                exit(-1);
            }
            printf("Received message: %s\n", mybuf.mtext);
        }
    } else {
        // отправляет сообщения на send_number
        while (1) {
            printf("Enter message to send (Ctrl+D to exit): ");
            if (fgets(mybuf.mtext, MESSAGE_LENGTH, stdin) == NULL) {
                printf("\nExiting...\n");
                exit(0);
            }

            // Удаляем символ перевода строки
            mybuf.mtext[strcspn(mybuf.mtext, "\n")] = '\0';
            mybuf.mtype = send_number;

            if (msgsnd(msqid, &mybuf, strlen(mybuf.mtext) + 1, 0) < 0) {
                perror("msgsnd");
                exit(-1);
            }
        }
    }

    return 0;
}