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

#define ROAD_MAX_CARS 2
#define DIRECTION_EAST 0
#define DIRECTION_WEST 1

struct car
{
    int id;
    int direction;
};

pthread_t *cars;
struct car *car_details;
struct car *road[ROAD_MAX_CARS];

sem_t sem_car;
sem_t sem_direction;

int current_road_direction;

int remaining_cars = 0;

pthread_mutex_t lock_road_position;
pthread_mutex_t lock_road_direction;

int get_direction();
void init_road();
void *car_enters_road(void *);

void *car_enters_road(void *arg)
{
    struct car *newcar = (struct car *)arg;

    sem_wait(&sem_car);
    printf("Car %d: Wants to enter road with direction ", newcar->id);
    if (newcar->direction == 0)
    {
        printf("East.\n");
    }
    else
    {
        printf("West.\n");
    }
    pthread_mutex_lock(&lock_road_direction);
    if (current_road_direction == -1)
    {
        current_road_direction = newcar->direction;
        sem_wait(&sem_direction);
    }
    if (current_road_direction != -1 && current_road_direction != newcar->direction)
    {
        printf("Car %d: Waiting to pass to ", newcar->id);
        if (newcar->direction == 0)
        {
            printf("East.\n");
        }
        else
        {
            printf("West.\n");
        }
        sem_wait(&sem_direction);
        current_road_direction = newcar->direction;
    }
    pthread_mutex_unlock(&lock_road_direction);

    pthread_mutex_lock(&lock_road_position);
    if (road[0] == NULL)
    {
        remaining_cars++;
        printf("Car %d: Coming from ", newcar->id);
        if (newcar->direction == 0)
        {
            printf("East.");
        }
        else
        {
            printf("West.");
        }
        printf(" Cars on the road: ");
        printf("%d\n", remaining_cars);
        road[0] = newcar;
    }
    else if (road[1] == NULL)
    {
        remaining_cars++;
        printf("Car %d: Coming from ", newcar->id);
        if (newcar->direction == 0)
        {
            printf("East.");
        }
        else
        {
            printf("West.");
        }
        printf(" Cars on the road: ");
        printf("%d\n", remaining_cars);
        road[1] = newcar;
    }
    pthread_mutex_unlock(&lock_road_position);
    sleep(2); /* Crossing road */
    pthread_mutex_lock(&lock_road_position);

    if (road[0] == newcar)
    {
        remaining_cars--;
        printf("Car %d: Reached to ", road[0]->id);
        if (road[0]->direction == 0)
        {
            printf("East.");
        }
        else
        {
            printf("West.");
        }
        printf(" Cars on the road: ");
        printf("%d\n", remaining_cars);
        road[0] = NULL;
    }
    if (road[1] == newcar)
    {
        remaining_cars--;
        printf("Car %d: Reached to ", road[1]->id);
        if (road[1]->direction == 0)
        {
            printf("East.");
        }
        else
        {
            printf("West.");
        }
        printf(" Cars on the road: ");
        printf("%d\n", remaining_cars);
        road[1] = NULL;
    }

    /* An adeiasei kseklidwse to direction */
    if (road[0] == NULL && road[1] == NULL)
    {
        sem_post(&sem_direction);
    }

    pthread_mutex_unlock(&lock_road_position);
    sem_post(&sem_car);
}

int main(int argc, char *argv[])
{
    int cars_number;
    int i;

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

    if (sem_init(&sem_car, 0, 2) == -1)
    {
        printf("Semaphore initialization error\n");
    }
    if (sem_init(&sem_direction, 0, 1) == -1)
    {
        printf("Semaphore initialization error\n");
    }
    if (pthread_mutex_init(&lock_road_position, NULL) != 0)
    {
        printf("Mutex initializaition failed\n");
        return 1;
    }
    if (pthread_mutex_init(&lock_road_direction, NULL) != 0)
    {
        printf("Mutex initializaition failed\n");
        return 1;
    }
    cars = (pthread_t *)malloc(sizeof(pthread_t) * cars_number);
    car_details = (struct car *)malloc(sizeof(struct car) * cars_number);

    init_road();

    for (i = 0; i < cars_number; i++)
    {
        car_details[i].id = i;
        car_details[i].direction = get_direction();
        pthread_create(&cars[i], NULL, car_enters_road, &car_details[i]);
    }

    for (i = 0; i < cars_number; i++)
    {
        pthread_join(cars[i], NULL);
    }

    free(cars);
    free(car_details);
    return 0;
}

void init_road()
{
    int i;
    current_road_direction = -1;
    for (i = 0; i < ROAD_MAX_CARS; i++)
    {
        road[i] = NULL;
    }
}

int get_direction()
{
    srand(time(NULL));
    sleep(1);
    int r = rand() % 2;
    return r;
}
