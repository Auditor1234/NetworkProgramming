#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {
    int fd[2];
    // socketpair用于父子进程或线程之间通信
    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    pid_t pid = fork();

    if(pid == 0) {
        // subprocess
        int val = 0;
        close(fd[0]);
        while(true) {
            sleep(1);
            cout << endl;
            cout << "subprocess sending data: " << val << endl;
            write(fd[1], &val, sizeof(val));
            read(fd[1], &val, sizeof(val));
            cout << "subprocess receive data: " << val << endl;
        }
    }else {
        // parent process
        close(fd[1]);
        int val;
        while(true) {
            read(fd[0], &val, sizeof(val));
            cout << "parent process receive data: " << val << endl; 
            val++;
            write(fd[0], &val, sizeof(val));
            cout << "parent process sending data: " << val << endl;
        }
    }
    return 0;
}