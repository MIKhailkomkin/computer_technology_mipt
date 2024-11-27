#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MESSAGE_LENGTH 256

struct message {
    long mtype;
    char mtext[MESSAGE_LENGTH];
};

int msqid;
long client_id;
char client_name[50];

void* heartbeat(void* arg);
void* receive_messages(void* arg);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <name>\n", argv[0]);
        exit(1);
    }

    strcpy(client_name, argv[1]);
    struct message msg;

    // Создаем очередь сообщений
    key_t key = ftok("arbiter.c", 0);
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        perror("msgget");
        exit(1);
    }

    // Регистрируемся у арбитра
    msg.mtype = 1;
    sprintf(msg.mtext, "#%s", client_name);
    if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) < 0) {
        perror("msgsnd");
        exit(1);
    }

    // Получаем свой ID
    if (msgrcv(msqid, &msg, sizeof(msg.mtext), 0, 0) > 0) {
        sscanf(msg.mtext, "OK %ld", &client_id);
        printf("Registered as %s with ID %ld\n", client_name, client_id);
    } else {
        perror("msgrcv");
        exit(1);
    }

    // Запускаем потоки
    pthread_t heartbeat_thread, receive_thread;
    pthread_create(&heartbeat_thread, NULL, heartbeat, NULL);
    pthread_create(&receive_thread, NULL, receive_messages, NULL);

    // Отправляем сообщения
    while (1) {
        char target[50], text[MESSAGE_LENGTH];
        printf("Enter message (<name>#<message>): ");
        if (fgets(msg.mtext, MESSAGE_LENGTH, stdin) == NULL) {
            break;
        }

        msg.mtext[strcspn(msg.mtext, "\n")] = '\0';
        if (strcmp(msg.mtext, "##") == 0) {
            // Завершение работы
            sprintf(msg.mtext, "##");
            msg.mtype = client_id;
            msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0);
            break;
        }

        msg.mtype = 1;
        msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0);
    }

    pthread_cancel(heartbeat_thread);
    pthread_cancel(receive_thread);
    pthread_join(heartbeat_thread, NULL);
    pthread_join(receive_thread, NULL);

    return 0;
}

void* heartbeat(void* arg) {
    struct message msg;
    msg.mtype = client_id;
    strcpy(msg.mtext, "PING");

    while (1) {
        if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) < 0) {
            perror("msgsnd");
        }
        sleep(10);
    }
}

void* receive_messages(void* arg) {
    struct message msg;

    while (1) {
        if (msgrcv(msqid, &msg, sizeof(msg.mtext), client_id, 0) > 0) {
            printf("Received: %s\n", msg.mtext);
        }
    }
}