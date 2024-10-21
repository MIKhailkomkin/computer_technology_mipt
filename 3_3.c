#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/* Дочерний процесс, анализирующий год и возвращающий различные сигналы,
 * в зависимости от его високосности.
 */
int main (int argc, char **argv)
{
    int year;

    // Проверка на наличие аргументов
    if (argc < 2) 
    {
        fprintf(stderr, "child: too few arguments\n");
        return 2;
    }

    // Преобразование строки в число
    year = atoi(argv[1]);

    // Проверка, что год больше нуля
    if (year <= 0)
        return 2;

    /* Проверка на високосный год:
     * - Год делится на 4, но не делится на 100
     * - Или год делится на 400
     */
    if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
        kill(getppid(), SIGUSR1);  // Високосный год, отправляем сигнал SIGUSR1
    else
        kill(getppid(), SIGUSR2);  // Невисокосный год, отправляем сигнал SIGUSR2

    return 0;
}