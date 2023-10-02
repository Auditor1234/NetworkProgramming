#include <iostream>
#include <sys/shm.h>
#include <unistd.h>

using namespace std;

struct student {
    char name[32];
    int age;
};

int main() {
    // 开辟共享内存空间
    int shmid = shmget((key_t)1234, 0, 0);
    // 把进程挂接到这块内存中
    void* shm = shmat(shmid, nullptr, 0);

    student* shared = (student *)shm;
    cout << "student name: " << shared->name << ", age = " << shared->age << endl;

    // 断开进程与内存的关联
    shmdt(shm);

    // 释放共享内存
    shmctl(shmid, IPC_RMID, 0);
    return 0;
}