#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void* thread_routine(void* arg) {
    struct timespec abstime;
    int timeout;
    printf("thread 0x%x is starting.\n", (int)pthread_self());
    threadpool_t* pool = (threadpool_t *)arg;

    while(true) {
        timeout = 0;

        condition_lock(&pool->ready);
        pool->idle++;
        while(pool->first == nullptr && !pool->quit) {
            printf("thread 0x%x is waiting.\n", (int)pthread_self());
            clock_gettime(CLOCK_REALTIME, &abstime);
            abstime.tv_sec += 2;

            int state = condition_timewait(&pool->ready, &abstime);
            if(state == ETIMEDOUT) {
                printf("thread 0x%x is time out.", (int)pthread_self());
                timeout = 1;
                break;
            }
        }

        pool->idle--;
        if(pool->first != nullptr) {
            task_t* t = pool->first;
            pool->first = t->next;
            condition_unlock(&pool->ready);

            t->run(t->arg);
            free(t);
            condition_lock(&pool->ready);
        }

        if(pool->quit && pool->first == nullptr) {
            pool->counter--;
            if(pool->counter == 0) {
                condition_signal(&pool->ready);
            }
            condition_unlock(&pool->ready);
            break;
        }

        if(timeout && pool->first == nullptr) {
            pool->counter--;
            condition_unlock(&pool->ready);
            break;
        }
        condition_unlock(&pool->ready);
    }

    printf("thread 0x%x is exiting.\n", pthread_self());

    return nullptr;
}

void threadpool_init(threadpool_t* pool, int threads) {
    condition_init(&pool->ready);
    pool->first = NULL;
    pool->last = NULL;
    pool->counter = 0;
    pool->idle = 0;
    pool->max_threads = threads;
    pool->quit = 0;
}

void threadpool_add_task(threadpool_t* pool, void* (*run)(void* arg), void* arg) {
    task_t* newtask = (task_t *)malloc(sizeof(task_t));
    newtask->run = run;
    newtask->arg = arg;
    newtask->next = NULL;

    condition_lock(&pool->ready);
    if(pool->first == NULL) {
        pool->first = newtask;
    }else {
        pool->last->next = newtask;
    }
    pool->last = newtask;

    if(pool->idle > 0) {
        condition_signal(&pool->ready);
    }else if(pool->counter < pool->max_threads) {
        pool->counter++;
        pthread_t tid;
        pthread_create(&tid, nullptr, thread_routine, pool);
    }
    condition_unlock(&pool->ready);
}

void threadpool_destroy(threadpool_t* pool) {
    if(pool->quit) {
        return;
    }

    condition_lock(&pool->ready);
    pool->quit = 1;
    if(pool->counter > 0) {
        if(pool->idle > 0) {
            condition_broadcast(&pool->ready);
        }

        while(pool->counter > 0) {
            condition_wait(&pool->ready);
        }
    }
    condition_unlock(&pool->ready);

    condition_destroy(&pool->ready);
}