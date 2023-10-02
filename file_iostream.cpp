#include <iostream>
#include <fstream>

using namespace std;

int main() {
    ifstream fin("./abc.txt", ios::in);
    ofstream fout("./abc.txt", ios::out);
    return 0;
}