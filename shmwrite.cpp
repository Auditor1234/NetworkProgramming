#include <iostream>
#include <sys/shm.h>
#include <cstring>
using namespace std;

struct student {
    char name[32];
    int age;
};

int main() {
    int shmid = shmget((key_t)1234, sizeof(student), 0666 | IPC_CREAT);

    void* shm = shmat(shmid, nullptr, 0);

    student* shared = (student *)shm;
    strcpy(shared->name, "hello");
    shared->age = 20;

    // shmdt(shm);

    // shmctl(shmid, IPC_RMID, 0);
    return 0;
}