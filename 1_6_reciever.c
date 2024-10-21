#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int is_number(const char *str) {
    // Проверяем, является ли строка числом
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int main() {
    int fd;
    char message[100];
    size_t size;

    // Бесконечный цикл для чтения сообщений
    while (1) {
        // Открываем файл для чтения
        fd = open("message.txt", O_RDONLY);
        if (fd < 0) {
            printf("Can't open file for reading\n");
            exit(-1);
        }

        // Читаем сообщение из файла
        size = read(fd, message, sizeof(message) - 1);
        close(fd);

        // Если сообщение пустое, продолжаем ожидание
        if (size <= 0) {
            sleep(1); // Ждем перед следующей проверкой
            continue;
        }

        // Добавляем нуль-терминатор для строки
        message[size] = '\0';

        printf("Received message: %s\n", message);

        // Проверяем, является ли сообщение числом
        if (is_number(message)) {
            int number = atoi(message);
            int square = number * number;
            printf("The square of %d is %d\n", number, square);
        }

        // Удаляем сообщение из файла (для новой записи)
        fd = open("message.txt", O_WRONLY | O_TRUNC);
        close(fd);

        // Ждем 1 секунду перед следующим чтением
        sleep(1);
    }

    return 0;
}