#include "threadpool.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// g++ -c condition.cpp -o condition.o -std=c++11
// ar -rsv libcondition.a condition.o

// g++ -c threadpool.cpp -o threadpool.o -std=c++11
// ar -rsv libthreadpool.a threadpool.o

// 自己定义的库文件顺序很重要，被依赖的库要放在最后面（这里condition要在threadpool后面）
// g++ test.cpp -o test -L. -lthreadpool -lcondition -lpthread -std=c++11

void* run(void* arg) {
    int num = *(int *)arg;
    printf("thread 0x%x is running on task %d.\n", pthread_self(), num);
    sleep(1);
    free(arg);
    return nullptr;
}

int main() {
    threadpool_t pool;
    threadpool_init(&pool, 3);

    for(int i = 0; i < 10; i++) {
        int* a = (int *)malloc(sizeof(int));
        *a = i;
        threadpool_add_task(&pool, run, a);
    }
    threadpool_destroy(&pool);
}