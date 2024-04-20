#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <random>
#include <iostream>

int semid; // IPC дескриптор для массива IPC семафоров

/* Имя файла, использующееся для генерации ключа.
 * Файл с таким именем должен существовать в текущей директории
 */
char pathname[] = ".";
key_t key;           // IPC ключ
struct sembuf mybuf; // Структура для задания операции над семафором

bool is_number(const std::string &s)
{
    return !s.empty() && (s.find_first_not_of("0123456789") == s.npos);
}

void Customer()
{
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, 6);
    pid_t my_number = getpid();
    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get semid\n");
        exit(-1);
    }

    int k = distrib(gen);

    for (size_t i = 0; i < k; i++)
    {
        mybuf.sem_op = -1;
        mybuf.sem_flg = 0;
        std::uniform_int_distribution<> distrib(1, 2);
        if (distrib(gen) % 2 == 0)
        {
            printf("\n%d: Товар находится в первом отделе.", my_number);
            printf("\n%d: Жду своей очереди", my_number);
            mybuf.sem_num = 0;
            if (semop(semid, &mybuf, 1) < 0)
            {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }
            printf("\n%d: Настала моя очередь. Приобретаю товар", my_number);
            usleep(2000000);
            printf("\n%d: Товар из первого отдела получен", my_number);
            mybuf.sem_op = 1;
            if (semop(semid, &mybuf, 1) < 0)
            {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }
        }
        else
        {
            printf("\n%d: Товар находится во втором отделе. Иду туда.", my_number);
            printf("\n%d: Жду своей очереди", my_number);
            mybuf.sem_num = 1;
            if (semop(semid, &mybuf, 1) < 0)
            {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }
            printf("\n%d: Настала моя очередь. Приобретаю товар", my_number);
            usleep(2000000);
            printf("\n%d: Товар из второго отдела получен", my_number);
            mybuf.sem_op = 1;
            if (semop(semid, &mybuf, 1) < 0)
            {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }
        }
        printf("\n");
    }

    printf("\n%d: Все товары куплены", my_number);

    printf("\n%d: Поход в магазин успешно завершён", my_number);
    exit(0);
}

int Nproc;
pid_t t[100];

void makeproc(int p)
{
    Nproc = p;
    for (int i = 0; i < p; i++)
    {
        t[i] = fork();
        if (t[i] == 0)
        {
            Customer();
            makeproc(p - 1);
        }
    }
    return;
}

int i = 0;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Неверное количество аргументов!" << std::endl;
        return 0;
    }
    if (!is_number(argv[1]) || std::stoi(argv[1]) <= 0)
    {
        std::cout << "Второй аргумент должен быть положительным числом!";
        return 0;
    }
    // переводим аргумент из командной строки в число
    int number = std::stoi(argv[1]);
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        printf("Что-то случилось");
    }
    else if (pid == 0)
    {
        makeproc(5);
    }
    while (true)
    {
    }
    return 0;
}