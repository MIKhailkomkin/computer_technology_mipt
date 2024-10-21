#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

/* Программа запускает дочерний процесс, который анализирует число (год),
 * введённое как параметр, и возвращает различные сигналы,
 * в зависимости от его високосности.*/
/* 0 - no signal, 1 - SIGUSR1, 2 - SIGUSR2 */
sig_atomic_t sig_status = 0;

void handle_usr1(int s_num) {
    sig_status = 1;
}

void handle_usr2(int s_num) {
    sig_status = 2;
}

int main(int argc, char **argv) {
    struct sigaction act_usr1, act_usr2;

    // Устанавливаем обработчики сигналов
    sigemptyset(&act_usr1.sa_mask);
    sigemptyset(&act_usr2.sa_mask);
    act_usr1.sa_flags = 0;
    act_usr2.sa_flags = 0;
    act_usr1.sa_handler = handle_usr1;
    act_usr2.sa_handler = handle_usr2;

    // Регистрируем обработчики
    if (sigaction(SIGUSR1, &act_usr1, NULL) == -1) {
        fprintf(stderr, "sigaction (act_usr1) error\n");
        return 1;
    }

    if (sigaction(SIGUSR2, &act_usr2, NULL) == -1) {
        fprintf(stderr, "sigaction (act_usr2) error\n");
        return 1;
    }

    // Проверяем количество аргументов
    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        return 1;
    }

    // Создаём дочерний процесс
    if (!fork()) {
        // Дочерний процесс: запускаем новый процесс
        int year = atoi(argv[1]); // Преобразуем аргумент в целое число

        // Проверяем, является ли год високосным
        if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
            kill(getppid(), SIGUSR1); // Отправляем SIGUSR1
        } else {
            kill(getppid(), SIGUSR2); // Отправляем SIGUSR2
        }
        return 0; // Завершаем дочерний процесс
    }

    // Родительский процесс: ждём сигналы
    while (1) {
        // Блокируем процесс до получения сигнала
        pause();

        // Проверяем статус сигнала
        if (sig_status == 1) {
            printf("%s: leap year\n", argv[1]);
            break; // Выходим из цикла
        }

        if (sig_status == 2) {
            printf("%s: not leap year\n", argv[1]);
            break; // Выходим из цикла
        }
    }

    // Ожидаем завершения дочернего процесса
    wait(NULL);

    return 0;
}
