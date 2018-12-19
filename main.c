#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <dispatch/dispatch.h>


#define RAND_DIVISOR 1000000
#define NUM_OF_PRODUCERS 3
#define NUM_OF_CONSUMERS 4
#define BUFFER_SIZE 6
#define PRODUCER_ITERS 40
#define CONSUMER_ITERS 30


pthread_mutex_t mutex;
sem_t full, empty;


int buffer[BUFFER_SIZE];
/* buffer counter */
int counter;

pthread_t tid_producers[NUM_OF_PRODUCERS];
pthread_t tid_consumers[NUM_OF_CONSUMERS];

pthread_attr_t attr;

void *producer (void *param);

void *consumer (void *param);

int consume (int *item);

int produce (int item);

void initializeData ()
{

    pthread_mutex_init (&mutex, NULL);
    sem_init (&full, 0, 0);
    sem_init (&empty, 0, BUFFER_SIZE);
    pthread_attr_init (&attr);

    counter = 0;
}


void *producer (void *param)
{
    int item;

    size_t i = 0;

    for (; i < PRODUCER_ITERS; ++i)
    {

        item = rand () / RAND_DIVISOR;

        sem_wait (&empty);

        pthread_mutex_lock (&mutex);

        produce (item);

        printf ("producer produced %d\n", item);

        pthread_mutex_unlock (&mutex);

        sem_post (&full);
    }
}

/* Consumer Thread */
void *consumer (void *param)
{
    int item;

    int i = 0;

    for (; i < CONSUMER_ITERS; ++i)
    {
        sem_wait (&full);

        pthread_mutex_lock (&mutex);

        consume (&item);

        printf ("consumer consumed %d\n", item);

        pthread_mutex_unlock (&mutex);

        sem_post (&empty);
    }
}


int produce (int item)
{

    if (counter < BUFFER_SIZE)
    {
        buffer[counter++] = item;
        return 0;
    } else
    {
        return -1;
    }

}


int consume (int *item)
{

    if (counter > 0)
    {
        *item = buffer[--counter];
        return 0;
    } else
    {
        return -1;
    }

}


int main (int argc, char *argv[])
{
    size_t i;

    /* Initialize the app */
    initializeData ();

    /* Create producer threads */
    for (i = 0; i < NUM_OF_PRODUCERS; ++i)
        pthread_create (&tid_producers[i], &attr, producer, NULL);


    /* Create consumer threads */
    for (i = 0; i < NUM_OF_CONSUMERS; ++i)
        pthread_create (&tid_consumers[i], &attr, consumer, NULL);


    for (i = 0; i < NUM_OF_PRODUCERS; ++i)
        pthread_join (tid_producers[i], NULL);

    for (i = 0; i < NUM_OF_PRODUCERS; ++i)
        pthread_join (tid_producers[i], NULL);


    pthread_mutex_destroy (&mutex);
    sem_destroy (&full);
    sem_destroy (&empty);

    return 0;
}
