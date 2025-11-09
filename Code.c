/*
   Compile :  gcc rw_simple.c -o rw -lpthread
   Run     :  ./rw
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int shared_data = 0;   // common resource
int readCount = 0;     // number of active readers

pthread_mutex_t rc_mutex;   // protects readCount
pthread_mutex_t rw_mutex;   // ensures writers exclusive access

/* ------------ READER ------------ */
void* reader(void* arg)
{
    int id = *(int*)arg;

    // ENTRY section
    pthread_mutex_lock(&rc_mutex);
    readCount++;
    if (readCount == 1)
        pthread_mutex_lock(&rw_mutex);   // first reader locks writers
    pthread_mutex_unlock(&rc_mutex);

    // CRITICAL section
    printf("Reader %d is reading value = %d\n", id, shared_data);
    sleep(1);

    // EXIT section
    pthread_mutex_lock(&rc_mutex);
    readCount--;
    if (readCount == 0)
        pthread_mutex_unlock(&rw_mutex); // last reader unlocks writers
    pthread_mutex_unlock(&rc_mutex);

    return NULL;
}

/* ------------ WRITER ------------ */
void* writer(void* arg)
{
    int id = *(int*)arg;

    // ENTRY section
    pthread_mutex_lock(&rw_mutex);

    // CRITICAL section
    shared_data++;
    printf("Writer %d wrote value = %d\n", id, shared_data);
    sleep(1);

    // EXIT section
    pthread_mutex_unlock(&rw_mutex);

    return NULL;
}

/* ------------ MAIN ------------ */
int main()
{
    pthread_t r1, r2, r3, w1, w2;
    int id1=1, id2=2, id3=3, id4=1, id5=2;

    pthread_mutex_init(&rc_mutex, NULL);
    pthread_mutex_init(&rw_mutex, NULL);

    // creating threads
    pthread_create(&r1, NULL, reader, &id1);
    pthread_create(&w1, NULL, writer, &id4);
    pthread_create(&r2, NULL, reader, &id2);
    pthread_create(&w2, NULL, writer, &id5);
    pthread_create(&r3, NULL, reader, &id3);

    // waiting for threads to finish
    pthread_join(r1, NULL);
    pthread_join(w1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w2, NULL);
    pthread_join(r3, NULL);

    pthread_mutex_destroy(&rc_mutex);
    pthread_mutex_destroy(&rw_mutex);

    return 0;
}
