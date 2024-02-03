#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

using namespace std;

typedef struct {
    char name[4];
    int age;
} STU;

int main(int argc, char *argv[]) {
    if(argc != 2) {
        cout << "param numbers error !" << endl;
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY, 0666);
    lseek(fd, 0, SEEK_SET);
    STU *ptr = (STU*)mmap(nullptr, sizeof(STU) * 6, PROT_READ, MAP_SHARED, fd, 0);
    for(int i = 0; i < 6; i++) {
        cout << "name = " << (ptr + i)->name << ", and age = " << (ptr + i)->age << endl;
    }
    munmap(ptr, sizeof(STU) * 6);

    return 0;
}