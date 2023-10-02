#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>

using namespace std;

struct Student {
    char name[32];
    int age;
};

int main() {
    int shmid = shm_open("/shm_write", O_CREAT | O_RDWR, 0666);

    Student* stu;

    ftruncate(shmid, sizeof(stu));

    struct stat buf;
    fstat(shmid, &buf);

    stu = (Student*)mmap(NULL, buf.st_size, PROT_WRITE, MAP_SHARED, shmid, 0);

    strcpy(stu->name, "hello");
    stu->age = 20;

    close(shmid);
    return 0;
}