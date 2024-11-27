#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX_CLIENTS 100
#define TIMEOUT 15
#define MESSAGE_LENGTH 256

struct message {
    long mtype;                 // Тип сообщения (идентификатор клиента или команда)
    char mtext[MESSAGE_LENGTH]; // Текст сообщения
};

struct client {
    long id;               // Идентификатор клиента
    char name[50];         // Имя клиента
    time_t last_ping;      // Последнее время отправки "PING"
};

struct client clients[MAX_CLIENTS];
int client_count = 0;

void handle_message(int msqid, struct message* msg);
void remove_inactive_clients();
int find_client_by_name(const char* name);
int find_client_by_id(long id);

int main() {
    int msqid;
    key_t key;
    struct message msg;

    // Создаем очередь сообщений
    key = ftok("arbiter.c", 0);
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        perror("msgget");
        exit(1);
    }

    printf("Arbiter started, waiting for clients...\n");

    while (1) {
        // Получаем сообщение из очереди
        if (msgrcv(msqid, &msg, sizeof(msg.mtext), 0, IPC_NOWAIT) > 0) {
            handle_message(msqid, &msg);
        }
        remove_inactive_clients(); // Удаляем неактивных клиентов
        sleep(1);
    }

    return 0;
}

void handle_message(int msqid, struct message* msg) {
    if (msg->mtext[0] == '#') {
        if (msg->mtext[1] == '#') {
            // Клиент завершает работу
            int idx = find_client_by_id(msg->mtype);
            if (idx != -1) {
                printf("Client %s disconnected\n", clients[idx].name);
                for (int i = idx; i < client_count - 1; i++) {
                    clients[i] = clients[i + 1];
                }
                client_count--;
            }
        } else {
            // Регистрация нового клиента
            char name[50];
            sscanf(msg->mtext, "#%s", name);
            if (find_client_by_name(name) == -1) {
                clients[client_count].id = msg->mtype;
                strcpy(clients[client_count].name, name);
                clients[client_count].last_ping = time(NULL);
                client_count++;
                printf("Client %s registered with ID %ld\n", name, msg->mtype);

                sprintf(msg->mtext, "OK %ld", msg->mtype);
                msgsnd(msqid, msg, strlen(msg->mtext) + 1, 0);
            } else {
                printf("Client %s is already registered\n", name);
            }
        }
    } else if (strcmp(msg->mtext, "PING") == 0) {
        // Обновить время активности клиента
        int idx = find_client_by_id(msg->mtype);
        if (idx != -1) {
            clients[idx].last_ping = time(NULL);
        }
    } else {
        // Сообщение для другого клиента
        char target_name[50];
        char* delim = strchr(msg->mtext, '#');
        if (delim) {
            *delim = '\0';
            strcpy(target_name, msg->mtext);
            char* message_body = delim + 1;

            int idx = find_client_by_name(target_name);
            if (idx != -1) {
                msg->mtype = clients[idx].id;
                strcpy(msg->mtext, message_body);
                msgsnd(msqid, msg, strlen(msg->mtext) + 1, 0);
                printf("Message sent from %ld to %s\n", msg->mtype, target_name);
            } else {
                printf("Client %s not found\n", target_name);
            }
        }
    }
}

void remove_inactive_clients() {
    time_t now = time(NULL);
    for (int i = 0; i < client_count; i++) {
        if (difftime(now, clients[i].last_ping) > TIMEOUT) {
            printf("Client %s timed out\n", clients[i].name);
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            i--; // Чтобы не пропустить следующего клиента
        }
    }
}
int find_client_by_name(const char* name) {
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_client_by_id(long id) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].id == id) {
            return i;
        }
    }
    return -1;
}