#ifndef SHMFIFO_H
#define SHMFIFO_H

#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <memory.h>

typedef struct shmfifo shmfifo_t;
typedef struct shmhead shmhead_t;

struct shmhead {
    unsigned int blocksize;
    unsigned int blocks;
    unsigned int rd_index;
    unsigned int wr_index;
};

struct shmfifo {
    shmhead_t* p_shm;
    char* p_payloadd;

    int shmid;
    int sem_mutex;
    int sem_full;
    int sem_empty;
};

shmfifo* shmfifo_init(int key, int blocksize, int blocks);
void shmfifo_put(shmfifo* fifo, const void* buf);
void shmfifo_get(shmfifo* fifo, char* buf);
void shmfifo_destroy(shmfifo* fifo);

#endif