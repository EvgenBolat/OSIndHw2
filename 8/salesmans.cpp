#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int semid; // IPC дескриптор для массива IPC семафоров

/* Имя файла,  использующееся для генерации ключа.
 * Файл с таким именем должен существовать в текущей директории
 */
char pathname[] = ".";
key_t key;           // IPC ключ
struct sembuf mybuf; // Структура для задания операции над семафором

void ExitProgram(int sigino)
{
    int err = semctl(semid, 0, IPC_RMID, 0);
    if (err < 0)
    {
        printf("Incorrect semaphour destroy\n");
    }
    exit(0);
}

int main()
{
    signal(SIGINT, ExitProgram);
    // ExitProgram(4);
    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((semid = semget(key, 2, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get semid\n");
        exit(-1);
    }

    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    if (semop(semid, &mybuf, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(-1);
    }
    mybuf.sem_num = 1;
    if (semop(semid, &mybuf, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(-1);
    }

    while (true)
    {
    }
    return 0;
}