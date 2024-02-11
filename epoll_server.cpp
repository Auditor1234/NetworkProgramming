#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

#define MAXNFDS 1024
#define err_die(m) \
        perror(m); \
        exit(1);

int initserver(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        err_die("sock");
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
            err_die("setsockopt");
    }

    if(bind(sock, (sockaddr *)&server, sizeof(server)) < 0) {
        err_die("bind");
    }

    if(listen(sock, 5) < 0) {
        err_die("listen");
    }

    return sock;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        err_die("argument number");
    }

    int listensock = initserver(atoi(argv[1]));
    cout << "listensock = " << listensock << endl;

    epoll_event event;
    epoll_event events[MAXNFDS];
    int epollInstance = epoll_create1(0);
    if(epollInstance < 0) {
        err_die("epoll instance");
    }

    event.events = EPOLLIN;
    event.data.fd = listensock;

    if(epoll_ctl(epollInstance, EPOLL_CTL_ADD, listensock, &event) < 0) {
        err_die("add server socket");
    }

    while(1) {
        int numEventsReady = epoll_wait(epollInstance, events, MAXNFDS, -1);
        if(numEventsReady < 0) {
            err_die("wait for events");
        }

        for(int i = 0; i < numEventsReady; i++) {
            if(events[i].data.fd == listensock) {
                sockaddr_in client;
                socklen_t clientlen;
                int clientsock = accept(listensock, (sockaddr *)&client, &clientlen);
                if(clientsock < 0) {
                    printf("client accept error.");
                    continue;
                }
                printf("client(socket=%d) connected ok.\n", clientsock);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clientsock;
                
                if(epoll_ctl(epollInstance, EPOLL_CTL_ADD, clientsock, &event) < 0) {
                    printf("clent sock add error");
                    continue;
                }
            }else {
                char buf[1024];
                memset(buf, 0, sizeof(buf));
                int connfd = events[i].data.fd;
                int n = read(connfd, buf, sizeof(buf));

                if(n <= 0) {
                    printf("client(sock=%d) disconnected.\n", connfd);
                    close(connfd);
                }else {
                    printf("recv(sock=%d, size=%d): %s\n", connfd, n, buf);
                    write(connfd, buf, sizeof(buf));
                }
            }
        }
    }

    close(listensock);

    return 0;
}