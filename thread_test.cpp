#include <iostream>
#include <unistd.h>
#include <thread>
#include <semaphore.h>

using namespace std;

#define CONSUMERS_COUNT 1
#define PRODUCERS_COUNT 5
#define BUFFSIZE 10

int g_buffer[BUFFSIZE];

unsigned short in = 0;
unsigned short out = 0;
unsigned short produce_id;
unsigned short consume_id;

sem_t g_sem_full;
sem_t g_sem_empty;

pthread_mutex_t g_mutex;

pthread_t g_thread[CONSUMERS_COUNT + PRODUCERS_COUNT];

void* consume(void* arg) {
    int num = *((int*)arg);
    while(true) {
        cout << num << " consumer is waiting." << endl;
        sem_wait(&g_sem_empty);
        pthread_mutex_lock(&g_mutex);

        for(int i = 0; i < BUFFSIZE; i++) {
            cout << i << ' ';

            if(g_buffer[i] == -1) {
                cout << "null";
            }else {
                cout << g_buffer[i];
            }

            if(i == out) {
                cout << "\t<--consume";
            }
            cout << endl;
        }

        consume_id = g_buffer[out];
        cout << num << " consumer begin consume product " << consume_id << endl;
        g_buffer[out] = -1;
        out = (out + 1) % BUFFSIZE;
        cout << num << " consumer end consumer product " << consume_id << endl;
        pthread_mutex_unlock(&g_mutex);
        sem_post(&g_sem_full);
        sleep(1);
    }

    return nullptr;
}

void* produce(void* arg) {
    int num = *((int *)arg);
    while(true) {
        cout << num << " producer is waiting " << endl;
        sem_wait(&g_sem_full);
        pthread_mutex_lock(&g_mutex);

        for(int i = 0; i < BUFFSIZE; i++) {
            cout << i << ' ';
            if(g_buffer[i] == -1) {
                cout << "null";
            }else {
                cout << g_buffer[i];
            }
            if(i == in) {
                cout << "\t<--produce";
            }
            cout << endl;
        }

        cout << num << " producer begin produce product " << produce_id << endl;
        g_buffer[in] = produce_id;
        cout << g_buffer[in] << endl;
        in = (in + 1) % BUFFSIZE;
        cout << num << " producer end produce product" << produce_id << endl;
        produce_id++;
        pthread_mutex_unlock(&g_mutex);
        sem_post(&g_sem_empty);
        sleep(5);
    }
    return nullptr;
}

int main() {
    for(int i = 0; i < BUFFSIZE; i++) {
        g_buffer[i] = -1;
    }

    sem_init(&g_sem_full, 0, BUFFSIZE);
    sem_init(&g_sem_empty, 0, 0);

    pthread_mutex_init(&g_mutex, nullptr);

    for(int i = 0; i < CONSUMERS_COUNT; i++) {
        // 线程id，线程属性，调用函数，函数参数
        pthread_create(&g_thread[i], nullptr, consume, &i);
    }

    for(int i = 0; i < PRODUCERS_COUNT; i++) {
        pthread_create(&g_thread[CONSUMERS_COUNT + i], nullptr, produce, &i);
    }

    for(int i = 0; i < CONSUMERS_COUNT + PRODUCERS_COUNT; i++) {
        // 阻塞等待的线程id, 返回值参数
        pthread_join(g_thread[i], nullptr);
    }

    sem_destroy(&g_sem_full);
    sem_destroy(&g_sem_empty);
    pthread_mutex_destroy(&g_mutex);
    cout << "thread_test exit." << endl;

    return 0;
}
