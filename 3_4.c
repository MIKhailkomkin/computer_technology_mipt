#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Для usleep

/* Восстановить нормальное поведение программы через 5 принятых сигналов. */
sig_atomic_t sig_count = 0; // Счётчик принятых сигналов

void sig_handler(int snum) {
    sig_count++; // Увеличиваем счётчик при получении сигнала
}

int main(void) {
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_handler = &sig_handler; // Устанавливаем обработчик сигнала
    act.sa_flags = 0;

    // Регистрируем обработчик для сигнала SIGINT
    if (sigaction(SIGINT, &act, NULL) == -1) {
        fprintf(stderr, "sigaction() error\n");
        return 1;
    }

    // Бесконечный цикл
    while (1) {
        // Проверяем, было ли получено 5 сигналов
        
        if (sig_count >= 5) {
            fprintf(stderr, "Exiting after %d signals.\n", sig_count);
            break; // Завершаем программу
        }

        // Если сигнал был получен, выводим сообщение
        if (sig_count > 0) {
            fprintf(stderr, "Received signal %d times...\n", sig_count);
            // Сбрасываем счётчик, но перед этим сохраняем его в отдельной переменной для вывода
        }

        // Задержка, чтобы избежать излишней загрузки CPU
        usleep(100000); // 100 миллисекунд
    }

    return 0;
}