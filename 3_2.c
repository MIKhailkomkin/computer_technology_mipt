#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // для функции getpid()
#include <sys/types.h>

/* Программа посылающая себе сигнал на уничтожение. */
/* Дописать игнорирование данного сигнала. */

int main (void)
{
    // Устанавливаем обработчик сигнала, который будет игнорировать SIGABRT
    signal(SIGABRT, SIG_IGN);

    // Получаем идентификатор текущего процесса
    pid_t dpid = getpid();

    // Отправляем сигнал SIGABRT самому себе
    if (kill(dpid, SIGABRT) == -1) {
        fprintf(stderr, "Cannot send signal\n");
        return 1;
    }

    // Сообщаем, что программа не завершилась после сигнала
    printf("Signal SIGABRT was ignored, program continues...\n");

    return 0;
}