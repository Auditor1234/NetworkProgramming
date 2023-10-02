#include "shmfifo.h"

struct student {
    char name[32];
    int age;
};

int main() {
    shmfifo_t* shmfifo1 = shmfifo_init(1234, sizeof(student), 3);
    shmfifo_destroy(shmfifo1);
    return 0;
}