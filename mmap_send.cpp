#include <unistd.h>
#include <sys/mman.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>

using namespace std;

typedef struct {
    char name[4];
    int age;
} STU;

int main(int argc, char* argv[]) {
    if(argc != 2) {
        cout << "param numbers error!" << endl;
        exit(-1);
    }

    int fd = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, 0666);

    // 没有write发生错误的原因是因为mmap不能去扩展一个内容为空的新文件，因为大小为0，所有本没有与之对应的合法的物理页，不能扩展。
    lseek(fd, sizeof(STU) * 6 - 1, SEEK_SET);
    write(fd, "", 1);

    STU *ptr;
    // mmap函数会对内存进行初始化
    ptr = (STU*)mmap(nullptr, sizeof(STU) * 6, PROT_WRITE, MAP_SHARED, fd, 0);

    char ch = 'a';
    for(int i = 0; i < 6; i++) {
        memcpy((ptr + i)->name, &ch, 1);
        (ptr + i)->age = 10 + i;
        ch++;
    }

    munmap(ptr, sizeof(STU) * 6);

    ptr = (STU*)mmap(nullptr, sizeof(STU) * 6, PROT_READ, MAP_SHARED, fd, 0);

    for(int i = 0; i < 6; i++) {
        cout << "name = " << (ptr + i)->name << ", and age = " << (ptr + i)->age << endl;
    }

    return 0;
}