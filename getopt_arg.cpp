#include <iostream>
#include <unistd.h>
using namespace std;

int main(int argc, char* argv[]) {
    cout << "argc = " << argc << endl;
    for(int i = 0; i < argc; i++) {
        cout << argv[i] << endl;
    }
    int opt;

    while(true) {
        opt = getopt(argc, argv, "nt::");
        // 在第一次出错时就退出
        if(opt == '?') {
            cout << "error" << endl;
            exit(1);
        }
        if(opt == -1) {
            break;
        }

        switch(opt) {
            case 'n':
                cout << "n: " << endl;
                break;
            case 't':
                cout << atoi(optarg) << endl;
                break;
        }
    }

    return 0;
}