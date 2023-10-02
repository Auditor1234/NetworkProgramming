#include "shmfifo.h"
#include <iostream>

using namespace std;

struct student {
    char name[32];
    int age;
};

int main() {
    shmfifo_t* shmfifo1 = shmfifo_init(1234, sizeof(student), 3);

    student* s = (student *)malloc(sizeof(student));

    for(int i = 0; i < 5; i++) {
        shmfifo_get(shmfifo1, (char *)s);
        cout << "name = " << s->name << ", age = " << s->age << endl;
    }

    shmfifo_destroy(shmfifo1);
    return 0;
}