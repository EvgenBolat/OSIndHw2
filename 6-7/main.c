#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

const char *shar_object = "/posix-shar-object";
int shmid; // дескриптор разделяемой памяти

void Customer()
{

    pid_t my_number = getpid();
    if ((shmid = shm_open(shar_object, O_RDWR, 0666)) == -1)
    {
        printf("Opening error\n");
        perror("shm_open");
        return;
    }
    sem_t *p_sem1 = mmap(0, sizeof(sem_t), PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);
    sem_t *p_sem2 = mmap(0, sizeof(sem_t), PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);

    int k = rand() % 5 + 1;

    for (size_t i = 0; i < k; i++)
    {
        if (rand() % 2 == 0)
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

    // if (sem_close(p_sem1) == -1)
    // {
    //     printf("sem_close: Incorrect close of posix semaphore");
    //     exit(-1);
    // };

    // if (sem_close(p_sem2) == -1)
    // {
    //     printf("sem_close: Incorrect close of posix semaphore");
    //     exit(-1);
    // };

    printf("\n%d: Поход в магазин успешно завершён", my_number);
    exit(0);
}

void SalesMans()
{
    // Создание объекта памяти
    if ((shmid = shm_open(shar_object, O_CREAT | O_RDWR, 0666)) == -1)
    {
        perror("shm_open: object is already open");
        exit(-1);
    }
    // Задание размера объекта памяти
    if (ftruncate(shmid, 2 * sizeof(sem_t)) == -1)
    {
        perror("ftruncate: memory sizing error");
        exit(-1);
    }
    else
    {
        printf("Memory size set and = %lu\n", 2 * sizeof(sem_t));
    }
    for (size_t i = 0; i < 2; i++)
    {
        sem_t *sem_p = mmap(0, sizeof(sem_t), PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);

        // Инициализация семафора в разделяемой памяти
        if (sem_init(sem_p, 1, 0) == -1)
        {
            perror("sem_init");
            exit(-1);
        }

        // Обнуленный семафор ожидает, когда его поднимут, чтобы вычесть 1
        if (sem_post(sem_p) == -1)
        {
            perror("sem_wait: Incorrect wait of posix semaphore");
            exit(-1);
        };
    }
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
    if (shm_unlink(shar_object) == -1)
    {
        perror("shm_unlink");
        exit(-1);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Неверное количество аргументов!");
        return 0;
    }
    if (!is_number(argv[1]) || atoi(argv[1]) <= 0)
    {
        printf("Второй аргумент должен быть положительным числом!");
        return 0;
    }
    // переводим аргумент из командной строки в число
    int number = atoi(argv[1]);
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
    while (1 == 1)
    {
    }
    return 0;
}