//Konstantinos Zioutos AM3946 csd3946@csd.uoc.gr//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <assert.h>

#define FERRY_MAX_CARS 20
#define CAR_STATE_BOARDED 1
#define CAR_STATE_WAITING 2
#define CAR_STATE_TRANSFERRED 3

struct car
{
    int id;
    int state;
};

pthread_t *cars;
struct car *car_details;
struct car *ferry[FERRY_MAX_CARS];

sem_t producer;
sem_t consumer;
int cars_number;
int cars_remaining;
int cars_entered_ferry_counter;
int ferry_full;
pthread_mutex_t lock_shared_mem;

void *car_enters_ferry(void *arg)
{ /* producer */
    struct car *newcar = (struct car *)arg;
    newcar->state = CAR_STATE_WAITING;
    sem_wait(&consumer);
    printf("Car %d embarking on ferry\n", newcar->id);
    newcar->state = CAR_STATE_BOARDED;
    sleep(1);
    pthread_mutex_lock(&lock_shared_mem);
    cars_entered_ferry_counter++;
    cars_remaining--;
    if (cars_entered_ferry_counter == 20 || cars_remaining == 0)
    {
        sem_post(&producer);
    }
    pthread_mutex_unlock(&lock_shared_mem);
}

void *car_leaves_ferry(void *arg)
{ /* consumer */

    while (1)
    {
        int i = 0;
        sem_wait(&producer);
        printf("All on board\n");
        sleep(2);
        for (i = 0; i < cars_number; i++)
        {
            if ((car_details + i)->state == CAR_STATE_BOARDED)
            {
                printf("Car %d disembarking ferry\n", (car_details + i)->id);
                (car_details + i)->state = CAR_STATE_TRANSFERRED;
            }
        }
        if (cars_remaining != 0)
        {
            printf("The ferry is waiting for cars to board\n");
            printf("Someone woke up the ferry\n");
        }
        pthread_mutex_lock(&lock_shared_mem);
        cars_entered_ferry_counter = 0;
        if (cars_remaining == 0)
        {
            break;
        }
        for (i = 0; i < 20; i++)
        {
            sem_post(&consumer);
        }
        pthread_mutex_unlock(&lock_shared_mem);
    }
    return NULL;
}

void init_ferry()
{
    int i;
    cars_entered_ferry_counter = 0;
    cars_remaining = cars_number;
    for (i = 0; i < FERRY_MAX_CARS; i++)
    {
        ferry[i] = NULL;
    }
}

int main(int argc, char *argv[])
{
    int i;
    pthread_t ferry_operator_thread;
    if (argc != 3)
    {
        printf("Missing arguments. Please specify...\n");
        return -1;
    }
    if (strcmp(argv[1], "-c"))
    {
        printf("Missing argument. Please use -c ...\n");
        return -1;
    }
    if ((cars_number = atoi(argv[2])) == 0)
    {
        printf("Invalid cars number. ...\n");
        return -1;
    }

    if (sem_init(&producer, 0, 0) == -1)
    {
        printf("Semaphore initialization error\n");
    }

    if (sem_init(&consumer, 0, 20) == -1)
    {
        printf("Semaphore initialization error\n");
    }

    if (pthread_mutex_init(&lock_shared_mem, NULL) != 0)
    {
        printf("Mutex initializaition failed\n");
        return 1;
    }

    pthread_create(&ferry_operator_thread, NULL, car_leaves_ferry, NULL);
    cars = (pthread_t *)malloc(sizeof(pthread_t) * cars_number);
    car_details = (struct car *)malloc(sizeof(struct car) * cars_number);

    init_ferry();
    printf("The ferry is waiting for cars to board\n");
    printf("Someone woke up the ferry\n");

    for (i = 0; i < cars_number; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i;
        car_details[i].id = i;
        pthread_create(&cars[i], NULL, car_enters_ferry, &car_details[i]);
    }

    for (i = 0; i < cars_number; i++)
    {
        pthread_join(cars[i], NULL);
    }
    pthread_join(ferry_operator_thread, NULL);

    free(cars);
    free(car_details);
    return 0;
}