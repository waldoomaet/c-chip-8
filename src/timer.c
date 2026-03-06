#include <pthread.h>
#include <time.h>
#include "timer.h"

void *threadTick(void *arg)
{
    Args *args = (Args *)arg;

    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 16666666L; // 16ms

    while (1)
    {
        args->tick(args->cpu);
        nanosleep(&tim, NULL);
    }
    pthread_exit(NULL);
}

void startTimerThread(Args *args)
{
    pthread_t thr;
    if (pthread_create(&thr, NULL, threadTick, args))
        printf("Not able to create the thread\n");
}