#include <iostream>

using namespace std;

int testFunc(int a, int b) {
    return a + b;
}

void testFunc(int a) {
    cout << "Hello World" << endl;
}

int main() {
    cout << testFunc(1, 3) << endl;
    return 0;
}