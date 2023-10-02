#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <memory.h>

using namespace std;

#define ERR_EXIT(m) \
        perror(m); \
        exit(1);

#define BUFFER_SIZE 1024

void handler(int sig) {
    cout << "recv a sig = " << sig << endl;
    exit(1);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr));
    
    char recvbuf[BUFFER_SIZE];
    char sendbuf[BUFFER_SIZE];

    pid_t pid = fork();
    if(pid == 0) {
        // 子进程读
        while(true) {
            int ret = read(sockfd, recvbuf, sizeof(recvbuf));
            if(ret == 0) {
                cout << "server close." << endl;
                break;
            }else if(ret == -1) {
                ERR_EXIT("read");
            }
            fputs(recvbuf, stdout);
        }

        cout << "child exit." << endl;
        cout << "chilg pid = " << getpid() << endl;
        kill(getpid(), SIGUSR1);
        close(sockfd);
    }else {
        // 父进程写
        signal(SIGUSR1, handler);

        while(fgets(sendbuf, sizeof(sendbuf), stdin) != nullptr) {
            write(sockfd, sendbuf, sizeof(sendbuf));
            memset(sendbuf, 0, sizeof(sendbuf));
        }
        cout << "parent exit." << endl;
        close(sockfd);
    }

    return 0;
}