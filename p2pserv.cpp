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
    exit(0);
}

int main() {
    int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, SOMAXCONN);

    sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(peeraddr);
    int connfd = accept(listenfd, (sockaddr *)&peeraddr, &peerlen);

    pid_t pid = fork();

    if(pid == 0) {
        // 子进程写
        signal(SIGUSR1, handler);
        char sendbuf[BUFFER_SIZE];

        while(fgets(sendbuf, sizeof(sendbuf), stdin) != nullptr) {
            write(connfd, sendbuf, sizeof(sendbuf));
            memset(sendbuf, 0, sizeof(sendbuf));
        }
        cout << "child exit." << endl;
        return 0;
    }else {
        // 父进程读
        char recvbuf[BUFFER_SIZE];

        while(true) {
            memset(recvbuf, 0, sizeof(recvbuf));
            int ret = read(connfd, recvbuf, sizeof(recvbuf));
            if(ret == -1) {
                ERR_EXIT("read");
            }else if(ret == 0) {
                cout << "peer close." << endl;
                break;
            }
            fputs(recvbuf, stdout);
        }
        cout << "parent exit." << endl;
        kill(pid, SIGUSR1);
        exit(0);
    }

    close(connfd);
    close(listenfd);

    return 0;
}