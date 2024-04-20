#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <random>
#include <signal.h>

sem_t *p_sem1;
char sem_name1[] = "/saleman1-semaphore"; // имя семафора

sem_t *p_sem2;
char sem_name2[] = "/saleman2-semaphore"; // имя семафора

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
    // Создание семафора
    if ((p_sem1 = sem_open(sem_name1, O_CREAT, 0666, 0)) == 0)
    {
        printf("sem_open: Can not create posix semaphore");
        exit(-1);
    };

    if ((p_sem2 = sem_open(sem_name2, O_CREAT, 0666, 0)) == 0)
    {
        printf("sem_open: Can not create posix semaphore");
        exit(-1);
    }

    int k = distrib(gen);

    for (size_t i = 0; i < k; i++)
    {
        std::uniform_int_distribution<> distrib(1, 2);
        if (distrib(gen) % 2 == 0)
        {
            printf("\n%d: Товар находится в первом отделе.", my_number);
            printf("\n%d: Жду своей очереди", my_number);
            if (sem_wait(p_sem1) == -1)
            {
                printf("sem_wait: Incorrect wait of posix semaphore");
                exit(-1);
            };
            printf("\n%d: Настала моя очередь. Приобретаю товар", my_number);
            usleep(2000000);
            printf("\n%d: Товар из первого отдела получен", my_number);
            if (sem_post(p_sem1) == -1)
            {
                printf("sem_post: Incorrect post of posix semaphore");
                exit(-1);
            };
        }
        else
        {
            printf("\n%d: Товар находится во втором отделе. Иду туда.", my_number);
            printf("\n%d: Жду своей очереди", my_number);
            if (sem_wait(p_sem2) == -1)
            {
                printf("sem_wait: Incorrect wait of posix semaphore");
                exit(-1);
            };
            printf("\n%d: Настала моя очередь. Приобретаю товар", my_number);
            usleep(2000000);
            printf("\n%d: Товар из второго отдела получен", my_number);
            if (sem_post(p_sem2) == -1)
            {
                printf("sem_post: Incorrect post of posix semaphore");
                exit(-1);
            };
        }
        printf("\n");
    }

    printf("\n%d: Все товары куплены", my_number);

    if (sem_close(p_sem1) == -1)
    {
        printf("sem_close: Incorrect close of posix semaphore");
        exit(-1);
    };

    if (sem_close(p_sem2) == -1)
    {
        printf("sem_close: Incorrect close of posix semaphore");
        exit(-1);
    };

    printf("\n%d: Поход в магазин успешно завершён", my_number);
    exit(0);
}

void SalesMans()
{
    // Создание семафора
    if ((p_sem1 = sem_open(sem_name1, O_CREAT, 0666, 0)) == 0)
    {
        printf("sem_open: Can not create posix semaphore");
        exit(-1);
    };

    if (sem_post(p_sem1) == -1)
    {
        printf("sem_post: Incorrect post of posix semaphore");
        exit(-1);
    };

    // Создание семафора
    if ((p_sem2 = sem_open(sem_name2, O_CREAT, 0666, 0)) == 0)
    {
        printf("sem_open: Can not create posix semaphore");
        exit(-1);
    };
    if (sem_post(p_sem2) == -1)
    {
        printf("sem_post: Incorrect post of posix semaphore");
        exit(-1);
    };
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
}

void ExitProgram(int sigino)
{
    if (sem_close(p_sem1) == -1)
    {
        printf("1");
        printf("sem_close: Incorrect close of posix semaphore");
        exit(-1);
    };
    if (sem_unlink(sem_name1) == -1)
    {
        printf("sem_unlink: Incorrect unlink of posix semaphore");
        exit(-1);
    };

    if (sem_close(p_sem2) == -1)
    {
        printf("2");
        printf("sem_close: Incorrect close of posix semaphore");
        exit(-1);
    };
    if (sem_unlink(sem_name2) == -1)
    {
        printf("sem_unlink: Incorrect unlink of posix semaphore");
        exit(-1);
    };
    exit(0);
}

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
    pid_t pid, chpid;
    pid = fork();
    if (pid == -1)
    {
        printf("Что-то случилось");
    }
    else if (pid != 0)
    {
        signal(SIGINT, ExitProgram);
        SalesMans();
    }
    else
    {
        makeproc(number);
    }
    while (true)
    {
    }
    return 0;
}