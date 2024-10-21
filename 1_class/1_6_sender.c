#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char message[100];

    // Бесконечный цикл для записи сообщений
    while (1) {
        printf("Enter message (type 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);

        // Удаляем символ новой строки из сообщения
        message[strcspn(message, "\n")] = '\0';

        // Если введено 'exit', завершаем работу
        if (strcmp(message, "exit") == 0) {
            break;
        }

        // Открываем файл для записи, если файла нет — создаем его
        fd = open("message.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            printf("Can't open file for writing\n");
            exit(-1);
        }

        // Записываем сообщение в файл
        write(fd, message, strlen(message));
        close(fd);

        printf("Message sent!\n");
    }

    return 0;
}