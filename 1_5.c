#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>  // Для системных вызовов read(), close()
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int fd;
    size_t size;
    char string[60];

    // Проверяем, был ли передан аргумент с именем файла
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    /* Попытаемся открыть файл с именем, переданным первым параметром вызова, только для чтения */
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        /* Если файл открыть не удалось, печатаем сообщение и прекращаем работу */
        printf("Can't open file: %s\n", argv[1]);
        exit(-1);
    }

    /* Читаем файл, пока не кончится, и печатаем прочитанное */
    size = read(fd, string, sizeof(string) - 1); // Читаем до 59 байт
    if (size < 0) {
        printf("Error reading file\n");
        close(fd);
        exit(-1);
    }

    string[size] = '\0';  // Добавляем нуль-терминатор для корректного вывода строки
    printf("Contents of the file:\n%s\n", string);

    /* Закрываем файл */
    if (close(fd) < 0) {
        printf("Can't close file\n");
        exit(-1);
    }

    return 0;
}