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
    memset(s, 0, sizeof(student));
    s->name[0] = 'A' - 1;
    for(int i = 0; i < 5; i++) {
        s->name[0] = s->name[0] + 1;
        s->age = 20 + i;
        shmfifo_put(shmfifo1, s);
        cout << "send ok." << endl;
    }
}