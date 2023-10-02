#include "shmfifo.h"
#include <iostream>

using namespace std;

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* __buf;
};

int sem_create(key_t key) {
    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    return semid;
}

void sem_setval(int semid, int val) {
    union semun su;
    su.val = val;
    int ret = semctl(semid, 0, SETVAL, su);
}

int sem_open(int key) {
    return semget(key, 0, 0);
}

void sem_p(int semid) {
    struct sembuf sembuf;
    sembuf.sem_num = 0;
    sembuf.sem_op = -1;
    sembuf.sem_flg = 0;
    semop(semid, &sembuf, 1);
}

void sem_v(int semid) {
    struct sembuf sembuf;
    sembuf.sem_num = 0;
    sembuf.sem_op = 1;
    sembuf.sem_flg = 0;
    semop(semid, &sembuf, 1);
}

void sem_d(int semid) {
    semctl(semid, 0, IPC_RMID, nullptr);
}

shmfifo* shmfifo_init(int key, int blocksize, int blocks) {
    shmfifo_t* shmfifo1 = (shmfifo_t *)malloc(sizeof(shmfifo_t));
    memset(shmfifo1, 0, sizeof(shmfifo_t));
    int shmid = shmget(key, 0, 0);
    int size = sizeof(shmhead_t) + blocksize * blocks;
    if(shmid == -1) {
        shmid = shmget((key_t)key, size, 0666 | IPC_CREAT);
        shmfifo1->shmid = shmid;
        
        void* shm = shmat(shmid, nullptr, 0);
        shmfifo1->p_shm = (shmhead_t *)shm;

        // create semaphore
        shmfifo1->sem_mutex = sem_create(key + 1);
        shmfifo1->sem_full = sem_create(key + 2);
        shmfifo1->sem_empty = sem_create(key + 3);

        sem_setval(shmfifo1->sem_mutex, 1);
        sem_setval(shmfifo1->sem_full, blocks);
        sem_setval(shmfifo1->sem_empty, 0);

        // initialize other variables
        memset(shmfifo1->p_shm, 0, sizeof(shmhead_t));
        shmfifo1->p_shm->blocks = blocks;
        shmfifo1->p_shm->blocksize = blocksize;
        shmfifo1->p_shm->rd_index = 0;
        shmfifo1->p_shm->wr_index = 0;

        shmfifo1->p_payloadd = (char *)(shmfifo1->p_shm + 1);
    }else {
        shmfifo1->shmid = shmid;
        void* shm = shmat(shmid, nullptr, 0);
        shmfifo1->p_shm = (shmhead_t *)shm;

        // get semaphore
        shmfifo1->sem_mutex = sem_open(key + 1);
        shmfifo1->sem_full = sem_open(key + 2);
        shmfifo1->sem_empty = sem_open(key + 3);

        shmfifo1->p_payloadd = (char *)(shmfifo1->p_shm + 1);
    }
}

void shmfifo_put(shmfifo* fifo, const void* buf) {
    sem_p(fifo->sem_full);
    sem_p(fifo->sem_mutex);
    memcpy(fifo->p_payloadd + (fifo->p_shm->wr_index * fifo->p_shm->blocksize), buf, fifo->p_shm->blocksize);
    fifo->p_shm->wr_index = (fifo->p_shm->wr_index + 1) % fifo->p_shm->blocks;
    sem_v(fifo->sem_mutex);
    sem_v(fifo->sem_empty);
}

void shmfifo_get(shmfifo* fifo, char* buf) {
    sem_p(fifo->sem_empty);
    sem_p(fifo->sem_mutex);
    memcpy(buf, fifo->p_payloadd + (fifo->p_shm->rd_index * fifo->p_shm->blocksize), fifo->p_shm->blocksize);
    fifo->p_shm->rd_index = (fifo->p_shm->rd_index + 1) % fifo->p_shm->blocks;
    sem_v(fifo->sem_mutex);
    sem_v(fifo->sem_full);
}

void shmfifo_destroy(shmfifo* fifo) {
    sem_d(fifo->sem_mutex);
    sem_d(fifo->sem_full);
    sem_d(fifo->sem_empty);
    shmctl(fifo->shmid, IPC_RMID, 0);
    free(fifo);
}